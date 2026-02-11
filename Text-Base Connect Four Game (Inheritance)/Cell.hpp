#ifndef CELL_HPP
#define CELL_HPP

#include <iostream>

namespace cfgame {
    // enum class to store cell state
    enum class CellState {
        EMPTY,
        USER_1,
        USER_2,
        COMP
    };

    // enum class to store cell's symbol
    enum CellSymbol {
        EMPTY = '.',
        USER_1 = 'X',
        USER_2 = 'O',
        NONE = ' ', // for unplayable cell loactions
    };

    // enum class to store the game mode
    enum class GameMode{
        PvsP,
        PvsC
    };

    // class to encapsulate the data of each cell
    class Cell {
        private:
            CellState state; // to which player type the cell belongs to
            char symbol = CellSymbol::NONE; // representation of the cell in console
            int row; // cell's locations
            int column;
            bool is_valid = false; // to determine if the cell is in playable area
        public:
            Cell(); // Default constructor
            Cell(int the_row, int the_col); // Constructor to initialize the cell at a specific location

            // getters
            CellState get_state();
            bool get_validity();

            // setters
            void set_state(CellState  c_state);
            void set_symbol(CellSymbol  c_symbol);
            void set_coordinates(int r, int c);
            void set_validity(bool s);

            // operators overloaded
            friend bool operator==(const Cell & cell1, const Cell & cell2);
            friend std::ostream & operator<<(std::ostream & output, const Cell & cell);
            friend std::istream & operator>>(std::istream & input, Cell & cell);
    }; // class Cell
} // namespace cfgame


#endif