#include <logic.h>
#include <gameboard.h>
#include <scoreboard.h>
#include <stdbool.h>
#include <stdio.h>

int turn = 0;
GameResult result = { NONE };
bool scoreboardUpdated = false;

const int WIN_STATES[8][3] = {
    {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, // Rows
    {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, // Columns
    {0, 4, 8}, {2, 4, 6}             // Diagonals
};

int CheckForTie()
{
    for(int i = 0; i < 9; i++)
    {
        if(markers[i] == 0)
            return 0;
    }

    return 1;
}

/// @brief Check for a winner
void CheckForWinner()
{
    for (int i = 0; i < 8; i++) {
        int a = WIN_STATES[i][0];
        int b = WIN_STATES[i][1];
        int c = WIN_STATES[i][2];

        // Check if all three are the same and not empty (0)
        if (markers[a] != 0 && markers[a] == markers[b] && markers[a] == markers[c]) {
            result.type = (markers[a] == 1) ? X : O;
            return;
        }
    }

    if(CheckForTie())
        result.type = TIE;
    else
        result.type = NONE;
}

void EndCurrentTurn()
{
    if(turn == 0)   
        turn = 1;
    else
        turn = 0;
}

void Reset()
{
    printf("Resetting logic\n");
    turn = 0;
    result.type = NONE;
    scoreboardUpdated = true;
}

GameResult GetGameResult()
{
    return result;
}

void UpdateScoreboard()
{
    printf("Entering UpdateScoreboard\n");
    if(result.type == NONE || scoreboardUpdated)
        return;

    printf("Calling AddResult\n");
    AddResult(result);
    scoreboardUpdated = true;
}