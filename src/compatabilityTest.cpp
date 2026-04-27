#include <iostream>
#include <vector>
#include "raylib.h"
#include "tileUtils.h"
#include "tile.h"
#include "visUtils.h"

using namespace std;


#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 800


int main(int argc, const char** argv)
{
  Vector2 windowDims{WINDOW_WIDTH,WINDOW_HEIGHT};
	string imagePath = "../assets/Flowers.png";

	int tileDim  = 3;

  processCommands(imagePath,tileDim,argc,argv);

	InitWindow(WINDOW_WIDTH,WINDOW_HEIGHT,"visualizing tiles");

// wrapping the below variables in a different scope so they deconstruct before CloseWindow call
  { 

// declaring our variables
  textureMapping genDetails(imagePath,tileDim);
  //genDetails.genOverlappingX = false;
  //genDetails.genOverlappingY = false;
  visualizerSettings visConfig;

/////////////////////////////////////////////////////
////// setting our variables and parameters /////////
/////////////////////////////////////////////////////

// setting genDetails Rectangles
  generateTileRecs(genDetails,windowDims);
  
// initializiing our textures and drawing the background
  RenderTexture2D t1         = LoadRenderTexture(genDetails.displayDims.x, genDetails.displayDims.y);
  RenderTexture2D background = LoadRenderTexture(windowDims.x       , windowDims.y       );

  BeginTextureMode(background);
    ClearBackground(BLACK);
		drawCheckeredBackground(WINDOW_WIDTH,WINDOW_HEIGHT, 40,Color{0,170,100,100}, Color{0,200,100,100});
  EndTextureMode();

// marking a compatability list to change which tiles should be marked as compatible
  vector<bool> compatibleList(genDetails.destRecs.size(),false);

  try
  {
    for(int i=0;i<400;i+=(9 + ((i * i) % 7)))
    {
      compatibleList.at(i) = true;
    }
  }catch(out_of_range e)
  {
    cout << "out of range" << endl;
  }

  float currentAngle(0.0f);
  Vector2 drawLocation{(float)WINDOW_WIDTH/2.0f,(float)WINDOW_HEIGHT/2.0f};
  float adjustmentAmount = 1.5f;


//////////////////////////
//// draw loop ///////////
//////////////////////////
	while(!WindowShouldClose())
	{
    if(IsKeyDown(KEY_LEFT )) { drawLocation.x -= adjustmentAmount; }
    if(IsKeyDown(KEY_RIGHT)) { drawLocation.x += adjustmentAmount; }
    if(IsKeyDown(KEY_UP   )) { drawLocation.y -= adjustmentAmount; }
    if(IsKeyDown(KEY_DOWN )) { drawLocation.y += adjustmentAmount; }
    if(IsKeyDown(KEY_A    )) { currentAngle   -= adjustmentAmount; }
    if(IsKeyDown(KEY_D    )) { currentAngle   += adjustmentAmount; }

  // checking for new user input
    updateConfig(visConfig);


// updating the compatability texture
    BeginTextureMode(t1);
      ClearBackground(BLANK);
      drawTileCompatabilities(visConfig,genDetails,compatibleList);
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
        (Rectangle){ 0, 0, (float)genDetails.displayDims.x, -(float)genDetails.displayDims.y},
        (Rectangle){ (float)drawLocation.x, (float)drawLocation.y, 400, 400},
        Vector2{(float)200.0f,(float)200.0f},
        currentAngle,
        WHITE
      );

    //currentAngle += 0.2f;

		EndDrawing();
	}

  UnloadRenderTexture(t1);
  UnloadRenderTexture(background);

  }

	CloseWindow();

	return 0;
}


