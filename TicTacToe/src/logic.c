#include <logic.h>
#include <gameboard.h>
#include <scoreboard.h>
#include <stdbool.h>
#include <stdio.h>

int turn = 0;
int aiLevel = 1;
static GameResult result = { NONE };
bool scoreboardUpdated = false;
static bool isTwoPlayerMode = true;

const int WIN_STATES[8][3] = {
    {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, // Rows
    {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, // Columns
    {0, 4, 8}, {2, 4, 6}             // Diagonals
};

int evaluate(int board[9])
{
    for(int i = 0; i < 8; i++)
    {
        int pos1 = WIN_STATES[i][0];
        int pos2 = WIN_STATES[i][1];
        int pos3 = WIN_STATES[i][2];

        if(board[pos1] != 0 && board[pos1] == board[pos2] && board[pos2] == board[pos3])
        {
            if(board[pos1] == 2)
                return 10;  // Computer wins
            else if(board[pos1] == 1)
                return -10; // Human wins
        }
    }

    return 0;   // No winner yet
}

int minimax(int board[9], int depth, bool isMax)
{
    int score = evaluate(board);

    // If a win/loss state is reached, return the score
    if(score == 10)
        return score - depth;   // Faster wins are better
    if( score == -10)
        return score + depth;   // Slower losses are better

    // Check for a draw (no moves left)
    int movesLeft = 0;
    for(int i = 0; i < 9; i++)
    {
        if(board[i] == 0)
            movesLeft++;
    }

    if(movesLeft == 0) return 0;

    int best;

    if(isMax)   // Computer's turn
    {
        best = INT8_MIN;
        for(int i = 0; i < 9; i++)
        {
            if(board[i] == 0)
            {
                board[i] = 2;   // Make the move
                int currentScore = minimax(board, depth + 1, !isMax);
                best = (best > currentScore) ? best : currentScore;
                board[i] = 0;   // Undo the move
            }
        }
    }
    else    // Human's turn. Assume optimal move
    {
        best = INT8_MAX;
        for(int i = 0; i < 9; i++)
        {
            if(board[i] == 0)
            {
                board[i] = 1;
                int currentScore = minimax(board, depth + 1, !isMax);
                best = (best < currentScore) ? best : currentScore;
                board[i] = 0;
            }
        }
    }

    return best;
}

/**
 * @brief Finds the best move for the computer
 * @param board The 9-element int array representing the board
 * @return The index (0-8) of the best move
 */
int FindBestMove(int board[9])
{
    int bestVal = INT8_MIN;
    int bestMove = -1;

    for(int i = 0; i < 9; i++)
    {
        if(board[i] == 0)
        {
            board[i] = 2;   //make the trial move
            int moveVal = minimax(board, 0, false);    // Set to false to simulate the human player making the next move
            board[i] = 0;

            // If this move has a higher scorethan the current best, update
            if(moveVal > bestVal)
            {
                bestMove = i;
                bestVal = moveVal;
            }
        }
    }

    return bestMove;
}
bool GetIsTwoPlayerMode()
{
    return isTwoPlayerMode;
}

void SetIsTwoPlayerMode(bool mode)
{
    isTwoPlayerMode = mode;
}

void SetAiLevel()
{
    aiLevel++;
    if(aiLevel > 3)
        aiLevel = 1;
}

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
    //printf("Resetting logic\n");
    turn = 0;
    result.type = NONE;
    scoreboardUpdated = false;
}

GameResult* GetGameResult(void)
{
    return &result;
}

void UpdateScoreboard()
{
    if(result.type == NONE || scoreboardUpdated)
        return;

    AddResult(result);
    scoreboardUpdated = true;
    SaveScoreboard();
}

/// @brief Level 1
/// AI plays the next available space
void PerformLevel1Move()
{
    for(int i = 0; i < 9; i++)
    {
        if(markers[i] == 0)
        {
            markers[i] = 2;
            return;
        }
    }
}

// Level 2:
//  AI finds a previously played marker, and plays horizontal space if available
//  If no previous play, find first available space
//  If no horizontal play, find a vertical play
// If no vertical, find the next available space.
void PerformLevel2Move()
{
    // Look for horizontal space
    for(int i = 0; i < 9; i++)
    {
        if(markers[i] != 2)
            continue;

        if(i == 0 || i == 3 || i == 9)
        {
            if(markers[i + 1] == 0)
            {
                markers[i + 1] = 2;
                return;
            }
        }
        else if(i == 1 || i == 4 || i == 7)
        {
            if(markers[i + 1] == 0)
            {
                markers[i + 1] = 2;
                return;
            }
            else if(markers[i - 1] == 0)
            {
                markers[i - 1] == 2;
                return;
            }
        }
        else if(i == 2 || i == 5 || i == 8)
        {
            if(markers[i - 1] == 0)
            {
                markers[i - 1] == 2;
                return;
            }
        }
    }

    // Look for adjacent vertical space
    for(int i = 0; i < 9; i++)
    {
        if(markers[i] != 2)
            continue;
        
        if(i >=0 && i <= 2)
        {
            if(markers[i + 3] == 0)
            {
                markers[i + 3] = 2;
                return;
            }
        }
        else if(i >= 3 && i <= 5)
        {
            if(markers[i + 3] == 0)
            {
                markers[i + 3] = 2;
                return;
            }
            if(markers[i - 3] == 0)
            {
                markers[i - 3] == 2;
                return;
            }
        }
        else if(i >= 6 && i <= 8)
        {
            if(markers[i - 3] == 0)
            {
                markers[i - 3] = 2;
                return;
            }
        }
    }

    PerformLevel1Move();
}

// Level 3:
// AI analyzes board and finds best move.
void PerformLevel3Move()
{
    int bestMove = FindBestMove(markers);
    markers[bestMove] = 2;
}

void PlacePlayer2Marker()
{
    if(aiLevel == 1)
        PerformLevel1Move();
    else if(aiLevel == 2)
        PerformLevel2Move();
    else
        PerformLevel3Move();
}


