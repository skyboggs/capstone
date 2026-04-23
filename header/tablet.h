#ifndef TABLET_H
#define TABLET_H

#include "raylib.h"
#include "cell.h"
#include <vector>

using std::vector;

struct tablet
{
	///////////////
	// VARIABLES //
	///////////////

  Vector2 tabletDims; // how many pixels/cells are in the x and y direction
  Vector2 pixelDims;  // the size in pixels of the actual pixel being drawn to screen
  vector<vector<cell>> tabletPixels;

  RenderTexture2D tabletScreen;


	/////////////////
	// CONTRUCTORS //
	/////////////////
  tablet(Vector2 tabletDimensions,Vector2 screenDimension);



	//////////////////////
	// MEMBER FUNCTIONS //
	//////////////////////
  void updateTexture();

  // updates the cell at loc inside of tabletPixels and sets its texture to be the one located at Vector2
  //   Vector2 loc    - cell location in tabletPixelx
  //   Vector2 texLoc - tile location 
  void updateCell(Vector2 loc,Vector2 texLoc);
};


#endif
