#include <iostream>
#include <vector>
#include <memory>
#include <unordered_set>
#include <algorithm>
#include <array>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <unordered_set>
#include <array>
#include <algorithm>


using namespace std;






// Custom hash function for arrays of size 5
struct GameStateHash {
   size_t operator()(const array<int, 5>& board) const {
       // Sort the board to treat permutations of numbers as equivalent
       array<int, 5> sortedBoard = board;
       std::sort(sortedBoard.begin(), sortedBoard.end());


       // Generate a hash
       size_t hash = 0;
       for (int num : sortedBoard) {
           hash ^= std::hash<int>()(num) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
       }
       return hash;
   }
};


// Custom equality comparison for arrays of size 5
struct GameStateEqual {
   bool operator()(const array<int, 5>& a, const array<int, 5>& b) const {
       array<int, 5> sortedA = a;
       array<int, 5> sortedB = b;
       std::sort(sortedA.begin(), sortedA.end());
       std::sort(sortedB.begin(), sortedB.end());
       return sortedA == sortedB;
   }
};




struct GameState;


void expandGameStates(vector<shared_ptr<GameState> >& initialStates,
                     vector<shared_ptr<GameState> >& winStates,
                     unordered_set<array<int, 5>, GameStateHash, GameStateEqual>& uniqueStates);


vector<shared_ptr<GameState> > initUnique(vector<shared_ptr<GameState> >& winStates);
void prune(shared_ptr<GameState> target);


struct GameState : enable_shared_from_this<GameState> {
   array<int, 5> board;
   bool isEvenTurn;
   int win;
   bool end;
   bool childrenGenerated;
   weak_ptr<GameState> parent;
   vector<shared_ptr<GameState> > children;


   GameState(int boardArr[5], bool isEvenTurn, shared_ptr<GameState> parent,
             vector<shared_ptr<GameState> >& winStates)
       : isEvenTurn(isEvenTurn), parent(parent), childrenGenerated(false), end(false) {
       copy(boardArr, boardArr + 5, board.begin());
       sort(board.begin(), board.end());
       getWin(winStates);
   }


   shared_ptr<GameState> isEnd(const vector<shared_ptr<GameState> >& winStates) {
       for (int i = 0; i < winStates.size(); ++i) {
           const auto& winState = winStates[i];
           if (board == winState->board) {
               end = true;
               return winState;
           }
       }
       end = false;
       return nullptr;
   }


   shared_ptr<GameState> copyNode(const shared_ptr<GameState>& toCopy,
                                  vector<shared_ptr<GameState> >& winStates) {
       int copiedBoard[5];
       for (int i = 0; i < 5; ++i) {
           copiedBoard[i] = toCopy->board[i];
       }
       return make_shared<GameState>(copiedBoard, !isEvenTurn, shared_from_this(), winStates);
   }


   void SpawnChildren(vector<shared_ptr<GameState> >& winStates,
                             unordered_set<array<int, 5>, GameStateHash>& uniqueStates) {
   int updatedBoard[5];
   shared_ptr<GameState> tempChild, returnedNode;


   if (!this->isEnd(winStates)) {
       for (int j = 0; j < 5; j++) {
           for (int k = 0; k < board[j]; k++) {
               for (int i = 0; i < 5; ++i) {
                   updatedBoard[i] = this->board[i];
               }
               updatedBoard[j] = k;


               // Convert updatedBoard to array format for unique check
               array<int, 5> boardArray;
               std::copy(std::begin(updatedBoard), std::end(updatedBoard), boardArray.begin());


               // Check if this state is already in uniqueStates
               if (uniqueStates.find(boardArray) == uniqueStates.end()) {
                   // If unique, add to uniqueStates
                   uniqueStates.insert(boardArray);


                   // Create a new GameState
                   tempChild = make_shared<GameState>(updatedBoard, !isEvenTurn, shared_from_this(), winStates);
                   children.push_back(tempChild);
               }
           }
       }
   }
   childrenGenerated = true;
}




   void getWin(vector<shared_ptr<GameState> >& winStates) {
       win = isEvenTurn ? -1 : 1;
       if (!isEnd(winStates)) {
           win = 0;
       }
   }
};


void prune(shared_ptr<GameState> target) {
   if (!target->children.empty()) {
       target->win = target->children.at(0)->win;
   }
   target->end = true;
   target->children.clear();
}


vector<shared_ptr<GameState> > initUnique(vector<shared_ptr<GameState> >& winStates) {
   string uniqueString = "0,0,0,0,1#0,0,0,0,2#0,0,0,1,1#0,0,0,0,3#0,0,0,1,2#0,0,1,1,1#0,0,0,0,4#0,0,0,1,3#0,0,0,2,2#0,0,1,1,2#0,1,1,1,1#0,0,0,0,5#0,0,0,1,4#0,0,0,2,3#0,0,1,1,3#0,0,1,2,2#0,1,1,1,2#1,1,1,1,1#0,0,0,1,5#0,0,0,2,4#0,0,0,3,3#0,0,1,1,4#0,0,1,2,3#0,0,2,2,2#0,1,1,1,3#0,1,1,2,2#1,1,1,1,2#0,0,0,2,5#0,0,0,3,4#0,0,1,1,5#0,0,1,2,4#0,0,1,3,3#0,0,2,2,3#0,1,1,1,4#0,1,1,2,3#0,1,2,2,2#1,1,1,1,3#1,1,1,2,2#0,0,0,3,5#0,0,0,4,4#0,0,1,2,5#0,0,1,3,4#0,0,2,2,4#0,0,2,3,3#0,1,1,1,5#0,1,1,2,4#0,1,1,3,3#0,1,2,2,3#0,2,2,2,2#1,1,1,1,4#1,1,1,2,3#1,1,2,2,2#0,0,0,4,5#0,0,1,3,5#0,0,1,4,4#0,0,2,2,5#0,0,2,3,4#0,0,3,3,3#0,1,1,2,5#0,1,1,3,4#0,1,2,2,4#0,1,2,3,3#0,2,2,2,3#1,1,1,1,5#1,1,1,2,4#1,1,1,3,3#1,1,2,2,3#1,2,2,2,2#0,0,1,4,5";
   vector<shared_ptr<GameState> > initialStates;


   unordered_set<array<int, 5>, GameStateHash, GameStateEqual> uniqueStates;
   stringstream ss(uniqueString);
   string boardStr;


   // Parse the string into separate board states
   while (getline(ss, boardStr, '#')) {
       int board[5] = {0};
       stringstream boardStream(boardStr);
       string num;
       int i = 0;


       // Convert board string to integer array
       while (getline(boardStream, num, ',') && i < 5) {
           board[i++] = stoi(num);
       }


       // Convert board array to std::array format for unique checking
       array<int, 5> boardArray;
       std::copy(std::begin(board), std::end(board), boardArray.begin());


       // Insert into uniqueStates only if it's a unique board state
       if (uniqueStates.find(boardArray) == uniqueStates.end()) {
           uniqueStates.insert(boardArray);


           // Create a GameState instance and add it to initialStates
           auto gameState = make_shared<GameState>(board, /*isEvenTurn=*/false, nullptr, winStates);
           initialStates.push_back(gameState);
       }
   }


   // Use expandGameStates to handle the iterative child generation
   expandGameStates(initialStates, winStates, uniqueStates);


   return initialStates;
}


void expandGameStates(vector<shared_ptr<GameState> >& initialStates,
                     vector<shared_ptr<GameState> >& winStates,
                     unordered_set<array<int, 5>, GameStateHash>& uniqueStates) {
   long long nodeCount = 0;
   long long estimatedTotalNodes = 1010101000;
   int progressInterval = 3000;
   vector<shared_ptr<GameState> > stack = initialStates;


   while (!stack.empty()) {
       auto current = stack.back();
       stack.pop_back();


       nodeCount++;


       // Display progress bar
       if (nodeCount % progressInterval == 0) {
           double progressPercent = (static_cast<double>(nodeCount) / estimatedTotalNodes) * 100;
           int barWidth = 75;
           int pos = static_cast<int>(barWidth * progressPercent / 100);


           cout << "\nProgress: [";
           for (int i = 0; i < barWidth; ++i) {
               cout << (i < pos ? "=" : " ");
           }
           cout << "] " << setw(8) << progressPercent << "%, Nodes Processed: " << nodeCount;
       }


       // Process current state
       if (!current->isEnd(winStates)) {
           current->SpawnChildren(winStates, uniqueStates);
           for (auto& child : current->children) {
               stack.push_back(child);
           }
       }
   }


   cout << "\nTotal Nodes Processed: " << nodeCount << endl;
}








int main() {
   cout << "\n----START----\n";
   fstream file("gamestates.txt", ios::out);
   if (!file) {
       cerr << "Error opening file!" << endl;
       return 1;
   }


   int initialBoard[5] = {1, 2, 3, 4, 5};
   int basicWin[5] = {0, 0, 0, 0, 0};
   vector<shared_ptr<GameState> > winStates;
   unordered_set<array<int, 5>, GameStateHash> uniqueStates;


   auto baseWin = make_shared<GameState>(basicWin, false, nullptr, winStates);
   winStates.push_back(baseWin);


   auto root = make_shared<GameState>(initialBoard, false, nullptr, winStates);


   // Use expandGameStates function
   vector<shared_ptr<GameState> > initialStates;
   initialStates.push_back(root);
   expandGameStates(initialStates, winStates, uniqueStates);


   cout << "\n----- END";
   file.close();
   return 0;
}
