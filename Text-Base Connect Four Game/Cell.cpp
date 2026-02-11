#include <iostream>
#include "ConnectFour.hpp"

/// \\\ CELL CLASS FUNCTIONS /// \\\ 

ConnectFour::Cell::Cell() {// Default constructor initialize an emty cell
    set_state(CellState::EMPTY);
    set_validity(true);
    this->symbol = CellSymbol::EMPTY;
}

ConnectFour::Cell::Cell(int row, int col) {// To initialize an empty cell at given location
    set_coordinates(row, col);
    set_state(CellState::EMPTY);
    set_validity(true);
    this->symbol = CellSymbol::EMPTY;

}
/*
 * Prefix operator
 */
ConnectFour::Cell& ConnectFour::Cell::operator++() {// Changing the state of the cell by incrementation
    if(this->get_state() == CellState::EMPTY)
        this->set_state(CellState::USER_1);
    else if(this->get_state() == CellState::USER_1)
        this->set_state(CellState::USER_2);
    else if(this->get_state() == CellState::USER_2)
        this->set_state(CellState::COMP);
    else
        this->set_state(CellState::EMPTY);

    return *this;
}

void ConnectFour::Cell::set_state(CellState state) {// Setting the state of the cell as th given
    this->cell_state = state;

    if(state == CellState::EMPTY)
        this->symbol = CellSymbol::EMPTY;
    else if(state == CellState::USER_1)
        this->symbol = CellSymbol::USER_1;
    else
        this->symbol = CellSymbol::USER_2;
}

void ConnectFour::Cell::set_coordinates(int row, int col) {// Setting the coordinate parameters of the cell
    if(row < 0 || col < 0 ) {
        std::cout << "Invalid row or column data! Exiting the program..." << std::endl;
        exit(0);
    }
    this->pos_row = row;
    this->pos_col = col;
}