#include <stdio.h>
#include <logic.h>
#include <scoreboard.h>

int xWins = 0;
int oWins = 0;
int ties = 0;

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

    printf("Result added to scoreboard:\tX: %d\tO: %d\tTies: %d\n", xWins, oWins, ties);
}

//TODO: Add saving/loading ability