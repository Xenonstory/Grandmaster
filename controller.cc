#include "controller.h"
#include "model/game.h"
#include "model/player.h"
#include "view/text_view.h"

#include <iostream>
#include <string>
#include <sstream>

using namespace std;

Controller::Controller() {
    // Arbitrarily set current game/view pointers
    this->game = 0;
    this->text_view = 0;

    // Construct a new game
    this->players = new map<string, Player *>();
}

Controller::~Controller() {
    // Delete all heap-allocated memory
    delete this->game;
    delete this->text_view;

    // Delete all players
    map<string, Player *>::iterator it = this->players->begin();
    for(; it != this->players->end(); it++) {
        delete it->second;
    }
    delete this->players;
}

void Controller::error(string err) const {
    // Prints out an appropriate error message
    cout << "> Error: " << err << endl;
}

void Controller::addPlayer(string name) {
    // Check if the player already exists
    if(this->players->count(name) != 0) {
        this->error("Player '" + name + "' already exists.");
    } else {
        // TODO: add a way to set the level of the player to computer
        Player * new_player = new Player(0);
        this->players->insert(pair<string, Player *>(name, new_player));
    }
}

void Controller::remPlayer(string name) {
    // Make sure the player exists
    map<string, Player *>::iterator it = this->players->find(name);
    if(it == this->players->end()) {
        this->error("Player '" + name + "' does not exist."); 
    } else {
        // Else, delete the player and all of his records
        delete it->second;
        this->players->erase(it);
    }
}

bool Controller::startGame(string name_1, string name_2) {
    // Make sure the players are unique
    if(name_1 == name_2) {
        this->error("Cannot play against yourself.");
        return 0;
    }
    // Make sure the players both exist
    map<string, Player *>::iterator it1 = this->players->find(name_1);
    map<string, Player *>::iterator it2 = this->players->find(name_2);
    if(it1 == this->players->end()) {
        this->error("Player '" + name_1 + "' does not exist.");
        return 0;
    } else if(it2 == this->players->end()) {
        this->error("Player '" + name_2 + "' does not exist.");
        return 0;
    }

    this->text_view = new Text_View();
    this->game = new Game(it1->second, it2->second, this);
    return 1;
}

void Controller::playGame() {
    // Plays the game until completion
    string input, parser, junk;
    this->text_view->print();
    while(getline(cin, input)) {
        istringstream input_ss(input);
        input_ss >> parser;

        // A move was made
        if(parser == "move") {
            // Check if the player is human, in which case, fetch the move
            if(this->game->getNext()->isHuman()) {
                // Get the move from standard input
                string pos_1, pos_2;
                if(!(input_ss >> pos_1 >> pos_2) || input_ss >> junk) {
                    this->error("Invalid input for 'move' command.");   
                } else {
                    if(this->game->move(pos_1, pos_2)) {
                        // If the move is a success, print the board
                        this->text_view->print();
                    }
                }
            } else {
                // TODO: generate a computer move
            }

        // A resign was called
        } else if(parser == "resign") {
            // Make sure no junk was given
            if(input_ss >> junk) {
                this->error("Invalid input for 'resign' command.");
            } else {
                this->endGame();
                return;
            }

        // An undo was called for
        } else if(parser == "undo") {
            // Make sure no junk was given
            if(input_ss >> junk) {
                this->error("Invalid input for 'undo' command.");
            } else {
                if(!this->game->undo()) {
                    this->text_view->print();
                }
            }

        // An invalid command was given
        } else {
            this->error("Command '" + parser + "' not found.");
        }
    }
}

void Controller::endGame() {
    // Ends the game
    cout << this->game->getPrevColor() << " wins!" << endl;
    this->game->getPrev()->wins++;
    this->game->getNext()->loses++;

    // Frees memory associated with the game
    delete this->game;
    delete this->text_view;
}

void Controller::play() {
    // Processess input
    string input, parser, junk;
    while(getline(cin, input)) {
        istringstream input_ss(input);
        input_ss >> parser;

        // A new player is to be added:
        if(parser == "add") {
            // Fetch the new player's name
            string name;
            // Check for valid inputs with no extras
            if(!(input_ss >> name) || input_ss >> junk) {
                this->error("Invalid input for 'add' command.");
            } else {
                // Add the player
                this->addPlayer(name);
            }

        // An old player is to be deleted:
        } else if(parser == "remove") {
            // Fetch the player's name
            string name;
            // Check for valid inputs with no extras
            if(!(input_ss >> name) || input_ss >> junk) {
                this->error("Invalid input for 'remove' command.");
            } else {
                this->remPlayer(name);
            }

        // Starts a game between the two given players
        } else if(parser == "game") {
            // Fetch the player's names
            string name_1, name_2;
            // Check for valid inputs with no extras
            if(!(input_ss >> name_1 && input_ss >> name_2) ||
                input_ss >> junk) {
                this->error("Invalid input for 'game' command.");
            } else {
                if(this->startGame(name_1, name_2)) {
                    this->playGame();
                }
            }

        // Invalid command given
        } else {
            this->error("Command '" + parser + "' not found.");
        }
    }
}

void Controller::notifyAdd(char piece, int row, int col) {
    // Adds the given piece to the view
    this->text_view->add(piece, row, col);
}

void Controller::notifyRem(int row, int col) {
    // Removes the given piece from the view
    this->text_view->remove(row, col);
}
