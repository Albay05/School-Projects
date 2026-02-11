#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include "Cell.cpp"
#include "CF_Abstract.cpp"
#include "CF_Diag.cpp"
#include "CF_Plus.cpp"
#include "CF_Undo.cpp"

using namespace cfgame;


int main() {
    std::string line, input, parameters[2];
    int num;
    int row = 0, col = 0;
    int valid_input = true;

    // The first menu to choose the game setting
    std::cout << "Starting a Connect-Four game..." << std::endl;
    std::cout << "-----------------------------------" << std::endl;
    std::cout << "1. Initialize a diagonal aimed Connect-Four game." << std::endl;
    std::cout << "2. Initialize a horizontal-vertical aimed Connect-Four game." << std::endl;
    std::cout << "2. Initialize a horizontal-vertical aimed Connect-Four game with capability of undoing moves." << std::endl;

    num = -1;
    while (true) {
        std::cout << "-----------------------------------" << std::endl;
        std::cout << "Enter the number adjacent to the definitions to choose a game: ";

        if(std::cin >> num && num >= 1 && num <= 3) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        }

        std::cout << "Invalid input, please enter 1, 2 or 3." << std::endl;

        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // The second menu to choose the game configuration
    std::cout << "\nPlease choose a game configuration from the below." << std::endl;
    std::cout << "-----------------------------------" << std::endl;
    std::cout << "1. Default 5x5 Board (Type 'y')" << std::endl;
    std::cout << "2. Custom Dimensions (Type the row and column values 'R C')" << std::endl;
    std::cout << "3. Load the Board From File (Type filename)" << std::endl;
    std::cout << "-----------------------------------" << std::endl;

    ConnectFourAbstract* active_game = nullptr;

    if(num == 1) {
        ConnectFourDiag* new_game = nullptr;
        
        while(new_game == nullptr) {
            std::cout << "Enter Your Choice (e.g., '6 7' or 'board.txt'):" << std::endl;

            getline(std::cin, line);
            std::stringstream istr(line);
            
            if(istr >> row >> col) {// Checking if the board sizes are valid
                if(row >= 4 && row <= 26 && col >= 4 && col <= 26) {
                    new_game = new ConnectFourDiag(row, col);
                }
                else {
                    std::cout << "Invalid board sizes. Please enter values between 4-26." << std::endl;
                }
            }
            else {
                istr.clear();
                istr.seekg(0);

                if(istr >> input) {
                    if(input == "Y" || input == "y") {// Checking if the input is y
                        new_game = new ConnectFourDiag();
                    }
                    else {
                        std::fstream file(input.c_str(), std::ios::in);// Otherwise tries open the file taken from user

                        if(file.is_open()) {
                            new_game = new ConnectFourDiag(file);
                        }
                        else {
                            std::cout << "Error: Cannot open file " << input << ". Please re-enter" << std::endl;
                        }
                    }
                }
                else {
                    std::cout << "No input provided or input invalid. Please re-enter." << std::endl;
                }
            }
        }
        active_game = new_game; // Pointer of the spesific derived class is assigned to the pointer of active game
    }
    else if(num == 2) {// Same process but for the plus derived class initialization
        ConnectFourPlus* new_game = nullptr;
        
        while(new_game == nullptr) {
            std::cout << "Enter Your Choice (e.g., '6 7' or 'board.txt'):" << std::endl;

            getline(std::cin, line);
            std::stringstream istr(line);
            
            if(istr >> row >> col) {
                if(row >= 4 && row <= 26 && col >= 4 && col <= 26) {
                    new_game = new ConnectFourPlus(row, col);
                }
                else {
                    std::cout << "Invalid board sizes. Please enter values between 4-26." << std::endl;
                }
            }
            else {
                istr.clear();
                istr.seekg(0);

                if(istr >> input) {
                    if(input == "Y" || input == "y") {
                        new_game = new ConnectFourPlus();
                    }
                    else {
                        std::fstream file(input.c_str(), std::ios::in);

                        if(file.is_open()) {
                            new_game = new ConnectFourPlus(file);
                        }
                        else {
                            std::cout << "Error: Cannot open file " << input << ". Please re-enter" << std::endl;
                        }
                    }
                }
                else {
                    std::cout << "No input provided or input invalid. Please re-enter." << std::endl;
                }
            }
        }

        active_game = new_game;
    }
    else if(num == 3) {// Same process but for the plusundo derived class initialization
        ConnectFourPlusUndo* new_game = nullptr;
        
        while(new_game == nullptr) {
            std::cout << "Enter Your Choice (e.g., '6 7' or 'board.txt'):" << std::endl;

            getline(std::cin, line);
            std::stringstream istr(line);
            
            if(istr >> row >> col) {
                if(row >= 4 && row <= 26 && col >= 4 && col <= 26) {
                    new_game = new ConnectFourPlusUndo(row, col);
                }
                else {
                    std::cout << "Invalid board sizes. Please enter values between 4-26." << std::endl;
                }
            }
            else {
                istr.clear();
                istr.seekg(0);

                if(istr >> input) {
                    if(input == "Y" || input == "y") {
                        new_game = new ConnectFourPlusUndo();
                    }
                    else {
                        std::fstream file(input.c_str(), std::ios::in);

                        if(file.is_open()) {
                            new_game = new ConnectFourPlusUndo(file);
                        }
                        else {
                            std::cout << "Error: Cannot open file " << input << ". Please re-enter" << std::endl;
                        }
                    }
                }
                else {
                    std::cout << "No input provided or input invalid. Please re-enter." << std::endl;
                }
            }
        }
        
        active_game = new_game;
    }
    char gamemode;

    while(true) {// Taking the gamemode parameter
        std::cout << "Choose game mode: (P) for two-player, (C) for User vs Computer: ";
        
        // Takes the game mode input and plays the game after
        if(std::cin >> gamemode) {
            if((toupper(gamemode) == 'P') || (toupper(gamemode) == 'C')) {
                break;
            }
            else {
                std::cout << "Invalid input. Please enter 'P' or 'C'." << std::endl;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
        else {
                std::cout << "Invalid input. Please enter 'P' or 'C'." << std::endl;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');                
        }
    }
    
    if(toupper(gamemode) == 'P') {// Mode is assigned to the game
        active_game->set_gamemode(GameMode::PvsP);
    }
    else {
        active_game->set_gamemode(GameMode::PvsC);
    }

    active_game->playGame();// Call to the playing mechansim

    delete active_game; // Destructors called

    return 0;
}