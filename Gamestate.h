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
    


    //setters 
        void setIsEven(bool even){this->isEven = even;}
        void setWin(int win){this->Win = win;}
        void setChildGenerated(bool a){childrenGenerated = a;}
        void addParent(shared_ptr<GameState> a){ parents.push_back(a);}
        void addChild(shared_ptr<GameState> a){ children.push_back(a);}
             
        
    //getters
        GameBoard GetBoard(){return this->board;}
        int getNumChildren(){return children.size();}
        int getNumParents(){return parents.size();}
        bool getChildrenGenerated(){return this->childrenGenerated;}
        shared_ptr<GameState> getChildAt(int a){
            return children.at(a);
        }    
        

    // constructors
    GameState(GameBoard board): board(board) {}

    // util 
    int expectedNumChildren(){
        int temp = 0;
        int expectedNumChildren = 0;

        for (int i = 0; i < 5; i++){
            if (board[i] != temp){
               expectedNumChildren += temp;
               temp = board[i];
            }
        };
        return expectedNumChildren;
    }

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
 
};


#endif 