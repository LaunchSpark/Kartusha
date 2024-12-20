#ifndef GRAPH_H
#define GRAPH_H
#include "Gamestate.h"

    struct Memo{
        array<shared_ptr<GameState>, TABLE_SIZE> nodes{nullptr};
        long long progressData[2]; // [0] = uniques found; [1] = total nodes processed
        int startingBoard[5];
        int progressinterval;
        int barWidth;
        int barWidth2;
        double progressPercent;
        double progressPercent2;
        int pos;
        int pos2;

        Memo(){ 
            nodes.fill(nullptr);
            progressData[0] = 1;//unique nodes
            progressData[1] = 0;//total nodes
            progressinterval = PROGRESS_INTERVAL;//sets progress interval
            barWidth = BAR_WIDTH;
            barWidth2 = BAR_WIDTH2;
        };

        void SetStartingBoard(int starting[5]){
            for (size_t i = 0; i <5; i++){startingBoard[i] = starting[i];};
        };

        void ProgressVisual(){
                // Periodic progress update every progressInterval nodes
            if (progressData[1] % progressinterval == 0) {
                if (progressData[1] > 2){ cout << "\x1b[1F\x1b[2K";}; // clear the last progress bar if not the root
                progressPercent = (static_cast<double>(progressData[1]) / 1010101000) * 100;
                progressPercent2 = (static_cast<double>(progressData[0]) / 130) * 100;
                pos = static_cast<int>(barWidth * progressPercent / 100);
                pos2 = static_cast<int>(barWidth2 * progressPercent2 / 100);

                // Display the progress bar
                cout << "Progress:  [";
                for (int i = 0; i < barWidth; ++i) {
                    if (i < pos) cout << "=";
                    else cout << " ";
                }
                cout << "] " << setw(8) << progressPercent ;
                cout << "%, Nodes Processed: " << progressData[1];

                cout << "\nUniques:   [";
                for (int i = 0; i < barWidth2; ++i) {
                    if (i < pos2) cout << "=";
                    else cout << " ";
                }
                cout << "] " << setw(8) << progressPercent2;
                cout << "%, Found: " << progressData[0];
            }
            return;
        };

        bool find(GameBoard board){
            int key = board.key;
            if(nodes[key] != nullptr ){return true;} else {return false;}
        };
       
        void insert(shared_ptr<GameState> newUnique){
            nodes[newUnique->GetBoard().key] = newUnique;
            progressData[0] += 1;
        }

        void printUniques(){
            int j = 0;
            for (size_t i = 0; i < TABLE_SIZE; i++)
            {
                if (nodes[i] != nullptr)
                {
                    cout << nodes[i].get()->GetBoard().board[0] << ",";  
                    cout << nodes[i].get()->GetBoard().board[1] << ","; 
                    cout << nodes[i].get()->GetBoard().board[2] << ","; 
                    cout << nodes[i].get()->GetBoard().board[3] << ","; 
                    cout << nodes[i].get()->GetBoard().board[4] << "; ";
                    cout << nodes[i].get()->GetBoard().key << "\n";
                    j++;
                }
            }
            cout << "\n" << j;
        }
        ~Memo(){};
    };

    class Graph {
        public:
        int startingBoard[5];
        Memo memo;
        GameBoard rootBoard;
        shared_ptr<GameState> root;

        Graph(){
         for (int i = 0; i < 5; i++) {startingBoard[i] = i+1;}; // sets starting board to 1,2,3,4,5
         memo.SetStartingBoard(startingBoard);
         GameBoard temp(startingBoard);
         rootBoard = temp;
         root = make_shared<GameState>(rootBoard);
         memo.insert(root);
        };
        

        void SpawnChildren(shared_ptr<GameState> inState) {
            
             int proposedBoard[5];
             for (int i = 0; i < 5; i++) { proposedBoard[i] = inState.get()->GetBoard()[i];}//reset proposed board
             memo.ProgressVisual(); //progress visual
             memo.progressData[1]++;//updates the progress tracking
             for (int j = 4; j >= 0; j--) { // j = what slot to look at
                 for (int k = 0; k < inState.get()->GetBoard()[j]; k++) { // k = what to reduce it to 
                     proposedBoard[j] = k; // reduces the number in the slot to k
                     GameBoard child(proposedBoard); // makes into a Gameboard object
                    


                     if (memo.find(child)) { // returns true if already known
                         inState.get()->addChild(memo.nodes[child.key]); 
                     } else {
                         memo.insert(make_shared<GameState>(child));//make new gameState object and add it to memo
                         inState.get()->addChild(memo.nodes[child.key]);
                        
                     }
                     memo.nodes[child.key].get()->addParent(inState);//add inState to the childs parent list
                     
                    


                     if (!(memo.nodes[child.key].get()->getChildrenGenerated() || child.isEnd)){SpawnChildren(memo.nodes[child.key]);} 
                     
        
                     for (int i = 0; i < 5; i++) { proposedBoard[i] = inState.get()->GetBoard()[i]; }//reset proposed board
                 }
             }

             inState.get()->setChildGenerated(true);
             return; // returns if loops through all children
        }
        
        ~Graph(){};


    };


#endif 