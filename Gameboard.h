#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include "Main.h"

struct GameBoard
{
    int  board[5];
    int  key;
    int  display;
    bool isEnd;

    GameBoard() : board{0, 0, 0, 0, 0}, key(0), isEnd(false) {};

    GameBoard(int inBoard[5]){
        
        //coppy in board
        for (int i = 0; i < 5; i++){board[i] = inBoard[i];}
        sort(begin(board),end(board));//sort board
        

        //handles key generation
        key = GetKey(board);
        // cheecks if its a terminal gamestate
        isEnd = (key == END_KEY);   
    }

    GameBoard& operator=(const GameBoard& other) {
        CopyOver(other);
        return *this; // Return *this to allow chained assignments
    }

    GameBoard& operator=(const GameBoard* otherPoint) {
        if (otherPoint == nullptr) {
            throw invalid_argument("Attempt to assign from a nullptr");
        }
        GameBoard other = *otherPoint;
        CopyOver(other);
        return *this; // Return *this to allow chained assignments
    }

    // Overload subscript operator for non-const objects
    int& operator[](size_t index) {
        if (index >= 5) {
            throw out_of_range("Index out of bounds");
        }
        return board[index];
    }

    void CopyBoardTo(int arr[5]){
        for (int i = 0; i < 5; i++) {arr[i] = this->board[i];}
        return;
    }
    
    private:

    // Function to convert a base-6 number to base-10
    int base6ToBase10(int base6Number) {
        int base10Number = 0;
        int power = 0;

        while (base6Number > 0) {
            int digit = base6Number % 10; // Get the last digit
            base6Number /= 10;           // Remove the last digit

            if (digit >= 6) {
                cerr << "Invalid digit " << digit << " in base-6 number.\n";
                return -1; // Indicate error if input is invalid
            }

            base10Number += digit * static_cast<int>(pow(6, power));
            ++power;
        }

        return base10Number;
    }

    int GetKey(int board[5]){
        //convert the board values into digits of a 6 digit long int
        int a = (board[0]*10000)+(board[1]*1000)+(board[2]*100)+(board[3]*10)+(board[4]);
        display = a;
        //convert to base 11
        a = base6ToBase10(a);
        if(key > TABLE_SIZE){cout<<"\n\n\n\n\n\n\n\n KEY TOO BIG " << key << "  "; for (size_t i = 0; i < 5; i++){cout << board[i] << ",";}};
        return a;
    };

    void CopyOver(const GameBoard& other){
            if (this != &other) { // Check for self-assignment
            for (int i = 0; i < 5; i++) {
                board[i] = other.board[i];
            }
            display = other.display;
            key = other.key;
            isEnd = other.isEnd;
        }
    }
};







#endif 