#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_rect.h>
#include <stdio.h>
#include <stdbool.h>
#include <buttons.h>
#include <gameboard.h>
#include <logic.h>
#include <scoreboard.h>
#include <string.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

float LOGICAL_W, LOGICAL_H;

SDL_AppResult HandleButtnEventResult(ButtonEvent *btnEvent)
{
    if(btnEvent->quit)
    {
        return SDL_APP_SUCCESS;
    }
    if(btnEvent->newGame)
    {
        Reset();
        ResetBoard();
        
        return SDL_APP_CONTINUE;
    }
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    int num_displays;
    SDL_DisplayID* displays = SDL_GetDisplays(&num_displays);
    if(!displays) return -1;
    SDL_DisplayID primary = displays[0];

    SDL_Rect bounds;
    SDL_GetDisplayUsableBounds(primary, &bounds);

    float width = bounds.w / 2;
    float height = bounds.h / 2;
    LOGICAL_W = bounds.w / 2;
    LOGICAL_H = bounds.h / 2;

    /* Create the window */
    if (!SDL_CreateWindowAndRenderer("Tic Tac Toe", width, height, SDL_WINDOW_MAXIMIZED, &window, &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetRenderLogicalPresentation(renderer, (int)LOGICAL_W, (int)LOGICAL_H, SDL_SCALEMODE_LINEAR);

    LoadScoreboard();
    CreateRects(LOGICAL_W, LOGICAL_H);
    CreateButtons(window);
    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{   ButtonEvent btnEvent = {false, false, false};
    SDL_ConvertEventToRenderCoordinates(renderer, event);
    HandleButtonEvent(event, &btnEvent);
    
    if(HandleButtnEventResult(&btnEvent) == SDL_APP_SUCCESS)
        return SDL_APP_SUCCESS;

    if(event->type == SDL_EVENT_MOUSE_BUTTON_DOWN && event->button.button == SDL_BUTTON_LEFT)
    {
        bool mode = GetIsTwoPlayerMode();
        if(GetIsTwoPlayerMode() || (!GetIsTwoPlayerMode() && turn == 0))
        {
            SDL_FPoint mousePoint = { event->button.x, (int)event->button.y};
            for(int i = 0; i < 9; i++)
            {
                if(SDL_PointInRectFloat(&mousePoint, &rects[i]))
                {
                    int marker = turn == 0 ? 1 : 2;
                    bool success = AddMarker(i, marker);
                    if(success)
                        EndCurrentTurn();
                }
            }
        }
    }

     if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

void ShowGameOverMessage(const char *message)
{
    int w = 0, h = 0;
    const float scale = 4.0f;

    /* Center the message and scale it up */
    SDL_GetRenderOutputSize(renderer, &w, &h);
    SDL_SetRenderScale(renderer, scale, scale);
    int x = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(message)) / 2;
    int y = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 2;

    /* Draw the message */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDebugText(renderer, x, y, message);
    SDL_SetRenderScale(renderer, 1.0f, 1.0f);
}

void ShowGameOverState()
{
    const char *message = " ";
    GameResult *result = GetGameResult();
        switch(result->type)
        {
            case X:
                message = "X Wins!";
                break;
            case O:
                message = "O Wins!";
                break;
            case TIE:
                message = "Tie :(";
                break;
            default:
                break;
        }
    
    if(GetShouldShowGameOverMessage())
        ShowGameOverMessage(message);
    else
        ShowGameBoard(renderer);
}

void ShowScoreboard()
{
    int w = 0, h = 0;
    float scale = 4.0f;
    char scores[100];
    char *title = "SCOREBOARD";

    /* Center the message and scale it up */
    SDL_GetRenderOutputSize(renderer, &w, &h);
    SDL_SetRenderScale(renderer, scale, scale);
    int x = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(title)) / 8;
    int y = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 3;

    /* Draw the title */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDebugText(renderer, x, y, title);
    SDL_RenderLine(renderer, x, y + 8.0f, x + SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(title), y + 8.0f);
    
    // Draw the scores
    scale = 2.0f;
    SDL_SetRenderScale(renderer, scale, scale);
    snprintf(scores, sizeof(scores), "X: %d\nO: %d\nTies: %d", GetXWins(), GetOWins(), GetTies());
    
    x = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(title)) / 8;
    y = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 3;
    SDL_RenderDebugText(renderer, x, y + 24.0f, scores);
    
    // Restore render scale to 1 before returning to the main loop.
    SDL_SetRenderScale(renderer, 1.0f, 1.0f);
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    SDL_SetRenderDrawColor(renderer, 46, 52, 64, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderScale(renderer, 1.0f, 1.0f);

    GameResult *result = GetGameResult();

    if(result->type == NONE)
    {
        ShowGameBoard(renderer);
        if(!GetIsTwoPlayerMode() && turn == 1)
        {
            PlacePlayer2Marker();
            EndCurrentTurn();
        }
    }
    else
    {
        ShowGameOverState();
    }
    //result->type == NONE ? ShowGameBoard(renderer) : ShowGameOverState();
    
    ShowScoreboard();

    // Draw buttons
    RenderButtons(renderer);
    SDL_RenderPresent(renderer);

    CheckForWinner();
    UpdateScoreboard();

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
}