
# Select platform by uncommenting one line (comment out the other):
#PLATFORM = mac
PLATFORM = WSL

header = ../header

ifeq ($(PLATFORM), mac)
    lib          = ../lib/mac/libraylib.a
    raylibHeader = ../raylibHeader/mac
    graphics     = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
    cc           = clang++
else ifeq ($(PLATFORM), WSL)
    lib          = ../lib/WSL/libraylib.a
    raylibHeader = ../raylibHeader/WSL
    graphics     = -lGL -lm -lpthread -ldl -lrt -lX11
    cc           = g++
endif

current: capstone

all: demoApp tile tileUtils tileUtilsVisual drawImage cell drawTileOptions compatabilityTest


demoApp: demoApp.cpp
	$(cc) demoApp.cpp -o $@ -I $(header) -I $(raylibHeader) $(lib) $(graphics)

cell: cell.o tile.o
	$(cc) cellTest.cpp cell.o tile.o -I $(header) -I $(raylibHeader) -o $@

cell.o: cell.cpp
	$(cc) -c cell.cpp -I $(header) -I $(raylibHeader) -o $@

tile: tile.o
	$(cc) tileTest.cpp tile.o -I $(header) -I $(raylibHeader) -o $@

tile.o: tile.cpp
	$(cc) -c tile.cpp -I $(header) -I $(raylibHeader) -o $@

tileUtils: tileUtilsTest.cpp tileUtils.o tile.o textureMapping.o
	$(cc) tileUtilsTest.cpp tileUtils.o tile.o -o $@ -I $(header) -I $(raylibHeader) $(lib) $(graphics)

tileUtils.o: tileUtils.cpp textureMapping.cpp tile.cpp
	$(cc) -c tileUtils.cpp -I $(header) -I $(raylibHeader) -o $@

tileUtilsVisual: tileUtilsVisual.cpp tileUtils.o tile.o
	$(cc) tileUtilsVisual.cpp tileUtils.o tile.o -o $@ -I $(header) -I $(raylibHeader) $(lib) $(graphics)

drawImage: drawImage.cpp tileUtils.o tile.o
	$(cc) drawImage.cpp tileUtils.o tile.o -o $@ -I $(header) -I $(raylibHeader) $(lib) $(graphics)

drawTileOptions: drawTileOptions.cpp tileUtils.o tile.o textureMapping.o visualizerSettings.o
	$(cc) drawTileOptions.cpp tileUtils.o tile.o textureMapping.o visualizerSettings.o -o $@ -I $(header) -I $(raylibHeader) $(lib) $(graphics)

visualizerSettings.o: visualizerSettings.cpp
	$(cc) visualizerSettings.cpp -c -o $@ -I $(header) -I $(raylibHeader) $(graphics)

textureMapping.o: textureMapping.cpp
	$(cc) textureMapping.cpp -c -o $@ -I $(header) -I $(raylibHeader) $(graphics)

visUtils.o: visUtils.cpp tileUtils.o tile.o visualizerSettings.o textureMapping.o
	$(cc) visUtils.cpp -c -o $@ -I $(header) -I $(raylibHeader) $(graphics)

compatabilityTest: compatabilityTest.cpp visUtils.o
	$(cc) compatabilityTest.cpp visUtils.o tileUtils.o tile.o visualizerSettings.o textureMapping.o -o $@ -I $(header) -I $(raylibHeader) $(lib) $(graphics)

tablet.o: tablet.cpp
	$(cc) tablet.cpp -c -o $@ -I $(header) -I $(raylibHeader)

tabletTest: tabletTest.cpp tablet.o cell.o tile.o textureMapping.o visUtils.o tileUtils.o visualizerSettings.o
	$(cc) tabletTest.cpp tablet.o cell.o tile.o textureMapping.o visUtils.o tileUtils.o visualizerSettings.o -o $@ -I $(header) -I $(raylibHeader) $(lib) $(graphics)

capstone: capstone.cpp tablet.o cell.o tile.o textureMapping.o visUtils.o tileUtils.o visualizerSettings.o
	$(cc) -std=c++17 capstone.cpp tablet.o cell.o tile.o textureMapping.o visUtils.o tileUtils.o visualizerSettings.o -o $@ -I $(header) -I $(raylibHeader) $(lib) $(graphics)


clean:
	-rm demoApp
	-rm cell
	-rm tile
	-rm tileUtils
	-rm tileUtilsVisual
	-rm demoApp
	-rm drawImage
	-rm compatabilityTest
	-rm drawTileOptions
	-rm *.o
