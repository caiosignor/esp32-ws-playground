#ifndef TICTACTOE_HPP
#define TICTACTOE_HPP

#include <cJSON.h>

typedef struct _Player
{
    int socket_id;
    char symbol;
    int score;
}Player;

class TicTacToe{

    private:
        char board[3][3];
        Player p_x;
        Player p_o;

    public:
        TicTacToe();
        void place(int id, int row, int col);
        char checkWin();
        void restart();
        char registerPlayer(int id);
        int getScoreX();
        int getScoreO();
        void clientDisconnect(int id);
        cJSON *getGameBoard();
};

#endif