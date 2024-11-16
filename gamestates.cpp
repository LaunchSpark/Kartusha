#include <iostream>
#include <vector>
#include <algorithm>
#include <array>
#include <sstream>
#include <iomanip>
#include <fstream>



using namespace std;



 const int WIN_KEY_ODD = 0;
 const int WIN_KEY_EVEN = 1;



struct GameBoard
{
    int  board[5];
    int  key;
    int  conjugateKey;
    bool isEnd;

    GameBoard() : board{0, 0, 0, 0, 0}, key(0), isEnd(false) {};

    GameBoard(int inBoard[5],bool inIsEven){
        //coppy in board
        for (int i = 0; i < 5; i++){board[i] = inBoard[i];}
        //handles key generation
        key = GetKey(inBoard,inIsEven);
        conjugateKey = GetKey(inBoard,!inIsEven);
        // cheecks if its a terminal gamestate
        isEnd = ((key == WIN_KEY_EVEN) || (key == WIN_KEY_ODD));   
    }

    GameBoard& operator=(const GameBoard& other) {
        CopyOver(other);
        return *this; // Return *this to allow chained assignments
    }

    GameBoard& operator=(const GameBoard* otherPoint) {
        GameBoard other = *otherPoint;
        CopyOver(other);
        return *this; // Return *this to allow chained assignments
    }

    // Overload subscript operator for non-const objects
    int& operator[](size_t index) {
        if (index >= 5) {
            throw std::out_of_range("Index out of bounds");
        }
        return board[index];
    }

    void CopyBoardTo(int arr[5]){
        for (int i = 0; i < 5; i++) {arr[i] = board[i];}
        return;
    }
    
    void UpdateKeys(bool isEven){
        //handles key generation
        key = GetKey(board,isEven);
        conjugateKey = GetKey(board,!isEven);
        // cheecks if its a terminal gamestate
        isEnd = ((key == WIN_KEY_EVEN) || (key == WIN_KEY_ODD));  
    }



    private:
    int convertToBase11(int num);

    int GetKey(int board[5],bool isEven){
        //convert the board values into digits of a 6 digit long int
        int a = (board[0]*1000000)+(board[1]*100000)+(board[2]*10000)+(board[3]*1000)+(board[4]*100)+(board[5]*10)+isEven;
        //convert to base 11
        a = convertToBase11(a);
        return a;
    };

    // Function to convert a decimal number to base 11 
    int convertToBase11(int num) {
        if (num == 0) return 0;  // Special case for 0

        int base11 = 0;
        int placeValue = 1; // Represents the positional multiplier (1, 10, 100, etc.)

        while (num > 0) {
            int remainder = num % 11; // Get the remainder
            base11 += remainder * placeValue; // Construct the base 11 integer
            placeValue *= 10; // Move to the next place value
            num /= 11; // Reduce the number
        }

        return base11;
    }

    void CopyOver(const GameBoard& other){
            if (this != &other) { // Check for self-assignment
            for (int i = 0; i < 5; i++) {
                board[i] = other.board[i];
            }
            key = other.key;
            isEnd = other.isEnd;
            conjugateKey = other.conjugateKey;
        }
    }


};


class GameState{
    private:

    
    bool isEven;
    int  Win; //0 undecided , 1 P1 , -1 P2
    long long progressData[2]{0,0}; // [0] = uniques found; [1] = total nodes processed
    GameBoard board;
    vector<GameState*> children;
    vector<GameState*> parents;


    public: // public

    GameState(GameBoard board, bool isEven,GameState* uniqueNodes[84829],GameState* parent, long long progressData[2]){
        this->board = board;
        this->isEven = isEven;
        this->progressData[0] = progressData[0];
        this->progressData[1] = progressData[1];
        this->parents.push_back(parent);
        if(!board.isEnd){SpawnChildren(this->board, !isEven, uniqueNodes);};
    };

    GameState& operator=(const GameState& other) {
        if(children.size() == 0){return *this;}// Base case:  return *this to allow chained assignments
         this->board  = other.board;
         this->isEven = other.isEven;
         this->children.clear();
        for (size_t i = 0; i < other.children.size(); i++)
        {
            this->children.at(i) = other.children.at(i);
        }
    };

    void ProgressVisual(){
        int progressinterval = 1000;
        int numUniquesDescovered = progressData[0];
        long long totalNodesProcessed = progressData[1];

            // Periodic progress update every progressInterval nodes
        if (totalNodesProcessed % progressinterval == 0) {
            double progressPercent = (static_cast<double>(totalNodesProcessed) / 1010101000) * 100;
            int barWidth = 75;
            int pos = static_cast<int>(barWidth * progressPercent / 100);

            // Display the progress bar
            cout << "\nProgress: [";
            for (int i = 0; i < barWidth; ++i) {
                if (i < pos) cout << "=";
                else cout << " ";
            }
            cout << "] " << setw(8) << progressPercent ;
            cout << "%, Nodes Processed: " << totalNodesProcessed;
        }
        return;
    };

    void SpawnChildren(GameBoard board,bool isEven,GameState* uniqueNodes[84829]){
        GameState* newborn;
        int proposedBoard[5];
        if (board.isEnd) {
            for (int j = 0; j < 5; j++) { // each row in the game
                for (int k = 0; k < board[j]; k++) { 
                    // simulates a turn of the game to generate permutations
                    board.CopyBoardTo(proposedBoard);
                    proposedBoard[j] = k;

                        // logic for each generated board
                        GameBoard child(proposedBoard,isEven);
                        switch (IsUnique(child,uniqueNodes))
                        {
                        case 0: // not found in unique list
                            newborn = new GameState(child,isEven,uniqueNodes,this,progressData);// make new gamestate
                            children.push_back(newborn);// add it to the vector of children
                            uniqueNodes[child.key] = newborn;// add it to children and too the unique list
                            delete newborn;// release the memory
                            progressData[0]++;// update number of uniques found for progress tracking reasons
                            break;
                        case 1: // already has a conjugate
                            // coppy the existing conjugate and change its isEven status
                            newborn = uniqueNodes[child.conjugateKey]; // copy over the data from the already existing one
                            newborn->isEven = !isEven; //invert its isEven
                            newborn->UpdateKeys();// update its keys

                            uniqueNodes[child.conjugateKey] = newborn; // add it to the list of uniques
                            newborn->parents.push_back(this); // add this as a parent of the new child
                            children.push_back(newborn);
                            
                            break;
                        case 2: // has been seen
                            uniqueNodes[child.key]->parents.push_back(this); // add this node as one of its parents
                            children.push_back(uniqueNodes[child.key]);// add it to the list of children
                            break;
                        }
                }
            }
            progressData[1]++;
            ProgressVisual();
        }
    };

    int IsUnique(GameBoard board,GameState* uniqueNodes[84829]){
        if ( uniqueNodes[board.key] != nullptr){return 2;} // found
        else if (uniqueNodes[board.conjugateKey] != nullptr){return 1;} // conjucate found but not itself
        else {return 0;} // not found
    };

    void UpdateKeys(){ // used to update the keys after copying over data
        this->board.UpdateKeys(isEven);return;
    }

    void Delete(){
        // recursive step
        for (size_t i = 0; i < this->children.size(); i++){this->children.at(i)->Delete();}// if children loop though and delete
        // base case
        if (this->board.isEnd||(this->children.size() == 0)){// if it has no children or if its an end state
            for (size_t i = 0; i < this->parents.size(); i++){//for each parent
                for (size_t j = 0; j < this->parents.at(i)->children.size(); j++){
                    //find this element in parents' children vector 
                    if (this->parents.at(i)->children.at(j)->board.key = this->board.key){
                        delete this->parents.at(i)->children[j];// delete self
                        this->parents.at(i)->children.erase(this->parents.at(i)->children.begin() + j);// erase from vector
            }}}
            // clear out parents
            for (size_t i = 0; i < this->parents.size(); i++)
            {
                delete this->parents[i];
            }
            this->parents.clear();
            return;
        };
    }

    ~GameState(){//release the memory!!
        Delete();
    }
};


int main(){

        GameState* UniqueNodes[84829] {nullptr};
        GameState* winState;
        GameState* bestMove;
        GameState* root;


        


    return 0;
};








