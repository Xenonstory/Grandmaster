#include "bishop.h"
#include "../game.h"
#include "../player.h"

Bishop::Bishop(char type, Player * const player, int row, int col, Game * game) :
    Piece(type, player, row, col, game) {
}

bool Bishop::validMove(int row, int col) const {
    // Check for diagonal movement
    if((row - this->row == col - this->col) ||
       (row - this->row == this->col - col)) {
        int mini_row = (row < this->row) ? row : this->row;
        int maxi_row = (row < this->row) ? this->row : row;
        int row_incr = (row < this->row) ? 1 : -1;
        int col_incr = (col < this->col) ? 1 : -1;
        for(int i = 1; mini_row + i < maxi_row; i++) {
            if(!this->game->isEmpty(row * (i * row_incr), col * (i * col_incr))) {
                return false;                                                    
            }                                                                    
        }                                                                       
        return true;                                                             
    } 
    return false;
}
