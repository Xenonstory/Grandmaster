#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

class Constants {
    public:
        // Make the class un-instantiable via a virtual destructor
        virtual ~Constants() = 0;

        // Piece representation constants are per PGN standards and are 
        // assumed in later parts of code (i.e. checking if a piece is white
        // is done by checking for a capital letter). Changing these will
        // break the code in some areas.
        const static char WHITE_KING = 'K';                                        
        const static char BLACK_KING = 'k';                                        
        const static char WHITE_QUEEN = 'Q';                                       
        const static char BLACK_QUEEN = 'q';                                       
        const static char WHITE_ROOK = 'R';                                        
        const static char BLACK_ROOK = 'r';                                        
        const static char WHITE_BISHOP = 'B';                                      
        const static char BLACK_BISHOP = 'b';                                      
        const static char WHITE_KNIGHT = 'N';                                      
        const static char BLACK_KNIGHT = 'n';                                      
        const static char WHITE_PAWN = 'P';                                        
        const static char BLACK_PAWN = 'p';  

        // Distances used on the board multiple times
        const static int BOARD_LEN = 8;
        const static int ENPASSENT_DIST = 3;

        // Constants as per PGN standards
        const static char PGN_CAPTURE = 'x';
        const static char PGN_CHECK = '+';
        const static char PGN_PROMOTION = '=';

        // Text representation of colored squares
        const static char WHITE_SQUARE = '_';
        const static char BLACK_SQUARE = ' ';
};

#endif