#ifndef CONNECTFOUR_HPP
#define CONNECTFOUR_HPP

#include <iostream>

/*
 * enum class to store cell state
 */
enum class CellState {
    EMPTY,
    USER_1,
    USER_2,
    COMP
};

/*
 * enum class to store the char to represent cell
 */
enum CellSymbol {
    EMPTY = '.',
    USER_1 = 'X',
    USER_2 = 'O',
};

/*
 *enum class to store game mode
 */
enum class GameMode{
    PvsP,
    PvsC
};

/*
 * ConnectFour Class
 */
class ConnectFour {
    private:
        class Cell;// Inner class Cell
        Cell ** gameboard;// Game board as double pointer

        void play_c();// Computer move
        void play_u(int col, CellState player_state);// Player move
        
        void printBoard() const;
        bool check_win(int row, int col, CellState player_state);
        void terminate();
        
        int num_of_rows;// To keep row and column data
        int num_of_cols;
        GameMode mode;// Keeps game mode PvsP or PvsC
        bool is_game_over = false;
    public:
        ConnectFour();// Default Constructor
        ConnectFour(int row, int col);// Constructor with row and col values
        ConnectFour(std::fstream& file);// Constructor set board from file
        
        void readBoard(std::fstream& file);// To read the board from the file
        void playGame();// Main playing algorithm
        
        bool is_move_possible(int col);// Checks if there is any empty cell in the column
        int possible_row(int col);// Finds the empty cell in the column
        void set_gamemode(GameMode g_mode);// To set the game mode

        ~ConnectFour();// Destructor to delete pointers
};

/*
 * Cell Class
 */
class ConnectFour::Cell {
    private:
        CellState cell_state;
        char symbol;// Caharcter to show in the console
        int pos_row;// Row and column values of the cell
        int pos_col;
        bool is_valid = false;// To determine if the cell is a playable area
    public:
        Cell();// Default constructor
        Cell(int row, int col);// Constructor to initialize the cell at a specific location

        // Equivalence operator overloaded
        friend bool operator==(const ConnectFour::Cell& cell1, const ConnectFour::Cell& cell2) {
            return cell1.cell_state == cell2.cell_state;
        }

        ConnectFour::Cell& operator++();// Prefix operator overloaded

        ConnectFour::Cell& operator++(int) {// Postfix operator overloaded
            ConnectFour::Cell* temp = this;
            if(this->get_state() == CellState::EMPTY)
                this->set_state(CellState::USER_1);
            else if(this->get_state() == CellState::USER_1)
                this->set_state(CellState::USER_2);
            else if(this->get_state() == CellState::USER_2)
                this->set_state(CellState::COMP);
            else
                this->set_state(CellState::EMPTY);

            return *temp;
        }
        
        // << opertor overloaded to be able print it into streams
        friend std::ostream& operator<<(std::ostream& output, ConnectFour::Cell& cell) {
            output << cell.symbol;
            return output;
        }
        // >> operator overloaded to be able insert a caharacter to a cell
        friend std::istream& operator>>(std::istream& input, ConnectFour::Cell& cell) {
            input >> cell.symbol;
            return input;
        }


        void set_state(CellState state);// To set the state of the cell

        void set_symbol(CellSymbol c) {
            this->symbol = c;
        }

        void set_validity(bool param) {
            this->is_valid = param;
        }
        bool get_validity() {
            return this->is_valid;
        }

        void set_coordinates(int row, int col);

        CellState get_state() {
            return this->cell_state;
        }
};

#endif