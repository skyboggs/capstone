#ifndef CELL_H
#define CELL_H

#include "raylib.h"
#include "tile.h"
#include <vector>

using std::vector;

struct textureMapping;

struct cell
{
	///////////////
	// VARIABLES //
	///////////////
	bool isSelected;
	bool generateRoughColor;

	float pixelTolerence;

	Color selectedColor;

	tile selectedTile;

	// indexed 1:1 with textureMapping::sourceRecs — true means still valid
	vector<bool> possibleTiles;
	// compact list of indexes where possibleTiles[i] is still true;
	// iterate this instead of the full possibleTiles to skip eliminated options
	vector<int> validIndexes;

	// this will get you a rough color of what
	// could be at the current cell or it will
	// grab the current selected color of the cell
	Color roughColor;




	/////////////////
	// CONTRUCTORS //
	/////////////////
	cell();
	cell(bool genRoughColor);



	//////////////////////
	// MEMBER FUNCTIONS //
	//////////////////////
	
	// call once after construction to size possibleTiles and fill validIndexes
	void init(const textureMapping& texMap);

	// returns selectedColor if selected, roughColor otherwise (BLACK if generateRoughColor is false)
	Color getCellColor();
	void pickTile(const textureMapping& texMap);
	void pickSpecificTile(int tileIdx, const textureMapping& texMap);
	bool updateTiles(Vector2 offset, Color newColor, const textureMapping& texMap);

	// Eliminates any tile from this cell that has no compatible match among
	// source's current valid tiles, given source is at grid offset (dx, dy) from this cell.
	// Compatible means the two tiles agree pixel-for-pixel on their shared overlap region.
	// Returns true if any tiles were eliminated.
	bool updateTilesCompatibleWith(const cell& source, int dx, int dy, const textureMapping& texMap);
	void updateRoughColor(const textureMapping& texMap);
	// take position of the updated cell ( we only really care about the pixel color at this location )
	// take the current position of this cell and check the pixels that was updated
	// have it so the inputted coordinate is relative to the current cell and we are also inputting the new color
	// this makes it so we don't have to check anything in the actual generated image and we can instead just 
	// we iterate through the possibleTiles list only checking that pixel location
	// have it so the color could be within a certain tolerence of the new pixel color or an exact match, have this be some sort of float value from 0 to 1 to see how similar a color needs to be in order to say that it can't be there
	//
	// have it so the bool that is inputted is to specify if we should be updating the rough color each time we update the tiles
	friend std::ostream& operator<<(std::ostream&,const cell&);
};


#endif
