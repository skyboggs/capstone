#include "tile.h"
#include "raylib.h"

#include <iostream>

using std::cout;
using std::endl;
using std::ostream;


tile::tile()
	: width(1),height(1),coordX(0),coordY(0)
{
}

tile::tile(int x, int y)
	: width(1),height(1),coordX(x),coordY(y)
{
}

tile::tile(int x, int y,int w, int h)
	: coordX(x),coordY(y),width(w),height(h)
{
}

tile::tile(int x, int y,int w)
	: coordX(x),coordY(y),width(w),height(w)
{
}

void tile::printTile()
{
	cout << "-- Tile --" << endl;
	cout << "width : " << width  << endl;
	cout << "height: " << height << endl;
	cout << "x     : " << coordX << endl;
	cout << "y     : " << coordY << endl;
	cout << "----------" << endl;
}

void tile::setDimensions(int w,int h)
{
	width  = w;
	height = h;
}

void tile::setCoords(int x,int y)
{
	coordX = x;
	coordY = y;
}

Rectangle tile::generateRec()
{
	return Rectangle{(float)coordX,(float)coordY,(float)width,(float)height};
}

ostream& operator<<(ostream& os, const tile& t)
{
	os << "-- Tile --" << endl;
	os << "x     : " << t.coordX << endl;
	os << "y     : " << t.coordY << endl;
	os << "width : " << t.width  << endl;
	os << "height: " << t.height << endl;
	os << "----------" << endl;
	return os;
}

