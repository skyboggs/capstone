#include <iostream>
#include "raylib.h"

using namespace std;

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 720

void drawCheckeredBackground(int squareWidth,Color c1, Color c2)
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


// verify that the CLI is inputted correctly
bool checkCLA(string& argv1,string& argv2,string& imageName)
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
				cout << "Loading default image instead: " << imageName << endl;
				cout << "!!!";
				cout << "\033[0m" << endl;
				cout << endl;
			}else
			{
				imageName = argv2;
				cout << "\033[32mValid input file: \033[0m" << imageName << endl;
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


int main(int argc, const char** argv)
{
	string imageName = "assets/City.png";

	// verifying the inputted CLA ( command - line - arguments )
	if(argc > 2)
	{
		string argv1 = argv[1];
		string argv2 = argv[2];

		checkCLA(argv1, argv2,imageName);
	}

	// starting up our window
	InitWindow(SCREEN_WIDTH,SCREEN_HEIGHT,"draw test image");

	// loading the image
	Image testImage = LoadImage(imageName.c_str());
	Texture2D testTexture = LoadTextureFromImage(testImage);



	//////////////////////////////////////////////////////////////////////////
	// calculating the scaled up ratio of the image that fits in the screen //
	//////////////////////////////////////////////////////////////////////////
	Vector2 imageRes{(float)testTexture.width,(float)testTexture.height}; // <-- setting this incase I decide to change the name to something more formal for the texture variable name

	// calculating the size of the biggest square the texture can be before it clips / goes over the edge of the screen
	int imageWidth = ((SCREEN_WIDTH <= SCREEN_HEIGHT) * SCREEN_WIDTH) + ((SCREEN_WIDTH > SCREEN_HEIGHT) * SCREEN_HEIGHT);
	
	// calculating the ratio each side of the texture is scaled to fill the screen
	Vector2 scaleRatio{(float)imageWidth / imageRes.x,(float)imageWidth / imageRes.y};

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

	// starting our draw loop
	while(!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(BLACK);

		//drawCheckeredBackground(40, WHITE, BLACK);
		drawCheckeredBackground(20, Color{255,200,200,100}, Color{0,0,0,100});

		//DrawTexturePro(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color tint);
		DrawTexturePro(testTexture, sourceImageRec, screenRec, Vector2{0,0}, 0.0f, WHITE);

		drawImageGrid(Vector2{screenRec.x,screenRec.y},Vector2{screenRec.width,screenRec.height},Vector2{imageRes.x,imageRes.y},RED);

		drawTextureDimensions(testTexture,dimensionDrawLocation);

		EndDrawing();
	}

	// making sure we unload the images
	UnloadTexture(testTexture);
	UnloadImage(testImage);

	CloseWindow();


	return 0;
}
