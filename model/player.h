#ifndef PLAYER_H
#define PLAYER_H

class Robot;
class Game;

#include <math.h>
#include <string>
#include "../constants.h"

class Player {
    private:
        Robot * robot; 
        std::string level;
        int kingRow, kingCol;
        int wins, loses, ties;
        int ELO_rating;
        int highestELO;
        std::string name;

        int calculateKFactor() const;
        void calculateNewRating(int, double);
        double expectedScoreAgainst(int) const;
    public:
        Player(std::string, const std::string);
        Player(std::string, std::string, int, int, int, int, int);
        ~Player();

        // Robot related functions
        void startGame(Game *);
        void robotMove();

        // Getters
        int getKingRow() const;
        int getKingCol() const;

        int getHighestELO() const;
        int getELOrating() const;

        int getWins() const;
        int getLoses() const;
        int getTies() const;

        std::string getName() const;
        std::string getLevel() const;
        bool isHuman() const;

        // End-game functions for the player
        static void winGame(Player *, Player *);

        static void tieGame(Player *, Player *);

        // Functions related to the player itself
        int totalGames() const;
        void setKingCoordinates(int, int);

        // Functions for player comparison
        friend bool operator> (Player&, Player&);
        friend bool operator< (Player&, Player&);
        friend bool operator<= (Player&, Player&);
        friend bool operator>= (Player&, Player&);
        friend bool operator== (Player&, Player&);
        friend bool operator!= (Player&, Player&);
};

#endif
