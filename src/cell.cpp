#include <iostream>
#include <vector>
#include "raylib.h"
#include "tile.h"
#include "cell.h"

using namespace std;


cell::cell()
	: 
		isSelected(false), 
		generateRoughColor(true),
		pixelTolerence(1.0f),
		selectedColor(RED),
		selectedTile(tile(0,0,1,1)),
		possibleTiles(vector<tile>()),
		roughColor(BLACK)
{
}

cell::cell(bool genRoughColor)
	: 
		isSelected(false), 
		generateRoughColor(genRoughColor),
		pixelTolerence(1.0f),
		selectedColor(RED),
		selectedTile(tile()),
		possibleTiles(vector<tile>()),
		roughColor(BLACK)
{
}


Color cell::getCellColor()
{
	// returning the current selected color if it 
	// has been selected
	if(isSelected) { return selectedColor; }

	return roughColor;
}

void cell::pickTile()
{
	cout << "\033[31m!! TODO : void cell::pickTile()!!\033[0m";
}

// this updates what tiles are now allowed at the current position
bool cell::updateTiles(Vector2 offset,Color newColor)
{
	cout << "\033[31m!! TODO : void cell::updateTiles(Vector2, Color)!!\033[0m";

  for(int i=0;i<possibleTiles.size();++i)
  {
    tile& currentTile = possibleTiles.at(i);
    //Color currentTileColor = 
    //float currentTolerence = generateColorTolerence(newColor,currentTileColor);



  }

  // if there are no options left after updating the cell, then we return false
  if(possibleTiles.size() == 0)
  {
    return false;
  }

  return true;
}

void cell::updateRoughColor()
{
	cout << "\033[31m!! TODO : void cell::updateRoughColor()!!\033[0m";
  if(!generateRoughColor) { return; }

  Color newColor{0,0,0,255};
  Vector3 colorTotals;

  for(int i=0;i<possibleTiles.size();++i)
  {
    Color currentColor;// = GetImageColor(
    colorTotals.x += currentColor.r;
    colorTotals.y += currentColor.g;
    colorTotals.z += currentColor.b;
  }

  colorTotals.x /= (float)possibleTiles.size();
  colorTotals.y /= (float)possibleTiles.size();
  colorTotals.z /= (float)possibleTiles.size();

  roughColor = newColor;
}

ostream& operator<<(ostream& os, const cell& rhs)
{
	os << "-- Cell --" << endl;
	os << "isSelected         : " << rhs.isSelected  << endl;
	os << "generateRoughColor : " << rhs.generateRoughColor << endl;
	os << "pixelTolerence     : " << rhs.pixelTolerence << endl;
	os << "selectedColor      : " ;
	os << "( ";
	os << to_string(rhs.selectedColor.r) << ", ";
	os << to_string(rhs.selectedColor.g) << ", ";
	os << to_string(rhs.selectedColor.b);
	os << " )" << endl;
	os << "selectedTile       : " << (rhs.selectedTile + "") << endl;
	os << "----------" << endl;
	return os;
}
