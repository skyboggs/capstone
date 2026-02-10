#include <iostream>
#include "raylib.h"

using namespace std;

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 720

#define IMAGE_PIXELS_X 9
#define IMAGE_PIXELS_Y 9

void drawCheckeredBackground(int squareWidth,Color c1, Color c2)
{
	Vector2 totalSquares{(float)SCREEN_WIDTH / (float)squareWidth, (float)SCREEN_HEIGHT / (float)squareWidth};

	//cout << "calculated squares: {" << totalSquares.x << ", " << totalSquares.y << "}";

	int squareCountX(totalSquares.x + 1);
	int squareCountY(totalSquares.y + 1);

	//cout << "squareCount: {" << squareCountX << ", " << squareCountY << "}";

	for(int i=0;i<squareCountY;++i)
	{
		for(int m=0;m<squareCountX;++m)
		{
			DrawRectangle(squareWidth * m,squareWidth * i,squareWidth,squareWidth,(i + m) % 2 == 0 ? c1 : c2);
		}
	}
}

void drawImageGrid(Vector2 origin,Vector2 dim,Vector2 pixelCount,Color gridColor)
{
	Vector2 gapSize{dim.y/pixelCount.y,dim.x/pixelCount.x};

	for(int i=0;i<=pixelCount.y;++i)
	{
		DrawLine(origin.x,origin.y + ((float)i * gapSize.y),origin.x + dim.x,origin.y + ((float)i * gapSize.y),gridColor);
	}

	for(int m=0;m<=pixelCount.x;++m)
	{
		DrawLine(origin.x + ((float)m * gapSize.x),origin.y,origin.x + ((float)m * gapSize.x),origin.y + dim.x,gridColor);
	}
}



int main(int argc, const char** argv)
{
	string imageName = "assets/City.png";

	if(argc > 2)
	{
		cout << "CLA [1]: " << argv[1] << ", [2]: " << argv[2] << endl;

		string commandLineArg = argv[1];

		// checking if the user wants to set a custom image
		if(commandLineArg == "--image")
		{
			imageName = argv[2];
		}
	}

	// starting up our window
	InitWindow(SCREEN_WIDTH,SCREEN_HEIGHT,"draw test image");

	// loading the image
	//Image testImage = LoadImage("assets/City.png");
	Image testImage = LoadImage(imageName.c_str());
	Texture2D testTexture = LoadTextureFromImage(testImage);

	Vector2 imageRes{(float)testTexture.width,(float)testTexture.height};
	//cout << "texture dimensions: " << testTexture.width << ", " << testTexture.height << endl;

	// defining the size of the image so we can easily draw it later
	Rectangle sourceImageRec{0,0,imageRes.x,imageRes.y};

	int imageWidth = ((SCREEN_WIDTH < SCREEN_HEIGHT) * SCREEN_WIDTH) + ((SCREEN_WIDTH > SCREEN_HEIGHT) * SCREEN_HEIGHT);

	Vector2 imageOffset{((float)SCREEN_WIDTH - (float)imageWidth)/2, ((float)SCREEN_HEIGHT - (float)imageWidth)/2};

	//Rectangle screenRec{imageOffset.x,imageOffset.y,(float)imageWidth,(float)imageWidth};
	Rectangle screenRec{imageOffset.x + 20,imageOffset.y + 20,(float)imageWidth-50,(float)imageWidth-50};
	



	// starting our draw loop
	while(!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(RAYWHITE);

		DrawRectangle(0,0,100,100,RED);
		//drawCheckeredBackground(40, WHITE, BLACK);
		drawCheckeredBackground(40, Color{255,200,200,255}, BLACK);

		//DrawTexturePro(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color tint);
		DrawTexturePro(testTexture, sourceImageRec, screenRec, Vector2{0,0}, 0.0f, WHITE);
		drawImageGrid(Vector2{screenRec.x,screenRec.y},Vector2{screenRec.width,screenRec.height},Vector2{imageRes.x,imageRes.y},RED);

		EndDrawing();
	}

	// making sure we unload the images
	UnloadTexture(testTexture);
	UnloadImage(testImage);

	CloseWindow();


	return 0;
}
