// Graphics
#include <SFML/Window.hpp>
#include <GL/gl.h>

// C Standard Library stuff
#include <cstdio>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <fstream>

// STL includes
#include <list>

#define WIDTH 800
#define HEIGHT 800
#define QUADTREE_CELL_CAP 100
#define DEPTH_LIMIT 10
#define ZOOM_CLICK_CHANGE 5

#define MIN(a,b) ((a>b) ? (b) : (a))
#define MAX(a,b) ((a<b) ? (b) : (a))

// ---------------------------------------------
// -------------- HERE BE DRAGONS --------------
// ---------------------------------------------
class Point {
	public:
		Point(float px, float py) {
			x = px;
			y = py;
		}

		float x, y;
};

float floatSwapBytes(float f) {
	char *a = (char*)&f;
	char res[4];
	res[0] = a[3];
	res[1] = a[2];
	res[2] = a[1];
	res[3] = a[0];
	float rf;
	rf = *(float*)res;
	return rf;
}

Point coordsForPt(Point p, Point viewRectOrigin, Point viewRectSize) {
	float x = p.x - viewRectOrigin.x;
	float y = p.y - viewRectOrigin.y;
	float modX = (x/viewRectSize.x);
	float modY = (y/viewRectSize.y);
	return Point(modX, -modY);
}

Point ptForCoords(Point p, Point viewRectOrigin, Point viewRectSize) {
	p = Point(((p.x/WIDTH)-0.5)*2, ((p.y/HEIGHT)-0.5)*2);
	float x = p.x*viewRectSize.x;
	float y = p.y*viewRectSize.y;
	return Point(x+viewRectOrigin.x, y+viewRectOrigin.y);
}

class QuadTreeNode {
	public:
		QuadTreeNode(float px, float py, float width, float height, unsigned int dp) {
			a = NULL;
			b = NULL;
			c = NULL;
			d = NULL;
			x = px;
			y = py;
			w = width;
			h = height;
			depth = dp;
		}

		~QuadTreeNode() {
			if(a) delete a;
			if(b) delete b;
			if(c) delete c;
			if(d) delete d;
			m_contentPoints.clear();
		}
		
		void addPoint(const Point& p) {
			// Needed for point classification
			float midWidth = w/2.0;
			float midHeight = h/2.0;
			
			// Do we add to subnodes, or to our content list?
			if(a) {
				int xIdx = (p.x > (x+midWidth));
				int yIdx = (p.y > (y+midWidth));
				QuadTreeNode* decoder[2][2] = {{a,b},{c,d}};
				QuadTreeNode* tgt = decoder[xIdx][yIdx];
				tgt->addPoint(p);
				return;
			}

			// Add the point
			Point* newPt = new Point(p.x,p.y);
			m_contentPoints.push_front(newPt);
			
			// Did this overflow the point list?
			if((m_contentPoints.size() > QUADTREE_CELL_CAP) && (depth < DEPTH_LIMIT)) {
				// Divide this node into 4 quadrants
				a = new QuadTreeNode(x,y,midWidth,midHeight, depth+1); // Top-left
				b = new QuadTreeNode(x,midHeight,midWidth,midHeight, depth+1); // Bottom-left
				c = new QuadTreeNode(midWidth,y,midWidth,midHeight, depth+1); // Top-right
				d = new QuadTreeNode(midWidth,midHeight,midWidth,midHeight, depth+1); // Bottom-right
				
				// Iterate through and add the content points to all subnodes (starting at the second, to avoid overload)
				std::list<Point*>::iterator i = m_contentPoints.begin();
				for(i++;i != m_contentPoints.end();i++) {
					Point* p = *i;
					int xIdx = (p->x > (x+midWidth));
					int yIdx = (p->y > (y+midWidth));
					QuadTreeNode* decoder[2][2] = {{a,b},{c,d}};
					QuadTreeNode* tgt = decoder[xIdx][yIdx];
					tgt->addPoint(*p);
				}

				// Remove our own point list
				Point* p = m_contentPoints.front();
				Point tmpPt = *p;
				m_contentPoints.clear();
				m_contentPoints.push_back(&tmpPt);
			}
		}

		std::list<Point*> pointsForRect(float px, float py, float width, float height) {
			float endX = px+width;
			float endY = py+height;
			std::list<Point*> l;

			// Return nothing if we're outside the rectangle
			if(!rectTest(px,py,width,height)) return l;

			// Do we have any subnodes?
			if(a) {
				l.insert(l.begin(), m_contentPoints.begin(), m_contentPoints.end());

				// Return the combined list of all subnodes that intersect
				if(a->rectTest(px, py, width, height)) {
					std::list<Point*> v = a->pointsForRect(px, py, width, height);
					l.insert(l.begin(), v.begin(), v.end());
				}
				if(b->rectTest(px, py, width, height)) {
					std::list<Point*> v = b->pointsForRect(px, py, width, height);
					l.insert(l.begin(), v.begin(), v.end());
				}
				if(c->rectTest(px, py, width, height)) {
					std::list<Point*> v = c->pointsForRect(px, py, width, height);
					l.insert(l.begin(), v.begin(), v.end());
				}
				if(d->rectTest(px, py, width, height)) {
					std::list<Point*> v = d->pointsForRect(px, py, width, height);
					l.insert(l.begin(), v.begin(), v.end());
				}
				return l;
			} else {
				// Return our points
				return m_contentPoints;
			}
		}
		
		std::list<Point*> allPoints() {
			if(a) {
				std::list<Point*> l;
				l.insert(l.begin(), m_contentPoints.begin(), m_contentPoints.end());
				std::list<Point*> v = a->allPoints();
				l.insert(l.begin(), v.begin(), v.end());
				v = b->allPoints();
				l.insert(l.begin(), v.begin(), v.end());
				v = c->allPoints();
				l.insert(l.begin(), v.begin(), v.end());
				v = d->allPoints();
				l.insert(l.begin(), v.begin(), v.end());
				return l;
			} else {
				return m_contentPoints;
			}
		}

		bool pointTest(float px, float py) {
			return (px > x && py > y && px < (x+w) && py < (y+h));
		}

		bool rectTest(float px, float py, float width, float height) {
			// Project each rectangle (me and the target) onto the line x=0
			int meXA = MIN(x,x+w);
			int meXB = MAX(x,x+w);
			int srcXA = MIN(px,px+width);
			int srcXB = MAX(px,px+width);

			// Project each rectangle onto line y=0
			int meYA = MIN(y,y+h);
			int meYB = MAX(y,y+h);
			int srcYA = MIN(py,py+height);
			int srcYB = MAX(py,py+height);
			
			// Check X collisions
			bool xIsect = !(meXA > srcXB || meXB < srcXA);
			bool yIsect = !(meYA > srcYB || meYB < srcYA);

			return xIsect && yIsect;
		}

		// This has to be preceded by glBegin(GL_LINES) and precede glEnd();
		void render(Point viewPos, Point viewSize, Point highlightPoint = Point(0,0)) {
			// Recurse
			if(a) {
				a->render(viewPos, viewSize, highlightPoint);
				b->render(viewPos, viewSize, highlightPoint);
				c->render(viewPos, viewSize, highlightPoint);
				d->render(viewPos, viewSize, highlightPoint);
			}

			// Render
			if(pointTest(highlightPoint.x, highlightPoint.y)) {
				glColor3f(1,1,0);
			} else {
				glColor3f(1,0,0);
			}

			float midHeight = h/2.0;
			float midWidth = w/2.0;
			
			Point pTopLeft = coordsForPt(Point(x,y), viewPos, viewSize);
			Point pTopRight = coordsForPt(Point(x+w,y), viewPos, viewSize);
			Point pBottomLeft = coordsForPt(Point(x,y+h), viewPos, viewSize);
			Point pBottomRight = coordsForPt(Point(x+w,y+h), viewPos, viewSize);
			Point pLeftMid = coordsForPt(Point(x, y+midHeight), viewPos, viewSize);
			Point pRightMid = coordsForPt(Point(x+w, y+midHeight), viewPos, viewSize);
			Point pBtmMid = coordsForPt(Point(x+midWidth, y+h), viewPos, viewSize);
			Point pTopMid = coordsForPt(Point(x+midWidth, y), viewPos, viewSize);

			glVertex2f(pTopLeft.x, pTopLeft.y);
			glVertex2f(pTopRight.x, pTopRight.y);
			
			glVertex2f(pTopRight.x, pTopRight.y);
			glVertex2f(pBottomRight.x, pBottomRight.y);

			glVertex2f(pBottomRight.x, pBottomRight.y);
			glVertex2f(pBottomLeft.x, pBottomLeft.y);

			glVertex2f(pBottomLeft.x, pBottomLeft.y);
			glVertex2f(pTopLeft.x, pTopLeft.y);

			glVertex2f(pBtmMid.x, pBtmMid.y);
			glVertex2f(pTopMid.x, pTopMid.y);

			glVertex2f(pLeftMid.x, pLeftMid.y);
			glVertex2f(pRightMid.x, pRightMid.y);
		}

	private:
		std::list<Point*> m_contentPoints;
		QuadTreeNode *a,*b,*c,*d;
		float x,y,w,h;
		unsigned int depth;
};

int main(int argc, const char** argv) {
	// Ensure we're getting args
	if(argc != 2) {
		fprintf(stderr, "Usage: %s [infile]\n", argv[0]);
		return 1;
	}

	// Initialize the All-Seeing, All-Knowing Quadtree
	QuadTreeNode *qt_root = new QuadTreeNode(-50000.0, -50000.0, 100000.0, 100000.0, 0);
	
	// Create the render window
	printf("Creating render window with dimensions (%dx%d)\n", WIDTH, HEIGHT);
	sf::Window win(sf::VideoMode(WIDTH, HEIGHT), "Particle Renderer");
	if(!win.IsOpened()) {
		fprintf(stderr, "Cannot create render window!\n");
		return 1;
	}
	win.SetActive(); // Enable the window as a GL context

	// Set GL params
	glClearColor(0,0,0,0);

	// Open the file
	std::fstream input(argv[1], std::fstream::in | std::fstream::binary);
	if(input.fail()) {
		fprintf(stderr, "Cannot open file\n");
		return 1;
	}
	
	// Get file size
	input.seekg(0, std::fstream::end);
	long int bytes = input.tellg();
	input.seekg(0);
	long int points = (bytes / 4) / 2;
	
	// Read in the file
	long int pointsRead = 0;
	float minX, maxX, minY, maxY;
	minX = 0;
	maxX = 0;
	minY = 0;
	maxY = 0;
	while(!input.eof()) {
		// Read a point
		float x, y;
		input.read((char*)&x, 4);
		if(input.gcount() != 4) break;
		input.read((char*)&y, 4);
		if(input.gcount() != 4) break;
		x = floatSwapBytes(x);
		y = floatSwapBytes(y);
		
		if(x == y) printf("\nALERT\n");

		minX = MIN(x,minX);
		maxX = MAX(x,maxX);
		minY = MIN(y,minX);
		maxY = MAX(y,maxX);

		// Add a point
		Point p(x,y);
		qt_root->addPoint(p);
		pointsRead++;

		if(pointsRead % 100 == 0) {
			// Render the percentage bar
			float percent = ((float)pointsRead) / ((float)points);
			printf("\r%f - %9ld / %9ld", percent, pointsRead, points);
			fflush(stdout);
			glClear(GL_COLOR_BUFFER_BIT);
			glColor3f(1,0,0);
			glBegin(GL_LINE_LOOP);
			glVertex2f(-0.5, 0.1);
			glVertex2f(-0.5,-0.1);
			glVertex2f(0.5,-0.1);
			glVertex2f(0.5,0.1);
			glEnd();
			glColor3f(0,1,0);
			glBegin(GL_QUADS);
			glVertex2f(-0.5, 0.1);
			glVertex2f(-0.5,-0.1);
			glVertex2f(percent-0.5,-0.1);
			glVertex2f(percent-0.5,0.1);
			glEnd();
			win.Display();
		}
	}
	
	// Main loop
	printf("\nRendering\n");
	fflush(stdout);
	float scale = 50.0;
	float xOffs = 0;
	float yOffs = 0;
	bool dragging = false;
	int origDragX, origDragY;
	float origXoffs, origYoffs;
	Point mousePos(0,0);
	while (win.IsOpened()) {
		// Process events
		sf::Event Event;
		while (win.GetEvent(Event))
		{
			// Close window : exit
			if(Event.Type == sf::Event::Closed) {
				win.Close();
			} else if(Event.Type == sf::Event::MouseButtonPressed) {
				dragging = true;
				origDragX = Event.MouseButton.X;
				origDragY = Event.MouseButton.Y;
				origXoffs = xOffs;
				origYoffs = yOffs;
			} else if(Event.Type == sf::Event::MouseButtonReleased) {
				dragging = false;
			} else if(Event.Type == sf::Event::MouseMoved) {
				if(dragging) {
					xOffs = origXoffs + (origDragX - Event.MouseMove.X)*scale;
					yOffs = origYoffs + (origDragY - Event.MouseMove.Y)*scale;
				}
				mousePos = Point(Event.MouseMove.X, Event.MouseMove.Y);
			} else if(Event.Type == sf::Event::MouseWheelMoved) {
				scale += Event.MouseWheel.Delta * ZOOM_CLICK_CHANGE;
			}
		}
		
		// Compute the view rectangle and get the points to render
		float x = minX+xOffs;
		float y = minY+yOffs;
		float width = maxX-minX;
		float height = maxY-minY;
		std::list<Point*> renderPts = qt_root->pointsForRect(x, y, width, height);
		
		// Render all points
		glClear(GL_COLOR_BUFFER_BIT);
		///*
		std::list<Point*>::iterator i;
		glBegin(GL_POINTS);
		glColor3f(0,1,0);
		for(i=renderPts.begin();i != renderPts.end();i++) {
			Point *p = *i;
			Point pt = coordsForPt(*p, Point(x,y), Point(width, height));
			glVertex2f(pt.x,pt.y);
		}
		glEnd();
		//*/
		glColor3f(1,0,0);
		glBegin(GL_LINES);
		Point newPt = ptForCoords(mousePos, Point(x,y), Point(width, height));
		qt_root->render(Point(x,y), Point(width, height), newPt);
		glEnd();
		
		// Flip the buffers
		win.Display();
	}

}

