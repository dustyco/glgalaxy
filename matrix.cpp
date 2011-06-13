
#include <cstdio>
#include <cmath>
#include <cstdlib>

class Matrix {
	public:
		void print(float* a, int cols, int rows) {
			for ( int r = 0; r<rows; r++ ) {
				for ( int c = 0; c<cols; c++ )
					std::printf("\t%.2f",a[r*rows+c]);
				std::printf("\n");
				}
			}
			
		// returns a 9 (3x3) array [row*3+col] rotation matrix
		float* makeR(float pitch, float yaw, float roll) {
		
			float RAD = 180.0/3.1415926536;
			
			float pcos = cos(pitch/RAD);
			float psin = sin(pitch/RAD);
			
			float ycos = cos(yaw/RAD);
			float ysin = sin(yaw/RAD);
			
			float rcos = cos(roll/RAD);
			float rsin = sin(roll/RAD);
			
			float Rx[9];
			Rx[0] = 1.0;		Rx[2] = 0.0;		Rx[3] = 0.0;
			Rx[3] = 0.0;		Rx[4] = pcos;		Rx[5] = psin*-1.0;
			Rx[6] = 0.0;		Rx[7] = psin;		Rx[8] = pcos;
			
			float Ry[9];
			Ry[0] = ycos;		Ry[1] = 0.0;		Ry[2] = ysin;
			Ry[3] = 0.0;		Ry[4] = 1.0;		Ry[5] = 0.0;
			Ry[6] = ysin*-1.0;	Ry[7] = 0.0;		Ry[8] = ycos;
			
			float Rz[9];
			Rz[0] = rcos;		Rz[1] = rsin*-1.0;	Rz[2] = 0.0;
			Rz[3] = rsin;		Rz[4] = rcos;		Rz[5] = 0.0;
			Rz[6] = 0.0;		Rz[7] = 0.0;		Rz[8] = 1.0;
			
			return mul3(mul3(Rx,Ry),Rz);
			}
			
		// Finds the inverse of a 9 (3x3) array [row*3+col] matrix, saving it to Mi
		void inv3(float* M, float* Mi) {
		
			float a = M[0];
			float b = M[1];
			float c = M[2];
		
			float d = M[3];
			float e = M[4];
			float f = M[5];
		
			float g = M[6];
			float h = M[7];
			float k = M[8];
		
			float det = a*(e*k - f*h) + b*(f*g - k*d) + c*(d*h - e*g);
		
			float A = e*k - f*h;
			float B = f*g - d*k;
			float C = d*h - e*g;
		
			float D = c*h - b*k;
			float E = a*k - c*g;
			float F = g*b - a*h;
		
			float G = b*f - c*e;
			float H = c*d - a*f;
			float K = a*e - b*d;
		
			Mi[0] = A/det;
			Mi[1] = D/det;
			Mi[2] = G/det;
		
			Mi[3] = B/det;
			Mi[4] = E/det;
			Mi[5] = h/det;
		
			Mi[6] = C/det;
			Mi[7] = F/det;
			Mi[8] = K/det;
			}
		
		// Multiples two 9 (3x3) array [row*3+col] matrices
		float* mul3(float* a, float* b) {
			float* c = (float*)malloc(sizeof(float) * 9);
			
			c[0] = a[0]*b[0] + a[1]*b[3] + a[2]*b[6];
			c[1] = a[0]*b[1] + a[1]*b[4] + a[2]*b[7];
			c[2] = a[0]*b[2] + a[1]*b[5] + a[2]*b[8];
		
			c[3] = a[3]*b[0] + a[4]*b[3] + a[5]*b[6];
			c[4] = a[3]*b[1] + a[4]*b[4] + a[5]*b[7];
			c[5] = a[3]*b[2] + a[4]*b[5] + a[5]*b[8];
		
			c[6] = a[6]*b[0] + a[7]*b[3] + a[8]*b[6];
			c[7] = a[6]*b[1] + a[7]*b[4] + a[8]*b[7];
			c[8] = a[6]*b[2] + a[7]*b[5] + a[8]*b[8];
		
			return c;
			}
		
		// Translates and multiplies a [9] (3x3) by a [3], saving it to c
		void mul3x1trans(float* a, float* b, float* c, float x, float y, float z) {
			float vx = b[0]-x;
			float vy = b[1]-y;
			float vz = b[2]-z;
					
			// [row*3 + col]
			c[0] = a[0]*vx + a[1]*vy + a[2]*vz;
			c[1] = a[3]*vx + a[4]*vy + a[5]*vz;
			c[2] = a[6]*vx + a[7]*vy + a[8]*vz;
			}
		
		// Multiplies a [9] (3x3) by a [3], saving it to c
		void mul3x1add(float* a, float* b, float* c) {
			
			// [row*3 + col]
			c[0] += a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
			c[1] += a[3]*b[0] + a[4]*b[1] + a[5]*b[2];
			c[2] += a[6]*b[0] + a[7]*b[1] + a[8]*b[2];
			}
	};
