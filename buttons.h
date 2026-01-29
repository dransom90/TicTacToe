#ifndef BUTTON_H
#define BUTTON_H

#include <SDL3/SDL.h>
#include <stdbool.h>

typedef struct 
{
    SDL_FRect rect;
    SDL_FColor color;
    bool isHovered;
    bool isPressed;
    char *label;
} Button;

typedef struct 
{
    bool newGame;
    bool quit;
    bool noAction;
} ButtonEvent;

void CreateButtons(SDL_Window *window);
void RenderButtons(SDL_Renderer* renderer);
ButtonEvent HandleButtonEvent(SDL_Event* event);

#endif