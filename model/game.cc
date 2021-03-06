#include "game.h"
#include "../controller.h"
#include "player.h"
#include "piece.h"
#include "move.h"
#include "PGN.h"

#include <fstream>
#include <iostream>
using namespace std;

Game::Game(Player * const player_1, Player * const player_2, Controller * c) :
    player_1(player_1), player_2(player_2) {

    // Sets the players in the game to the given players
    this->control = c;

    // Initialize the board with empty squares
    for(int i = 0; i < Constants::BOARD_LEN; i++) {
        for(int j = 0; j < Constants::BOARD_LEN; j++) {
            this->board[i][j] = NULL;
        }
    }

    // Load up the standard board
    this->loadStandard();

    // Make player one move first
    this->next = this->player_1;
    this->movesSinceCapture = 0;
}

Game::~Game() {
    // Delete all pieces
    this->clearBoard();

    // Deletes all stored moves
    PGN::deleteStack(&this->moves);
}

void Game::loadStandard() {
    // Loads the standard game setup
    ifstream standard("model/standard.txt");

    // Load the board one piece at a time
    char piece;
    for(int i = 0; i < Constants::BOARD_LEN; i++) {
        for(int j = 0; j < Constants::BOARD_LEN; j++) {
            standard >> piece;

            // Determine which player this piece belongs to
            Player * player = (Piece::isWhiteType(piece)) ? this->player_1 : this->player_2;
            this->board[i][j] = Piece::generatePiece(piece, player, i, j, this);
            if(this->board[i][j] != NULL) {
                this->updateAdd(piece, i, j);

                // Check if a king was created
                if(Piece::isKing(piece)) {
                    player->setKingCoordinates(i, j);
                }
            }
        }
    }
}

void Game::clearBoard() {
    // Clears all piece from the board
    for(int i = 0; i < Constants::BOARD_LEN; i++) {
        for(int j = 0; j < Constants::BOARD_LEN; j++) {
            delete this->board[i][j];
        }
    }
}

void Game::updateAdd(char piece, int row, int col) {
    // Updates the view with an additional piece
    this->control->notifyAdd(piece, row, col);
}

void Game::updateRem(int row, int col) {
    // Updates the view with a removal
    this->control->notifyRem(row, col);
}

void Game::switchTurns() {
    // Changes whose turn it is
    if(this->next == this->player_1) {
        this->next = this->player_2;
    } else {
        this->next = this->player_1;
    }
}

Player * Game::getNext() const {
    // Returns a pointer to the player whose turn is next
    return this->next;
}

Player * Game::getPrev() const {
    // Returns a pointer to the player who's turn it's not
    if(this->next == this->player_1) {
        return this->player_2;
    } else {
        return this->player_1;
    }
}

string Game::getNextColor() const {
    // Returns the color of the next player
    if(this->next == this->player_2) {
        return "Black";
    } else {
        return "White";
    }
}

string Game::getPrevColor() const {
    // Returns the color of the previous player
    if(this->next == this->player_2) {
        return "White";
    } else {
        return "Black";
    }
}

void Game::robotMove(int row_1, int col_1, int row_2, int col_2) {
    // The move function for robots to call. Assumes a valid move
    // TODO Account for promotions
    this->movePiece(row_1, col_1, row_2, col_2, 0, true);
    this->switchTurns();
}

bool Game::move(string pos_1, string pos_2, string promotion) {
    // Make sure the given positions are valid
    if(!Move::validPosition(pos_1) || !Move::validPosition(pos_2)) {
        this->control->error("Invalid position given.");
        return 0;
    }

    // Convert the positions
    int row_1, col_1, row_2, col_2;
    Move::convertPosToInt(pos_1, &row_1, &col_1);
    Move::convertPosToInt(pos_2, &row_2, &col_2);

    // Check if its a valid move
    if(!this->validMove(row_1, col_1, row_2, col_2, false, promotion)) {
        return 0;
    }

    // Extract the promotion
    char promotionType = 0;
    if(promotion.length() > 0) {
        promotionType = promotion[0];
    }

    // Move the piece
    this->movePiece(row_1, col_1, row_2, col_2, promotionType, true);
    this->switchTurns();

    return 1;
}

Player * Game::getPlayer(int row, int col) const {
    // Returns the player at the given position with a check for NULL
    if(this->board[row][col] == NULL) {
        return NULL;
    } else {
        return this->board[row][col]->getPlayer();
    }
}

char Game::getType(int row, int col) const {
    // Returns the type at the given position with a check for NULL
    if(this->board[row][col] == NULL) {
        return 0;
    } else {
        return this->board[row][col]->getType();
    }
}

bool Game::validMove(int row_1, int col_1, int row_2, int col_2, bool mute, string promotion) {
    // Check if a valid position was given
    if(!Move::validPosition(row_1, col_1) || !Move::validPosition(row_2, col_2)) {
        if(!mute) this->control->error("Invalid position given.");
        return 0;
    }

    // Returns true if the given move is valid
    Player * player = this->getPlayer(row_1, col_1);

    // Make sure a piece is there
    if(player == NULL) {
        if(!mute) this->control->error("There is no piece there.");
        return 0;
    }

    // Make sure the position moving from belongs to the right player
    if(player != this->next) {
        if(!mute) this->control->error("Cannot move opponent's piece.");
        return 0;
    }

    // Make sure the piece is not moving onto itself
    if(row_1 == row_2 && col_1 == col_2) {
        if(!mute) this->control->error("Cannot move a piece onto itself.");
        return 0;
    }

    // Make sure its a valid move for the piece
    if(!this->board[row_1][col_1]->validMove(row_2, col_2)) {
        if(!mute) this->control->error("Cannot move that piece there.");
        return 0;
    }

    // Make sure they're not capturing their own piece
    if(this->getPlayer(row_2, col_2) == this->next) {
        if(!mute) this->control->error("Cannot capture your own piece.");
        return 0;
    }

    // Check for promotions
    char type = this->board[row_1][col_1]->getType();
    if(Piece::isPawn(type) && (row_2 == Constants::BOARD_LEN || row_2 == 0) && promotion.length() != 0) {
        // Check if the promotion given is a valid one
        if(promotion.length() != 1 || !Piece::isValidType(promotion[0])) {
            if(!mute) this->control->error("Invalid promotion given.");
            return 0;
        // Check if the promoted piece belongs to the correct played by FIDE rules
        } else if(Piece::isWhiteType(promotion[0]) ^ (this->next == this->player_1)) {
            if(!mute) this->control->error("Cannot promote to opponent's piece.");
            return 0;
        }
    } else if(promotion.length() != 0) {
        // Make sure no promotion was given
        if(promotion.length() != 0) {
            if(!mute) this->control->error("No promotion is to be made.");
            return 0;
        }
    }

    // Make sure the move does not put them into check. We do this
    // by doing the actual move and checking if the king is in check.
    this->movePiece(row_1, col_1, row_2, col_2, 0, false);
    bool check = this->inCheck(this->board[row_2][col_2]->getPlayer());
    this->undo();        // Undo switches turns, so we switch again
    this->switchTurns(); // to make up for the fake move
    if(check) {
        if(!mute) this->control->error("Cannot keep your king into check.");
        return 0;
    }

    return 1;
}

void Game::forceMovePiece(int row_1, int col_1, int row_2, int col_2) {
    // Forces the piece to move, regardless of any special circumstances.
    // Simply updates the position and the view
    delete this->board[row_2][col_2];
    this->board[row_2][col_2] = this->board[row_1][col_1];
    this->board[row_1][col_1] = NULL;
    this->updateAdd(this->board[row_2][col_2]->getType(), row_2, col_2);
    this->updateRem(row_1, col_1);
    this->board[row_2][col_2]->updateMove(row_2, col_2);
}

void Game::movePiece(int row_1, int col_1, int row_2, int col_2, char promotion, bool recursion) {
    // TODO Make a generate move function based on rows and cols
    // Moves a piece from row/col_1 to row/col_2
    char type = this->getType(row_1, col_1);
    char captured = this->getType(row_2, col_2);
    this->forceMovePiece(row_1, col_1, row_2, col_2);
    Player * player = this->board[row_2][col_2]->getPlayer();

    // Check if a king was moved
    if(Piece::isKing(type)) {
        player->setKingCoordinates(row_2, col_2);
    }

    // Check if an en-passent occured
    bool enPassentOccured = false;
    if((Piece::isPawn(type)) &&
       col_1 != col_2 &&
       this->enPassent() == col_2 &&
       ((row_1 - row_2 == 1 &&
         row_1 == Constants::ENPASSENT_DIST) ||
        (row_2 - row_1 == 1 &&
         row_1 == Constants::BOARD_LEN - Constants::ENPASSENT_DIST - 1))) {
        captured = this->getType(row_1, col_2);
        delete this->board[row_1][col_2];
        this->updateRem(row_1, col_2);
        enPassentOccured = true;
    }

    // Check if a castling occurred
    if(Piece::isKing(type)) {
        // Check for a right castling
        if(col_2 == col_1 + 2) {
            this->forceMovePiece(row_2, col_2 + 2, row_2, col_2 - 1);
        } else if(col_2 == col_1 - 2) {
            this->forceMovePiece(row_2, col_2 - 1, row_2, col_2 + 1);
        }
    }

    // Check if a promotion occurred
    if(Piece::isPawn(type) && (row_2 == Constants::BOARD_LEN || row_2 == 0)) {
        // Replace the given piece with the proper promotion
        delete this->board[row_2][col_2];
        // If no promotion was specified, use a queen
        if(promotion == 0) {
            promotion = (this->getNext() == this->player_1) ? Constants::WHITE_QUEEN : Constants::BLACK_QUEEN;
        }
        this->board[row_2][col_2] = Piece::generatePiece(promotion, player, row_2, col_2, this);
        this->updateAdd(promotion, row_2, col_2);
    }

    // Get additional information about the move
    bool otherInCheck = recursion && this->inCheck(this->getNext());
    bool otherInCheckmate = recursion && this->checkmate();

    // Push the new move onto the stack
    Move * move = new Move(row_1, col_1, row_2, col_2, type);
    if(enPassentOccured) move->setEnpassent(true);
    if(captured)         move->setCaptured(captured);
    if(otherInCheck)     move->setCheck(true);
    if(otherInCheckmate) move->setCheckmate(true);
    if(promotion)        move->setPromotion(promotion);

    // Set the number of moves since capture
    if(!captured && !Piece::isPawn(type)) {
        this->movesSinceCapture++;
        move->setSinceCapture(this->movesSinceCapture);
    } else {
        this->movesSinceCapture = 0;
    }
    this->moves.push(move);
}

bool Game::isEmpty(int row, int col) const {
    // Returns true if the given square is empty
    return (this->board[row][col] == NULL);
}

bool Game::isMoved(int row, int col) const {
    // Returns true if the piece in the given position
    // has not moved yet. Let's empty spaces be moved
    if(!this->isEmpty(row, col)) {
        return this->board[row][col]->moved;
    } else {
        return true;
    }
}

int Game::enPassent() const {
    // Returns -1 if the last move was not an en passent, else returns
    // the row in which an en passent occured.

    // Check if a move has even occured yet
    if(this->moves.empty()) {
        return -1;
    }
    Move * lastMove = this->moves.top();

    // Check if it was a pawn that moved
    char type = this->getType(lastMove->row_2, lastMove->col_2);
    if(!Piece::isPawn(type)) {
        return -1;
    }

    // Check if the pawn double-stepped
    if(lastMove->row_2 - lastMove->row_1 != 2 &&
       lastMove->row_2 - lastMove->row_1 != -2) {
        return -1;
    }

    // All checks made, return the column
    return lastMove->col_1;
}

int Game::undo() {
    // Checks if a move was made
    if(this->moves.empty()) {
        this->control->error("There is nothing to undo.");
        return 1;
    }
    Move * lastMove = this->moves.top();

    // Variables declared for readability sake
    int row_1 = lastMove->row_1;
    int row_2 = lastMove->row_2;
    int col_1 = lastMove->col_1;
    int col_2 = lastMove->col_2;

    // Moves the piece back to its original position, update the piece
    this->forceMovePiece(row_2, col_2, row_1, col_1);

    // Check if a king was moved
    Piece * piece = this->board[row_1][col_1];
    char type = this->board[row_1][col_1]->getType();
    if(Piece::isKing(type)) {
        Player * player = this->board[row_1][col_1]->getPlayer();
        player->setKingCoordinates(row_1, col_1);
    }

    // Undoes the move made and the undo itself (two moves)
    piece->moved -= 2;

    // Replace any captured pieces (check if en-passent!)
    if(lastMove->captured != 0) {
            Player * player = this->getPrev();
            if(lastMove->enpassent) {
                // If its an en passent, treat the row as the previous one
                row_2 = row_1;
            }
            Piece * regenerated = Piece::generatePiece(lastMove->captured,
                                                       player, row_2, col_2, this);
            this->board[row_2][col_2] = regenerated;
            this->updateAdd(lastMove->captured, row_2, col_2);
    }

    // Undo any castling that was done
    if(Piece::isKing(type)) {
        if(col_2 == col_1 + 2) {
            this->forceMovePiece(row_2, col_2 - 1, row_2, col_2 + 2);
        } else if(col_2 == col_1 - 2) {
            this->forceMovePiece(row_2, col_2 + 1, row_2, col_2 - 1);
        }
    }

    // Undo any promotions that were done
    if(lastMove->promotion) {
        Player * player = this->board[row_1][col_1]->getPlayer();
        delete this->board[row_1][col_1];
        char type = (player == this->player_1) ? Constants::WHITE_PAWN : Constants::BLACK_PAWN;
        this->board[row_1][col_1] = Piece::generatePiece(type, player, row_1, col_1, this);
        this->updateAdd(type, row_1, col_1);
    }

    // Sets the number of moves since last capture
    this->movesSinceCapture = lastMove->sinceCapture;

    // Changes the turn, deletes the last move
    delete lastMove;
    this->moves.pop();
    this->switchTurns();
    return 0;
}

bool Game::isDangerousTo(Player * player, int row, int col) const {
    // Returns true if the given square is dangerous to the given player
    for(int i = 0; i < Constants::BOARD_LEN; i++) {
        for(int j = 0; j < Constants::BOARD_LEN; j++) {
            if(this->getPlayer(i, j) != NULL &&
               this->getPlayer(i, j) != player &&
               this->board[i][j]->validMove(row, col)) {
                    return true;
            }
        }
    }
    return false;
}

bool Game::inCheck(Player * player) const {
    // Returns true if the given player is in check
    int kingRow = player->getKingRow();
    int kingCol = player->getKingCol();
    if(this->isDangerousTo(player, kingRow, kingCol)) {
        return true;
    } else {
        return false;
    }
}

bool Game::noValidMove(Player * player) {
    // Returns true if the given player has no valid moves
    for(int i = 0; i < Constants::BOARD_LEN; i++) {
        for(int j = 0; j < Constants::BOARD_LEN; j++) {
            // Find pieces that belong to the player
            if(this->getPlayer(i, j) == player) {
                // Generate all moves that piece can make
                vector<pair<int, int>> moves = this->board[i][j]->generateMoves();
                if(!moves.empty()) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool Game::checkmate() {
    // Returns true if the game is a checkmate
    Player * player = this->getNext();
    if(this->inCheck(player) && this->noValidMove(player)) {
        return true;
    } else {
        return false;
    }
}

bool Game::stalemate() {
    // Returns true if the game is a checkmate
    Player * player = this->getNext();
    if(!inCheck(player) && this->noValidMove(player)) {
        return true;
    } else {
        return false;
    }
}

bool Game::drawManyNoncaptures() const {
    return this->movesSinceCapture >= Constants::NO_CAPTURE_STALEMATE;
}

void Game::PGN_export(string filename) {
    // Send the stack and the filename to PGN for processing
    PGN::PGN_export(filename, &this->moves, Constants::IN_PROGRESS);
}

void Game::FEN_export(string filename) {
    // Send the board and the filename to FEN for processing
    PGN::FEN_export(filename, this->board);
}
