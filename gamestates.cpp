#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <memory>
#include <fstream>


using namespace std;




struct GameState : enable_shared_from_this<GameState> {
  int board[5];
  bool isEvenTurn;
  int win;
  bool end;
  bool childrenGenerated;
  weak_ptr<GameState> parent; // weak_ptr to prevent cyclic reference
  vector<shared_ptr<GameState> > children;


  GameState(int board[5], bool IsEvenTurn, shared_ptr<GameState> parent,vector<shared_ptr<GameState> >& winStates)
      : isEvenTurn(isEvenTurn), parent(parent), childrenGenerated(false), end(false) {
      for (int i = 0; i < 5; ++i) {
          this->board[i] = board[i];
      }
      getWin(winStates);
  }


   bool isEnd(vector<shared_ptr<GameState> >& winStates) {
      for (const auto& winState : winStates) {
          bool match = true;
          for (int j = 0; j < 5; j++) {
              if (this->board[j] != winState->board[j]) {
                  match = false;
                  break;
              }
          }
          if (match) {
              end = true;
              return true;
          }
      }
      end = false;
      return false;
  }


   shared_ptr<GameState> isEnd(vector<shared_ptr<GameState> >& winStates,bool returnPointer) {
      for (const auto& winState : winStates) {
          bool match = true;
          shared_ptr<GameState> temp;
          for (int j = 0; j < 5; j++) {
              if (this->board[j] != winState->board[j]) {
                  match = false;
                  temp = winState;
                  break;
              }
          }
          if (match) {
              end = true;
              return temp;
          }
      }
      end = false;
      return nullptr;
  }


   shared_ptr<GameState>  copyNode(shared_ptr<GameState> toCopy,vector<shared_ptr<GameState> >& winStates){
       int copiedBoard [5];
       for (int i = 0; i < 5; ++i) {
           copiedBoard[i] = this->board[i];
        }
       shared_ptr<GameState> copy = make_shared<GameState>(copiedBoard,!isEvenTurn, shared_from_this(), winStates); ;
       return copy;


   };


  void SpawnChildren(vector<shared_ptr<GameState> >& winStates) {
      int updatedBoard[5];
      int winStreak;
      int lookWin;
      bool allEndgameChildren = true;
      shared_ptr<GameState> returnedNode;
      shared_ptr<GameState> tempChild;




      // handles revisit
      if (childrenGenerated){
           if(win != 0)return;
           winStreak = children.at(0).get()->win;
           for (int i = 0; i < children.size(); i++){
               lookWin = children.at(i).get()->win;
               if (lookWin == 0 )return;
               if (lookWin != winStreak)return;
           }
           winStates.push_back(copyNode(this,winStates));
      }


      if (!this->isEnd(winStates)) {
          for (int j = 0; j < 5; j++) {
              for (int k = 0; k < board[j]; k++) {
                  for (int i = 0; i < 5; ++i) {
                      updatedBoard[i] = this->board[i];
                   }
                   updatedBoard[j] = k;
                   tempChild = make_shared<GameState>(updatedBoard, isEvenTurn, shared_from_this(), winStates);
                   returnedNode = copyNode(tempChild.get()->isEnd(winStates,true),winStates);
                   if (returnedNode != nullptr)// Check if the new child is an endgame
                   {
                       children.push_back(returnedNode);
                   } else
                   {
                       children.push_back(tempChild);
                   }
              }
          }
      }
      childrenGenerated = true;
  }


   void getWin(vector<shared_ptr<GameState> >& winStates) {
      if (isEnd(winStates)) {
          win = (isEvenTurn) ? -1 : 1; // Assign +1 for odd turn (winning), -1 for even turn (losing)
      } else {
          win = 0; // Non-terminal state
      }
   }


  void printChildren() const {
      for (const auto& child : children) {
          cout << "\n" << child->board[0] << " "
               << child->board[1] << " "
               << child->board[2] << " "
               << child->board[3] << " "
               << child->board[4];
      }
  }
};




void prune(shared_ptr<GameState>);


int main() {
  cout << "\n----START----\n";
  fstream file("gamestates.txt", ios::out);
  if (!file) {
      cerr << "Error opening file!" << endl;
      return 1;
  }


  int initialBoard[5] = {1, 2, 3, 4, 5};
  int basicWin[5] = {0, 0, 0, 0, 0};
  int nodeCount = 0;
  float progress = 0.0;
  vector<shared_ptr<GameState> > winStates;
  auto baseWin = make_shared<GameState>(basicWin, 0, nullptr, winStates);
  winStates.push_back(baseWin);


  auto root = make_shared<GameState>(initialBoard, 0, nullptr, winStates);


  root->SpawnChildren(winStates);
  int rootChildSize = root->children.size();


  vector<shared_ptr<GameState> > stack;
  stack.push_back(root);


  while (!stack.empty()) {
      auto current = stack.back();
      stack.pop_back();
 
      // PROGRESS BAR
       if (auto parent = current->parent.lock(); parent && parent == root) {
            ++progress;
       }
       // Print progress bar every 100 nodes
       if (nodeCount % 100 == 0) {
           float progressPercent = (progress / static_cast<float>(rootChildSize)) * 100;
           int barWidth = 50; // Width of the progress bar
           int pos = static_cast<int>(barWidth * progressPercent / 100);


           cout << "\nProgress: ["; // Start progress bar
           for (int i = 0; i < barWidth; ++i) {
               if (i < pos) cout << "=";
               else cout << " ";
           }
           cout << "] " << progressPercent << "%, Nodes Processed: " << nodeCount;
       }
      


      if (!current->isEnd(winStates)) {
          current->SpawnChildren(winStates);
          for (auto& child : current->children) {
              stack.push_back(child);
              ++nodeCount;
          }
      }
  }


  cout << "\nTotal Nodes Processed: " << nodeCount << endl;
  file.close();
  return 0;
}


void prune(shared_ptr<GameState> target) {
  if (!target->children.empty()) {
      target->win = target->children.at(0)->win;
  }
  target->end = true;
  target->children.clear();
}
