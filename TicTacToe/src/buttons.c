#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <buttons.h>
#include <logic.h>

#define NELEMS(x) (sizeof(x) / sizeof((x)[0]))
static Button buttons[5];
static bool shouldShowGameOverMessage = true;

void IndicateNewGameAction(ButtonEvent *event);
void IndicateQuitAction(ButtonEvent *event);
void IndicateNoAction(ButtonEvent *event);
void HandleShowBoardClick();

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

    // Show Board
    Button showBoard = { {0, 0, btnWidth, btnHeight}, {0,0,0,0}, false, false, "SHOW BOARD"};
    buttons[2] = showBoard;

    Button mode = { {0, 0, btnWidth, btnHeight}, {0,0,0,0}, false, false, "SINGLE PLAYER"};
    buttons[3] = mode;

    Button level = { {0, 0, btnWidth, btnHeight}, {0,0,0,0}, false, false, "EASY"};
    buttons[4] = level;

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

void HandleButtonEvent(SDL_Event* event, ButtonEvent *btnEvent)
{
    if (event->type == SDL_EVENT_WINDOW_MOUSE_LEAVE || event->type == SDL_EVENT_WINDOW_FOCUS_LOST) {
        for(int i = 0; i < NELEMS(buttons); i++) {
            buttons[i].isHovered = false;
            buttons[i].isPressed = false;
        }
        
        IndicateNoAction(btnEvent);
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
                            btn->isPressed = false;
                            IndicateNewGameAction(btnEvent);
                            return;
                        }
                        if (SDL_strcmp(btn->label, "QUIT") == 0)
                        {
                            btn->isPressed = false;
                            IndicateQuitAction(btnEvent);
                            return;
                        }
                        if(SDL_strcmp(btn->label, "SHOW BOARD") == 0)
                        {
                            btn->isPressed = false;
                            IndicateNoAction(btnEvent);
                            HandleShowBoardClick();
                            return;
                        }
                        if(SDL_strcmp(btn->label, "SINGLE PLAYER") == 0)
                        {
                            btn->isPressed = false;
                            IndicateNewGameAction(btnEvent);
                            btn->label = "2 PLAYERS";
                            SetIsTwoPlayerMode(false);
                            return;
                        }
                        if(SDL_strcmp(btn->label, "2 PLAYERS") == 0)
                        {
                            btn->isPressed = false;
                            IndicateNewGameAction(btnEvent);
                            btn->label = "SINGLE PLAYER";
                            SetIsTwoPlayerMode(true);
                            return;
                        }
                        if(SDL_strcmp(btn->label, "EASY") == 0)
                        {
                            btn->isPressed = false;
                            IndicateNewGameAction(btnEvent);
                            btn->label = "MEDIUM";
                            SetIsTwoPlayerMode(false);
                            SetAiLevel();
                            return;
                        }
                        if(SDL_strcmp(btn->label, "MEDIUM") == 0)
                        {
                            btn->isPressed = false;
                            IndicateNewGameAction(btnEvent);
                            btn->label = "HARD";
                            SetIsTwoPlayerMode(false);
                            SetAiLevel();
                            return;
                        }
                        if(SDL_strcmp(btn->label, "HARD") == 0)
                        {
                            btn->isPressed = false;
                            IndicateNewGameAction(btnEvent);
                            btn->label = "EASY";
                            SetIsTwoPlayerMode(false);
                            SetAiLevel();
                            return;
                        }
                    }

                    btn->isPressed = false;
                }
            }

            IndicateNoAction(btnEvent);
    }
}

void IndicateNewGameAction(ButtonEvent *event)
{
    event->newGame = true;
    event->quit = false;
    event-> noAction = false;
}

void IndicateQuitAction(ButtonEvent *event)
{
    event->newGame = false;
    event->quit = true;
    event-> noAction = false;
}

void IndicateNoAction(ButtonEvent *event)
{
    event->newGame = false;
    event->quit = false;
    event-> noAction = true;
}

bool GetShouldShowGameOverMessage()
{
    return shouldShowGameOverMessage;
}

void HandleShowBoardClick()
{
    shouldShowGameOverMessage = !shouldShowGameOverMessage;
}