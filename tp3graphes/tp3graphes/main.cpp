//
//  main.cpp
//  tp3graphes
//
//  Created by Matthieu de La Roche Saint-Andre and Victor Haffreingue on 12/20/13.
//

#include <iostream>
#include <cstdio>
#include <vector>

using namespace std;

// Cell of the adjacency matrix
// This structure is used to store values in adjacency matrix
// The field "valid" is null when the value is not valid,
// and has any other value otherwise.
// The field "value" is the weight of the link.
typedef struct _adjCell {
    int value, valid;
} AdjCell;

// Link in the graph.
// This structure is used to store a link.
// The field "value" is the weight of the link.
// The field "edge1" is the first extremity of the link.
// The field "value" is the second extremity of the link.
typedef struct _link {
    int value, edge1, edge2;
} Link;


AdjCell** parseFile(string path, int *nbNodes); // parses the file and creates the adjacency matrix
void print(AdjCell** adjMatrix, int size); // prints adjacency matrix
void print(const vector<vector<AdjCell> > &adjMatrix, int size); // prints adjacency matrix
bool cycle(AdjCell** adjMatrix, int size); // checks cycle presence in adjacency matrix

bool cycle(const vector<vector<AdjCell> > &adjMatrix, int size); // checks cycle presence in adjacency matrix
vector<Link> getSortedLinkList(AdjCell** adjMatrix, int size); // return an ordered array of links of the graph
void generateAdjMatrix(vector<Link> linkArray, vector<vector<AdjCell> > &adjMatrix, int size); // generate adjacency matrix based on an array of links
void coveringTree(AdjCell** adjMatrix, int size, vector<vector<AdjCell> > &adjMatrixCoveringTree); // generates covering tree

int main(int argc, const char * argv[]) {
    string path;
//    if (argc == 2) {
//        path = argv[1];
//    } else {
//        cout << "Which file do you want to open ? ";
//        cin >> path;
//        cout << endl;
    //    }
    path = "/Users/matthieudelaro/Documents/classes/L3/S5/theorie_des_graphes/TP/tp32/Haffreingue-de_La_Roche_Saint_Andre-EFREI-L3-TG-TP3-2.txt";
    
    
    int size = 0; //size of adjacency matrix and of Bellman's matrix
    AdjCell **adjMatrix = parseFile(path, &size); // creates the adjacency matrix
    
    // handle reading errors
    if (!adjMatrix) {
        cout << "Error reading " << path << "." << endl;
    } else {
        // if adgency matrix was successfully created
        cout << endl;
        print(adjMatrix, size);
        cout << endl;
        
        cout << "Generating covering tree :" << endl;
        vector<vector<AdjCell> > adjMatrixCoveringTree;
        coveringTree(adjMatrix, size, adjMatrixCoveringTree);
        cout << endl << "Covering tree :" << endl;
        print(adjMatrixCoveringTree, (int) adjMatrixCoveringTree.size());
        cout << endl;
        
        
        // deletes adjacency matrix
        for (int i = 0; i < size; ++i) {
            delete[] adjMatrix[i];
        }
        delete[] adjMatrix;
    }
}

AdjCell** parseFile(string path, int *nbNodes) {
    cout << "Opening " << path  << "." << endl;
    FILE *file;
    int endOfFile = 0;
    file = fopen(path.c_str(), "r");
    
    AdjCell **matrix = NULL;
    
    if (file) {
        int c = 0;//current int we are parsing
        int nbLinks = 0;
        
        // Gets number of nodes
        endOfFile = fscanf(file, "%d", &c);
        if (endOfFile != EOF) {
            *nbNodes = c;
            
            // initializes the matrix
            matrix = new AdjCell*[*nbNodes];
            for (int i = 0; i < *nbNodes; ++i) {
                matrix[i] = new AdjCell[*nbNodes];
                for (int j = 0; j < *nbNodes; ++j) {
                    matrix[i][j].valid = 0;
                }
            }
        } else {
            cout << "Number of nodes is missing." << endl;
            return NULL;//number of nodes is missing
        }
        
        // Gets number of links
        // Note :
        // This is useless for the program. However, this
        // is required to comply with out teacher's
        // example of file format, so that it will make
        // verifications steps easier.
        endOfFile = fscanf(file, "%d", &c);
        if (endOfFile != EOF) {
            nbLinks = c;
        } else {
            cout << "Number of links is missing." << endl;
            return NULL;//number of links is missing
        }
        
        // read file ~line by line
        endOfFile = fscanf(file, "%d", &c);
        while (endOfFile != EOF) {
            if (c == -1) {
                // end of definition if we read -1
                return matrix;
            }
            
            //trying to get the terminal node of the link
            int line = c;
            int column = 0;
            //fflush(stdout);
            endOfFile = fscanf(file, "%d", &c);
            if (endOfFile != EOF) {
                column = c;
            } else {
                cout << "Terminal node is missing." << endl;
                return NULL;//terminal node is missing
            }
            
            //trying to get the value of the link
            endOfFile = fscanf(file, "%d", &c);
            if (endOfFile != EOF) {
                // In case a link is declared several times,
                // we keep the shortest link.
                if (matrix[line][column].valid) {
                    if (matrix[line][column].value > c) {
                        matrix[line][column].value = c;
                        matrix[line][column].valid = 1;
                        
                        
                        matrix[column][line].value = c;
                        matrix[column][line].valid = 1;
                    }
                } else {
                    matrix[line][column].value = c;
                    matrix[line][column].valid = 1;
                    
                    matrix[column][line].value = c;
                    matrix[column][line].valid = 1;
                }
            } else {
                cout << "Value is missing." << endl;
                return NULL;//value is missing
            }
            
            endOfFile = fscanf(file, "%d", &c);
        }
    } else {
        cout << "Error while opening the file." << endl;
        return NULL; // error while opening the file
    }
    
    return matrix;
}

void print(AdjCell** adjMatrix, int size) {
    for (int j = 0; j < size; ++j) {
        cout << "\t" << j;
    }
    for (int i = 0; i < size; ++i) {
        cout << endl << i << "\t";
        for (int j = 0; j < size; ++j) {
            if (adjMatrix[i][j].valid) {
                cout << adjMatrix[i][j].value << "\t";
            } else {
                cout << ".\t";
            }
        }
    }
    cout << endl;
}

void print(const vector<vector<AdjCell> > &adjMatrix, int size) {
    for (int j = 0; j < size; ++j) {
        cout << "\t" << j;
    }
    for (int i = 0; i < size; ++i) {
        cout << endl << i << "\t";
        for (int j = 0; j < size; ++j) {
            if (adjMatrix[i][j].valid) {
                cout << adjMatrix[i][j].value << "\t";
            } else {
                cout << ".\t";
            }
        }
    }
    cout << endl;
}

// checks cycle presence in adjacency matrix
bool cycle(const vector<vector<AdjCell> > &adjMatrix, int size) {
    // Create an array the size of the matrix.
    // For each node i, deleted[i]
    // stores whether i has been deleted.
    vector<bool> deleted(size, false);
    
    // Each time we find a new node,
    // we incremente the value of deletedQuantity.
    // This way, we know when we reach the end
    // of the algorithm (deletedQuantity == size).
    int deletedQuantity = 0;
    
    // Used to store previous
    // value of foundQuantity.
    int previousDeletedQuantity = 0;
    
    int current = 0;
    bool goOn = true;
    while (deletedQuantity < size && goOn) {
        if (deleted[current] == false) {
            
            int destinationQuantity = 0;
            for (int destination = 0; destination < size && destinationQuantity < 2; ++destination) {
                if (adjMatrix[current][destination].valid && !deleted[destination]) {
                    destinationQuantity++;
                }
            }
            
            // if the node is an entry point in the graph,
            // we delete this node
            if (destinationQuantity < 2) {
                deleted[current] = true;
                deletedQuantity++;
//                cout << current << " has " << destinationQuantity << " neighbour. We delete it." << endl;
            }
        }
        
        if (current == size - 1) {
            if (deletedQuantity == previousDeletedQuantity) {
                goOn = false;
            } else {
                previousDeletedQuantity = deletedQuantity;
                current = 0;
            }
        } else {
            current++;
        }
    }
    
//    cout << endl;
//    cout << endl;
    if (deletedQuantity == size) {
//        cout << "Each node has been deleted." << endl;
        return false;
    } else {
//        cout << "Some node(s) has not been deleted." << endl;
        return true;
    }
}

// return an ordered array of links of the graph
vector<Link> getSortedLinkList(AdjCell** adjMatrix, int size) {
    vector<Link> out;
    
    // 2D array used to say whether a link has been used/recorded
    vector< vector<bool> > used(size);
    for (int i = 0; i < size; ++i) {
        used[i].resize(size, false);
    }
    
    // retrieve quantity of links and minimal weight
    int quantity = 0;
    bool foundMinimal = false;
    Link minimal;
    for (int i = 0; i < size; ++i) {
        for (int j = i; j < size; ++j) {
            if (adjMatrix[i][j].valid) {
                quantity++;
                if (!foundMinimal) {
                    minimal.value = adjMatrix[i][j].value;
                    minimal.edge1 = i;
                    minimal.edge2 = j;
                    
                    foundMinimal = true;
                } else if (adjMatrix[i][j].value < minimal.value) {
                    minimal.value = adjMatrix[i][j].value;
                    minimal.edge1 = i;
                    minimal.edge2 = j;
                }
            }
        }
    }
    
    if (quantity > 0) {// Retrieve ordered array
        out.resize(quantity);
        out[0] = minimal;
        int nextIndex = 1;
        used[minimal.edge1][minimal.edge2] = true;
        
        while (nextIndex < quantity) {
            foundMinimal = false;
            for (int i = 0; i < size; ++i) {
                for (int j = i; j < size; ++j) {
                    if (adjMatrix[i][j].valid && !used[i][j]) {
                        if (!foundMinimal) {
                            minimal.value = adjMatrix[i][j].value;
                            minimal.edge1 = i;
                            minimal.edge2 = j;
                            
                            foundMinimal = true;
                        } else if (adjMatrix[i][j].value < minimal.value) {
                            minimal.value = adjMatrix[i][j].value;
                            minimal.edge1 = i;
                            minimal.edge2 = j;
                            quantity++;
                        }
                    }
                }
            }
            
            used[minimal.edge1][minimal.edge2] = true;
            out[nextIndex] = minimal;
            nextIndex++;
        }
    }
    
//    for (int j = 0; j < out.size(); ++j) {
//        cout << out[j].edge1 << " " << out[j].edge2 << " " << out[j].value << endl;
//    }
    return out;
}

// generate adjacency matrix based on an array of links
void generateAdjMatrix(vector<Link> linkArray, vector<vector<AdjCell> > &adjMatrix, int size) {
    AdjCell init;
    init.valid = false;
    adjMatrix.resize(size);
    for (int i = 0; i < size; ++i) {
        adjMatrix[i].resize(size);
        for (int j = 0; j < size; ++j) {
            adjMatrix[i][j] = init;
        }
    }
    
    for (int i = 0; i < linkArray.size(); ++i) {
        adjMatrix[linkArray[i].edge1][linkArray[i].edge2].valid = true;
        adjMatrix[linkArray[i].edge1][linkArray[i].edge2].value = linkArray[i].value;
        
        
        adjMatrix[linkArray[i].edge2][linkArray[i].edge1].valid = true;
        adjMatrix[linkArray[i].edge2][linkArray[i].edge1].value = linkArray[i].value;
    }
}

// generates covering tree
void coveringTree(AdjCell** adjMatrix, int size, vector<vector<AdjCell> > &adjMatrixCoveringTree) {
    vector<Link> links = getSortedLinkList(adjMatrix, size);

    vector<Link> tree;
    // add links one after another. Checks cycle presence. Revoce link if there is a cycle.
    for (int i = 0; i < links.size(); ++i) {
        tree.push_back(links[i]);
        generateAdjMatrix(tree, adjMatrixCoveringTree, size);
        
        if (cycle(adjMatrixCoveringTree, size)) {
            tree.pop_back();
            cout << links[i].edge1 << "\t" << links[i].edge2 << "\t" << links[i].value;
            cout << " is revoked, for it would create a cycle." << endl;
        } else {
            cout << links[i].edge1 << "\t" << links[i].edge2 << "\t" << links[i].value;
            cout << " added." << endl;
        }
    }
    
    generateAdjMatrix(tree, adjMatrixCoveringTree, size);
}