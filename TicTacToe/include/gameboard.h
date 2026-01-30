#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <SDL3/SDL.h>
#include <stdbool.h>

extern SDL_FRect rects[9];
extern SDL_FRect background;
extern int markers[9];

void InitGameboard();
void CreateRects(float logicalWidth, float logicalHeight);
void ShowGameBoard(SDL_Renderer *renderer);
void DrawGameMarkers(SDL_Renderer *renderer);
void ResetBoard();
bool AddMarker(int space, int marker);

#endif