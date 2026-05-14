#include <iostream>
#include <vector>
#include "raylib.h"
#include "tileUtils.h"
#include "visualizerSettings.h"
#include "textureMapping.h"
#include "tile.h"

using namespace std;


#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 800



// checks for user input and updates visualizerSettings if the user activated any shortcut keys
void updateConfig(visualizerSettings& visConfig)
{
  // toggling drawing the lines for the grid
  if(IsKeyPressed(KEY_L)) { visConfig.drawLines = !visConfig.drawLines; }
  
  // toggling adding a highlight around valid cell options
  if(IsKeyPressed(KEY_H)) { visConfig.highlight = !visConfig.highlight; }
}

void printVec(const Vector2& v, const string vName)
{
  cout << endl;
  cout << "-- -- -- " << vName << "-- -- -- " << endl;
  cout << "  " << vName << ".x : " << v.x << endl;
  cout << "  " << vName << ".y : " << v.y << endl;
  cout << endl;
}

void drawTileCompatabilities
(
  visualizerSettings visConfig,
  textureMapping detailMapping,
  int tileDim,
  vector<bool> validOptions
)
{
  vector<Rectangle>& tileSourceRecs = detailMapping.sourceRecs;
  vector<Rectangle>& tileDestRecs   = detailMapping.destRecs;
  Texture2D currentTexture          = detailMapping.texture;

// extracting our settings
  bool&  drawLines      = visConfig.drawLines;
  bool&  highlight      = visConfig.highlight;
  Color& highlightColor = visConfig.highlightColor;

// setting tileWindowDims
  // holds the dimensions of how many tiles fit in the x and y direction on the screen
	Vector2 tileWindowDims{currentTexture.width - (float)tileDim, currentTexture.height - (float)tileDim};
  
// drawing all of the destination rectangles
  for(int i=0;i<tileSourceRecs.size();++i)
  {
  	Rectangle currentRectangle = tileDestRecs[i];
  
  	DrawTexturePro(currentTexture,tileSourceRecs[i],currentRectangle,Vector2{0.0f,0.0f},0.0f,(highlight && validOptions[i]) ? highlightColor : Color{150,150,150,255});
  }
  
// drawing lines to add some seperation between the tiles
  if(drawLines)
  {
  	Color lineColor = BLACK;
  
  // drawing a vertical line to show where the tiles are
  	for(int i=1;i<=tileWindowDims.x;++i)
  	{
  		DrawLineEx
      (
        Vector2
        {
          tileDestRecs[0].x + (i * tileDestRecs[0].width),
          tileDestRecs[0].y
        },
        { 
          tileDestRecs[0].x + (i * tileDestRecs[0].width),
          tileDestRecs[0].y + ((tileWindowDims.y + 1) * tileDestRecs[0].height),
        },
        1.9f,
        lineColor
      );
  	}
  
  // drawing a horizontal line to show where the tiles are
  	for(int m=1;m<=tileWindowDims.y;++m)
  	{
      DrawLineEx
      (
        Vector2
        {
          tileDestRecs[0].x,
          tileDestRecs[0].y + (m * tileDestRecs[0].height)
        },
        Vector2
        {
          tileDestRecs[0].x + ((float)(tileWindowDims.x + (float)1) * tileDestRecs[0].width),
          tileDestRecs[0].y + (m * tileDestRecs[0].height)
        },
        1.9f,
        lineColor
      );

  	}
  }
}





void generateTileRecs
(
 textureMapping& detailMapper,
 const int& tileDim // the size of the tiles we are working with
)
{
// extracting our variables
 vector<Rectangle>& sourceRecs    = detailMapper.sourceRecs; // the source locations of rectangles on the texture
 vector<Rectangle>& destRecs      = detailMapper.destRecs;   // the destination location of rectangles on the screen
 const Texture2D&   sourceTexture = detailMapper.texture;    // the texture that we are using <-- just need the dimensions from the texture
                                                          
// setting genTileList options                                 
  bool mirroredX          (false);
  bool mirroredY          (false);
  bool printGeneratedTiles(false);
  bool testing            (false);

// calculating variables for tiles
	// resolution of the texture that was inputted
	Vector2 imageRes{(float)sourceTexture.width,(float)sourceTexture.height}; 


	// holds a vector of the tiles that go over the inputted texture
	vector<tile> tileHolder = genTileListBasic(imageRes.x,imageRes.y,(float)tileDim,(float)tileDim,mirroredX,mirroredY,printGeneratedTiles,testing);

	// holds the dimensions of how many tiles fit in the x and y direction on the screen
	Vector2 tileWindowDims{(imageRes.x-tileDim) + 1, (imageRes.y-tileDim) + 1};

	// holds the dimensions of the tile windows that are drawn to screen
	Vector2 tileDisplayDims{static_cast<float>(WINDOW_WIDTH)/tileWindowDims.x,static_cast<float>(WINDOW_HEIGHT)/tileWindowDims.y};

// generating source rectangles
	// generating and putting all the source recs in a list
	for(int i=0;i<tileHolder.size();++i)
	{
		sourceRecs.push_back(tileHolder[i].generateRec());
	}

// generating variables for destination rectangles
	// how much the individual tiles need to be scaled up to fit in their displays
	Vector2 tileScale{tileDisplayDims.x/(float)tileDim,tileDisplayDims.y/(float)tileDim};

	if(tileScale.x < tileScale.y) { tileScale.y = tileScale.x; }
	if(tileScale.y < tileScale.x) { tileScale.x = tileScale.y; }

  Vector2 displayDimensions{tileScale.x * (float)tileDim,tileScale.y * (float)tileDim};


	Vector2 scaledTileSize
  {
    (float)static_cast<int>(tileDim * tileScale.x),
    (float)static_cast<int>(tileDim * tileScale.y)
  };

// printing debug info
	if(detailMapper.debugMode)
	{
    printVec(tileDisplayDims,"tileDisplayDims");
    printVec(displayDimensions,"displayDimensions");
    printVec(tileWindowDims,"tileWindowDims");
    printVec(tileScale,"tileScale");
	}

// generating destination rectangles
	for(int i=0;i<tileWindowDims.y;++i)
	{
		for(int m=0;m<tileWindowDims.x;++m)
		{
			destRecs.push_back
      (
        Rectangle
        {
          (scaledTileSize.x * m),
          (scaledTileSize.y * i),
          scaledTileSize.x,
          scaledTileSize.y
        }
      );
		}
	}
}








int main(int argc, const char** argv)
{
	string imagePath = "../assets/Flowers.png";

	vector<string> commandStack;

// selecting how big a tile is in our case
	int tileDim(3);

// pushing inputting arguments to the command stack
	for(int i=1;i<argc;++i)
	{
		commandStack.push_back(argv[i]);
	  cout << "pushed: " << commandStack[commandStack.size()-1] << endl;
	}

// processing the command stack and setting program configuration
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

            tileDim = newWidth;
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

// declaring our variables
  float currentAngle(0.0f);
  textureMapping generationDetails(imagePath,tileDim);
  visualizerSettings visConfig;

/////////////////////////////////////////////////////
////// setting our variables and parameters /////////
/////////////////////////////////////////////////////


	//cout << "Tile size: " << tileDim << endl;

// setting generationDetails variables
  generationDetails.image     = LoadImage(imagePath.c_str());
  generationDetails.texture   = LoadTextureFromImage(generationDetails.image);
  generationDetails.debugMode = false;

// setting generationDetails Rectangles
  generateTileRecs(generationDetails,tileDim);
  
  vector<Rectangle>& tileSourceRecs = generationDetails.sourceRecs;
  vector<Rectangle>& tileDestRecs   = generationDetails.destRecs;

// getting the dimensions of the window that the compatability visualizer is being drawn in
  const Rectangle& lastRec = tileDestRecs.at(tileDestRecs.size()-1);

  Vector2 displayDimensions
  {
    (lastRec.x + lastRec.width)  - tileDestRecs[0].x,
    (lastRec.y + lastRec.height) - tileDestRecs[0].y
  };

// initializiing our textures and drawing the background
  RenderTexture2D t1         = LoadRenderTexture(displayDimensions.x, displayDimensions.y);
  RenderTexture2D background = LoadRenderTexture(WINDOW_WIDTH       , WINDOW_HEIGHT      );

  BeginTextureMode(background);
    ClearBackground(BLACK);
		drawCheckeredBackground(WINDOW_WIDTH,WINDOW_HEIGHT, 40,Color{0,170,100,100}, Color{0,200,100,100});
  EndTextureMode();

// marking a compatability list to change which tiles should be marked as compatible
  vector<bool> compatibleList(tileDestRecs.size(),false);

  try
  {
    for(int i=0;i<400;i+=(9 + ((i * i) % 7)))
    {
      compatibleList.at(i) = true;
    }
    compatibleList.at(4  ) = true;
    compatibleList.at(9  ) = true;
    compatibleList.at(39 ) = true;
    compatibleList.at(49 ) = true;
    compatibleList.at(149) = true;
    compatibleList.at(119) = true;
    compatibleList.at(129) = true;
    compatibleList.at(139) = true;
  }catch(out_of_range e)
  {
    cout << "out of range" << endl;
  }


//////////////////////////
//// draw loop ///////////
//////////////////////////
	while(!WindowShouldClose())
	{
  // checking for new user input
    updateConfig(visConfig);


// updating the compatability texture
    BeginTextureMode(t1);
      ClearBackground(BLANK);
      drawTileCompatabilities(visConfig,generationDetails,tileDim,compatibleList);
    EndTextureMode();

  // starting to draw
		BeginDrawing();
		  ClearBackground(BLACK);

  // drawing background
      DrawTexturePro
      (
        background.texture,
        (Rectangle){ 0, 0, (float)WINDOW_WIDTH, -(float)WINDOW_HEIGHT },
        (Rectangle){ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT},
        Vector2{0,0},
        0.0f,
        WHITE
      );

  // drawing compatability visualizer
      DrawTexturePro
      (
        t1.texture,
        (Rectangle){ 0, 0, (float)displayDimensions.x, -(float)displayDimensions.y},
        (Rectangle){ (float)WINDOW_WIDTH/2.0f, (float)WINDOW_HEIGHT/2.0f, 400, 400},
        Vector2{(float)200.0f,(float)200.0f},
        currentAngle,
        WHITE
      );

    //currentAngle += 0.2f;

		EndDrawing();
	}

// unloading images and textures
	UnloadImage(generationDetails.image);
	UnloadTexture(generationDetails.texture);

	CloseWindow();

	return 0;
}


