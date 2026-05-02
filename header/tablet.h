#ifndef TABLET_H
#define TABLET_H

#include "raylib.h"
#include "cell.h"
#include <vector>

using std::vector;

struct textureMapping;

struct tablet
{
	///////////////
	// VARIABLES //
	///////////////

  Vector2 tabletDims; // how many pixels/cells are in the x and y direction
  Vector2 pixelDims;  // the size in pixels of the actual pixel being drawn to screen
  vector<vector<cell>> tabletPixels;

  // shuffled at construction — G pops from back, skipping already-selected cells
  vector<Vector2> generationQueue;

  RenderTexture2D tabletScreen;


	/////////////////
	// CONTRUCTORS //
	/////////////////
  tablet(Vector2 tabletDimensions,Vector2 screenDimension);



	//////////////////////
	// MEMBER FUNCTIONS //
	//////////////////////
  // draws to screen the selected cell colors using drawTexturePro of the current tablet screen
  void updateTexture();

  // updates the cell at loc: prunes valid tiles given that pixelOffset (relative to
  // the cell's tile origin) now has newColor, then redraws that cell to tabletScreen
  void updateCell(Vector2 loc, Vector2 pixelOffset, Color newColor, const textureMapping& texMap);

  // pops from generationQueue until it finds an unselected cell, then picks its tile;
  // does nothing if the queue is empty or all remaining cells are already selected
  void generateCell(const textureMapping& texMap);

  // clears isSelected on all cells, re-inits their tile options, updates rough colors,
  // then refills and reshuffles generationQueue
  void reset(const textureMapping& texMap);

  // used in case I want to have multiple tablets being ran at once generating an image
  void step(const textureMapping& texMap);

};


#endif
