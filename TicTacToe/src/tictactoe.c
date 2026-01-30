#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_rect.h>
#include <stdio.h>
#include <stdbool.h>
#include <buttons.h>
#include <string.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
const int WIN_STATES[8][3] = {
    {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, // Rows
    {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, // Columns
    {0, 4, 8}, {2, 4, 6}             // Diagonals
};

// Array to hold each square of the game board. The array should go left->right, top->bottom
// i.e. top left is rects[0], middle is rects[4], bottom right is rects[8]
static SDL_FRect rects[9];
static SDL_FRect background;

// Array to hold the marks in each square.
// 0: Empty/usable space
// 1: An X has been drawn here
// 2: An O has been drawn here
static int markers[9] = {0};
static int turn = 0;
static int winner = 0;

float LOGICAL_W, LOGICAL_H;


void DrawThickBar(SDL_Renderer* renderer, float cx, float cy, float len, float thickness, float angle) {
    float halfLen = len / 2.0f;
    float halfThick = thickness / 2.0f;

    // 1. Define the 4 corners of a horizontal bar centered at (0,0)
    SDL_FPoint corners[4] = {
        {-halfLen, -halfThick}, {halfLen, -halfThick},
        {halfLen, halfThick},  {-halfLen, halfThick}
    };

    float rad = angle * (SDL_PI_F / 180.0f);
    float cosA = SDL_cosf(rad);
    float sinA = SDL_sinf(rad);

    // 2. Rotate and Translate corners to the screen position
    SDL_Vertex vertices[4];
    for (int i = 0; i < 4; i++) {
        float rotX = corners[i].x * cosA - corners[i].y * sinA;
        float rotY = corners[i].x * sinA + corners[i].y * cosA;
        
        vertices[i].position.x = rotX + cx;
        vertices[i].position.y = rotY + cy;
        vertices[i].color = (SDL_FColor){1.0f, 0.0f, 0.0f, 1.0f}; // Red
    }

    // 3. Define the triangles (Indices: 0-1-2 and 0-2-3)
    int indices[] = {0, 1, 2, 0, 2, 3};

    // 4. Send to GPU
    SDL_RenderGeometry(renderer, NULL, vertices, 4, indices, 6);
}

void DrawBoldO(SDL_Renderer* renderer, float cx, float cy, float outerRadius, float thickness) {
    const int segments = 64; // Higher = smoother circle
    float innerRadius = outerRadius - thickness;
    
    // We need 2 vertices per segment (inner and outer)
    // Plus 2 extra to close the loop (total = segments * 2 + 2)
    int numVertices = (segments + 1) * 2;
    SDL_Vertex* vertices = (SDL_Vertex*)SDL_malloc(sizeof(SDL_Vertex) * numVertices);
    
    for (int i = 0; i <= segments; i++) {
        float angle = i * (2.0f * SDL_PI_F / segments);
        float cosA = SDL_cosf(angle);
        float sinA = SDL_sinf(angle);

        // Outer vertex
        vertices[i * 2].position.x = cx + outerRadius * cosA;
        vertices[i * 2].position.y = cy + outerRadius * sinA;
        vertices[i * 2].color = (SDL_FColor){0.0f, 0.5f, 1.0f, 1.0f}; // Blue-ish

        // Inner vertex
        vertices[i * 2 + 1].position.x = cx + innerRadius * cosA;
        vertices[i * 2 + 1].position.y = cy + innerRadius * sinA;
        vertices[i * 2 + 1].color = (SDL_FColor){0.0f, 0.5f, 1.0f, 1.0f};
    }

    // Define indices for a triangle strip: 0, 1, 2, 3, 4, 5...
    int numIndices = segments * 6;
    int* indices = (int*)SDL_malloc(sizeof(int) * numIndices);
    for (int i = 0; i < segments; i++) {
        int v = i * 2;
        indices[i * 6 + 0] = v;
        indices[i * 6 + 1] = v + 1;
        indices[i * 6 + 2] = v + 2;
        indices[i * 6 + 3] = v + 1;
        indices[i * 6 + 4] = v + 3;
        indices[i * 6 + 5] = v + 2;
    }

    SDL_RenderGeometry(renderer, NULL, vertices, numVertices, indices, numIndices);

    SDL_free(vertices);
    SDL_free(indices);
}

void createRects()
{
    float cellSize = 100.0f;
    float spacing = 10.0f;
    int rectCount = 0;

    // Calculate total footprint of the 3x3 grid
    float totalGridWidth = (cellSize * 3) + (spacing * 2);
    float totalGridHeight = (cellSize * 3) + (spacing * 2);

    // Find the starting top-left corner to keep it centered
    float startX = (LOGICAL_W - totalGridWidth) / 2.0f;
    float startY = (LOGICAL_H - totalGridHeight) / 2.0f;

    background.w = totalGridWidth;
    background.h = totalGridHeight;
    background.x = startX;
    background.y = startY;

    for(int row = 0; row < 3; row++)
    {
        for(int col = 0; col < 3; col++)
        {
            SDL_FRect rect;
            rect.w = cellSize;
            rect.h = cellSize;
            rect.x = startX + (col * (cellSize + spacing));
            rect.y = startY + (row * (cellSize + spacing));

            rects[rectCount++] = rect;
        }
    }
}

int checkForTie()
{
    for(int i = 0; i < 9; i++)
    {
        if(markers[i] == 0)
            return 0;
    }

    return 1;
}

/// @brief Check for a winner
/// @return 0 if no winner/game in progress, 1 if X wins, 2 if O wins, 3 if it's a tie
int checkForWinner()
{
    for (int i = 0; i < 8; i++) {
        int a = WIN_STATES[i][0];
        int b = WIN_STATES[i][1];
        int c = WIN_STATES[i][2];

        // Check if all three are the same and not empty (0)
        if (markers[a] != 0 && markers[a] == markers[b] && markers[a] == markers[c]) {
            return markers[a]; // Returns 1 (X) or 2 (O) as the winner
        }
    }

    if(checkForTie())
        return 3;

    return 0;
}

SDL_AppResult HandleButtnEventResult(ButtonEvent btnEvent)
{
    if(btnEvent.noAction) return SDL_APP_CONTINUE;
    if(btnEvent.quit)
    {
        return SDL_APP_SUCCESS;
    }
    if(btnEvent.newGame)
    {
        winner = 0;
        turn = 0;
        memset(markers, 0, sizeof(markers));
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

    createRects();
    CreateButtons(window);
    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{    
    SDL_ConvertEventToRenderCoordinates(renderer, event);
    ButtonEvent btnEvent = HandleButtonEvent(event);
    if(HandleButtnEventResult(btnEvent) == SDL_APP_SUCCESS)
        return SDL_APP_SUCCESS;

    if(event->type == SDL_EVENT_MOUSE_BUTTON_DOWN && event->button.button == SDL_BUTTON_LEFT)
    {
        SDL_FPoint mousePoint = { event->button.x, (int)event->button.y};
        for(int i = 0; i < 9; i++)
        {
            if(SDL_PointInRectFloat(&mousePoint, &rects[i]))
            {
                SDL_FRect rect = rects[i];
                int marker = markers[i];
                if(marker == 0)
                {
                    if(turn == 0)
                    {
                        markers[i] = 1;
                        turn = 1;
                    }
                    else
                    {
                        // Place an O
                        markers[i] = 2;
                        turn = 0;
                    }
                }
            }
        }

        winner = checkForWinner();
    }

     if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

void DrawCircleSmooth(SDL_Renderer* renderer, float centerX, float centerY, float radius)
{
    const int segments = 36;
    float step = 2.0f * SDL_PI_F / segments;

    for(int i = 0; i < segments; i++)
    {
        float x1 = centerX + radius * SDL_cosf(i * step);
        float y1 = centerY + radius * SDL_sinf(i * step);
        float x2 = centerX + radius * SDL_cosf((i + 1) * step);
        float y2 = centerY + radius * SDL_sinf((i + 1) * step);
        SDL_RenderLine(renderer, x1, y1, x2, y2);
    }
}

bool checkLine(int a, int b, int c)
{
    return (a != 0 && a == b && b == c);
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
        switch(winner)
        {
            case 1:
                message = "X Wins!";
                break;
            case 2:
                message = "O Wins!";
                break;
            case 3:
                message = "Tie :(";
                break;
            default:
                break;
        }
    
    ShowGameOverMessage(message);
}

void DrawGameMarkers()
{
    for(int i = 0; i < 9; i++)
    {
        SDL_FRect rect = rects[i];
        if(markers[i] == 1)
        {
            //Place an X
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            DrawThickBar(renderer, rect.x + rect.w / 2.0f, rect.y + rect.h / 2.0f, rect.w * 0.9f, 12.0f, 45.0f);
            DrawThickBar(renderer, rect.x + rect.w / 2.0f, rect.y + rect.h / 2.0f, rect.w * 0.9f, 12.0f, -45.0f);
        }
        else if(markers[i] == 2)
        {
            // Place an O
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            DrawBoldO(renderer, rect.x + rect.w / 2.0f, rect.y + rect.h / 2.0f, (rect.w / 2.0f) * 0.8f, 12.0f);
        }
    }
}

void ShowGameBoard()
{
    // Draw game board
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);  /* black, full alpha */
    SDL_RenderFillRect(renderer, &background);

    SDL_SetRenderDrawColor(renderer, 46, 52, 64, SDL_ALPHA_OPAQUE);  /* white, full alpha */
    for(int i = 0; i < 9; i++)
    {
        SDL_RenderFillRect(renderer, &rects[i]);
    }

    DrawGameMarkers();
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    SDL_SetRenderDrawColor(renderer, 46, 52, 64, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderScale(renderer, 1.0f, 1.0f);

    winner > 0 ? ShowGameOverState() : ShowGameBoard();

    // Draw buttons
    RenderButtons(renderer);
    SDL_RenderPresent(renderer);
    
    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
}