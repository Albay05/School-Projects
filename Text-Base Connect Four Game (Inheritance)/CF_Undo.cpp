#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <vector>
#include "CF_Undo.hpp"
#include "Cell.hpp"

namespace cfgame {
    void ConnectFourPlusUndo::play(std::fstream & move_file) {
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
                pushMove(temp_row, temp_col, CellState::COMP, move_file);
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
                pushMove(temp_row, temp_col, CellState::COMP, move_file);
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

        if (num_of_valid_moves == 0) { // If no moves left then prints message and terminates
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
        pushMove(random_row, random_col, CellState::COMP, move_file);

        // If the computer wins, then message is written and game is terminated
        if(check_win(random_row, random_col, CellState::COMP)) {
            std::cout << "The computer has won, good luck next time!" << std::endl;
            this->terminate();
        }

        return;
    }

    void ConnectFourPlusUndo::play(int col, CellState player_state, std::fstream & move_file) {
        // Player's move is performed wrt it's state and pushed to the stack in the file
        if(is_move_possible(col)) {
            for(int r = this->height - 1; r >= 0; r--) {
                if(gameboard[r][col].get_state() == CellState::EMPTY &&
                gameboard[r][col].get_validity() == true)
                {
                    gameboard[r][col].set_state(player_state);
                    pushMove(r, col, player_state, move_file);

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


    void ConnectFourPlusUndo::pushMove(int row, int col, CellState c_state, std::fstream & move_file) {
        // The moves are pushed to the stack as row col and the enumerated digit state data
        if(c_state == CellState::COMP) {
            move_file << row << " " << col << " " << 0 << std::endl;
        }
        else if(c_state == CellState::USER_1) {
            move_file << row << " " << col << " " << 1 << std::endl;
        }
        else if(c_state == CellState::USER_2) {
            move_file << row << " " << col << " " << 2 << std::endl;
        }
    }

    void ConnectFourPlusUndo::undoLastMove(const std::string & file_name) {
        std::vector<std::string> lines;

        if(this->mode == GameMode::PvsC) { // Undo mechanism for PvsC
            int line_count = 0;
            std::string line;
            std::fstream move_file(file_name);

            while(true) { // Reads the lines in the file
                if (std::getline(move_file, line)){
                    lines.push_back(line);
                }
                else break;
            }

            move_file.close();

            if(lines.size() < 2) { // Returns if there are not enough moves to undo
                std::cout << "There are no moves enough to undo!" << std::endl;
                return;
            }

            int r, c, m;

            for(int i = 0; i < 2 ; i++) { // Done twice as both the computer and player's moves are to be popped
                std::stringstream ss(lines[lines.size() - 1]);
                if(ss >> r >> c >> m) {
                    // The data of the cell is reset to epty and the move is popped from the stack
                    this->gameboard[r][c].set_state(CellState::EMPTY);
                    this->gameboard[r][c].set_symbol(CellSymbol::EMPTY);
                    
                    lines.pop_back();
                }
                else {
                    std::cout << "Invalid configuration in move file, cannot undo." << std::endl;
                    return;
                }
            }
            
            std::ofstream output_file(file_name);

            for(int i = 0; i < lines.size(); i++) { // The moves (with the last popped) are written back to file 
                output_file << lines[i];
                if(i < lines.size() - 1)
                    output_file << "\n";
            }

            output_file.close();
        }
        else { // Same process for PvsP
            int line_count = 0;
            std::string line;
            std::fstream move_file(file_name);

            while(std::getline(move_file, line)) {
                lines.push_back(line);
            }

            move_file.close();

            if(lines.size() < 1) {
                std::cout << "There are no moves enough to undo!" << std::endl;
                return;
            }            

            int r, c, m;
            
            std::stringstream ss(lines[lines.size() - 1]);
            if(ss >> r >> c >> m) { // Done once as only the last players move is to be popped
                if(m == 1 || m == 2) {
                    this->gameboard[r][c].set_state(CellState::EMPTY);
                    this->gameboard[r][c].set_symbol(CellSymbol::EMPTY);
                }
                lines.pop_back();
            }
            else {
                std::cout << "Invalid configuration in move file, cannot undo." << std::endl;
                return;
            }
            
            std::ofstream output_file(file_name);

            for(int i = 0; i < lines.size(); i++) {
                output_file << lines[i];
                if(i < lines.size() - 1)
                    output_file << "\n";
            }

            output_file.close();
        }
    }


    ConnectFourPlusUndo::ConnectFourPlusUndo() {

        //Default constructor to initialize a basic 5x5 board
        int num_of_rows_and_cols = 5;

        this->width = num_of_rows_and_cols;
        this->height = num_of_rows_and_cols;
        
        gameboard = new Cell*[this->width];

        for(int i = 0; i < this->width; i++) {
            gameboard[i] = new Cell[this->height];
        }

        for(int i = 0; i < this->height; i++) {
            for(int j = 0; j < this->width; j++) {
                gameboard[i][j].set_symbol(CellSymbol::EMPTY);
                gameboard[i][j].set_validity(true);
            }
        }
    }

    ConnectFourPlusUndo::ConnectFourPlusUndo(int row, int col) {

        // Constructor to initialize a board with given sizes
        gameboard = new Cell*[row];

        for(int i = 0; i < row; i++) {
            gameboard[i] = new Cell[col];
        }

        this->height = row;
        this->width = col;

        for(int i = 0; i < this->height; i++) {
            for(int j = 0; j < this->width; j++) {
                gameboard[i][j].set_symbol(CellSymbol::EMPTY);
                gameboard[i][j].set_validity(true);
            }
        }
    }

    ConnectFourPlusUndo::ConnectFourPlusUndo(std::fstream &file) {
        readBoard(file); // readBoard function called to handle process
    }


    void ConnectFourPlusUndo::playGame() {

        bool is_player1_turn = true;
        bool move_is_an_undo = false;
        std::string filename = "moves.txt";
        std::fstream m_file(filename, std::ios::out);

        while(!is_game_over) {
            // Keeps the game running until one has won
            this->printBoard();

            if(is_player1_turn) {// Player 1's move
                std::string input;

                while(true) {// Taking input from user
                    std::cout << "Player 1's turn (X)." << std::endl;
                    std::cout << "Please enter the column letter to play or type 'undo' to undo the last move: ";
                    std::cin >> input;

                    if(input == "undo" || input == "Undo" || input == "UNDO") {
                        move_is_an_undo = true; // Flag is switched if the input is an undo
                        break;
                    }
                    else if(input.size() == 1 && (input[0] - 'a' >= 0 && input[0] - 'a' < this->width)) {
                        move_is_an_undo = false;
                        break;
                    }
                    else {
                        std::cout << "Please provide a valid input!" << std::endl;
                    }
                }
                if(!move_is_an_undo) {
                    std::cout << "Player 1 makes the move at column: " << (char)(toupper(input[0])) << std::endl;
                    this->play(input[0] - 'a', CellState::USER_1, m_file);
                }
                else { // Undoes the last move
                    m_file.close();
                    undoLastMove(filename);
                    m_file.open(filename, std::fstream::out | std::fstream::app);
                    std::cout << "Player 1 undoes the last move." << std::endl;
                }
                
                // Switching the flag
                is_player1_turn = !is_player1_turn;
            }

            else {// Second Players move
                if(this->mode == GameMode::PvsP) {// PLayer 2's move same as the first
                    std::string input;

                    while(true) {// Taking input from user
                        std::cout << "Player 2's turn (X)." << std::endl;
                        std::cout << "Please enter the column letter to play or type 'undo' to undo the last move: ";
                        std::cin >> input;

                        if(input == "undo" || input == "Undo" || input == "UNDO") {
                            move_is_an_undo = true; // Flag is switched if the input is an undo
                            break;
                        }
                        else if(input.size() == 1 && (input[0] - 'a' >= 0 && input[0] - 'a' < this->width)) {
                            move_is_an_undo = false;
                            break;
                        }
                        else {
                            std::cout << "Please provide a valid input!" << std::endl;
                        }
                    }
                    if(!move_is_an_undo) {
                        std::cout << "Player 2 makes the move at column: " << (char)(toupper(input[0])) << std::endl;
                        this->play(input[0] - 'a', CellState::USER_2, m_file);
                    }
                    else { // Undoes the last move
                        m_file.close();
                        undoLastMove(filename);
                        m_file.open(filename, std::fstream::out | std::fstream::app);
                        std::cout << "Player 2 undoes the last move." << std::endl;
                    }    
                }

                else {// Computers move
                    std::cout << "Computer makes the move!" << std::endl;
                    this->play(m_file);
                }
                // Flag change
                is_player1_turn = !is_player1_turn;   
            }
        }
        // Prints the board one final time
        std::cout << "Printing the board one last time." << std::endl;
        this->printBoard();

        char choice;

        // Asks the user if s/he wants to write final state of the board to a file
        std::cout << std::endl << "Do you want to print the final board layout to a file before terminating ('Y' or 'N'): ";
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

    ConnectFourPlusUndo::~ConnectFourPlusUndo() { // Deleting the cell pointers
        for(int i = 0; i < this->height; i++) {
            delete [] gameboard[i];
        }

        delete [] gameboard;
        gameboard = nullptr;
    }
} // namespace cfgame