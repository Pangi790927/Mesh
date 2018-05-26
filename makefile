
ifeq ($(OS),Windows_NT)
	CXX = x86_64-w64-mingw32-g++
	CXX_FLAGS = -lopengl32 -lgdi32 -lglu32 -o test.exe
else
	CXX = g++-7
	CXX_FLAGS = -lGLEW -lGLU -lGL -lX11 -o test
endif

CXX_INCLUDE = -I../Window -I../Math4f -I../Shaders -I../Texture -I../Misc

all: clean
	$(CXX) -std=c++17 main.cpp $(CXX_FLAGS) $(CXX_INCLUDE)
	./test

clean:
	rm -rf test