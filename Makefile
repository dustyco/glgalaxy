all: debug release

debug: main.cpp
	g++ -pg -o cpprender_dbg main.cpp -lsfml-system -lsfml-window -lGL

release: main.cpp
	g++ -O3 -o cpprender main.cpp -lsfml-system -lsfml-window -lGL

gldemo: gldemo.cpp
	g++ -O3 -o gldemo gldemo.cpp -lsfml-system -lsfml-window -lGL

gltexture: gltexture.cpp
	g++ -g -o gltexture gltexture.cpp -lsfml-system -lsfml-window -lGL

gljitter: gljitter.cpp
	g++ -g -o gljitter gljitter.cpp -lsfml-system -lsfml-window -lGL
