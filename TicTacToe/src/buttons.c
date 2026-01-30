#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
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
            SDL_SetRenderDrawColor(renderer, 0, 123, 255, 255);
        }
        else if(buttons[i].isHovered)
        {
            SDL_SetRenderDrawColor(renderer, 0, 105, 217, 255);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 0, 86, 179, 255);
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

    if (event->type == SDL_EVENT_WINDOW_MOUSE_LEAVE || event->type == SDL_EVENT_WINDOW_FOCUS_LOST) {
        for(int i = 0; i < NELEMS(buttons); i++) {
            buttons[i].isHovered = false;
            buttons[i].isPressed = false;
        }
    }

    if(event->type == SDL_EVENT_MOUSE_MOTION || event->type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
        event->type == SDL_EVENT_MOUSE_BUTTON_UP)
    {
            float mX, mY;
            SDL_FPoint mousePos;
            if (event->type == SDL_EVENT_MOUSE_MOTION) 
            {
                mousePos.x = event->motion.x;
                mousePos.y = event->motion.y;
            } 
            else 
            {
                mousePos.x = event->button.x;
                mousePos.y = event->button.y;
            }
            
            for(int i = 0; i < NELEMS(buttons); i++)
            {
                Button *btn = &buttons[i];

                btn->isHovered = SDL_PointInRectFloat(&mousePos, &btn->rect);

                if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN && event->button.button == SDL_BUTTON_LEFT) 
                {
                    if (btn->isHovered) btn->isPressed = true;
                }

                if(event->type == SDL_EVENT_MOUSE_BUTTON_UP && event->button.button == SDL_BUTTON_LEFT)
                {
                    if(btn->isPressed && btn->isHovered)
                    {
                        if (SDL_strcmp(btn->label, "NEW GAME") == 0)
                        {
                            printf("New Game press detected\n");
                            btnEvent.newGame = true;
                            btn->isPressed = false;
                            return btnEvent;
                        }
                        if (SDL_strcmp(btn->label, "QUIT") == 0)
                        {
                            btnEvent.quit = true;
                            btn->isPressed = false;
                            return btnEvent;
                        } 
                            
                    }

                    btn->isPressed = false;
                }
            }

            btnEvent.noAction = true;
            return btnEvent;
    }
}