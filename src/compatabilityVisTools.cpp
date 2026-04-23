#include <iostream>
#include <vector>
#include "compatabilityVisTools.h"
#include "raylib.h"
#include "tileUtils.h"
#include "tile.h"

using namespace std;


textureMapping::textureMapping(string imagePath, Vector2 tileSize)
  : 
    debugMode(false)
{
  tileDims = tileSize;

  try
  {
    image   = LoadImage(imagePath.c_str());
    texture = LoadTextureFromImage(image);

    cout << "\033[0;32msuccessfully loaded image!\033[0;0m" << endl;
  } catch(...)
  {
    cout << "\033[0;31merror when loading image!\033[0;0m" << endl;
  }
};

textureMapping::~textureMapping()
{
  UnloadImage(image);
  UnloadTexture(texture);
}

void textureMapping::setNewTileSize(Vector2 newTileSize)
{
  tileDims = newTileSize;
};

visualizerSettings::visualizerSettings()
  :
    drawLines(true),
    drawWithGap(true),
    highlight(true),
    gapSize(10),
    highlightColor(GREEN)
{
};

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
  const textureMapping& detailMapping,
  vector<bool> validOptions
)
{
  const vector<Rectangle>& tileSourceRecs = detailMapping.sourceRecs;
  const vector<Rectangle>& tileDestRecs   = detailMapping.destRecs;
  const Texture2D          currentTexture = detailMapping.texture;
  const Vector2&           tileDims       = detailMapping.tileDims;

// extracting our settings
  bool&  drawLines      = visConfig.drawLines;
  bool&  drawWithGap    = visConfig.drawWithGap;
  int&   gapSize        = visConfig.gapSize;
  bool&  highlight      = visConfig.highlight;
  Color& highlightColor = visConfig.highlightColor;

// setting tileWindowDims
  // holds the dimensions of how many tiles fit in the x and y direction on the screen
	Vector2 tileWindowDims{currentTexture.width - tileDims.x, currentTexture.height - tileDims.y};
  
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
 const Vector2& windowDims
)
{
// extracting our variables
  vector<Rectangle>& sourceRecs    = detailMapper.sourceRecs; // the source locations of rectangles on the texture
  vector<Rectangle>& destRecs      = detailMapper.destRecs;   // the destination location of rectangles on the screen
  const Texture2D&   sourceTexture = detailMapper.texture;    // the texture that we are using <-- just need the dimensions from the texture
  const Vector2& tileDims = detailMapper.tileDims;
                                                          
// setting genTileList options                                 
  bool mirroredTiles      (false);
  bool printGeneratedTiles(false);
  bool testing            (false);

// calculating variables for tiles
	// resolution of the texture that was inputted
	Vector2 imageRes{(float)sourceTexture.width,(float)sourceTexture.height}; 


	// holds a vector of the tiles that go over the inputted texture
	vector<tile> tileHolder = genTileList(imageRes.x,imageRes.y,tileDims.x,tileDims.y,mirroredTiles,printGeneratedTiles,testing);

	// holds the dimensions of how many tiles fit in the x and y direction on the screen
	Vector2 tileWindowDims{(imageRes.x-tileDims.x) + 1, (imageRes.y-tileDims.y) + 1};

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
	Vector2 tileScale{tileDisplayDims.x/tileDims.x,tileDisplayDims.y/tileDims.y};

	if(tileScale.x < tileScale.y) { tileScale.y = tileScale.x; }
	if(tileScale.y < tileScale.x) { tileScale.x = tileScale.y; }

	Vector2 scaledTileSize
  {
    (float)static_cast<int>(tileDims.x * tileScale.x),
    (float)static_cast<int>(tileDims.y * tileScale.y)
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




