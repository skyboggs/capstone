all: drawImage
	
drawImage: drawImage.cpp
	clang++ -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL libraylib.a drawImage.cpp -o $@

demoApp: demoApp.cpp
	clang++ -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL libraylib.a demoApp.cpp -o $@

clean:
	rm demoApp
	rm drawImage
