#ifndef GRAPH_H
#define GRAPH_H
#include "Gamestate.h"

    struct Memo{
        array<shared_ptr<GameState>, TABLE_SIZE> nodes{nullptr};
        long long progressData[2]; // [0] = uniques found; [1] = total nodes processed
        int startingBoard[5];

        Memo(){ 
            nodes.fill(nullptr);
            progressData[0] = 1;
            progressData[1] = 0;
        };

        void SetStartingBoard(int starting[5]){
            for (size_t i = 0; i <5; i++){startingBoard[i] = starting[i];};
        };

        void ProgressVisual(long long progressData[2]){
         
            int progressinterval = 1000;
            int numUniquesDescovered = progressData[0];
            long long totalNodesProcessed = progressData[1];
            int barWidth = 75;
            int barWidth2 = 75;

                // Periodic progress update every progressInterval nodes
            if (totalNodesProcessed % progressinterval == 0) {
                cout << "\x1b[1F\x1b[2K"; // clear the last progress bar if not the root
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

        bool find(GameBoard board){
            int key = board.key;
            if(nodes[key] != nullptr ){return true;} else {return false;}
        };
       
        void insert(shared_ptr<GameState> newUnique){
            nodes[newUnique->GetBoard().key] = newUnique;
        }

        ~Memo();
    };

    class Graph {
        public:
        int startingBoard[5]{0};
        Memo memo;
        GameBoard rootBoard;
        shared_ptr<GameState> root;

        Graph(){
         memo.SetStartingBoard(startingBoard);
         GameBoard rootBoard(startingBoard);
         root = make_shared<GameState>(rootBoard, true);
         memo.insert(root);
        };
        

        void expandGameStates(shared_ptr<GameState>node) {
            GameBoard board = node.get()->GetBoard();

            if (!board.isBegining) {
                vector<shared_ptr<GameState>> stack;
                stack.push_back(node); // Start with the current node

                while (!stack.empty()) {
                    // Pop the last element (DFS)
                    auto current = stack.back().get();
                    stack.pop_back();


                    int curKey = current.get()-> GetBoard().key;

                    // Skip if already processed
                    if (current.get()->getChildrenGenerated()) {
                        cout << "\nSkipping node (already generated)";
                        continue;
                    }

              

                    // Spawn children if not terminal
                    if (!current->board.isBegining) {
                        current->SpawnChildren(current->board, !current->isEven, uniqueNodes, progressData);

                        // Push newly created children to the stack
                        for (int i = 0; i < current->children.size(); i++) {
                            auto child = current->children.at(i);
                            if (child && !child->getChildrenGenerated()) {
                                stack.push_back(child);
                                cout << "\nChild added to stack: " << child->board.display;
                            }
                        }
                    }

                    current->childrenGenerated = true;

                    // Optional: Update progress visualization
                    cout << "\nStack size after processing: " << stack.size();
                }

                if (stack.empty()) {
                    cout << "\nStack is empty. Exiting loop.";
                } else {
                    cout << "\nUnexpected exit. Stack size: " << stack.size();
                }
            } else {
                cout << "\nTerminal state reached. Exiting.";
            }
        }

        
        ~Graph();


    };


#endif 