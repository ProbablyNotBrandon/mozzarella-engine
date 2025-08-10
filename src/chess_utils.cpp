#include "chess_utils.h"

Player chtopl(char c) {
    if (c == 'P' || c == 'N' || c == 'B' || c == 'R' || c == 'Q' || c == 'K'){
        return Player::WHITE;
    } else if (c == 'p' || c == 'n' || c == 'b' || c == 'r' || c == 'q' || c == 'k') {
        return Player::BLACK;
    }
    else {
        // Piece is not valid
        return (Player) -1;
    }
}

Piece chtopc(char c) {
    return _chtopc.at(std::tolower(c));
}


// GPT CODE
// Splits a string based on delimiter and returns the pieces as a vector of strings
std::vector<std::string> split_str(const std:: string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;

    for (char c: s) {
        if (c == delimiter) {
            tokens.push_back(token);
            token.clear();
        } else {
            token += c;
        }
    }
    tokens.push_back(token);
    return tokens;
}
// END GPT CODE
