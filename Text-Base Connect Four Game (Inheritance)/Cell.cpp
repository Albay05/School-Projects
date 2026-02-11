#include <iostream>
#include "Cell.hpp"

namespace cfgame {
    Cell::Cell() {
        set_state(CellState::EMPTY);
    }
    
    Cell::Cell(int the_row, int the_col) {
        set_coordinates(the_row, the_col);
        set_state(CellState::EMPTY);
    }



    CellState Cell::get_state() {
        return this->state;
    }

    bool Cell::get_validity() {
        return this->is_valid;
    }



    void Cell::set_state(CellState c_state) {
        this->state = c_state;
    }

    void Cell::set_symbol(CellSymbol c_symbol) {
        this->symbol = c_symbol;
    }

    void Cell::set_coordinates(int r, int c) {
        this->row = r;
        this->column = c;
    }

    void Cell::set_validity(bool s) {
        this->is_valid = s;
    }


    bool operator==(const Cell & cell1, const Cell & cell2) {
        return (cell1.state == cell2.state) && (cell1.symbol == cell2.symbol) && (cell1.is_valid == cell2.is_valid);
    }

    std::ostream & operator<<(std::ostream & output, const Cell & cell) {
        output << cell.symbol;

        return output;
    }

    std::istream & operator>>(std::istream & input, Cell & cell) {
        char s;

        input >> s;
        cell.symbol = s;

        return input;
    }
} // namespace cfgame