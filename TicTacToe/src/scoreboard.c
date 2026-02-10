#include <SDL3/SDL.h>
#include <stdio.h>
#include <logic.h>
#include <scoreboard.h>

int xWins = 0;
int oWins = 0;
int ties = 0;

static char *savePath;

int GetXWins()
{
    return xWins;
}

int GetOWins()
{
    return oWins;
}

int GetTies()
{
    return ties;
}

void AddResult(GameResult result)
{
    switch(result.type)
    {
        case X:
            xWins++;
            break;
        case O:
            oWins++;
            break;
        case TIE:
            ties++;
            break;
        default:
            break;
    }
}

int SaveScoreboard()
{
    savePath = SDL_GetPrefPath("dj", "tictactoe");
    FILE *fptr = fopen("scores.txt", "w");
    if(fptr == NULL) return -1;

    int charsSaved = fprintf(fptr, "%d %d %d", xWins, oWins, ties);
    fclose(fptr);

    return charsSaved;
}

int LoadScoreboard()
{
    FILE *fptr = fopen("scores.txt", "r");
    if(fptr == NULL)
        return -1;

    if(fscanf(fptr, "%d %d %d", &xWins, &oWins, &ties) == 3)
    {
        //printf("Loaded scores\n");
    }

    fclose(fptr);
}