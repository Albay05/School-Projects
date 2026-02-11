#ifndef CFDIAG_HPP
#define CFPDIAG_HPP

#include <iostream>
#include "CF_Abstract.hpp"

namespace cfgame {
    class ConnectFourDiag : public ConnectFourAbstract {
        private:
            bool check_win(int row, int col, CellState player_state) override; // check_win is overridden as the goal of the game differs
        public:
            ConnectFourDiag();// Default Constructor
            ConnectFourDiag(int row, int col);// Constructor with row and col values
            ConnectFourDiag(std::fstream& file);// Constructor to set the board from file

            ~ConnectFourDiag();
    }; // class ConnectFourDiag
} // namespace cfgame

#endif