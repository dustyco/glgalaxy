// Graphics
#include <SFML/Window.hpp>
#include <GL/gl.h>

// C Standard Library stuff
#include <cstdio>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>

#define WIDTH 1024
#define HEIGHT 768

#define FRAMERATE 60

#define JOYSTICKID = 0

#define LINC 0.1
#define LDRAG 0.05

#define MINC 10
#define MDRAG 0.05


#include "matrix.cpp"

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

void printMatrix(int m) {
	float mtx[16];
	glGetFloatv(m, (float*)mtx);
	int i;
	for(i=0;i<4;i++)	printf("%f\t%f\t%f\t%f\n", mtx[i], mtx[i+1], mtx[i+2], mtx[i+3]);
	}

int main(int argc, const char** argv) {
	// Ensure we're getting args
	if(argc != 2) {
		fprintf(stderr, "Usage: %s [infile]\n", argv[0]);
		return 1;
	}
	
	
	// Create the render window
	printf("Creating render window with dimensions (%dx%d)\n", WIDTH, HEIGHT);
	sf::Window win(sf::VideoMode(WIDTH, HEIGHT), "Particle Renderer");
	if(!win.IsOpened()) {
		fprintf(stderr, "Cannot create render window!\n");
		return 1;
		}
	//win.SetFramerateLimit(60);
	win.SetActive(); // Enable the window as a GL context

	// Set GL params
	glClearColor(0,0,0,0);
	glViewport(0,0,WIDTH,HEIGHT);
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, WIDTH, HEIGHT, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();







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
	long int points = (bytes / 4) / 3;
	
	// Allocate array for coordinates (world and camera)
	float scene[3][points][3];
	
	// Read in the file
	printf("Loading %ld points\n",points);
	long int pointsRead = 0;
	float x,y,z;
	char d[12];
	char p[12];
	while(!input.eof() && bytes - pointsRead*4*3 >= 12) {
	
	/*	char *a = (char*)&f;
		char res[4];
		res[0] = a[3];
		res[1] = a[2];
		res[2] = a[1];
		res[3] = a[0];
		float rf;
		rf = *(float*)res;
	*/	
		
		input.read(d,12);
		
		p[0] = d[3];
		p[1] = d[2];
		p[2] = d[1];
		p[3] = d[0];
		
		p[4] = d[7];
		p[5] = d[6];
		p[6] = d[5];
		p[7] = d[4];
		
		p[8] = d[11];
		p[9] = d[10];
		p[10] = d[9];
		p[11] = d[8];
		
		x = *(float*)(&p[0]);
		y = *(float*)(&p[4]);
		z = *(float*)(&p[8]);
		
		
		
		
	/*	// Read a point
		input.read((char*)&x, 4);
		//if(input.gcount() != 4) break;
		input.read((char*)&y, 4);
		//if(input.gcount() != 4) break;
		input.read((char*)&z, 4);
		//if(input.gcount() != 4) break;
		x = floatSwapBytes(x);
		y = floatSwapBytes(y);
		x = floatSwapBytes(z);
	*/	
		//if(true) printf("Point: %f,%f,%f\n",x,y,z);
		
		// Add it
		scene[0][pointsRead][0] = x;
		scene[0][pointsRead][1] = y;
		scene[0][pointsRead][2] = z;

		pointsRead++;
		}
	
	
	
	
	
	
	
	
	// Main loop
	printf("\nRendering\n");
	fflush(stdout);
	sf::Event event;
	//sf::Clock clock;
	float mouseX = 0; float mouseY = 0;
	float pz = 0;
	float px = 0; float lastx = 0;
	float py = 0; float lasty = 0;
	float camX = 0;
	float camY = 0;
	float camZ = 0;
	int width = WIDTH; int height = HEIGHT;
	Matrix mat;
	float* R = mat.makeR(0,0,0);
	float Ri[9];
	float localM[3] = {0,0,0};
	float worldM[3] = {0,0,0};
	
	const sf::Input& controls = win.GetInput();
	float relcamP = 0;
	float relcamY = 0;
	float relcamR = 0;
	float relX = 0;
	float relY = 0;
	float relZ = 0;
	int fps = 0;
	float time = 0;
	int setA = 1;
	int setB = 2;
	bool first = true;
	int JOYID = 0;
	
	while (win.IsOpened()) {
		
	
		// Process events
		while (win.GetEvent(event)) {
			if(event.Type == sf::Event::Closed)
				win.Close();
			else if(event.Type == sf::Event::MouseMoved) {
				mouseX = event.MouseMove.X;
				mouseY = event.MouseMove.Y;
				}
			else if(event.Type == sf::Event::Resized) {
				width = event.Size.Width;
				height = event.Size.Height;
				//glViewport(0,0,width,height);
	glClearColor(0,0,0,0);
	glViewport(0,0,width,height);
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
				}
			}
		
		
		
		
		// Joystick shit
		float joy1 = controls.GetJoystickAxis(1,sf::Joy::AxisX) * 0.9 + 10.0;
		
		     if( joy1 > 10.0 )  { joy1 = joy1-20.0; }
		else if( joy1 < -10.0 ) { joy1 = joy1+20.0; }
		else joy1 = 0.0;
		
		//printf( "%.2f\n",joy1 );
		
		// Find the inverse of the camera's rotation matrix (the UNrotation matrix)
		mat.inv3(R,Ri);
		
		// Relative movement
		float frameratio = 1.0; // win.GetFrameTime()/(1.0/FRAMERATE);
		//printf( "frameratio: %.4f\n",frameratio );
		localM[0] = 0; localM[1] = 0; localM[2] = 0;
			// Keyboard
			if( controls.IsKeyDown(sf::Key::D)       ) localM[0]++;
			if( controls.IsKeyDown(sf::Key::A)       ) localM[0]--;
			if( controls.IsKeyDown(sf::Key::Space)   ) localM[1]--;
		//	if( controls.IsKeyDown(sf::Key::CTRL)    ) localM[1]++;
			if( controls.IsKeyDown(sf::Key::W)       ) localM[2]++;
			if( controls.IsKeyDown(sf::Key::S)       ) localM[2]--;
			float mag = sqrt( localM[0]*localM[0] + localM[1]*localM[1] + localM[2]*localM[2] );
			if(mag!=0) { localM[0]/=mag; localM[1]/=mag; localM[2]/=mag; }
		
			// Joystick
			localM[0]+= controls.GetJoystickAxis(JOYID,sf::Joy::AxisX)/100;
			localM[2]-= controls.GetJoystickAxis(JOYID,sf::Joy::AxisY)/100;

			relcamY += controls.GetJoystickAxis(JOYID,sf::Joy::AxisU)/100*LINC*frameratio;
			relcamP += controls.GetJoystickAxis(JOYID,sf::Joy::AxisV)/100*LINC*frameratio;
			
			relcamR -= (controls.GetJoystickAxis(JOYID,sf::Joy::AxisZ)/200+.5)*LINC*frameratio;
			relcamR += (controls.GetJoystickAxis(JOYID,sf::Joy::AxisR)/200+.5)*LINC*frameratio;
			
		mat.mul3x1add( R,localM,worldM );
		worldM[0] -= worldM[0]*MDRAG*frameratio;
		worldM[1] -= worldM[1]*MDRAG*frameratio;
		worldM[2] -= worldM[2]*MDRAG*frameratio;
		
		camX += worldM[0]*MINC;
		camY += worldM[1]*MINC;
		camZ += worldM[2]*MINC;
		
		// Relative rotation
		if( controls.IsKeyDown(sf::Key::Up)      ) relcamP+=LINC*frameratio;
		if( controls.IsKeyDown(sf::Key::Down)    ) relcamP-=LINC*frameratio;
		if( controls.IsKeyDown(sf::Key::Right)   ) relcamY+=LINC*frameratio;
		if( controls.IsKeyDown(sf::Key::Left)    ) relcamY-=LINC*frameratio;
		if( controls.IsKeyDown(sf::Key::Q)       ) relcamR-=LINC*frameratio;
		if( controls.IsKeyDown(sf::Key::E)       ) relcamR+=LINC*frameratio;
		relcamP -= relcamP*LDRAG*frameratio;
		relcamY -= relcamY*LDRAG*frameratio;
		relcamR -= relcamR*LDRAG*frameratio;
		R = mat.mul3( R,mat.makeR(relcamP,relcamY,relcamR) );
		
		
		
		// Translate and rotate coordinates around the camera
		for( long i = 0; i<points; i++ )
			mat.mul3x1trans( Ri,scene[0][i],scene[setA][i],camX,camY,camZ );
		
		if(first) {
			first = false;
			for( long i = 0; i<points; i++ )
				mat.mul3x1trans( Ri,scene[0][i],scene[setB][i],camX,camY,camZ );
			}
			
		
		
	
		// Render
		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(1,0,0);
		glPointSize(4.0f);
		glLineWidth(4.0f);
		glEnable(GL_POINT_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
		glEnable(GL_LINE_STIPPLE);
		glEnable (GL_LINE_SMOOTH);
		glBegin(GL_LINES);
		for( long i = 0; i<points; i++ ) {
			pz = scene[setA][i][2];
			px = scene[setA][i][0] / pz * height + width/2;
			py = scene[setA][i][1] / pz * height + height/2;
			lastx = scene[setB][i][0] / scene[setB][i][2] * height + width/2;
			lasty = scene[setB][i][1] / scene[setB][i][2] * height + height/2;
			
			float mag = sqrt( (px-lastx)*(px-lastx) + (py-lasty)*(py-lasty) );
			float color = 400.0 / ( (scene[setA][i][2]+scene[setA][i][2]) / 2 );
			
			if( pz>10 && (  (px>0 && px<width && py>0 && py<height) || (lastx>0 && lastx<width && lasty>0 && lasty<height)  ) ) {
				if(mag>4) {
					color /= (mag/4);
					glColor3f(color,color,color);
					glVertex2f( px, py );
					glVertex2f( lastx, lasty );
					}
				else {
					glColor3f(color,color,color);
					glVertex2f( px, py+2 );
					glVertex2f( px, py-2 );
					}
				}
			}
		glEnd();
		
		
		// Flip the sets to save the last screen points for motion blur
		if(setA==1) { setA = 2; setB = 1; }
		else        { setA = 1; setB = 2; }
		
		//printf( "\b\b\b\b\b\b\b\b\b\b\r FPS: %.1f",1.f / win.GetFrameTime() );
		fps++;
		//if( clock.GetElapsedTime() > time ) { printf("FPS: %d\n",fps); fps = 0; time++; }
		win.Display();
		}

	}

