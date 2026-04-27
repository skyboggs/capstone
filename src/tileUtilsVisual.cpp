#include <iostream>
#include <vector>
#include "raylib.h"
#include "tileUtils.h"
#include "tile.h"

using namespace std;




#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 600

#define PADDING_WINDOW 10
//#define PADDING_TILE 30

Rectangle tileWindowRec{PADDING_WINDOW,PADDING_WINDOW,WINDOW_WIDTH - (2 * PADDING_WINDOW),WINDOW_HEIGHT - (2 * PADDING_WINDOW)};

void generateTileRecs(vector<Rectangle>& sourceRecs,vector<Rectangle>& destRecs,const Texture2D& sourceTexture,Vector2 imageRes,const Vector2& tileDims,bool printDebugInfo)
{
	// resolution of the texture that was inputted
	//Vector2 imageRes{(float)sourceTexture.width,(float)sourceTexture.height}; 

	// holds a vector of the tiles that go over the inputted texture
	//vector<tile> tileHolder = genTileList(imageRes.x,imageRes.y,tileDims.x,tileDims.y,false,true,false);
	vector<tile> tileHolder = genTileListBasic(imageRes.x,imageRes.y,tileDims.x,tileDims.y,false,false,false,false);

	// holds the dimensions of how many tiles fit in the x and y direction on the screen
	Vector2 tileWindowDims{(imageRes.x-tileDims.x) + 1, (imageRes.y-tileDims.y) + 1};
	//cout << tileWindowDims.x << ", " << tileWindowDims.y << endl << flush;

	// holds the dimensions of the tile windows that are drawn to screen
	Vector2 tileDisplayDims{tileWindowRec.width/tileWindowDims.x,tileWindowRec.height/tileWindowDims.y};


	// generating and putting all the source recs in a list
	for(int i=0;i<tileHolder.size();++i)
	{
		sourceRecs.push_back(tileHolder[i].generateRec());
	}

	// how much the individual tiles need to be scaled up to fit in their displays
	Vector2 tileScale{tileDisplayDims.x/tileDims.x,tileDisplayDims.y/tileDims.y};

	if(tileScale.x < tileScale.y) { tileScale.y = tileScale.x; }
	if(tileScale.y < tileScale.x) { tileScale.x = tileScale.y; }

	//tileScale.x *= 0.90f;
	//tileScale.y *= 0.90f;

	Vector2 scaledTileSize{(float)static_cast<int>(tileDims.x * tileScale.x),(float)static_cast<int>(tileDims.y * tileScale.y)};

	Vector2 extraTileSpace{tileDisplayDims.x - scaledTileSize.x,tileDisplayDims.y - scaledTileSize.y};
	Vector2 tileOffset
	{
		(float)static_cast<int>((extraTileSpace.x/2.0f) * tileWindowDims.x),
		(float)static_cast<int>((extraTileSpace.y/2.0f) * tileWindowDims.y)
	};


	if(printDebugInfo)
	{
		cout << endl;
		cout << "-- -- -- tileWindowDims -- -- --" << endl;
		cout << "tileWindowDims.x : " << tileWindowDims.x << endl;
		cout << "tileWindowDims.y : " << tileWindowDims.y << endl;
		cout << "-- -- -- -- -- -- -- -- -- -- -- " << endl;
		cout << endl;

	}
		cout << "tileScale.x : " << tileScale.x << endl;
		cout << "tileScale.y : " << tileScale.y << endl;

	for(int i=0;i<tileWindowDims.y;++i)
	{
		for(int m=0;m<tileWindowDims.x;++m)
		{
			//destRecs.push_back(Rectangle{imageRes.x * i,imageRes.y,tileDims.x,tileDims.y});
			//destRecs.push_back(Rectangle{tileDisplayDims.x * m,tileDisplayDims.y * i,tileDims.x,tileDims.y});
			//destRecs.push_back(Rectangle{tileDisplayDims.x * m,tileDisplayDims.y * i,tileDims.x * tileScale.x,tileDims.y * tileScale.y});
			//destRecs.push_back(Rectangle{(tileDisplayDims.x * m) + PADDING_WINDOW,(tileDisplayDims.y * i) + PADDING_WINDOW,scaledTileSize.x,scaledTileSize.y});
			destRecs.push_back(Rectangle{(scaledTileSize.x * m) + PADDING_WINDOW + tileOffset.x,(scaledTileSize.y * i) + PADDING_WINDOW + tileOffset.y,scaledTileSize.x,scaledTileSize.y});
			if(printDebugInfo)
			{
				cout << endl;
				cout << "pushing back dest Rec:" << endl;
				cout << "  tileDisplayDims.x * m: " << (tileDisplayDims.x * m) << endl;
				cout << "  tileDisplayDims.y * i: " << (tileDisplayDims.y * i) << endl;
				cout << "  tileDims.x           : " << tileDims.x << endl;
				cout << "  tileDims.y           : " << tileDims.y << endl;
				cout << endl;
			}
		}
	}

}


int main(int argc, const char** argv)
{
	string imagePath = "../assets/Flowers.png";

	vector<string> commandStack;
	int tileWidth  = 3;
	int tileHeight = 3;

	for(int i=1;i<argc;++i)
	{
		commandStack.push_back(argv[i]);
	//cout << "pushed: " << commandStack[commandStack.size()-1] << endl;
	}

	for(int i=0;i<commandStack.size();++i)
	{
		int remainingArgs = (commandStack.size() - i - 1);

		cout << "remaining args: " << (remainingArgs) << endl;
		cout << "[" << i << "]: '" << commandStack[i] << "'" << endl;

		// checking if the next argument is a flag
		if(commandStack[i].substr(0,2) == "--")
		{
			if(commandStack[i] == "--image")
			{
				if(remainingArgs >= 1)
				{
					checkCLA(commandStack[i], commandStack[i + 1],imagePath);
					++i;
				}else
				{
					cout << "\033[31mNo inputted image\033[0m after --image flag!" << endl;

				}

			}else if(commandStack[i] == "--tile")
			{
				if(remainingArgs >= 1)
				{
					try
					{
						int newWidth  = stoi(commandStack[i + 1]);
						int newHeight = newWidth;

						if(remainingArgs >= 2)
						{
							newHeight = stoi(commandStack[i + 2]);
						}

						tileWidth = newWidth;
						tileHeight = newHeight;
					}catch(const invalid_argument& e)
					{
						cout << "\033[31mNot a valid\033[0m CLI number for the --tile command!" << endl;
					}
				}else
				{
					cout << "\033[31mError! You inputted " << remainingArgs << " dimensions after the --tile flag!\033[0m" << endl;

				}
			}
		}
	}
	

	InitWindow(WINDOW_WIDTH,WINDOW_HEIGHT,"visualizing tiles");


	Image testImage = LoadImage(imagePath.c_str());
  Image oImage;
  generateOverlappingImage(testImage,oImage);
	//Texture2D testTexture = LoadTextureFromImage(testImage);
	Texture2D testTexture = LoadTextureFromImage(oImage);
	//Vector2 tileDims{(float)testTexture.width-2,(float)testTexture.height-2};
	//Vector2 tileDims{(float)7,(float)7};
	Vector2 tileDims{(float)tileWidth,(float)tileHeight};
	//Vector2 tileDims{(float)1,(float)1};

	cout << "Tile size: " << tileDims.x << " x " << tileDims.y << endl;
	vector<Rectangle> tileSourceRecs;
	vector<Rectangle> tileDestRecs;

  Vector2 imageDims{(float)testImage.width,(float)testImage.height};

  imageDims.x += tileDims.x - 1;
  imageDims.y += tileDims.y - 1;

	// generating source and destination recs
	generateTileRecs(tileSourceRecs,tileDestRecs,testTexture,imageDims,tileDims,false);

	bool drawLines = true;
	bool drawWithGap = true;

	// holds the dimensions of how many tiles fit in the x and y direction on the screen
	Vector2 tileWindowDims{testImage.width -1.0f, testImage.height -1.0f};

	while(!WindowShouldClose())
	{
		// toggling drawing the lines for the grid
		if(IsKeyPressed(KEY_L)) { drawLines = !drawLines; }

		// toggling adding a visual gap between tiles
		if(IsKeyPressed(KEY_G)) { drawWithGap = !drawWithGap; }

		BeginDrawing();
		ClearBackground(BLACK);
		//DrawRectangle(0,0,100,100,RED);
		drawCheckeredBackground(WINDOW_WIDTH,WINDOW_HEIGHT, 40,Color{0,170,100,100}, Color{0,200,100,100});

		int gapSize = 10;

		for(int i=0;i<tileSourceRecs.size();++i)
		{
			Rectangle currentRectangle = tileDestRecs[i];

			// shrinking the size of the tile with the center being the origin point
			if(drawWithGap)
			{
				currentRectangle.x += gapSize/2;
				currentRectangle.y += gapSize/2;
				currentRectangle.width -= gapSize;
				currentRectangle.height -= gapSize;
			}

			//DrawTexturePro(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color tint);
			DrawTexturePro(testTexture,tileSourceRecs[i],currentRectangle,Vector2{0.0f,0.0f},0.0f,WHITE);
			//DrawTexturePro(testTexture,tileSourceRecs[i],tileDestRecs[i],Vector2{0.0f,0.0f},0.0f,(i % 2 == 0 ? BLUE : RED));
		}


		// drawing lines to add some seperation between the tiles
		if(drawLines)
		{
			Color lineColor = (drawWithGap ? WHITE : BLACK);

			// drawing a vertical line to show where the tiles are
			for(int i=1;i<=tileWindowDims.y;++i)
			{
				DrawLine(tileDestRecs[0].x,tileDestRecs[0].y + (i * tileDestRecs[0].height),tileDestRecs[0].x + ((tileWindowDims.x + 1) * tileDestRecs[0].width),tileDestRecs[0].y + (i * tileDestRecs[0].height),lineColor);
			}

			// drawing a horizontal line to show where the tiles are
			for(int m=1;m<=tileWindowDims.x;++m)
			{
				DrawLine(tileDestRecs[0].x + ((float)m * tileDestRecs[0].width),tileDestRecs[0].y,tileDestRecs[0].x + ((float)m * tileDestRecs[0].width),tileDestRecs[0].y + ((tileWindowDims.y + 1) * tileDestRecs[0].height),lineColor);
			}
		}


		EndDrawing();
	}
	UnloadImage(testImage);
	UnloadImage(oImage);
	UnloadTexture(testTexture);

	CloseWindow();

	return 0;
}
