#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <array>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <unordered_map>


using namespace std;


struct GameState;






vector<shared_ptr<GameState> > initUnique(vector<shared_ptr<GameState> >& );
void prune(shared_ptr<GameState> target);






struct GameStateHash {
  size_t operator()(const GameState& game) const {
      // Sort the board to treat permutations of numbers as equivalent
      array<int, 5> sortedBoard = game.board;
      std::sort(sortedBoard.begin(), sortedBoard.end());
      // Generate a hash
      size_t hash = 0;
      hash = (sortedBoard[0]*1000000)+(sortedBoard[1]*100000)+(sortedBoard[2]*10000)+(sortedBoard[3]*1000)+(sortedBoard[4]*100)+(sortedBoard[5]*10)+game.isEven;
      return hash;
  }
};








struct GameStateEqual {
  bool operator()(const GameState& lhs, const GameState& rhs) const {
      return lhs.board == rhs.board && lhs.isEven == rhs.isEven;
  }
};






void expandGameStates(vector<shared_ptr<GameState>>& initialStates,
                     vector<shared_ptr<GameState>>& winStates,
                     unordered_map<shared_ptr<GameState>, shared_ptr<GameState>, GameStateHash, GameStateEqual>& uniqueStates);




struct GameState : enable_shared_from_this<GameState> {
   array<int, 5> board;
   bool isEven;
   int win;
   bool end;
   bool childrenGenerated;
   vector<weak_ptr<GameState> > parent;
   vector<shared_ptr<GameState> > children;


   GameState(const int* boardArr, bool isEven, vector<weak_ptr<GameState> > parent,
           vector<shared_ptr<GameState>>& winStates)
       : board{boardArr[0], boardArr[1], boardArr[2], boardArr[3], boardArr[4]},
       isEven(isEven), parent(parent), childrenGenerated(false), end(false) {
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
       return make_shared<GameState>(copiedBoard, !isEven, shared_from_this(), winStates);
   }


  
   void SpawnChildren(vector<shared_ptr<GameState>>& winStates,
                   unordered_map<shared_ptr<GameState>, shared_ptr<GameState>, GameStateHash, GameStateEqual>& uniqueStates) {
       int updatedBoard[5];
       shared_ptr<GameState> tempChild;


       if (!this->isEnd(winStates)) {
           for (int j = 0; j < 5; j++) {
               for (int k = 0; k < board[j]; k++) {
                   // Copy the current board state into `updatedBoard`
                   std::copy(std::begin(this->board), std::end(this->board), std::begin(updatedBoard));
                   updatedBoard[j] = k;


                   // Create a potential child GameState with updated board and flipped `isEven`
                   auto potentialChild = make_shared<GameState>(updatedBoard, !isEven, shared_from_this(), winStates);


                   // Insert only if not already in uniqueStates
                   if (uniqueStates.find(potentialChild) == uniqueStates.end()) {
                       uniqueStates[potentialChild] = potentialChild; // Use GameState as both key and value
                       children.push_back(potentialChild);
                   } else {
                       // If it exists, retrieve the existing GameState
                       tempChild = uniqueStates[potentialChild];
                       children.push_back(tempChild);
                       tempChild->parent.push_back(shared_from_this());
                   }
               }
           }
       }
       childrenGenerated = true;
   }




   void getWin(vector<shared_ptr<GameState> >& winStates) {
       win = isEven ? -1 : 1;
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
   string uniqueString = "0,0,0,0,1#0,0,0,0,2";
   vector<shared_ptr<GameState>> initialStates;


   unordered_map<shared_ptr<GameState>, shared_ptr<GameState>, GameStateHash, GameStateEqual> uniqueStates;
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


       // Create a GameState instance
       auto gameState = make_shared<GameState>(board, /*isEven=*/false, vector<weak_ptr<GameState>>{}, winStates);


       // Insert only if this game state is unique
       if (uniqueStates.find(gameState) == uniqueStates.end()) {
           uniqueStates[gameState] = gameState;
           initialStates.push_back(gameState);
       }
   }


   // Use expandGameStates to handle the iterative child generation
   expandGameStates(initialStates, winStates, uniqueStates);


   return initialStates;
}


void expandGameStates(vector<shared_ptr<GameState>>& initialStates,
                     vector<shared_ptr<GameState>>& winStates,
                     unordered_map<shared_ptr<GameState>, shared_ptr<GameState>, GameStateHash, GameStateEqual>& uniqueStates) {
   long long nodeCount = 0;
   long long estimatedTotalNodes = 1010101000;
   int progressInterval = 3000;
   vector<shared_ptr<GameState>> stack = initialStates;


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
};


// Utility function to format the board as a string for CSV output
string formatBoard(const array<int, 5>& board) {
   stringstream ss;
   for (int i = 0; i < 5; ++i) {
       ss << board[i];
       if (i < 4) ss << ",";
   }
   return ss.str();
}




void writeGameStateToCSV(ofstream& file, shared_ptr<GameState> gameState,
                        unordered_map<shared_ptr<GameState>, int>& stateToIdMap, int& idCounter, int parentId = -1) {
   // Assign an ID to the current GameState if it doesn't have one yet
   if (stateToIdMap.find(gameState) == stateToIdMap.end()) {
       stateToIdMap[gameState] = idCounter++;
   }
  
   int currentId = stateToIdMap[gameState];
  
   // Write the current GameState to the CSV file
   file << currentId << ","
        << parentId << ","
        << "\"" << formatBoard(gameState->board) << "\","
        << (gameState->end ? "true" : "false") << "\n";
  
   // Recursively write each child
   for (const auto& child : gameState->children) {
       writeGameStateToCSV(file, child, stateToIdMap, idCounter, currentId);
   }
}


// Correct declaration in exportTreeToCSV function
void exportTreeToCSV(const string& filename, shared_ptr<GameState> root) {
   ofstream file(filename);
   if (!file.is_open()) {
       cerr << "Error: Could not open file " << filename << " for writing." << endl;
       return;
   }


   // Write CSV header
   file << "ID,ParentID,Board,IsEnd\n";


   // Use unordered_map to store IDs for each unique GameState and initialize ID counter
   unordered_map<shared_ptr<GameState>, int> stateToIdMap; // Fixed unordered_map usage
   int idCounter = 1;


   // Start the recursive export with the root GameState
   writeGameStateToCSV(file, root, stateToIdMap, idCounter);


   file.close();
   cout << "Tree successfully exported to " << filename << endl;
}


int main() {
   cout << "\n----START----\n";




   int initialBoard[5] = {1, 2, 3, 4, 5};
   int basicWin[5] = {0, 0, 0, 0, 0};
   vector<shared_ptr<GameState> > winStates;
   unordered_map<shared_ptr<GameState>, shared_ptr<GameState>, GameStateHash, GameStateEqual> uniqueStates;


   auto baseWin = make_shared<GameState>(basicWin, false, nullptr, winStates);
   winStates.push_back(baseWin);


   auto root = make_shared<GameState>(initialBoard, false, nullptr, winStates);


   // Use expandGameStates function
   vector<shared_ptr<GameState> > initialStates;
   initialStates.push_back(root);
   expandGameStates(initialStates, winStates, uniqueStates);


   cout << "\n----- END";


   return 0;
}


