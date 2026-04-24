#include <iostream>
#include <vector>
#include "raylib.h"
#include "tile.h"
#include "tileUtils.h"

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::flush;

// generates a vector<tile> given:
// int width  of image
// int height of image
// int tileSize in a square shape
// bool mirror the tiles in the x or y axis | account for overlap when generating coords
// bool print generated tiles in console
vector<tile> genTileList(int width,int height,int tileWidth, int tileHeight,bool mirroredX,bool mirroredY, bool printGeneratedTiles, bool testing)
{
	vector<tile> tileList;
  Vector2 tileListBoost{0.0f,0.0f};

  if(mirroredX) { tileListBoost.x = (float)(tileWidth  - 1); }
  if(mirroredY) { tileListBoost.y = (float)(tileHeight - 1); }

	// this generates x's for the source squares, for example:
	// a 3 x 3 cube with a tile size of 3 x 1 will look like:
	//
	// # # #
	// o o o
	// o o o
	//
	if(printGeneratedTiles) 
	{ 
		cout << "Tile size: " << endl; 

		for(int i=0;i<height + tileListBoost.y;++i)
		{
			for(int m=0;m<width + tileListBoost.x;++m)
			{
				if(i < tileHeight)
				{
					if(m < tileWidth)
					{
						if(testing)
						{
							cout << "# ";
						}else
						{
							cout << "\033[90m\033[107m. \033[0m";
						}
					}else
					{
						cout << ". ";
					}
				}else
				{
					cout << ". ";
				}
			}
			cout << endl;
		}


		cout << endl;
		cout << "Tile origin locations: " << endl;
	}


	for(int i=0;i < height + tileListBoost.y;++i)
	{
		for(int m=0;m < width + tileListBoost.x;++m)
		{
			if(i + tileHeight <= height + tileListBoost.y)
			{
				if(m + tileWidth <= width + tileListBoost.x)
				{
					tileList.push_back(tile(m,i,tileWidth,tileHeight));
					if(printGeneratedTiles) 
					{ 
						if(testing)
						{
							cout << "# ";
						}else
						{
							cout << "\033[90m\033[107m. \033[0m"; 
						}
					}
				}else
				{
					if(printGeneratedTiles) { cout << ". "; }
				}
			}else
			{
				if(printGeneratedTiles) { cout << ". "; }
			}
		}
		if(printGeneratedTiles) { cout << endl; }
	}

	if(printGeneratedTiles) 
	{ 
		cout << endl;
		cout << flush; 

		// printing out the tiles
		cout << "There are " << tileList.size() << " tiles:" << endl;
		for(tile currentTile : tileList)
		{
			cout << currentTile << endl;
		}
	}
	return tileList;
}

// verify that the CLI is inputted correctly
bool checkCLA(string& argv1,string& argv2,string& imagePath)
{
		// checking if the user wants to set a custom image
		if(argv1 == "--image")
		{
			if(!FileExists(argv2.c_str()))
			{
				cout << endl;
				cout << "\033[31m";
				cout << "!!!" << endl;
				cout << "Invalid input file: " << argv2 << endl;
				cout << "Loading default image instead: " << imagePath << endl;
				cout << "!!!";
				cout << "\033[0m" << endl;
				cout << endl;
			}else
			{
				imagePath = argv2;
				cout << "\033[32mValid input file: \033[0m" << imagePath << endl;
			}

			// returning 0 even if the image loaded bad
			return 0;
		}else
		{
			// printing out an error if the user is not inputted a valid argument
			cout << endl;
			cout << endl;
			cout << "\033[31mError: \033[37mUnrecognized flag: \033[31m" << argv1 << "\033[37m" << endl;
			cout << "CLA argv[1]: \033[31m" << argv1 << "\033[37m, argv[2]: " << argv2 << endl;
			cout << endl;
			cout << "\033[32mUsage:\033[37m" << endl;
			cout << "		./drawImage --image path/to/image" << endl;
			cout << endl;
			cout << endl;

			// returning 1 because there was an error with what argument was inputted
			return 1;
		}
		return 0;
}

void drawCheckeredBackground(int SCREEN_WIDTH,int SCREEN_HEIGHT, int squareWidth,Color c1, Color c2)
{
	Vector2 totalSquares{(float)SCREEN_WIDTH / (float)squareWidth, (float)SCREEN_HEIGHT / (float)squareWidth};

	int squareCountX(totalSquares.x + 1);
	int squareCountY(totalSquares.y + 1);

	for(int i=0;i<squareCountY;++i)
	{
		for(int m=0;m<squareCountX;++m)
		{
			DrawRectangle(squareWidth * m,squareWidth * i,squareWidth,squareWidth,(i + m) % 2 == 0 ? c1 : c2);
		}
	}
}

float generateColorTolerence(Color c1, Color c2)
{
  float colorTolerence = 1.0f;
  cout << "\033[31m!! TODO : void generateColorTolerence(c1,c2)!!\033[0m" << endl;


  return colorTolerence;
}

void generateOverlappingImage(const Image& sourceImage,Image& newImage)
{
  newImage = GenImageColor(sourceImage.width * 2,sourceImage.height * 2,BLANK);

  Rectangle sourceRec{0,0,(float)sourceImage.width,(float)sourceImage.height};

  Rectangle destRec{0,0,(float)sourceImage.width,(float)sourceImage.height};
  ImageDraw(&newImage,sourceImage,sourceRec,destRec,WHITE);

  destRec.x = (float)sourceImage.width;
  ImageDraw(&newImage,sourceImage,sourceRec,destRec,WHITE);

  destRec.x = 0.0f;
  destRec.y = (float)sourceImage.height;
  ImageDraw(&newImage,sourceImage,sourceRec,destRec,WHITE);

  destRec.x = (float)sourceImage.width;
  ImageDraw(&newImage,sourceImage,sourceRec,destRec,WHITE);

}
