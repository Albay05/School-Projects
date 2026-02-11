#include <iostream>
#include <fstream>
#include <random>
#include "CF_Abstract.hpp"

namespace cfgame {
    void get_file_sizes(int & max_width, int & max_height, std::fstream& file); // Fore-hand declaration

    void ConnectFourAbstract::printBoard() {
        char chr = 'A';
    
        for(int i = 0; i < width; i++) { // Prints the column characters
            std::cout << (char)(chr + i);
        }

        std::cout << std::endl;

        for(int i = 0; i < height; i++) { // Prints the board
            for(int j = 0; j < width; j++) {
                std::cout << gameboard[i][j];
            }
            std::cout << std::endl;
        }
    }

    void ConnectFourAbstract::terminate() {// Terminates the game
        is_game_over = true;
    }


    void ConnectFourAbstract::play() {
        // Array to keep the coordinates of the valid moves
        int valid_moves[this->width][2];
        int num_of_valid_moves = 0;
        int move_row, move_col;

        // Loop for filling in the array of valid moves
        for(int m = 0; m < this->width; m++) {
        
            int probable_row = possible_row(m);

            if(probable_row != -1) {
                valid_moves[num_of_valid_moves][0] = probable_row;
                valid_moves[num_of_valid_moves][1] = m;
                num_of_valid_moves++;
            }
        }

        // Loop for the defence mechanism
        for(int i = 0; i < num_of_valid_moves; i++) {
            int temp_row = valid_moves[i][0];
            int temp_col = valid_moves[i][1];

            gameboard[temp_row][temp_col].set_state(CellState::USER_2);

            if(check_win(temp_row, temp_col, CellState::USER_2)) {
                gameboard[temp_row][temp_col].set_state(CellState::COMP);
                gameboard[temp_row][temp_col].set_symbol(CellSymbol::USER_2);
                // If the computer wins, then message is written and game is terminated
                if(check_win(temp_row, temp_col, CellState::COMP)) {
                    std::cout << "The computer has won, good luck next time!" << std::endl;
                    this->terminate();
                }
                return;
            }
            else
                gameboard[temp_row][temp_col].set_state(CellState::EMPTY);
        }

        // Loop for the attack mechanism
        for(int i = 0; i < num_of_valid_moves; i++) {
            int temp_row = valid_moves[i][0];
            int temp_col = valid_moves[i][1];

            gameboard[temp_row][temp_col].set_state(CellState::COMP);

            if(check_win(temp_row, temp_col, CellState::COMP)) {
                gameboard[temp_row][temp_col].set_symbol(CellSymbol::USER_2);
                // If the computer wins, then message is written and game is terminated
                if(check_win(temp_row, temp_col, CellState::COMP)) {
                    std::cout << "The computer has won, good luck next time!" << std::endl;
                    this->terminate();
                }
                return;
            }
            else
                gameboard[temp_row][temp_col].set_state(CellState::EMPTY);
        }

        if (num_of_valid_moves == 0) {// If no moves left then prints message and terminates
            std::cout << "No valid moves left for Computer!" << std::endl;
            this->terminate();
            return;
        }

        // If no defence or attack is critically needed system chooses the move randomly
        std::random_device rd;
        std::mt19937 generator(rd());

        std::uniform_int_distribution<int> distribution(0, num_of_valid_moves - 1);

        int random_move = distribution(generator);

        int random_row = valid_moves[random_move][0];
        int random_col = valid_moves[random_move][1];

        gameboard[random_row][random_col].set_state(CellState::COMP);
        gameboard[random_row][random_col].set_symbol(CellSymbol::USER_2);

        // If the computer wins, then message is written and game is terminated
        if(check_win(random_row, random_col, CellState::COMP)) {
            std::cout << "The computer has won, good luck next time!" << std::endl;
            this->terminate();
        }

        return;
    }

    void ConnectFourAbstract::play(int col, CellState player_state) {
        // Player's move is performed wrt it's state
        if(is_move_possible(col)) {
            for(int r = this->height - 1; r >= 0; r--) {
                if(gameboard[r][col].get_state() == CellState::EMPTY &&
                gameboard[r][col].get_validity() == true)
                {
                    gameboard[r][col].set_state(player_state);

                    if(player_state == CellState::USER_1)
                        gameboard[r][col].set_symbol(CellSymbol::USER_1);
                    else
                        gameboard[r][col].set_symbol(CellSymbol::USER_2);
                    
                    if(check_win(r, col, player_state)) {
                        if(player_state == CellState::USER_1)
                            std::cout << "Player 1 (X) won!" << std::endl;
                        else
                            std::cout << "Player 2 (O) won!" << std::endl;

                        this->terminate();
                    }
                    break;
                }
            }
        }
        else {// Message is printed if no possible moves
            std::cout << "No possible move for column: " << (char)('A' + col) << ". Please try another one" << std::endl;
        }
    }


    void ConnectFourAbstract::playGame() {
        bool is_player1_turn = true;

        while(!is_game_over) {
            // Keeps the game running until one has won
            this->printBoard();

            if(is_player1_turn) {// Player 1's move
                char move;

                while(true) {// Taking input from user
                    std::cout << "Player 1's turn (X)." << std::endl;
                    std::cout << "Please enter the column letter to play: ";
                    std::cin >> move;

                    if((tolower(move) - 'a') < 0 || (tolower(move) - 'a') >= this->width) {
                        std::cout << "Please enter a valid column!" << std::endl;
                        std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    }
                    else {
                        break;
                    }
                }

                std::cout << "Player 1 makes the move at column: " << (char)(toupper(move)) << std::endl;
                this->play(tolower(move) - 'a', CellState::USER_1);
                // Switching the flag
                is_player1_turn = !is_player1_turn;
            }

            else {// Second Plaeyrs move
                if(this->mode == GameMode::PvsP) {// PLayer 2's move same as the first
                    char move;

                while(true) {
                    std::cout << "Player 2's turn (O)." << std::endl;
                    std::cout << "Please enter the column letter to play: ";
                    std::cin >> move;

                    if((tolower(move) - 'a') < 0 || (tolower(move) - 'a') >= this->width) {
                        std::cout << "Please enter a valid column!" << std::endl;
                        std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    }
                    else {
                        break;
                    }
                }

                std::cout << "Player 2 makes the move at column: " << (char)(toupper(move)) << std::endl;
                this->play(tolower(move) - 'a', CellState::USER_2);
                }

                else {// Computers move
                    std::cout << "Computer makes the move!" << std::endl;
                    this->play();
                }
                // Flag change
                is_player1_turn = !is_player1_turn;   
            }
        }
        std::cout << "Printing the board one last time." << std::endl;
        this->printBoard();

        char choice;

        // Asks the user if s/he wants to write final state of the board to a file
        std::cout << std::endl << "Do you want to print the final board layout to a file before terminating ('Y' or 'N'): ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if(std::cin >> choice && toupper(choice) == 'Y' || toupper(choice) == 'N') {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            
            if(toupper(choice) == 'Y') { // Prints the board to final_board.txt
                std::string f_name = "final_board.txt";
                std::ofstream fstr(f_name);

                printToFile(fstr);
                fstr.close();

                std::cout << "Board printed to " << f_name << "." << std::endl;
            }
            else if(toupper(choice) == 'N') {
                std::cout << "Terminating the program..." << std::endl;
            }
            
        }
        else {
            std::cout << "Invalid input. Terminating the program..." << std::endl;
        }
    }


    void ConnectFourAbstract::printToFile(std::ostream & file) {
        char chr = 'A';
    
        for(int i = 0; i < width; i++) { // The col letters are printed
            file << (char)(chr + i);
        }

        file << std::endl;

        for(int i = 0; i < height; i++) { // The board is printed to file
            for(int j = 0; j < width; j++) {
                file << gameboard[i][j];
            }
            file << std::endl;
        }
    }

    void ConnectFourAbstract::readBoard(std::fstream & file) {
        if(file.fail()) { // Terminates the program if cannot open the file
            std::cout << "Error opening file!" << std::endl;
            std::cout << "Press enter to terminate the program: ";
            std::cin.ignore();
            std::cin.get();
            exit(0);
        }
        int max_width, max_height;
        get_file_sizes(max_width, max_height, file);
        // Getting the sizes of the file
        this->width = max_width;
        this->height = max_height;

        gameboard = new Cell*[max_height];
        
        for(int i = 0; i < max_height; i++) {
            gameboard[i] = new Cell[max_width];

            for(int j = 0; j < max_width; j++) {
                gameboard[i][j].set_validity(false); 
            }
        }

        int i = 0, j = 0;
        char c;
        // Loop to read the file
        while((c = file.get()) != EOF) {
            if(c == '\r') {
                continue;
            }
            if(c == '\n') {
                i++;
                j = 0;
                continue;
            }
            if(i >= max_height || j >= max_width) {
                continue;
            }
            if(c == '*') { // Symbols are assigned according to the what it is represented with in the file
                gameboard[i][j].set_coordinates(i,j);
                gameboard[i][j].set_state(CellState::EMPTY);
                gameboard[i][j].set_validity(true);
                gameboard[i][j].set_symbol(CellSymbol::EMPTY);
            }
            else if(c == 'X') {
                gameboard[i][j].set_coordinates(i,j);
                gameboard[i][j].set_state(CellState::USER_1);
                gameboard[i][j].set_validity(true);
                gameboard[i][j].set_symbol(CellSymbol::USER_1);
            }
            else if(c == 'O') {
                gameboard[i][j].set_coordinates(i,j);
                gameboard[i][j].set_state(CellState::USER_2);
                gameboard[i][j].set_validity(true);
                gameboard[i][j].set_symbol(CellSymbol::USER_2);
            }
            else if(c == ' ') {
                gameboard[i][j].set_coordinates(i,j);
                gameboard[i][j].set_state(CellState::EMPTY);
                gameboard[i][j].set_symbol(CellSymbol::NONE);
            }
            else {
                std::cout << "Invalid character ('" << c << "')" << "found in the file!" << std::endl;
                std::cout << "Press enter to terminate the program: ";
                std::cin.ignore();
                std::cin.get();
                exit(0);
            }

            j++;
        } // while loop
    } // readBoard function

    int ConnectFourAbstract::getWidth() {
        return this->width;
    }

    int ConnectFourAbstract::getHeight() {
        return this->height;
    }

    void get_file_sizes(int & max_width, int & max_height, std::fstream& file) {
        std::string current_line;
        int max_w = 0, line_count = 0;
        // Reads the whole file and updates the size data
        file.clear();
        file.seekg(0, std::ios::beg);

        while(getline(file, current_line)) {
            if(current_line.size() == 0)
                continue;

            line_count++;
            if(current_line.size() > max_w)
                max_w = current_line.size();
        }

        if(max_w < 4 || max_w > 26 || line_count < 4 || line_count > 26) { // Exits if the file is too large to be represented by the english alphabet
            std::cout << "Invalid board configuration in the file, please make sure the board sizes are in between 4 - 26." << std::endl;
            std::cout << "Press enter to terminate the program: ";
            std::cin.ignore();
            std::cin.get();
            exit(0);
        }

        max_width = max_w;
        max_height = line_count;

        file.clear();
        file.seekg(0, std::ios::beg);
    }

    bool ConnectFourAbstract::is_move_possible(int col) {
        int current_row;
        // Returns true if there is any empty and playable cells in the column
        for(current_row = this->height - 1; current_row >= 0; current_row--) {
            if(gameboard[current_row][col].get_state() == CellState::EMPTY &&
            gameboard[current_row][col].get_validity() == true) {

                return true;
            }
        }
        return false;
    }

    int ConnectFourAbstract::possible_row(int col) {
        int current_row;
        // Returns the possible row in the column if there is any
        for(current_row = this->height - 1; current_row >= 0; current_row--) {
            if(gameboard[current_row][col].get_state() == CellState::EMPTY &&
            gameboard[current_row][col].get_validity() == true) {

                return current_row;
            }
        }
        return -1;
    }

    ConnectFourAbstract::~ConnectFourAbstract() {
        // Intentionally left blank as the derived destructors handle the job
    }
} // namespace cfgame