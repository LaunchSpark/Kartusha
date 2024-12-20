#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include "Main.h"

struct GameBoard
{
    int  board[5];
    int  key;
    int  display;
    bool isBegining;

    GameBoard() : board{0, 0, 0, 0, 0}, key(0), isBegining(false) {};

    GameBoard(int inBoard[5]){
        //coppy in board
        for (int i = 0; i < 5; i++){board[i] = inBoard[i];}

        sort(board, board + 5);//sort board
        //handles key generation
        key = GetKey(inBoard);
        // cheecks if its a terminal gamestate
        isBegining = (key == START_KEY);   
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

    int convertToBase13(int number) {
        // Characters used in base 13 representation (0-9, A, B, C)
        const std::string base13Chars = "0123456789ABC";

        // Handle the case where the number is 0
        if (number == 0) {
            return 0;
        }

        // Convert to base 13
        int result = 0;
        int multiplier = 1;

        while (number > 0) {
            int remainder = number % 13;
            result += remainder * multiplier;
            multiplier *= 10;
            number /= 13;
        }

        return result;
    }

    int GetKey(int board[5]){
        //convert the board values into digits of a 6 digit long int
        int a = (board[0]*10000)+(board[1]*1000)+(board[2]*100)+(board[3]*10)+(board[4]);
        this->display = a;
        //convert to base 11
        a = convertToBase13(a);
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
            isBegining = other.isBegining;
        }
    }


};







#endif 