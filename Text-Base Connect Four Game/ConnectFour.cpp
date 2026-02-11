#include <iostream>
#include <fstream>
#include <random>
#include "ConnectFour.hpp"

/// \\\ CONNECTFOUR CLASS FUNCTIONS /// \\\ 

void get_file_sizes(int & max_width, int & max_length, std::fstream& file);// Declaring the function before-hand

ConnectFour::ConnectFour() {
    //Default constructor to initialize a basic 5x5 board
    int num_of_rows_and_cols = 5;

    this->num_of_rows = num_of_rows_and_cols;
    this->num_of_cols = num_of_rows_and_cols;
    
    gameboard = new Cell*[this->num_of_rows];

    for(int i = 0; i < this->num_of_rows; i++) {
        gameboard[i] = new Cell[this->num_of_cols];
    }

}

ConnectFour::ConnectFour(int row, int col) {
    // Constructor to initialize a board with given sizes
    gameboard = new ConnectFour::Cell*[row];

    for(int i = 0; i < row; i++) {
        gameboard[i] = new Cell[col];
    }

    this->num_of_rows = row;
    this->num_of_cols = col;
}

ConnectFour::ConnectFour(std::fstream& file) {
    // Constructor to read the board from a file
    if(file.fail()) {
        std::cout << "Error opening file! Terminating the program..." << std::endl;
        exit(0);
    }

    int max_width, max_length;
    get_file_sizes(max_width, max_length, file);
    // Getting the sizes of the file
    this->num_of_cols = max_width;
    this->num_of_rows = max_length;

    gameboard = new ConnectFour::Cell*[max_length];

    for(int i = 0; i < max_length; i++) {
        gameboard[i] = new ConnectFour::Cell[max_width];

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
        if(i >= max_length || j >= max_width) {
            continue;
        }
        if(c == '*') {
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
            gameboard[i][j].set_validity(false);
        }
        else {
            std::cout << "Invalid character ('" << c << "')" << "found in the file! Terminating the program..." << std::endl;
            exit(0);
        }

        j++;
    }

    std::string line;
}

ConnectFour::~ConnectFour() {// Destructor deleting pointers
    for(int i = 0; i < this->num_of_rows; i++) {
            delete [] gameboard[i];
    }

    delete [] gameboard;
    gameboard = nullptr;
}
 
void ConnectFour::playGame() {// Main function to play the game
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

                if((tolower(move) - 'a') < 0 || (tolower(move) - 'a') >= this->num_of_cols) {
                    std::cout << "Please enter a valid column!" << std::endl;
                }
                else {
                    break;
                }
            }

            this->play_u(tolower(move) - 'a', CellState::USER_1);
            std::cout << "Player 1 makes the move at column: " << (char)(toupper(move)) << std::endl;
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

                if((tolower(move) - 'a') < 0 || (tolower(move) - 'a') >= this->num_of_cols) {
                    std::cout << "Please enter a valid column!" << std::endl;
                }
                else {
                    break;
                }
            }

            this->play_u(tolower(move) - 'a', CellState::USER_2);
            std::cout << "Player 2 makes the move at column: " << (char)(toupper(move)) << std::endl;
            }

            else {// Computers move
                this->play_c();
                std::cout << "Computer makes the move!" << std::endl;
            }
            // Flag change
            is_player1_turn = !is_player1_turn;   
        }
    }
    std::cout << "Printing the board one last time." << std::endl;
    this->printBoard();
}

void ConnectFour::play_c() {
    // Array to keep the coordinates of the valid moves
    int valid_moves[num_of_cols][2];
    int num_of_valid_moves = 0;
    int move_row, move_col;

    // Loop for filling in the array of valid moves
    for(int m = 0; m < num_of_cols; m++) {
    
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

    if (num_of_valid_moves == 0) {
        std::cout << "No valid moves left for Computer!" << std::endl;
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

void ConnectFour::play_u(int col, CellState player_state) {
    // Player's move is performed wrt it's state
    if(is_move_possible(col)) {
        for(int r = this->num_of_rows - 1; r >= 0; r--) {
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
        std::cout << "No possible move for column: " << col << ". Please try another one" << std::endl;
    }
}

void ConnectFour::printBoard() const {// Prints the whole board
    char chr = 'A';
    
    for(int i = 0; i < num_of_cols; i++) {
        std::cout << (char)(chr + i);
    }

    std::cout << std::endl;

    for(int i = 0; i < num_of_rows; i++) {
        for(int j = 0; j < num_of_cols; j++) {
            std::cout << gameboard[i][j];
        }
        std::cout << std::endl;
    }
}

void ConnectFour::terminate() {// Terminates the program
    is_game_over = true;
}

bool ConnectFour::check_win(int row, int col, CellState player_state) {
    int array[4][2] = {{+1,0},{0,+1},{+1,+1},{-1,+1}};
    int sum;
    // All 4 directions are stored in the array
    for(int k = 0; k < 4; k++) {
        int count1 = 0, count2 = 0;
        int delta_r = array[k][0], delta_c = array[k][1];
        // Loop for the positive directions
        for(int i = 0;i < 3; i++) {
            int r_new = row + (i+1)*delta_r;
            int c_new = col + (i+1)*delta_c;

            if(r_new >= 0 && r_new < num_of_rows && c_new >= 0 && c_new < num_of_cols) {
                if(gameboard[r_new][c_new].get_validity() == true) {
                    if(gameboard[r_new][c_new].get_state() == player_state)
                    count1++;
                    else
                        break;
                }
                else
                    break;
                
            }
            else
                break;
        }
        // Loop for the negative direction
        for(int i = 0; i < 3; i++) {
            int r_new = row - (i+1)*delta_r;
            int c_new = col - (i+1)*delta_c;

            if(r_new >= 0 && r_new < num_of_rows && c_new >= 0 && c_new < num_of_cols) {
                if(gameboard[r_new][c_new].get_validity() == true) {
                    if(gameboard[r_new][c_new].get_state() == player_state)
                    count2++;
                    else
                        break;
                }
                else
                    break;
                
            }
            else
                break;
        }
        sum = count1 + count2 + 1;
        if(sum >= 4)// Returns true if 4 is connected
            return true;
    }
    return false;
}

bool ConnectFour::is_move_possible(int col) {
    int current_row;
    // Returns true if there is any empty and playable cells in the column
    for(current_row = this->num_of_rows - 1; current_row >= 0; current_row--) {
        if(gameboard[current_row][col].get_state() == CellState::EMPTY &&
           gameboard[current_row][col].get_validity() == true) {

            return true;
           }
    }
    return false;
}

int ConnectFour::possible_row(int col) {
    int current_row;
    // Returns the possible row in the column if there is any
    for(current_row = this->num_of_rows - 1; current_row >= 0; current_row--) {
        if(gameboard[current_row][col].get_state() == CellState::EMPTY &&
           gameboard[current_row][col].get_validity() == true) {

            return current_row;
           }
    }
    return -1;
}

void ConnectFour::set_gamemode(GameMode g_mode) {
    this->mode = g_mode;
}



void get_file_sizes(int & max_width, int & max_length, std::fstream& file) {
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
    max_width = max_w;
    max_length = line_count;

    file.clear();
    file.seekg(0, std::ios::beg);
}