#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include "ConnectFour.hpp" /* Connecting the other files*/
#include "Cell.cpp"
#include "ConnectFour.cpp"

using namespace std;


int main() {
    string line, parameters[2];
    string input;
    int row = 0, col = 0;

    // Menu
    cout << "Starting a Connect-Four game..." << endl;
    cout << "-----------------------------------" << endl;
    cout << "1. Default 5x5 Board (Type 'y')" << endl;
    cout << "2. Custom Dimensions (Type 'R C')" << endl;
    cout << "3. Load from File (Type filename)" << endl;
    cout << "-----------------------------------" << endl;
    cout << "Enter your choice (e.g., '6 7' or 'board.txt'):" << endl;
    
    getline(cin, line);
    stringstream istr(line);

    istr.clear();
    istr.seekg(0);
    ConnectFour* new_game;

    if(istr >> row && istr >> col) {// Tries to take two integers from the console and initializes the game
        new_game = new ConnectFour(row, col);
    }
    else {
        istr.clear();
        istr.seekg(0);

        if(istr >> input) {
            if(input == "Y" || input == "y") {
                new_game = new ConnectFour();
            }
            else {
                fstream file(input.c_str(), ios::in);

                if(file.is_open()) {
                    new_game = new ConnectFour(file);
                }
                else {
                    cout << "Error: Cannot open file " << input << ". Terminating..." << endl;
                    return 1;
                }
            }
        }

        else {
            cout << "No input provided. Terminating..." << endl;
            return 1;
        }
    }

    char gamemode;

    cout << "Choose game mode: (P) for two-player, (C) for User vs Computer: ";
    cin >> gamemode;
    // Takes the game mode input and plays the game after
    if((toupper(gamemode) != 'P') && (toupper(gamemode) != 'C')) {
        cout << "Invalid input. Defaulting to User vs Computer." << endl;
        gamemode = 'C';
    }
    if(toupper(gamemode) == 'P') {
        new_game->set_gamemode(GameMode::PvsP);
    }
    else {
        new_game->set_gamemode(GameMode::PvsC);
    }

    new_game->playGame();

    delete new_game;

    return 0;
}