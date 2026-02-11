#include <iostream>
#include <fstream>
#include <random>
#include "CF_Plus.hpp"

namespace cfgame {

    bool ConnectFourPlus::check_win(int row, int col, CellState player_state) {
        int array[2][2] = {{+1,0},{0,+1}};
        int sum;
        // All 4 directions are stored in the array
        for(int k = 0; k < 2; k++) {
            int count1 = 0, count2 = 0;
            int delta_r = array[k][0], delta_c = array[k][1];
            // Loop for the positive directions
            for(int i = 0; i < 3; i++) {
                int r_new = row + (i+1)*delta_r;
                int c_new = col + (i+1)*delta_c;

                if(r_new >= 0 && r_new < height && c_new >= 0 && c_new < width) {
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

                if(r_new >= 0 && r_new < height && c_new >= 0 && c_new < width) {
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


    ConnectFourPlus::ConnectFourPlus() {
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

    ConnectFourPlus::ConnectFourPlus(int row, int col) {
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

    ConnectFourPlus::ConnectFourPlus(std::fstream& file) {
        readBoard(file); // readBoard function called to handle process
    }
    

    ConnectFourPlus::~ConnectFourPlus() { // Deleting the cell pointers
        for(int i = 0; i < this->height; i++) {
            delete [] gameboard[i];
        }

        delete [] gameboard;
        gameboard = nullptr;
    }
} // namespace cfgame