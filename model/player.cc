#include "player.h"

Player::Player(const int level) : level(level) {
    // Sets the level of the new player
    this->wins = 0;
    this->loses = 0;
    this->ties = 0;
    this->ELO_rating = Constants::DEFAULT_ELO;
    this->highestELO = Constants::DEFAULT_ELO;
}

Player::~Player() {
}

bool Player::isHuman() const {
    // Returns true if this player is a human
    return (this->level == 0);
}

int Player::totalGames() const {
    // Returns the total number of games played by this player
    return this->wins + this->loses + this->ties;
}

void Player::winGame(Player * winner, Player * loser) {
    // The first player has beat the second player
    winner->wins++;
    loser->loses++;
}

void Player::tieGame(Player * player_1, Player * player_2) {
    // The two players have just tied
    player_1->ties++;
    player_2->ties++;
}

void Player::setKingCoordinates(int row, int col) {
    // Sets the co-ordinates of the king
    this->kingRow = row;
    this->kingCol = col;
}

int Player::getKingRow() const {
    return this->kingRow;
}

int Player::getKingCol() const {
    return this->kingCol;
}

int Player::getELOrating() const {
    return this->ELO_rating;
}

// The following few functions are definitions for the comparisons of different
// players. Comparisons are defined by the ELO rating of the given players

bool operator> (Player &player1, Player &player2) {
    return player1.ELO_rating > player2.ELO_rating;
}
bool operator< (Player &player1, Player &player2) {
    return player1.ELO_rating < player2.ELO_rating;
}
bool operator>= (Player &player1, Player &player2) {
    return player1.ELO_rating >= player2.ELO_rating;
}
bool operator<= (Player &player1, Player &player2) {
    return player1.ELO_rating <= player2.ELO_rating;
}
bool operator== (Player &player1, Player &player2) {
    return player1.ELO_rating == player2.ELO_rating;
}
bool operator!= (Player &player1, Player &player2) {
    return player1.ELO_rating != player2.ELO_rating;
}

int Player::calculateKFactor() const {
    // Returns the K-factor for ELO ratings of the given player
    if(this->totalGames() < Constants::THRESHOLD_MATCHES) {
        return Constants::ELO_K_TIER_1;
    // TODO actually update highestELO
    } else if(this->highestELO < Constants::THRESHOLD_SCORE) {
        return Constants::ELO_K_TIER_2;
    } else {
        return Constants::ELO_K_TIER_3;
    }
}
