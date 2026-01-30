#include <logic.h>
#include <gameboard.h>

int turn = 0;
GameResult result = { NONE };

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
/// @return 0 if no winner/game in progress, 1 if X wins, 2 if O wins, 3 if it's a tie
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
    turn = 0;
    result.type = NONE;
}

GameResult GetGameResult()
{
    return result;
}