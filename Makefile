CC=g++
#SRC=src/enemigo.cpp src/maths_funcs.cpp src/gl_utils.cpp src/main.cpp
SRC=src/*.cpp src/*.c
LIBS=`pkg-config --libs freealut` -lGL -lGLEW -lglfw -lassimp
EXEC=bin/prog

all: 
	${CC} ${SRC} ${LIBS} -o ${EXEC} 
	./bin/prog


clear:
	rm bin/*
