#include <iostream>
#include <vector>

#include "tile.h"
#include "tileUtils.h"

using namespace std;

int main(int argc, const char** argv)
{
	bool testMode = false;

	int sampleImageWidth  = 1;
	int sampleImageHeight = 1;
	int sampleTileWidth   = 1;
	int sampleTileHeight  = 1;
	vector<tile> testTile;

	// checking if the user used the --test command
	// if the user did, then we want to subtract 
	// from the argc count and set the flag
	string lastCommand = argv[argc-1];
	if(lastCommand == "--test")
	{
		testMode = true;
		argc--;
	}

	if(argc < 3)
	{
		sampleImageWidth  = 3;
		sampleImageHeight = 3;
		sampleTileWidth   = 2;
		sampleTileHeight  = 2;

		testTile = genTileListBasic(sampleImageWidth,sampleImageHeight,sampleTileWidth,sampleTileHeight,false,false,true,testMode);

		vector<tile> testTile2 = genTileListBasic(sampleImageWidth,sampleImageHeight,sampleTileWidth,sampleTileHeight,false,false,false,testMode);
		cout << "testTile2.size(): " << testTile2.size() << endl;
		//vector<tile> genTileList(int width,int height,int tileSize,bool mirroredTiles)
	}else
	{

		sampleImageWidth  = stoi(argv[1]);
		sampleImageHeight = stoi(argv[2]);

		if(argc >= 4) { sampleTileWidth   = stoi(argv[3]); }
		if(argc >= 5) { sampleTileHeight  = stoi(argv[4]); }

		

		
		cout << "width: " << sampleImageWidth << endl;
		cout << "height: " << sampleImageHeight << endl;
		cout << "tile width : " << sampleTileWidth << endl;
		cout << "tile height: " << sampleTileHeight << endl;
		cout << endl;

		cout << "Generate tile list:" << endl;
		cout << "----------------------" << endl;
		testTile = genTileListBasic(sampleImageWidth,sampleImageHeight,sampleTileWidth,sampleTileHeight,false,false,true,testMode);
		cout << "----------------------" << endl;
	}
	cout << flush;

	return 0;
}
