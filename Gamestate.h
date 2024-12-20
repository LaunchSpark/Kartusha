#ifndef GAMESTATE_H
#define GAMESTATE_H
#include "Gameboard.h"


class GameState: public enable_shared_from_this<GameState>{
    private:


    bool isEven;
    bool childrenGenerated = false;
    int  Win; //0 undecided , 1 P1 , -1 P2
    GameBoard board;
    vector<shared_ptr<GameState>> children;
    vector<shared_ptr<GameState>> parents;



    public: // public
    
    // prototypes
    // void Initialize(array<shared_ptr<GameState>, TABLE_SIZE>& uniqueNodes,long long progressData[2] );
    // void ProgressVisual(long long progressData[2]);
    // void SpawnChildren(GameBoard board, bool isEven, array<shared_ptr<GameState>, TABLE_SIZE>& uniqueNodes, long long progressData[2] );
    // int  IsUnique(GameBoard board, const array<shared_ptr<GameState>, TABLE_SIZE>& uniqueNodes);
    // void UpdateKeys();

    //setters 
        void setIsEven(bool even){
            this->isEven = even;
        };

        void setWin(int win){this->Win = win;};

    //getters
        GameBoard GetBoard(){
            return this->board;
        }
        
        bool getChildrenGenerated(){
            return this->childrenGenerated;
        }

    // constructors
    GameState(GameBoard board, bool isEven): board(board), isEven(isEven) {}

    // util 

    GameState& operator=(const GameState& other) {
        if(children.size() == 0){return *this;}// Base case:  return *this to allow chained assignments
         this->board  = other.board;
         this->isEven = other.isEven;
         this->children.clear();
        for (size_t i = 0; i < other.children.size(); i++)
        {
            this->children.at(i) = other.children.at(i);
        }
        return *this;
    };

  
    
 

    void SpawnChildren(GameBoard inBoard, bool inIsEven, array<shared_ptr<GameState>, TABLE_SIZE>& uniqueNodes, long long progressData[2]) {
        if (!inBoard.isEnd) {
            int proposedBoard[5];
            cout << "\nPARENT: " << setfill('0') << setw(6) << inBoard.display;
            progressData[1]++;

            for (int j = 0; j < 5; j++) {
                for (int k = 0; k < inBoard[j]; k++) {
                    for (int i = 0; i < 5; i++) { proposedBoard[i] = inBoard[i]; }
                    proposedBoard[j] = k;
                    sort(proposedBoard, proposedBoard + 5);

                    GameBoard child(proposedBoard, inIsEven);
                    if (child.key < 0 || child.key >= TABLE_SIZE) {
                        throw out_of_range("Child key out of range");
                    }

                    shared_ptr<GameState> newborn = nullptr;
                    int uniqueStatus = IsUnique(child, uniqueNodes);
                    if (uniqueStatus == 0) {
                        newborn = make_shared<GameState>(child, inIsEven);
                        uniqueNodes[child.key] = newborn;
                        children.push_back(newborn);
                    } else if (uniqueStatus == 1) {
                        newborn = uniqueNodes[child.conjugateKey];
                        newborn->setIsEven(!inIsEven);
                        uniqueNodes[child.key] = newborn;
                        children.push_back(newborn);
                    } else {
                        newborn = uniqueNodes[child.key];
                        children.push_back(newborn);
                    }

                    if (newborn) {
                        newborn->parents.push_back(shared_from_this());
                    }
                }
            }
        } else {
            cout << "\nTried to spawn children of terminal state";
        }
    }




  
};


#endif 