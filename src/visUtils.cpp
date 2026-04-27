#include <iostream>
#include <vector>
#include "raylib.h"
#include "tileUtils.h"
#include "visualizerSettings.h"
#include "textureMapping.h"
#include "tile.h"

using namespace std;


void processCommands(string& imagePath,int& tileDims,int argc, const char** argv)
{
  vector<string> commandStack;

  for(int i=1;i<argc;++i)
	{
		commandStack.push_back(argv[i]);
	  //cout << "pushed: " << commandStack[commandStack.size()-1] << endl;
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
						int newHeight = newWidth;

            tileDims = newWidth;
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
}

// generates a vector<tile> given:
// int width  of image
// int height of image
// bool print generated tiles in console
vector<tile> genTileList(const textureMapping& tM, bool printGeneratedTiles, bool nonColorConsoleOutput)
{
  const int& width      = tM.imageDims.x;
  const int& height     = tM.imageDims.y;
  const int& tileWidth  = tM.tileDims;
  const int& tileHeight = tM.tileDims;

	vector<tile> tileList;
  Vector2 tileListBoost{0.0f,0.0f};

  if(tM.usingOverlappingImage)
  {
    if(tM.genOverlappingX) { tileListBoost.x = (float)(tileWidth  - 1); }
    if(tM.genOverlappingY) { tileListBoost.y = (float)(tileHeight - 1); }
  }


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
						if(nonColorConsoleOutput)
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
						if(nonColorConsoleOutput)
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
  //cout << "width: " << width << endl;
  //cout << "height: " << height << endl;
  //cout << "tileList.size(): " << tileList.size() << endl;
	return tileList;
}
// checks for user input and updates visualizerSettings if the user activated any shortcut keys
void updateConfig(visualizerSettings& visConfig)
{
  // toggling drawing the lines for the grid
  if(IsKeyPressed(KEY_L)) { visConfig.drawLines = !visConfig.drawLines; }
  
  // toggling adding a visual gap between tiles
  if(IsKeyPressed(KEY_G)) { visConfig.drawWithGap = !visConfig.drawWithGap; }
  
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
  const textureMapping& detailMapper,
  vector<bool> validOptions
)
{
  const vector<Rectangle>& tileSourceRecs = detailMapper.sourceRecs;
  const vector<Rectangle>& tileDestRecs   = detailMapper.destRecs;
  const Texture2D          currentTexture = detailMapper.texture;
  const int&               tileDims       = detailMapper.tileDims;
  const Vector2&           imageDims      = detailMapper.imageDims;

// extracting our settings
  bool&  drawLines      = visConfig.drawLines;
  bool&  drawWithGap    = visConfig.drawWithGap;
  int&   gapSize        = visConfig.gapSize;
  bool&  highlight      = visConfig.highlight;
  Color& highlightColor = visConfig.highlightColor;

// setting tileWindowDims
  // holds the dimensions of how many tiles fit in the x and y direction on the screen
	Vector2 tileWindowDims{imageDims.x - 1.0f, imageDims.y - 1.0f};
  
// drawing all of the destination rectangles
  for(int i=0;i<tileSourceRecs.size();++i)
  {
  	Rectangle currentRectangle = tileDestRecs[i];
  
  	// shrinking the size of the tile with the center being the origin point
  	if(drawWithGap)
  	{
  		currentRectangle.x      += gapSize/2;
  		currentRectangle.y      += gapSize/2;

  		currentRectangle.width  -= gapSize;
  		currentRectangle.height -= gapSize;

      DrawRectangleRec(tileDestRecs[i],BLACK);
  	}
  
  	DrawTexturePro(currentTexture,tileSourceRecs[i],currentRectangle,Vector2{0.0f,0.0f},0.0f,(highlight && validOptions[i]) ? highlightColor : Color{150,150,150,255});
  }
  
// drawing lines to add some seperation between the tiles
  if(drawLines)
  {
    //printVec(tileWindowDims,"tileWindowDims");
    //printVec(imageDims,"imageDims");
  	Color lineColor = (drawWithGap ? WHITE : BLACK);
  
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
        (detailMapper.genOverlappingY ? ((i == (int)(imageDims.x - tileDims + 1)) ? RED : lineColor) : lineColor)
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
        //lineColor
        (m == (int)(imageDims.y - tileDims + 1) ? RED : lineColor)
      );

  	}
  }
}





void generateTileRecs
(
 textureMapping& detailMapper,
 const Vector2& windowDims
)
{
// extracting our variables
  vector<Rectangle>& sourceRecs    = detailMapper.sourceRecs; // the source locations of rectangles on the texture
  vector<Rectangle>& destRecs      = detailMapper.destRecs;   // the destination location of rectangles on the screen
  const Texture2D&   sourceTexture = detailMapper.texture;    // the texture that we are using <-- just need the dimensions from the texture
  const int& tileDims = detailMapper.tileDims;
                                                          
// setting genTileList options                                 
  bool printGeneratedTiles(false);
  bool nonColorConsoleOutput     (false);

// calculating variables for tiles
	// resolution of the texture that was inputted
	//Vector2 imageRes{(float)sourceTexture.width,(float)sourceTexture.height}; 
	Vector2 imageRes = detailMapper.imageDims;

  printVec(imageRes,"imageRes");

  // accounting for if the dimensions should be expanded for overlapping
  if(detailMapper.genOverlappingX) { imageRes.x += tileDims - 1.0f; }
  if(detailMapper.genOverlappingY) { imageRes.y += tileDims - 1.0f; }

  printVec(imageRes,"imageRes");

	// holds a vector of the tiles that go over the inputted texture
	vector<tile> tileHolder = genTileList(detailMapper,printGeneratedTiles,nonColorConsoleOutput);


	// holds the dimensions of how many tiles fit in the x and y direction on the screen
	Vector2 tileWindowDims{(imageRes.x-tileDims) + 1, (imageRes.y-tileDims) + 1};

  // calculating how many pixels the current tiles would take up
  Vector2 currentPixelDims{tileWindowDims.x * detailMapper.tileDims,tileWindowDims.y * detailMapper.tileDims};

  Vector2 atlasDims = currentPixelDims;





  RenderTexture2D textureAtlasGPU = LoadRenderTexture(atlasDims.x,atlasDims.y);

  vector<tile> newTileHolder;
  Vector2 scaleAmount{1.0f,1.0f};

  BeginTextureMode(textureAtlasGPU);
  ClearBackground(BLANK);

  for(int i=0;i<tileHolder.size();++i)
  {
    cout << i << endl;
		Rectangle sourceRec = tileHolder[i].generateRec();
    Rectangle destRec   = Rectangle{tileHolder[i].coordX * (float)tileDims, tileHolder[i].coordY * (float)tileDims,(float)tileDims,(float)tileDims};
    DrawTexturePro
    (
      detailMapper.texture,
      sourceRec,
      destRec,
      Vector2{0.0f,0.0f},
      0.0f,
      WHITE
    );

    newTileHolder.push_back(tile(destRec.x,destRec.y,destRec.width));
  }

  EndTextureMode();



  detailMapper.tileData = LoadImageFromTexture(textureAtlasGPU.texture);
  ImageFlipVertical(&detailMapper.tileData);
  detailMapper.tileAtlas = LoadTextureFromImage(detailMapper.tileData);
  UnloadRenderTexture(textureAtlasGPU);



  scaleAmount.x = 1.0f;
  scaleAmount.y = 1.0f;
  if(detailMapper.genMirroredX   ) 
  { 
    Rectangle currentImageDims{0,0,(float)detailMapper.tileData.width,(float)detailMapper.tileData.height};

    scaleAmount.x *= 2; 
    atlasDims.x *= (float)scaleAmount.x;

    textureAtlasGPU = LoadRenderTexture(atlasDims.x,atlasDims.y);

    BeginTextureMode(textureAtlasGPU);
    Rectangle destRec = currentImageDims;


    // drawing what we already have to the canvas
    DrawTexturePro
    (
      detailMapper.tileAtlas,
      currentImageDims,
      destRec,
      Vector2{0.0f,0.0f},
      0.0f,
      WHITE
    );

    destRec.x += destRec.width;
    currentImageDims.width *= -1;

    // drawing the new portion to the canvas
    DrawTexturePro
    (
      detailMapper.tileAtlas,
      currentImageDims,
      destRec,
      Vector2{0.0f,0.0f},
      0.0f,
      WHITE
    );

    EndTextureMode();

    detailMapper.tileData = LoadImageFromTexture(textureAtlasGPU.texture);
    ImageFlipVertical(&detailMapper.tileData);
    detailMapper.tileAtlas = LoadTextureFromImage(detailMapper.tileData);
    UnloadRenderTexture(textureAtlasGPU);
  }



  scaleAmount.x = 1.0f;
  scaleAmount.y = 1.0f;
  if(detailMapper.genMirroredY   ) 
  { 
    Rectangle currentImageDims{0,0,(float)detailMapper.tileData.width,(float)detailMapper.tileData.height};

    scaleAmount.y *= 2; 
    atlasDims.y *= (float)scaleAmount.y;

    textureAtlasGPU = LoadRenderTexture(atlasDims.x,atlasDims.y);

    BeginTextureMode(textureAtlasGPU);
    Rectangle destRec = currentImageDims;


    // drawing what we already have to the canvas
    DrawTexturePro
    (
      detailMapper.tileAtlas,
      currentImageDims,
      destRec,
      Vector2{0.0f,0.0f},
      0.0f,
      WHITE
    );

    destRec.y += destRec.height;
    currentImageDims.height *= -1.0f;

    // drawing the new portion to the canvas
    DrawTexturePro
    (
      detailMapper.tileAtlas,
      currentImageDims,
      destRec,
      Vector2{0.0f,0.0f},
      0.0f,
      WHITE
    );

    EndTextureMode();

    detailMapper.tileData = LoadImageFromTexture(textureAtlasGPU.texture);
    ImageFlipVertical(&detailMapper.tileData);
    detailMapper.tileAtlas = LoadTextureFromImage(detailMapper.tileData);
    UnloadRenderTexture(textureAtlasGPU);
  }


  scaleAmount.x = 1.0f;
  scaleAmount.y = 1.0f;
  if(detailMapper.genRotatedTiles) 
  { 
    Rectangle currentImageDims{0,0,(float)detailMapper.tileData.width,(float)detailMapper.tileData.height};

    scaleAmount.x *= 2; 
    scaleAmount.y *= 2; 

    atlasDims.x *= (float)scaleAmount.x;
    atlasDims.y *= (float)scaleAmount.y;

    textureAtlasGPU = LoadRenderTexture(atlasDims.x,atlasDims.y);

    BeginTextureMode(textureAtlasGPU);
    Rectangle destRec = currentImageDims;


    // drawing what we already have to the canvas
    DrawTexturePro
    (
      detailMapper.tileAtlas,
      currentImageDims,
      destRec,
      Vector2{0.0f,0.0f},
      0.0f,
      WHITE
    );

	  vector<tile> newTileList = genTileList(detailMapper,false,false);

    for(int y=0;y<2;++y)
    {
      for(int x=0;x<2;++x)
      {
        // skipping the top left spot since that once already is already done
        if(y == 0 && x == 0) { continue; }

        Vector2 tileCount
        {
          currentImageDims.width / (float)detailMapper.tileDims, 
          currentImageDims.height / (float)detailMapper.tileDims
        };

        // the offset of the current tile in pixels
        Vector2 currentOffset
        {
          (x == 0) ? 0 : currentImageDims.width, 
          (y == 0) ? 0 : currentImageDims.height
        };

        for(int i=0;i<tileCount.y;++i)
        {
          for(int m=0;m<tileCount.x;++m)
          {
            Rectangle sourceRec = Rectangle
            {
              (float)(m * detailMapper.tileDims),
              (float)(i * detailMapper.tileDims),
              (float)detailMapper.tileDims,
              (float)detailMapper.tileDims
            };

            Rectangle destRec   = sourceRec;
            destRec.x += currentOffset.x;
            destRec.y += currentOffset.y;
            tileHolder.push_back(tile((int)destRec.x,(int)destRec.y,(int)detailMapper.tileDims,(int)detailMapper.tileDims));

            destRec.x += (float)detailMapper.tileDims / 2.0f;
            destRec.y += (float)detailMapper.tileDims / 2.0f;

            // drawing the new portion to the canvas
            DrawTexturePro
            (
              detailMapper.tileAtlas,
              sourceRec,
              destRec,
              Vector2{(float)detailMapper.tileDims / 2.0f,(float)detailMapper.tileDims / 2.0f},
              (m == 0 ? 90.0f : 0.0f) + (i == 0 ? 180.0f : 0.0f),
              WHITE
            );
          }
        }
      }
    }


    EndTextureMode();

    detailMapper.tileData = LoadImageFromTexture(textureAtlasGPU.texture);
    ImageFlipVertical(&detailMapper.tileData);
    detailMapper.tileAtlas = LoadTextureFromImage(detailMapper.tileData);
    UnloadRenderTexture(textureAtlasGPU);
  }





  if(ExportImage(detailMapper.tileData,"../atlasDump/atlasTest.png"))
  {
    cout << "exported tile atlas" << endl;
  }else
  {
    cout << "failed to export tile atlas" << endl;
  }

  UnloadRenderTexture(textureAtlasGPU);

  


	tileWindowDims = Vector2{detailMapper.image.width / (float)detailMapper.tileDims, detailMapper.image.height / (float)detailMapper.tileDims};



	// holds the dimensions of the tile windows that are drawn to screen
	Vector2 tileDisplayDims{static_cast<float>(windowDims.x)/tileWindowDims.x,static_cast<float>(windowDims.y)/tileWindowDims.y};

// generating source rectangles
	// generating and putting all the source recs in a list
	for(int i=0;i<tileHolder.size();++i)
	{
		sourceRecs.push_back(tileHolder[i].generateRec());
	}

// generating variables for destination rectangles
	// how much the individual tiles need to be scaled up to fit in their displays
	Vector2 tileScale{tileDisplayDims.x/(float)tileDims,tileDisplayDims.y/(float)tileDims};

	if(tileScale.x < tileScale.y) { tileScale.y = tileScale.x; }
	if(tileScale.y < tileScale.x) { tileScale.x = tileScale.y; }

	Vector2 scaledTileSize
  {
    (float)tileDims * tileScale.x,
    (float)tileDims * tileScale.y
  };

// printing debug info
	if(detailMapper.debugMode)
	{
    printVec(tileDisplayDims,"tileDisplayDims");
    printVec(tileWindowDims,"tileWindowDims");
    printVec(tileScale,"tileScale");
	}

  detailMapper.displayDims = Vector2{scaledTileSize.x * tileWindowDims.x, scaledTileSize.y * tileWindowDims.y};

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




