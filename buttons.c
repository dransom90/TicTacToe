#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <stdio.h>
#include <stdbool.h>
#include <buttons.h>

#define NELEMS(x) (sizeof(x) / sizeof((x)[0]))
static Button buttons[2];

void CreateButtons(SDL_Window *window)
{
    // totalWidth = (btnWidth * N) + (btnGap * (N - 1))
    int btnCount = NELEMS(buttons);
    float btnWidth = 180.0f;
    float btnHeight = 50.0f;
    float btnGap = 100.0f;
    float totalWidth = (btnWidth * btnCount) + (btnGap * (btnCount - 1));

    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    float topCenterX = w / 2.0f;
    float topCenterY = 0.0f;
    float startX = (w - totalWidth) / 2.0f;

    // New Game Button
    Button newGame = { {topCenterX, topCenterY, btnWidth, btnHeight}, {0,0,0,0}, false, false, "NEW GAME"};
    buttons[0] = newGame;

    // Quit
    Button quit = {{0,0,btnWidth, btnHeight}, {0,0,0,0}, false, false, "QUIT"};
    buttons[1] = quit;

    for(int i = 0; i < btnCount; i++)
    {
        buttons[i].rect.x = startX + (i * (btnWidth + btnGap));
        buttons[i].rect.y = 50.0f;
    }
}

void RenderButtons(SDL_Renderer* renderer)
{
    for(int i = 0; i < NELEMS(buttons); i++)
    {
        if(buttons[i].isPressed)
        {
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        }
        else if(buttons[i].isHovered)
        {
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
        }

        SDL_RenderFillRect(renderer, &buttons[i].rect);

        // Render label
        float textX = buttons[i].rect.x + (buttons[i].rect.w / 2.0f) - (strlen(buttons[i].label) * 8.0f / 2.0f);
        float textY = buttons[i].rect.y + (buttons[i].rect.h / 2.0f) - (8.0f / 2.0f);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        //TODO: Figure out a better way to do text.
        SDL_RenderDebugText(renderer, textX, textY, buttons[i].label);
    }
}

ButtonEvent HandleButtonEvent(SDL_Event* event)
{
    ButtonEvent btnEvent = {false, false, false};

    if(event->type == SDL_EVENT_MOUSE_MOTION || event->type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
        event->type == SDL_EVENT_MOUSE_BUTTON_UP)
    {
            float mX, mY;
            SDL_FPoint mousePos = {event->button.x, event->button.y};
            
            for(int i = 0; i < NELEMS(buttons); i++)
            {
                Button *btn = &buttons[i];

                btn->isHovered = SDL_PointInRectFloat(&mousePos, &btn->rect);

                if(btn->isHovered && event->type == SDL_EVENT_MOUSE_BUTTON_DOWN && event->button.button == SDL_BUTTON_LEFT)
                {
                    btn->isPressed = true;
                }

                if(event->type == SDL_EVENT_MOUSE_BUTTON_UP)
                {
                    if(btn->isPressed && btn->isHovered)
                    {
                        if(btn->label == "NEW GAME")
                        {
                            btnEvent.newGame = true;
                            return btnEvent;
                        }
                        if(btn->label == "QUIT")
                        {
                            btnEvent.quit = true;
                            return btnEvent;
                        }
                        // Handle Button Clicked Logic
                        // How do I clear the game state from this file?
                        // return a struct of results and let the calling function handle it?
                    }

                    btn->isPressed = false;
                }
            }

            btnEvent.noAction = true;
            return btnEvent;
    }
}