#include <iostream>
#include <vector>
#include "raylib.h"
#include "tileUtils.h"
#include "tile.h"

using namespace std;


#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 600

void drawTileCompadabilities
(
  bool drawLines,
  bool drawWithGap,
  vector<Rectangle>& tileSourceRecs,
  vector<Rectangle>& tileDestRecs,
  Texture2D currentTexture,
  Vector2 tileDims
)
{
	Vector2 tileWindowDims{currentTexture.width - tileDims.x, currentTexture.height - tileDims.y};

  int gapSize = 10;
  
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
  	}
    /*
    if(i < 10)
    {
      cout << endl;
      cout << "rectangles drawing[" << i << "]:" << endl;
      cout << " source:" << endl;
      cout << tileSourceRecs[i].x << " " << tileSourceRecs[i].y << " " << tileSourceRecs[i].width << " " << tileSourceRecs[i].height << endl;
      cout << " dest:" << endl;
      cout << currentRectangle.x << " " << currentRectangle.y << " " << currentRectangle.width << " " << currentRectangle.height << endl;
      cout << endl;
    }
    */
  
  	//DrawTexturePro(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color tint);
  	DrawTexturePro(currentTexture,tileSourceRecs[i],currentRectangle,Vector2{0.0f,0.0f},0.0f,WHITE);

    const Rectangle& currentRec = tileDestRecs[i];
    Color currentColor = WHITE;

    switch(i % 4)
    {
      case 0:
        currentColor = Color{ 230, 41, 55, 100};
        break;
      case 1:
        currentColor = Color{ 0, 121, 241, 100};
        break;
      case 2:
        currentColor = Color{255, 161, 0, 100};
        break;
      case 3:
        currentColor = Color{0, 228, 48, 100};
        break;
    }

    DrawRectangleRec(currentRec,currentColor);

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
}





void generateTileRecs
(
 vector<Rectangle>& sourceRecs,  // the source locations of rectangles on the texture
 vector<Rectangle>& destRecs,    // the destination location of rectangles on the screen
 const Texture2D& sourceTexture, // the texture that we are using <-- just need the dimensions from the texture
 const Vector2& tileDims,        // the size of the tiles we are working with
 bool printDebugInfo
)
{
	// resolution of the texture that was inputted
	Vector2 imageRes{(float)sourceTexture.width,(float)sourceTexture.height}; 

	// holds a vector of the tiles that go over the inputted texture
	//vector<tile> tileHolder = genTileList(imageRes.x,imageRes.y,tileDims.x,tileDims.y,false,true,false);
	vector<tile> tileHolder = genTileList(imageRes.x,imageRes.y,tileDims.x,tileDims.y,false,false,false);

	// holds the dimensions of how many tiles fit in the x and y direction on the screen
	Vector2 tileWindowDims{(imageRes.x-tileDims.x) + 1, (imageRes.y-tileDims.y) + 1};

	// holds the dimensions of the tile windows that are drawn to screen
	Vector2 tileDisplayDims{static_cast<float>(WINDOW_WIDTH)/tileWindowDims.x,static_cast<float>(WINDOW_HEIGHT)/tileWindowDims.y};


  cout << endl;
  cout << endl;
  cout << "tileDisplayDims:" << endl;
  cout << "  " << tileDisplayDims.x << " x " << tileDisplayDims.y << endl;
  cout << endl;
  cout << endl;



	// generating and putting all the source recs in a list
	for(int i=0;i<tileHolder.size();++i)
	{
		sourceRecs.push_back(tileHolder[i].generateRec());
	}


	// how much the individual tiles need to be scaled up to fit in their displays
	Vector2 tileScale{tileDisplayDims.x/tileDims.x,tileDisplayDims.y/tileDims.y};

	if(tileScale.x < tileScale.y) { tileScale.y = tileScale.x; }
	if(tileScale.y < tileScale.x) { tileScale.x = tileScale.y; }


	Vector2 scaledTileSize
  {
    (float)static_cast<int>(tileDims.x * tileScale.x),
    (float)static_cast<int>(tileDims.y * tileScale.y)
  };


	if(printDebugInfo)
	{
		cout << endl;
		cout << "-- -- -- tileWindowDims -- -- --" << endl;
		cout << "tileWindowDims.x : " << tileWindowDims.x << endl;
		cout << "tileWindowDims.y : " << tileWindowDims.y << endl;
		cout << "-- -- -- -- -- -- -- -- -- -- -- " << endl;
		cout << endl;
		cout << "tileScale.x : " << tileScale.x << endl;
		cout << "tileScale.y : " << tileScale.y << endl;
	}

	for(int i=0;i<tileWindowDims.y;++i)
	{
		for(int m=0;m<tileWindowDims.x;++m)
		{
			//destRecs.push_back(Rectangle{imageRes.x * i,imageRes.y,tileDims.x,tileDims.y});
			//destRecs.push_back(Rectangle{tileDisplayDims.x * m,tileDisplayDims.y * i,tileDims.x,tileDims.y});
			//destRecs.push_back(Rectangle{tileDisplayDims.x * m,tileDisplayDims.y * i,tileDims.x * tileScale.x,tileDims.y * tileScale.y});
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
			if(printDebugInfo)
			{
        if(i < 4 && m < 4)
        {
          cout << endl;
          cout << "pushing back dest Rec:" << endl;
          cout << "  tileDisplayDims.x * m: " << (tileDisplayDims.x * m) << endl;
          cout << "  tileDisplayDims.y * i: " << (tileDisplayDims.y * i) << endl;
          cout << "  tileDims.x           : " << tileDims.x << endl;
          cout << "  tileDims.y           : " << tileDims.y << endl;
          cout << "  rectangle: { ";
          cout << destRecs.at(destRecs.size()-1).x << ", ";
          cout << destRecs.at(destRecs.size()-1).y << ", ";
          cout << destRecs.at(destRecs.size()-1).width << ", ";
          cout << destRecs.at(destRecs.size()-1).height << "}" << endl;
          cout << endl;
        }
			}
		}
	}
}











// things needed to make this a function call
//   window dimensions to have accurate drawing proportions
//   current texture we are drawing
//   tile dimensions











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
	







  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  ////////////////// drawing section //////////////////
  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////





	InitWindow(WINDOW_WIDTH,WINDOW_HEIGHT,"visualizing tiles");


	Image testImage = LoadImage(imagePath.c_str());
	Texture2D testTexture = LoadTextureFromImage(testImage);
	//Vector2 tileDims{(float)testTexture.width-2,(float)testTexture.height-2};
	//Vector2 tileDims{(float)7,(float)7};
	Vector2 tileDims{(float)tileWidth,(float)tileHeight};
	//Vector2 tileDims{(float)1,(float)1};

	cout << "Tile size: " << tileDims.x << " x " << tileDims.y << endl;
	vector<Rectangle> tileSourceRecs;
	vector<Rectangle> tileDestRecs;

	// generating source and destination recs
	generateTileRecs(tileSourceRecs,tileDestRecs,testTexture,tileDims,true);

	bool drawLines = true;
	bool drawWithGap = true;

	// holds the dimensions of how many tiles fit in the x and y direction on the screen
	//Vector2 tileWindowDims{testTexture.width - tileDims.x, testTexture.height - tileDims.y};

  //RenderTexture2D t1 = LoadRenderTexture(testTexture.width, testTexture.height);
  RenderTexture2D t1 = LoadRenderTexture(WINDOW_WIDTH, WINDOW_HEIGHT);

  BeginTextureMode(t1);
    //ClearBackground(WHITE);
  EndTextureMode();

  RenderTexture2D background = LoadRenderTexture(WINDOW_WIDTH, WINDOW_HEIGHT);

  BeginTextureMode(background);
    ClearBackground(BLACK);
		drawCheckeredBackground(WINDOW_WIDTH,WINDOW_HEIGHT, 40,Color{0,170,100,100}, Color{0,200,100,100});
  EndTextureMode();
  

	while(!WindowShouldClose())
	{
		// toggling drawing the lines for the grid
		if(IsKeyPressed(KEY_L)) { drawLines = !drawLines; }

		// toggling adding a visual gap between tiles
		if(IsKeyPressed(KEY_G)) { drawWithGap = !drawWithGap; }


    // updating the compatability texture
    BeginTextureMode(t1);
      ClearBackground(BLANK);
      //DrawRectangle(0,0,100,100,RED);
      drawTileCompadabilities(drawLines,drawWithGap,tileSourceRecs,tileDestRecs,testTexture,tileDims);
    EndTextureMode();

    float x,y,w,h;
    x = 0.0f;
    y = 0.0f;
    w = static_cast<float>(WINDOW_WIDTH);
    h = static_cast<float>(WINDOW_HEIGHT);



		BeginDrawing();
		  ClearBackground(BLACK);

      // drawing background
      DrawTexturePro
      (
        background.texture,
        (Rectangle){ 0, 0, (float)WINDOW_WIDTH, -(float)WINDOW_HEIGHT },
        (Rectangle){ x, y, w, h},
        Vector2{0,0},
        0.0f,
        WHITE
      );

      // drawing compatability visualizer
      DrawTexturePro
      (
        t1.texture,
        //(Rectangle){ 0, 0, (float)testTexture.width, -(float)testTexture.height },
        (Rectangle){ 0, 0, (float)WINDOW_WIDTH, -(float)WINDOW_HEIGHT},
        (Rectangle){ x, y, w, h},
        Vector2{0,0},
        0.0f,
        WHITE
      );



      //drawTileCompadabilities(drawLines,drawWithGap,tileSourceRecs,tileDestRecs,testTexture,tileDims);

		EndDrawing();
	}

	UnloadImage(testImage);
	UnloadTexture(testTexture);

	CloseWindow();

	return 0;
}


