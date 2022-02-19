#include "tictactoe.hpp"
#include <cJSON.h>

TicTacToe::TicTacToe()
{
    restart();
    this->p_x.socket_id = 0;
    this->p_x.symbol = 'x';
    this->p_x.score = 0;
    this->p_o.socket_id = 0;
    this->p_o.symbol = 'o';
    this->p_o.score = 0;
}

void TicTacToe::place(int id, int row, int col)
{
    char player = ' ';
    if (id == this->p_x.socket_id)
        player = this->p_x.symbol;
    else if (id == this->p_o.socket_id)
        player = this->p_o.symbol;

    if (row < 3 && col < 3)
    {
        if (board[row][col] == 'n')
        {
            board[row][col] = player;
        }
    }
}

char TicTacToe::checkWin()
{
    // check rows
    for (int i = 0; i < 3; i++)
    {
        if (board[i][0] == 'x' && board[i][1] == 'x' && board[i][2] == 'x')
            return 'x';
        if (board[0][i] == 'x' && board[1][i] == 'x' && board[2][i] == 'x')
            return 'x';

        if (board[i][0] == 'o' && board[i][1] == 'o' && board[i][2] == 'o')
            return 'o';
        if (board[0][i] == 'o' && board[1][i] == 'o' && board[2][i] == 'o')
            return 'o';
    }

    return 'n';
}

void TicTacToe::restart()
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            board[i][j] = 'n';
        }
    }
}

bool TicTacToe::registerPlayer(int id)
{
    if (this->p_x.socket_id == 0)
    {
        this->p_x.socket_id = id;
        return true;
    }
    else if (this->p_o.socket_id == 0)
    {
        this->p_o.socket_id = id;
        return true;
    }

    return false;
}

void TicTacToe::clientDisconnect(int id)
{
    if (id == this->p_x.socket_id)
    {
        this->p_x.socket_id = 0;
        this->p_x.score = 0;
    }
    else if (id == this->p_x.socket_id)
    {
        this->p_o.socket_id = 0;
        this->p_o.score = 0;
    }
}

cJSON *TicTacToe::getGameBoard()
{
    cJSON *root = cJSON_CreateObject();
    cJSON *table = cJSON_CreateArray();

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            cJSON_AddItemToArray(table, cJSON_CreateNumber(board[i][j]));
        }
    }

    cJSON_AddItemToObject(root, "table", table);

    return root;
}