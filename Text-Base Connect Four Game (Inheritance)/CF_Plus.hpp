#ifndef CFPLUS_HPP
#define CFPLUS_HPP

#include <iostream>
#include "CF_Abstract.hpp"

namespace cfgame {
    class ConnectFourPlus : public ConnectFourAbstract {
        protected:
            bool check_win(int row, int col, CellState player_state) override;
        public:
            ConnectFourPlus();// Default Constructor
            ConnectFourPlus(int row, int col);// Constructor with row and col values
            ConnectFourPlus(std::fstream& file);// Constructor to set the board from file
            
            ~ConnectFourPlus();
    }; // class ConnectFourPlus
} // namespace cfgame

#endif