#ifndef TABLET_H
#define TABLET_H

#include "raylib.h"
#include "cell.h"
#include <vector>
#include <set>
#include <tuple>

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

  RenderTexture2D tabletScreen;

  // Min-entropy priority queue: (validCount, row, col).
  // Kept in sync with each cell's validIndexes.size() so step() is O(log N)
  // instead of O(W*H).
  std::set<std::tuple<int, int, int>> entropyQueue;
  // Parallel grid tracking each cell's current count in entropyQueue (-1 = not present).
  // Needed for O(1) erase-by-cell when counts change during BFS propagation.
  vector<vector<int>> cellQueueCount;


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

  // collapses the cell at coord and propagates constraints to neighbors via BFS
  void generateCell(Vector2 coord, const textureMapping& texMap);

  // clears isSelected on all cells and re-inits their tile options and rough colors
  void reset(const textureMapping& texMap);

  // finds the unselected cell with the fewest valid tiles (lowest entropy),
  // collapses it, and propagates; returns false when all cells are selected
  bool step(const textureMapping& texMap);

  // collapses the cell at the given screen coordinate and propagates constraints
  bool step(Vector2 coord, const textureMapping& texMap);

};


#endif
