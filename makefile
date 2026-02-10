all: testApp
	

testApp: testApp.cpp
	clang -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL libraylib.a testApp.cpp -o $@

clean:
	rm testApp
