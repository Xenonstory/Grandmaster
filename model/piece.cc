#include "piece.h"
#include "game.h"
#include "player.h"

#include "pieces/king.h"
#include "pieces/queen.h"
#include "pieces/rook.h"
#include "pieces/bishop.h"
#include "pieces/knight.h"
#include "pieces/pawn.h"

Piece::Piece(char type, Player * const player, int row, int col, Game * game)
             : type(type), player(player) {
    // Sets the type of the piece and the player
    this->row = row;
    this->col = col;
    this->game = game;
    this->moved = 0;
}

Piece::~Piece() {
    // No heap-allocated memory so no destructor required
}

char Piece::getType() const {
    // A getter for the type of the piece
    return this->type;
}

Player * Piece::getPlayer() const {
    // A getter for the player this piece belongs to
    return this->player;
}

void Piece::updateMove(int row, int col) {
    // Updates the row and column of this piece
    this->row = row;
    this->col = col;
    this->moved++;
}

Piece * Piece::generatePiece(const char type, Player * const player,
                             int row, int col, Game * game) {
    Piece * new_piece;
    switch(type) {
        case Constants::WHITE_KING: case Constants::BLACK_KING:
            return new King(type, player, row, col, game);
        break;
        case Constants::WHITE_QUEEN: case Constants::BLACK_QUEEN:
            return new Queen(type, player, row, col, game);
        break;
        case Constants::WHITE_ROOK: case Constants::BLACK_ROOK:
            return new Rook(type, player, row, col, game);
        break;
        case Constants::WHITE_BISHOP: case Constants::BLACK_BISHOP:
            return new Bishop(type, player, row, col, game);
        break;
        case Constants::WHITE_KNIGHT: case Constants::BLACK_KNIGHT:
            return new Knight(type, player, row, col, game);
        break;
        case Constants::WHITE_PAWN: case Constants::BLACK_PAWN:
            return new Pawn(type, player, row, col, game);
        break;
    }
    // An invalid piece was given, return no piece
    return 0;
}
