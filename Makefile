all: gljitter

gldemo: gldemo.cpp
	g++ -O3 -o gldemo gldemo.cpp -lsfml-system -lsfml-window -lGL

gltexture: gltexture.cpp
	g++ -O3 -o gltexture gltexture.cpp -lsfml-system -lsfml-window -lGL

gljitter: gljitter.cpp
	g++ -O3 -o gljitter gljitter.cpp -lsfml-system -lsfml-window -lGL

gl3d: gl3d.cpp
	g++ -O3 -o gl3d gl3d.cpp -lsfml-system -lsfml-window -lGL
