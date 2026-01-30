#include <SDL3/SDL.h>
#include <string.h>

// Array to hold each square of the game board. The array should go left->right, top->bottom
// i.e. top left is rects[0], middle is rects[4], bottom right is rects[8]
SDL_FRect rects[9];
SDL_FRect background;

// Array to hold the marks in each square.
// 0: Empty/usable space
// 1: An X has been drawn here
// 2: An O has been drawn here
int markers[9] = {0};

void InitGameboard()
{

}

void CreateRects(float logicalWidth, float logicalHeight)
{
    float cellSize = 100.0f;
    float spacing = 10.0f;
    int rectCount = 0;

    // Calculate total footprint of the 3x3 grid
    float totalGridWidth = (cellSize * 3) + (spacing * 2);
    float totalGridHeight = (cellSize * 3) + (spacing * 2);

    // Find the starting top-left corner to keep it centered
    float startX = (logicalWidth - totalGridWidth) / 2.0f;
    float startY = (logicalHeight - totalGridHeight) / 2.0f;

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

void DrawGameMarkers(SDL_Renderer *renderer)
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

void ShowGameBoard(SDL_Renderer *renderer)
{
    // Draw game board
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);  /* black, full alpha */
    SDL_RenderFillRect(renderer, &background);

    SDL_SetRenderDrawColor(renderer, 46, 52, 64, SDL_ALPHA_OPAQUE);  /* white, full alpha */
    for(int i = 0; i < 9; i++)
    {
        SDL_RenderFillRect(renderer, &rects[i]);
    }

    DrawGameMarkers(renderer);
}

void ResetBoard()
{
    memset(markers, 0, sizeof(markers));
}

void AddMarker(int space, int marker)
{
    if((space >= 0 && space < 9) && markers[space] == 0)
        markers[space] = marker;
}