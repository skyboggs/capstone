#ifndef CELL_H
#define CELL_H

#include "raylib.h"
#include "tile.h"
#include <vector>

using std::vector;

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
	vector<tile> possibleTiles;

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
	
	// adds a way to grab the cell color even if the cell is not currently selected
	// if generateRoughColor is false, then we will just return BLACK
	Color getCellColor(); 
	void pickTile();
	void updateTiles(Vector2,Color);
	void updateRoughColor();
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
