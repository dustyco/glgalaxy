all: debug release

gldemo: gldemo.cpp
	g++ -O3 -o gldemo gldemo.cpp -lsfml-system -lsfml-window -lGL

gltexture: gltexture.cpp
	g++ -g -o gltexture gltexture.cpp -lsfml-system -lsfml-window -lGL

gljitter: gljitter.cpp
	g++ -g -o gljitter gljitter.cpp -lsfml-system -lsfml-window -lGL

gl3d: gl3d.cpp
	g++ -g -o gl3d gl3d.cpp -lsfml-system -lsfml-window -lGL
