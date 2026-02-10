#include <iostream>
#include "raylib.h"

using namespace std;

#define SCREEN_WIDTH 1000.0f
#define SCREEN_HEIGHT 600.0f

int main()
{
	InitWindow(SCREEN_WIDTH,SCREEN_HEIGHT,"window in testApp!");

	int squareWidth(20);
	int cellCount(10);
	int gridWidth = squareWidth * cellCount;

	while(!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(WHITE);

		if(IsKeyDown(KEY_SPACE))
		{
			squareWidth = 30;
			gridWidth = squareWidth * cellCount;

			DrawText("SPACE DOWN",20,20,40,BLACK);
		}else
		{
			squareWidth = 20;
			gridWidth = squareWidth * cellCount;
			//DrawText("SPACE UP",20,20,40,BLACK);
		}


		int gridX = (SCREEN_WIDTH  / 2.0f) - ((cellCount * squareWidth) / 2.0f);
		int gridY = (SCREEN_HEIGHT / 2.0f) - ((cellCount * squareWidth) / 2.0f);

		int outlineThickness(5);

		DrawRectangle(gridX-outlineThickness,gridY-outlineThickness,(cellCount * squareWidth) + (outlineThickness * 2),(cellCount * squareWidth) + (outlineThickness * 2),BLACK);
		DrawRectangle(gridX,gridY,cellCount * squareWidth,cellCount * squareWidth,WHITE);

		for(int i=0;i<cellCount;++i)
		{
			for(int m=0;m<cellCount;++m)
			{
				int currentXCoord = ((SCREEN_WIDTH  / 2.0f) + (i * squareWidth)) - ((cellCount * squareWidth) / 2.0f);
				int currentYCoord = ((SCREEN_HEIGHT / 2.0f) + (m * squareWidth)) - ((cellCount * squareWidth) / 2.0f);

				if((i + m) % 2 == 0)
				{
					DrawRectangle(currentXCoord,currentYCoord,squareWidth,squareWidth,RED);
				}
			}
		}

		Vector2 mousePos = GetMousePosition();

		DrawLine(mousePos.x,mousePos.y,gridX            ,gridY            ,BLACK);
		DrawLine(mousePos.x,mousePos.y,gridX + gridWidth,gridY            ,BLACK);
		DrawLine(mousePos.x,mousePos.y,gridX            ,gridY + gridWidth,BLACK);
		DrawLine(mousePos.x,mousePos.y,gridX + gridWidth,gridY + gridWidth,BLACK);

		EndDrawing();

	}

	CloseWindow();


	return 0;
}
