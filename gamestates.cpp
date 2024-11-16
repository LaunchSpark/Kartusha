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

        sort(board, board + 5);//sort board
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
            throw out_of_range("Index out of bounds");
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

    int GetKey(int board[5],bool isEven){
        //convert the board values into digits of a 6 digit long int
        int a = (board[0]*100000)+(board[1]*10000)+(board[2]*1000)+(board[3]*100)+(board[4]*10)+isEven;
        //convert to base 11
        a = convertToBase11(a);
        if(key >= 84830){cout<<"\n\n\n\n\n\n\n\n KEY TOO BIG " << key << "  "; for (size_t i = 0; i < 5; i++){cout << board[i] << ",";}};
        return a;
    };

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


class GameState: public enable_shared_from_this<GameState>{
    private:

    
    bool isEven;
    int  Win; //0 undecided , 1 P1 , -1 P2
    GameBoard board;
    vector<shared_ptr<GameState>> children;
    vector<shared_ptr<GameState>> parents;



    public: // public
    
    // constructors
    GameState(GameBoard board, bool isEven): board(board), isEven(isEven) {}

    void Initialize(array<shared_ptr<GameState>, 84829>& uniqueNodes,long long progressData[2] ) {
        if (!board.isEnd) {
            SpawnChildren(this->board, !isEven, uniqueNodes, progressData);
        }
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

    void ProgressVisual(long long progressData[2]){
         
        int progressinterval = 1000;
        int numUniquesDescovered = progressData[0];
        long long totalNodesProcessed = progressData[1];
        int barWidth = 75;
        int barWidth2 = 75;

            // Periodic progress update every progressInterval nodes
        if (totalNodesProcessed % progressinterval == 0) {
            if (parents.size() != 0){cout << "\x1b[1F\x1b[2K";} // clear the last progress bar if not the root
            double progressPercent = (static_cast<double>(totalNodesProcessed) / 1010101000) * 100;
            double progressPercent2 = (static_cast<double>(numUniquesDescovered) / 262) * 100;
            int pos = static_cast<int>(barWidth * progressPercent / 100);
            int pos2 = static_cast<int>(barWidth2 * progressPercent2 / 100);

            // Display the progress bar
            cout << "Progress:  [";
            for (int i = 0; i < barWidth; ++i) {
                if (i < pos) cout << "=";
                else cout << " ";
            }
            cout << "] " << setw(8) << progressPercent ;
            cout << "%, Nodes Processed: " << totalNodesProcessed;

            cout << "\nUniques:   [";
            for (int i = 0; i < barWidth2; ++i) {
                if (i < pos2) cout << "=";
                else cout << " ";
            }
            cout << "] " << setw(8) << progressPercent2;
            cout << "%, Found: " << numUniquesDescovered;
        }
        return;
    };

    void SpawnChildren(GameBoard board, bool isEven, array<shared_ptr<GameState>, 84829>& uniqueNodes, long long progressData[2] ) {
        
    ProgressVisual(progressData);
    int proposedBoard[5];

    if (!board.isEnd) {
        progressData[1]++;
        for (int j = 0; j < 5; j++) {
            for (int k = 0; k < board[j]; k++) {
                board.CopyBoardTo(proposedBoard);
                proposedBoard[j] = k;

                GameBoard child(proposedBoard, isEven);
                shared_ptr<GameState> newborn;

                switch (IsUnique(child, uniqueNodes)) {
                    case 0: // Not found in unique list
                        newborn = make_shared<GameState>(child, isEven);
                        children.push_back(newborn);
                        uniqueNodes[child.key] = newborn;
                        progressData[0]++;
                        break;

                    case 1: // Already has a conjugate
                        newborn = uniqueNodes[child.conjugateKey];
                        newborn->isEven = !isEven;
                        newborn->UpdateKeys();
                        children.push_back(newborn);
                        break;

                    case 2: // Has been seen
                        newborn = uniqueNodes[child.key];
                        children.push_back(newborn);
                        break;
                    }

                    if (newborn) {
                        newborn->parents.push_back(shared_from_this());
                        // Recursively call SpawnChildren on the new child to expand the tree
                        
                        newborn->SpawnChildren(newborn->board, !isEven, uniqueNodes,progressData);
                    }
                }
            }
        }
    }

    int IsUnique(GameBoard board, const array<shared_ptr<GameState>, 84829>& uniqueNodes) {
        if (uniqueNodes[board.key] != nullptr) return 2;
        if (uniqueNodes[board.conjugateKey] != nullptr) return 1;
        return 0;
    }

    void UpdateKeys(){ // used to update the keys after copying over data
        this->board.UpdateKeys(isEven);return;
    }

};



int main() {
    cout << "\n_____-(Start)-_____\n";

     array<shared_ptr<GameState>, 84829> UniqueNodes{nullptr};
    long long progressData[2]{0,0}; // [0] = uniques found; [1] = total nodes processed
    int startingBoard[5]{1, 2, 3, 4, 5};
    GameBoard rootBoard(startingBoard, true);

    // Create root node
    shared_ptr<GameState> root = make_shared<GameState>(rootBoard, true);

    // Initialize children after construction
    root->Initialize(UniqueNodes,progressData);

    cout << "\n------_(END)_------\n";
    return 0;
}