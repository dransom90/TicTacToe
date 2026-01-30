#ifndef LOGIC_H
#define LOGIC_H

extern int winner;
extern int turn;

typedef enum 
{
    NONE = 0,
    X = 1,
    O = 2,
    TIE = 3,
} WinnerType;

typedef struct
{
    WinnerType type;
} GameResult;

void CheckForWinner();
void EndCurrentTurn();
void Reset();
GameResult GetGameResult();
void UpdateScoreboard();

#endif