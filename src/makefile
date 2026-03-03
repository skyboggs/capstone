
lib=../lib/libraylib.a
header=../header
graphics=-framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
cc=clang++

current: cell

all: demoApp tile tileUtils tileUtilsVisual drawImage cell


demoApp: demoApp.cpp
	$(cc) $(graphics) $(lib) demoApp.cpp -o $@ -I $(header)

cell: cell.o tile.o
	$(cc) cellTest.cpp cell.o tile.o -I $(header) -o $@

cell.o: cell.cpp
	$(cc) -c cell.cpp -I $(header) -o $@

tile: tile.o
	#$(cc) tileTest.cpp -I ../header -o $@
	$(cc) tileTest.cpp tile.o -I $(header) -o $@

tile.o: tile.cpp
	$(cc) -c tile.cpp -I $(header) -o $@

tileUtils: tileUtilsTest.cpp tileUtils.o tile.o
	$(cc) $(graphics) $(lib) tileUtilsTest.cpp tileUtils.o tile.o -I $(header) -o $@

tileUtils.o: tileUtils.cpp 
	$(cc) -c tileUtils.cpp -I $(header) -o $@

tileUtilsVisual: tileUtilsVisual.cpp tileUtils.o tile.o
	#$(cc) tileUtilsVisual.cpp -I ../header
	$(cc) $(graphics) $(lib) tileUtilsVisual.cpp tileUtils.o tile.o -o $@ -I $(header)

drawImage: drawImage.cpp tileUtils.o tile.o
	$(cc) $(graphics) $(lib) drawImage.cpp tileUtils.o tile.o -o $@ -I $(header)


clean:
	-rm demoApp
	-rm tile
	-rm tileUtils
	-rm tileUtilsVisual
	-rm demoApp
	-rm drawImage
	-rm cell
	-rm *.o
