#include <iostream>
#include "tileUtils.h"
#include "raylib.h"

using namespace std;

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 720


void drawImageGrid(Vector2 origin,Vector2 dim,Vector2 pixelCount,Color gridColor)
{
	Vector2 gapSize{dim.y/pixelCount.y,dim.x/pixelCount.x};

	for(int i=0;i<=pixelCount.y;++i)
	{
		DrawLine(origin.x,origin.y + ((float)i * gapSize.y),origin.x + dim.x,origin.y + ((float)i * gapSize.y),gridColor);
	}

	for(int m=0;m<=pixelCount.x;++m)
	{
		DrawLine(origin.x + ((float)m * gapSize.x),origin.y,origin.x + ((float)m * gapSize.x),origin.y + dim.y,gridColor);
	}
}

// displays the dimensions of the inputted texture on the top left
void drawTextureDimensions(Texture2D& inputtedTexture, Vector2& drawLocation)
{
	string textWidth  = "width:  ";
	string textHeight = "height: ";

	textWidth  += to_string(inputtedTexture.width);
	textHeight += to_string(inputtedTexture.height);

	int fontSize = 20;

	int stringWidth = MeasureText(textWidth.c_str(),fontSize);
	stringWidth = (MeasureText(textHeight.c_str(),fontSize) > stringWidth) * MeasureText(textHeight.c_str(),fontSize);

	// drawing a background so the dimension text stands out
	DrawRectangle(drawLocation.x + 0,drawLocation.y + 0,stringWidth + 20,(fontSize * 2) + 20,BLACK);
	DrawRectangle(drawLocation.x + 5,drawLocation.y + 5,stringWidth + 10,(fontSize * 2) + 10,WHITE);

	// drawing the dimensions to the screen
	DrawText(textWidth.c_str() ,drawLocation.x + 10,drawLocation.y + 10           ,fontSize,BLACK);
	DrawText(textHeight.c_str(),drawLocation.x + 10,drawLocation.y + 10 + fontSize,fontSize,BLACK);
}



void drawTextureTitle(string& imageName)
{
	//cout << imageName << endl;
	int fontSize(32);
	float textWidth = MeasureText(imageName.c_str(),fontSize);
	int recPadding = 5;
	int recBorderSize = 5;

	// drawing our background to make the text stand out
	DrawRectangle((SCREEN_WIDTH/2) - ((textWidth + (2 * (recPadding + recBorderSize)))/2),0            ,textWidth + (2 * (recPadding + recBorderSize)),fontSize + (2 * (recPadding + recBorderSize)),BLACK);
	DrawRectangle((SCREEN_WIDTH/2) - ((textWidth + (2 * recPadding))/2)                  ,recBorderSize,textWidth + (2 * recPadding)                  ,fontSize + (2 * recPadding)                  ,WHITE);

	DrawText(imageName.c_str(),(SCREEN_WIDTH/2) - (textWidth/2),recBorderSize + recPadding,fontSize,BLACK);
}

int main(int argc, const char** argv)
{
	string imagePath = "../assets/City.png";

	// verifying the inputted CLA ( command - line - arguments )
	if(argc > 2)
	{
		string argv1 = argv[1];
		string argv2 = argv[2];

		checkCLA(argv1, argv2,imagePath);
	}

	// starting up our window
	InitWindow(SCREEN_WIDTH,SCREEN_HEIGHT,"draw test image");

	// loading the image
	Image testImage = LoadImage(imagePath.c_str());
	Texture2D testTexture = LoadTextureFromImage(testImage);
	//Texture2D testTexture = LoadTextureFromImage(testImage);



	//////////////////////////////////////////////////////////////////////////
	// calculating the scaled up ratio of the image that fits in the screen //
	//////////////////////////////////////////////////////////////////////////
	Vector2 imageRes{(float)testTexture.width,(float)testTexture.height}; // <-- setting this incase I decide to change the name to something more formal for the texture variable name

	// calculating the size of the biggest square the texture can be before it clips / goes over the edge of the screen
	int imageWidth = ((SCREEN_WIDTH <= SCREEN_HEIGHT) * SCREEN_WIDTH) + ((SCREEN_WIDTH > SCREEN_HEIGHT) * SCREEN_HEIGHT);
	
	// calculating the ratio each side of the texture is scaled to fill the screen
	Vector2 scaleRatio{(float)imageWidth / imageRes.x,(float)imageWidth / imageRes.y};
	//cout << scaleRatio.x << ", " << scaleRatio.y << endl;

	float screenScale = ((scaleRatio.x <= scaleRatio.y) * scaleRatio.x) + ((scaleRatio.x > scaleRatio.y) * scaleRatio.y);

	// multiplying by 0.95 to allow a gap between the image and the border
	screenScale *= 0.95f; 
	screenScale = (int)screenScale;

	// setting the offset of the texture from the middle of the screen
	Vector2 imageOffset{((float)SCREEN_WIDTH - (float)(imageRes.x * screenScale))/2, ((float)SCREEN_HEIGHT - (float)(imageRes.y * screenScale))/2};



	// defining the rectangles that we will use to draw the texture to the screen
	Rectangle sourceImageRec{0,0,imageRes.x,imageRes.y};
	Rectangle screenRec{imageOffset.x,imageOffset.y,imageRes.x * screenScale, imageRes.y * screenScale};

	


	Vector2 dimensionDrawLocation{0,0};

	string imageName = GetFileName(imagePath.c_str());

	// starting our draw loop
	while(!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(BLACK);

		//drawCheckeredBackground(40, WHITE, BLACK);
		drawCheckeredBackground(SCREEN_WIDTH,SCREEN_HEIGHT,20, Color{255,200,200,100}, Color{0,0,0,100});

		//DrawTexturePro(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color tint);
		DrawTexturePro(testTexture, sourceImageRec, screenRec, Vector2{0,0}, 0.0f, WHITE);
		

		drawImageGrid(Vector2{screenRec.x,screenRec.y},Vector2{screenRec.width,screenRec.height},Vector2{imageRes.x,imageRes.y},RED);

		drawTextureDimensions(testTexture,dimensionDrawLocation);
		drawTextureTitle(imageName);

		EndDrawing();
	}

	// making sure we unload the images
	UnloadTexture(testTexture);
	UnloadImage(testImage);

	CloseWindow();


	return 0;
}
