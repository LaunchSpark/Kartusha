#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <memory>
#include <fstream>


using namespace std;


void prune(shared_ptr<GameState>);


struct GameState : enable_shared_from_this<GameState> {
  int board[5];
  int turnNum;
  int win;
  bool end;
  bool childrenGenerated;
  weak_ptr<GameState> parent; // weak_ptr to prevent cyclic reference
  vector<shared_ptr<GameState>> children;


  GameState(int board[5], int turnNum, shared_ptr<GameState> parent, const vector<shared_ptr<GameState>>& winStates)
      : turnNum(turnNum), parent(parent), childrenGenerated(false), end(false) {
      for (int i = 0; i < 5; ++i) {
          this->board[i] = board[i];
      }
      getWin(winStates);
  }


bool isEnd(const vector<shared_ptr<GameState> >& winStates) {
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


  void SpawnChildren(const vector<shared_ptr<GameState>>& winStates) {
      if (childrenGenerated) return;


      int updatedBoard[5];


      if (!this->isEnd(winStates)) {
          for (int j = 0; j < 5; j++) {
              for (int k = 0; k < board[j]; k++) {
                  for (int i = 0; i < 5; ++i) {
                      updatedBoard[i] = this->board[i];
                  }
                  updatedBoard[j] = k;
                  children.push_back(make_shared<GameState>(updatedBoard, turnNum + 1, shared_from_this(), winStates));
              }
          }
      }
      childrenGenerated = true;
  }


   void getWin(const vector<shared_ptr<GameState> >& winStates) {
      if (isEnd(winStates)) {
          win = (turnNum % 2 == 0) ? -1 : 1; // Assign +1 for odd turn (winning), -1 for even turn (losing)
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
  vector<shared_ptr<GameState>> winStates;
  auto baseWin = make_shared<GameState>(basicWin, 0, nullptr, winStates);
  winStates.push_back(baseWin);


  auto root = make_shared<GameState>(initialBoard, 0, nullptr, winStates);


  root->SpawnChildren(winStates);
  int rootChildSize = root->children.size();


  vector<shared_ptr<GameState>> stack;
  stack.push_back(root);


  while (!stack.empty()) {
      auto current = stack.back();
      stack.pop_back();


      if (auto parent = current->parent.lock(); parent && parent == root) {
          cout << "\nProgress: " << (progress / rootChildSize) * 100 << "%";
          ++progress;
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
