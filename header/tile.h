#ifndef TILE_H
#define TILE_H

#include <iostream>
#include "raylib.h"


struct tile
{
	////////////////
	// VARIABLES: //
	////////////////
	int width;
	int height;
	int coordX;
	int coordY; // follows graphics convention of starting at 0 on the top

	///////////////////
	// CONSTRUCTORS: //
	///////////////////
	
	tile();

	// x , y
	tile(int,int);

	// x , y , width
	tile(int,int,int);

	// x , y , width , height
	tile(int,int,int,int);

	////////////////
	// FUNCTIONS: //
	////////////////
	void printTile();
	void setDimensions(int,int);
	void setCoords(int,int);
	Rectangle generateRec();

	friend std::ostream& operator<<(std::ostream&,const tile&);
	friend std::string operator+(const tile&, std::string);
	//friend std::ostream& operator>>(std::ostream&,const tile&);
};


#endif
