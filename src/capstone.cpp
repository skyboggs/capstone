#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <memory>
#include <filesystem>
#include "raylib.h"
#include "tablet.h"
#include "textureMapping.h"
#include "visUtils.h"
#include "visualizerSettings.h"

// ── Fixed panel sizes — do not scale with the window ─────────────────────────
constexpr int TABLET_DISPLAY_WIDTH = 600;
constexpr int VIS_PANEL_WIDTH      = 800;
constexpr int VIS_PANEL_MARGIN     = 20;
// ─────────────────────────────────────────────────────────────────────────────

using namespace std;
namespace fs = filesystem;

int main(int argc, const char** argv)
{
  string imagePath = "../assets/Flowers.png";
  int    tileDim   = 3;

  vector<string> commandStack;

  for(int i=1;i<argc;++i)
  {
    commandStack.push_back(argv[i]);
    cout << "pushed: " << commandStack[commandStack.size()-1] << endl;
  }

  for(int i=0;i<(int)commandStack.size();++i)
  {
    int remainingArgs = (commandStack.size() - i - 1);

    cout << "remaining args: " << (remainingArgs) << endl;
    cout << "[" << i << "]: '" << commandStack[i] << "'" << endl;

    if(commandStack[i].substr(0,2) == "--")
    {
      if(commandStack[i] == "--image")
      {
        if(remainingArgs >= 1)
        {
          checkCLA(commandStack[i], commandStack[i + 1], imagePath);
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
            tileDim = stoi(commandStack[i + 1]);
            ++i;
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

  unsigned int currentSeed = (unsigned int)time(NULL);
  srand(currentSeed);

  // Dynamic window dimensions — updated on fullscreen toggle
  int winW = 1920;
  int winH = 800;

  InitWindow(winW, winH, "capstone presentation");
  SetTargetFPS(60);

  // ── Background shaders ──────────────────────────────────────────────────────
  const char* BG_SHADER_PATHS[3] = {
    "../shaders/bg_grid.fs",
    "../shaders/bg_crt.fs",
    "../shaders/bg_plasma.fs"
    //"../shaders/try1.fs"
    //"../shaders/try2.fs"
  };
  Shader bgShaders[3];
  int    bgTimeLocs[3];
  int    bgResLocs[3];
  for(int i = 0; i < 3; ++i)
  {
    bgShaders[i]  = LoadShader(0, BG_SHADER_PATHS[i]);
    bgTimeLocs[i] = GetShaderLocation(bgShaders[i], "time");
    bgResLocs[i]  = GetShaderLocation(bgShaders[i], "resolution");
  }
  int bgMode = 0; // 0=grid  1=crt  2=plasma  (cycle with B)
  // ────────────────────────────────────────────────────────────────────────────

  Shader glowShader  = LoadShader(0, "../shaders/rect_glow.fs");
  int glowTimeLoc    = GetShaderLocation(glowShader, "time");
  int glowResLoc     = GetShaderLocation(glowShader, "resolution");
  int glowBoundsLoc  = GetShaderLocation(glowShader, "rectBounds");
  int glowColorLoc   = GetShaderLocation(glowShader, "glowColor");
  int glowRadiusLoc  = GetShaderLocation(glowShader, "glowRadius");

  {
  // ── Tablet geometry ─────────────────────────────────────────────────────────
  Vector2 tabletDims        { 50.0f, 35.0f };
  float   tabletCellSize    = TABLET_DISPLAY_WIDTH / tabletDims.x;
  float   tabletDisplayHeight = tabletCellSize * tabletDims.y;
  // These are updated by updateLayout() on fullscreen toggle
  float   tabletTopY        = (winH - tabletDisplayHeight) / 2.0f;
  int     tabletLeftMargin  = (winW - VIS_PANEL_WIDTH - TABLET_DISPLAY_WIDTH) / 2;
  int     visPanelX         = winW - VIS_PANEL_WIDTH;
  Vector2 tabletScreenDims  { (float)TABLET_DISPLAY_WIDTH, tabletDisplayHeight };
  Vector2 visDims           { (float)(VIS_PANEL_WIDTH  - 2 * VIS_PANEL_MARGIN),
                              (float)(winH - 2 * VIS_PANEL_MARGIN) };
  Vector2 visOffset         { (float)(visPanelX + VIS_PANEL_MARGIN),
                              (float)VIS_PANEL_MARGIN };
  // ────────────────────────────────────────────────────────────────────────────

  // Scan ../assets/ and pre-load a textureMapping for each image
  vector<string> assetPaths;
  for(auto& entry : fs::directory_iterator("../assets/"))
  {
    if(entry.is_regular_file())
      assetPaths.push_back(entry.path().string());
  }
  sort(assetPaths.begin(), assetPaths.end());

  vector<unique_ptr<textureMapping>> presentations;
  for(auto& path : assetPaths)
  {
    auto tm = make_unique<textureMapping>(path, tileDim);
    generateTileRecs(*tm, visDims);
    presentations.push_back(move(tm));
  }

  tablet t(tabletDims, tabletScreenDims);

  textureMapping genDetails(imagePath, tileDim);
  genDetails.genOverlappingX = true;
  generateTileRecs(genDetails, visDims);

  t.reset(genDetails);
  t.updateTexture();

  visualizerSettings visConfig;

  // -1,-1 means "none"
  Vector2 hoveredCell    { -1, -1 };
  Vector2 selectedCell   { -1, -1 };
  Vector2 lastShownCell  { -1, -1 };
  bool    xrayMode          = false;
  bool    isFullscreen      = false;
  bool    roughColorEnabled = false;

  int          presIndex  = 0;
  int          loopNumber = 0;
  unsigned int baseSeed   = 0;

  // ── Helpers ─────────────────────────────────────────────────────────────────

  // Draws the active background shader as a fullscreen rect.
  auto drawBackground = [&]() {
    float t      = (float)GetTime();
    int   rw     = GetRenderWidth();
    int   rh     = GetRenderHeight();
    float res[2] = { (float)rw, (float)rh };
    SetShaderValue(bgShaders[bgMode], bgTimeLocs[bgMode], &t,  SHADER_UNIFORM_FLOAT);
    SetShaderValue(bgShaders[bgMode], bgResLocs[bgMode],  res, SHADER_UNIFORM_VEC2);
    BeginShaderMode(bgShaders[bgMode]);
      DrawRectangle(0, 0, rw, rh, WHITE);
    EndShaderMode();
  };

  // Returns the tablet cell (col, row) under the mouse, or {-1,-1} if outside.
  auto getHoveredCell = [&]() -> Vector2 {
    Vector2 mouse  = GetMousePosition();
    float   localX = mouse.x - tabletLeftMargin;
    float   localY = mouse.y - tabletTopY;
    if(localX < 0 || localY < 0) return {-1, -1};
    int col = (int)(localX / t.pixelDims.x);
    int row = (int)(localY / t.pixelDims.y);
    if(col >= 0 && col < (int)tabletDims.x && row >= 0 && row < (int)tabletDims.y)
      return {(float)col, (float)row};
    return {-1, -1};
  };

  auto drawTablet = [&]() {
    DrawTexturePro(
      t.tabletScreen.texture,
      Rectangle{0.0f, 0.0f, (float)TABLET_DISPLAY_WIDTH, -tabletDisplayHeight},
      Rectangle{(float)tabletLeftMargin, tabletTopY, (float)TABLET_DISPLAY_WIDTH, tabletDisplayHeight},
      Vector2{0.0f, 0.0f},
      0.0f,
      WHITE
    );
  };


  // Draws the visualizer panel: hovered cell takes priority over selected cell.
  // When useFallback is true and neither is valid, shows the last cell that was
  // actually rendered (lastShownCell). lastShownCell is updated here on every draw.
  auto drawVisualizer = [&](const textureMapping& texMap, bool useFallback = false) {
    Vector2 displayCell = (hoveredCell.x >= 0) ? hoveredCell : selectedCell;
    if(displayCell.x < 0 && useFallback) displayCell = lastShownCell;
    if(displayCell.x >= 0)
    {
      lastShownCell = displayCell;
      const cell& c = t.tabletPixels[(int)displayCell.y][(int)displayCell.x];
      int selectedIdx = -1;
      if(c.isSelected)
      {
        for(int i = 0; i < (int)c.possibleTiles.size(); ++i)
          if(c.possibleTiles[i]) { selectedIdx = i; break; }
      }
      drawTileCompatabilities(visConfig, texMap, c.possibleTiles, visOffset, selectedIdx);
    }
    // no cell selected — shader background shows through
  };

  // Draws an orange border on the tablet cell currently shown in the visualizer.
  auto drawLastShownCellHighlight = [&]() {
    if(lastShownCell.x < 0) return;
    DrawRectangleLinesEx(
      Rectangle{
        (float)tabletLeftMargin + lastShownCell.x * t.pixelDims.x,
        tabletTopY              + lastShownCell.y * t.pixelDims.y,
        t.pixelDims.x,
        t.pixelDims.y
      },
      2.0f,
      ORANGE
    );
  };

  auto drawSeedOverlay = [&](unsigned int seed, int loop) {
    const char* text = TextFormat("Seed: %u  Loop: %d", seed, loop);
    int w = MeasureText(text, 20);
    DrawRectangle(5, 5, w + 10, 30, Color{0, 0, 0, 180});
    DrawText(text, 10, 10, 20, WHITE);
  };

  auto drawCountdownOverlay = [&](int sec) {
    const char* text = TextFormat("New image in %d", sec);
    int w = MeasureText(text, 30);
    int x = (winW - w) / 2;
    int y = winH - 60;
    DrawRectangle(x - 10, y - 5, w + 20, 40, Color{0, 0, 0, 180});
    DrawText(text, x, y, 30, WHITE);
  };

  auto drawFilenameOverlay = [](const string& path) {
    string filename = fs::path(path).filename().string();
    const char* text = TextFormat("File: %s", filename.c_str());
    int w = MeasureText(text, 20);
    DrawRectangle(5, 40, w + 10, 30, Color{0, 0, 0, 180});
    DrawText(text, 10, 45, 20, WHITE);
  };

  // Outlines the selected cell in the tablet with a yellow border.
  auto drawSelectedCellHighlight = [&]() {
    if(selectedCell.x < 0) return;
    DrawRectangleLinesEx(
      Rectangle{
        (float)tabletLeftMargin + selectedCell.x * t.pixelDims.x,
        tabletTopY              + selectedCell.y * t.pixelDims.y,
        t.pixelDims.x,
        t.pixelDims.y
      },
      2.0f,
      YELLOW
    );
  };

  // In x-ray mode, tints every unresolved cell with a semi-transparent overlay.
  auto drawSelectionOverlay = [&]() {
    if(!xrayMode) return;
    Color tint { 255, 0, 100, 80 };
    for(int row = 0; row < (int)tabletDims.y; ++row)
    {
      for(int col = 0; col < (int)tabletDims.x; ++col)
      {
        if(!t.tabletPixels[row][col].isSelected)
        {
          DrawRectangle(
            (int)(tabletLeftMargin + col * t.pixelDims.x),
            (int)(tabletTopY       + row * t.pixelDims.y),
            (int)t.pixelDims.x,
            (int)t.pixelDims.y,
            tint
          );
        }
      }
    }
  };

  // Recomputes all layout variables and regenerates tile recs for the new screen size.
  auto updateLayout = [&]() {
    winW             = GetScreenWidth();
    winH             = GetScreenHeight();
    tabletLeftMargin = (winW - VIS_PANEL_WIDTH - TABLET_DISPLAY_WIDTH) / 2;
    visPanelX        = winW - VIS_PANEL_WIDTH;
    tabletTopY       = (winH - tabletDisplayHeight) / 2.0f;
    visDims          = { (float)(VIS_PANEL_WIDTH - 2 * VIS_PANEL_MARGIN),
                         (float)(winH - 2 * VIS_PANEL_MARGIN) };
    visOffset        = { (float)(visPanelX + VIS_PANEL_MARGIN),
                         (float)VIS_PANEL_MARGIN };
    regenTileRecs(genDetails, visDims);
    for(auto& tm : presentations)
      regenTileRecs(*tm, visDims);
    float res[2] = { (float)GetRenderWidth(), (float)GetRenderHeight() };
    for(int i = 0; i < 3; ++i)
      SetShaderValue(bgShaders[i], bgResLocs[i], res, SHADER_UNIFORM_VEC2);
  };

  // ────────────────────────────────────────────────────────────────────────────

  while(!WindowShouldClose())
  {
    hoveredCell = getHoveredCell();
    updateConfig(visConfig);
    if(IsKeyPressed(KEY_X)) { xrayMode = !xrayMode; }
    if(IsKeyPressed(KEY_B)) { bgMode = (bgMode + 1) % 3; }
    if(IsKeyPressed(KEY_V))
    {
      roughColorEnabled = !roughColorEnabled;
      for(int row = 0; row < (int)tabletDims.y; ++row)
        for(int col = 0; col < (int)tabletDims.x; ++col)
        {
          cell& c = t.tabletPixels[row][col];
          c.generateRoughColor = roughColorEnabled;
          //if(roughColorEnabled && !c.isSelected)
          if(!c.isSelected)
            c.updateRoughColor(genDetails);
        }
      t.updateTexture();
    }

    if(IsKeyPressed(KEY_F))
    {
      ToggleFullscreen();
      isFullscreen = !isFullscreen;
      updateLayout();
    }

    // ── Click handling (idle only) ────────────────────────────────────────────
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
      Vector2 mouse = GetMousePosition();

      if(hoveredCell.x >= 0)
      {
        // Click on tablet: select cell if not already collapsed
        if(!t.tabletPixels[(int)hoveredCell.y][(int)hoveredCell.x].isSelected)
          selectedCell = hoveredCell;
      }
      else if(mouse.x >= visPanelX && selectedCell.x >= 0)
      {
        // Click on visualizer: collapse selected cell to the clicked tile
        cell& sc = t.tabletPixels[(int)selectedCell.y][(int)selectedCell.x];
        if(!sc.isSelected)
        {
          float localX = mouse.x - visOffset.x;
          float localY = mouse.y - visOffset.y;
          const vector<Rectangle>& dRecs = genDetails.destRecs;
          for(int i = 0; i < (int)dRecs.size(); ++i)
          {
            if(CheckCollisionPointRec({localX, localY}, dRecs[i]))
            {
              if(sc.possibleTiles[i])
              {
                sc.pickSpecificTile(i, genDetails);
                t.generateCell(selectedCell, genDetails);
                t.updateTexture();
              }
              break;
            }
          }
        }
      }
    }
    // ─────────────────────────────────────────────────────────────────────────

    if(IsKeyPressed(KEY_G))
    {
      bool cancelled = false;
      int stepCount = 0;
      while(!cancelled && t.step(genDetails))
      {
        ++stepCount;
        if(stepCount % 10 == 0)
        {
          t.updateTexture();
          hoveredCell = getHoveredCell();
          BeginDrawing();
            ClearBackground(Color{10, 4, 20, 255});
            drawBackground();
            drawTablet();
            drawSelectedCellHighlight();
            drawSelectionOverlay();
            drawSeedOverlay(currentSeed, 0);
            drawVisualizer(genDetails);
            drawLastShownCellHighlight();
          EndDrawing();
          if(IsKeyPressed(KEY_C)) { cancelled = true; }
          if(IsKeyPressed(KEY_SPACE))
          {
            while(!WindowShouldClose())
            {
              hoveredCell = getHoveredCell();
              if(IsKeyPressed(KEY_X)) { xrayMode = !xrayMode; }
              BeginDrawing();
                ClearBackground(Color{10, 4, 20, 255});
            drawBackground();
                drawTablet();
                drawSelectedCellHighlight();
                drawSelectionOverlay();
                drawSeedOverlay(currentSeed, 0);
                drawVisualizer(genDetails);
                drawLastShownCellHighlight();
              EndDrawing();
              if(IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_G)) { break; }
              if(IsKeyPressed(KEY_C)) { cancelled = true; break; }
            }
          }
        }
      }
      if(cancelled)
      {
        srand(currentSeed);
        t.reset(genDetails);
        t.updateTexture();
        selectedCell = {-1, -1};
        hoveredCell  = getHoveredCell();
        BeginDrawing();
          ClearBackground(Color{10, 4, 20, 255});
          drawBackground();
          drawTablet();
          drawSelectedCellHighlight();
          drawSelectionOverlay();
          drawSeedOverlay(currentSeed, 0);
          drawVisualizer(genDetails);
          drawLastShownCellHighlight();
        EndDrawing();
      }
    }

    if(IsKeyPressed(KEY_P) && !presentations.empty())
    {
      presIndex    = 0;
      loopNumber   = 0;
      baseSeed     = (unsigned int)time(NULL);
      selectedCell = {-1, -1};
      cout << "Entering presentation mode." << endl;

      while(!WindowShouldClose())
      {
        srand(baseSeed + (unsigned int)loopNumber);
        t.reset(*presentations[presIndex]);
        t.updateTexture();
        lastShownCell = {-1, -1};

        // Generation loop
        bool genSkipped = false;
        int stepCount = 0;
        while(!genSkipped && !WindowShouldClose() && t.step(*presentations[presIndex]))
        {
          ++stepCount;
          if(stepCount % 10 == 0)
          {
            t.updateTexture();
            hoveredCell = getHoveredCell();
            BeginDrawing();
              ClearBackground(Color{10, 4, 20, 255});
            drawBackground();
              drawTablet();
              drawSelectedCellHighlight();
              drawSelectionOverlay();
              drawSeedOverlay(baseSeed, loopNumber);
              drawFilenameOverlay(assetPaths[presIndex]);
              drawVisualizer(*presentations[presIndex], true);
              drawLastShownCellHighlight();
            EndDrawing();

            if(IsKeyPressed(KEY_P))  { genSkipped = true; goto exitPresentation; }
            if(IsKeyPressed(KEY_LEFT))
            {
              --presIndex;
              if(presIndex < 0) { presIndex = (int)presentations.size() - 1; --loopNumber; }
              genSkipped = true;
            }
            if(IsKeyPressed(KEY_RIGHT))
            {
              ++presIndex;
              if(presIndex >= (int)presentations.size()) { presIndex = 0; ++loopNumber; }
              genSkipped = true;
            }
          }
        }

        if(genSkipped) continue;

        // Countdown phase
        int nextIndex = (presIndex + 1) % (int)presentations.size();
        bool countdownSkipped = false;

        for(int sec = 10; sec >= 1 && !countdownSkipped; --sec)
        {
          cout << "New image in " << sec << endl;
          double startTime = GetTime();
          while(GetTime() - startTime < 1.0 && !countdownSkipped && !WindowShouldClose())
          {
            t.updateTexture();
            hoveredCell = getHoveredCell();
            BeginDrawing();
              ClearBackground(Color{10, 4, 20, 255});
            drawBackground();
              drawTablet();
              drawSelectedCellHighlight();
              drawSelectionOverlay();
              drawSeedOverlay(baseSeed, loopNumber);
              drawFilenameOverlay(assetPaths[presIndex]);
              drawCountdownOverlay(sec);
              drawVisualizer(*presentations[presIndex], true);
              drawLastShownCellHighlight();
            EndDrawing();

            if(IsKeyPressed(KEY_P))  { goto exitPresentation; }
            if(IsKeyPressed(KEY_LEFT))
            {
              --presIndex;
              if(presIndex < 0) { presIndex = (int)presentations.size() - 1; --loopNumber; }
              countdownSkipped = true;
            }
            if(IsKeyPressed(KEY_RIGHT))
            {
              ++presIndex;
              if(presIndex >= (int)presentations.size()) { presIndex = 0; ++loopNumber; }
              countdownSkipped = true;
            }
          }
        }

        if(countdownSkipped) continue;

        // Natural advance
        presIndex = nextIndex;
        if(presIndex == 0) ++loopNumber;
        cout << "Loading " << assetPaths[presIndex] << endl;
      }

      exitPresentation:
      cout << "Exiting presentation mode." << endl;
      srand(currentSeed);
      t.reset(genDetails);
      t.updateTexture();
      selectedCell = {-1, -1};
    }

    if(IsKeyPressed(KEY_R))
    {
      if(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
      {
        srand(currentSeed);
      }
      else
      {
        currentSeed = (unsigned int)time(NULL);
        srand(currentSeed);
      }
      t.reset(genDetails);
      t.updateTexture();
      selectedCell = {-1, -1};
      BeginDrawing();
        ClearBackground(Color{10, 4, 20, 255});
        drawBackground();
        drawTablet();
        drawSelectedCellHighlight();
        drawSelectionOverlay();
        drawSeedOverlay(currentSeed, 0);
        drawVisualizer(genDetails);
        drawLastShownCellHighlight();
      EndDrawing();
    }

    if(IsKeyDown(KEY_S) && (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)))
    {
      t.step(genDetails);
    }
    else if(IsKeyPressed(KEY_S))
    {
      t.step(genDetails);
    }

    t.updateTexture();

    BeginDrawing();
      ClearBackground(Color{10, 4, 20, 255});
      drawBackground();
      drawTablet();
      drawSelectedCellHighlight();
      drawSelectionOverlay();
      drawSeedOverlay(currentSeed, 0);
      drawVisualizer(genDetails);
      drawLastShownCellHighlight();
    EndDrawing();
  }

  UnloadRenderTexture(t.tabletScreen);
  } // t, genDetails, and presentations destroyed here, before CloseWindow

  UnloadShader(glowShader);
  for(int i = 0; i < 3; ++i) UnloadShader(bgShaders[i]);
  CloseWindow();
  return 0;
}
