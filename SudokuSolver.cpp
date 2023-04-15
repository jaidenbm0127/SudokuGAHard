#include <iostream>
#include <vector>
#include <random>
#include <cstring>
#include <algorithm> 
#include <ctime>
#include <bits/stdc++.h>
#include <time.h>

using namespace std;

// Our "Chromosome"
class SudokuGrid{   
    public:
        vector<vector<int>> mySudokuVector;
        int fitness;

        // Passing the grid we're working with to the SudokuGrid class
        void setSudokuGrid(vector<vector<int>> sudokuVector){
            mySudokuVector = sudokuVector;
        }

        // Useful retrieve operation to get the vector of the SudokuGrid
        vector<vector<int>> getSudokuGrid(){
            return mySudokuVector;
        }

        int getFitness(){
            return fitness;
        }
        
        void setFitness(int fit){
            fitness = fit; 
        }

        void printSudokuGrid(){
            for(int row = 0; row < 9; row++){
                for(int column = 0; column < 9; column++){
                    cout << mySudokuVector.at(row).at(column) << " ";
                }
                cout << endl;
            }
            cout << fitness << endl;
        }
};



// Specifically used to intialize the grid and help calculate fitness 
class Initialization{
    private:
        vector<vector<int>> givenNumbers;
        vector<SudokuGrid> collectionOfGrids;

    public: 
        // Takes the grid we're trying to solve and saves the initial configuration
        void getGivenNumbers(SudokuGrid initialGrid){
            givenNumbers = initialGrid.getSudokuGrid();
        }

        // To start our GA, I simply give each non-preset (AKA the zeros) number a random assignment from 1-9
        vector<vector<int>> setInitialNumbers(vector<vector<int>> grid){
            vector<vector<int>> randomizeZeros = grid;
            for(int row = 0; row < 9; row++){
                for(int column = 0; column < 9; column++){
                    // Checking to see if the number at the index has been given to us or is a zero
                    if(!isNumberPreset(row, column))
                        randomizeZeros.at(row).at(column) = rand() % 9 + 1; 
                }
            }
            return randomizeZeros;
        }

        // Very useful function to check whether a number at a given index was a preset number 
        bool isNumberPreset(int row, int column){
            if(givenNumbers.at(row).at(column) != 0)
                return true;
            else
                return false;
        }

        // Takes our sudoku grid and makes X SudokuGrids for our population where X is how many candidates we want per generation. Also sets their initial numbers at random 
        vector<SudokuGrid> initalizeGrids(vector<vector<int>> mySudokuVector){
            for(int grid = 0; grid < 200; grid++){
                SudokuGrid tempGrid;
                tempGrid.setSudokuGrid(setInitialNumbers(mySudokuVector));
                tempGrid.setFitness(getFitnessValue(tempGrid));
                collectionOfGrids.push_back(tempGrid);
                //tempGrid.printSudokuGrid();
            }
            return collectionOfGrids;
        }

        int checkRowForDuplicates(SudokuGrid grid){
            int fitness{0};
            vector<vector<int>> sudokuVector = grid.getSudokuGrid();
            for(int row = 0; row < 9; row++){
                vector<int> usedNumbers; 
                for(int gettingRowNumbers = 0; gettingRowNumbers < 9; gettingRowNumbers++){
                        usedNumbers.push_back(sudokuVector.at(row).at(gettingRowNumbers));
                    }
                for(int column = 0; column < 9; column++){
                    if(!isNumberPreset(row, column) && count(usedNumbers.begin(), usedNumbers.end(), sudokuVector.at(row).at(column)) > 1)
                        fitness++; 
                }
            }
            return fitness;
        }

        int checkColumnForDuplicates(SudokuGrid grid){
            int fitness{0};
            vector<vector<int>> sudokuVector = grid.getSudokuGrid();
            for(int column = 0; column < 9; column++){
                vector<int> usedNumbers; 
                for(int gettingColumnNumbers = 0; gettingColumnNumbers < 9; gettingColumnNumbers++){
                        usedNumbers.push_back(sudokuVector.at(gettingColumnNumbers).at(column));
                    }
                for(int row = 0; row < 9; row++){
                    if(!isNumberPreset(row, column) && count(usedNumbers.begin(), usedNumbers.end(), sudokuVector.at(row).at(column)) > 1)
                        fitness++; 
                }
            }
            return fitness;
        }

        int analyzeSubGrids(int row, int column, vector<vector<int>> sudokuGrid){
            int fitness{0};
            vector<int> usedNumbers; 
            for(int i = row*3; i < row*3+3; i++){
                for(int j = column*3; j < column*3+3; j++){
                    usedNumbers.push_back(sudokuGrid.at(i).at(j));
                }
            }

            for(int i = row*3; i < row*3+3; i++){
                for(int j = column*3; j < column*3+3; j++){
                    if(!isNumberPreset(i, j) && count(usedNumbers.begin(), usedNumbers.end(), sudokuGrid.at(i).at(j)) > 1)
                        fitness++;
                }
            }
            return fitness;
        }

        int checkSubGridsForDuplicates(SudokuGrid grid){
            int fitness{0};
            vector<vector<int>> sudokuVector = grid.getSudokuGrid();
            for(int row = 0; row < 3; row++){
                for(int column = 0; column < 3; column++){
                    fitness += analyzeSubGrids(row, column, sudokuVector);
                }
            }
            return fitness;
        }

        // Our fitness is calculated  by checking how many "out of place" (ie duplicate entries) are in each row, column, and subgrid
        int getFitnessValue(SudokuGrid grid){
            int fitness{0};

            fitness += checkRowForDuplicates(grid);
            fitness += checkColumnForDuplicates(grid);
            fitness += checkSubGridsForDuplicates(grid);

            return fitness;
        }
};

class Tournament{
    public: 
        static SudokuGrid doTourney(vector<SudokuGrid> grids){

            
            SudokuGrid firstSelected = grids.at(rand() % (grids.size()));
            SudokuGrid secondSelected = grids.at(rand() % (grids.size()));

            if(firstSelected.fitness < secondSelected.fitness)
                return firstSelected;
            else
                return secondSelected; 
        }


};

class Crossover{
    public:
        static vector<SudokuGrid> doCrossover(SudokuGrid winnerOne, SudokuGrid winnerTwo){
            vector<SudokuGrid> tempCollection; 
            for(int i = 0; i < 1; i++){
                int rowGrid = rand() % 3; 
                int columnGrid = rand() % 3; 
                for(int row = rowGrid * 3; row < rowGrid*3+3;row++){
                    for(int column = columnGrid * 3; column< columnGrid*3+3; column++){
                        int placeHolder = winnerOne.mySudokuVector.at(row).at(column);
                        winnerOne.mySudokuVector.at(row).at(column) = winnerTwo.mySudokuVector.at(row).at(column);
                        winnerTwo.mySudokuVector.at(row).at(column) = placeHolder; 
                    }
                }
            }

            tempCollection.push_back(winnerOne);
            tempCollection.push_back(winnerTwo);

            return tempCollection;

        }
};

class Mutation{
    public:
        static vector<SudokuGrid> doMutate(SudokuGrid winnerOne, SudokuGrid winnerTwo, Initialization init){
            vector<SudokuGrid> tempCollection;
            int row{};
            int column{}; 
            for(int attempt = 0; attempt < 10; attempt++){
                row = rand() % 9;
                column = rand() % 9;
                if(!init.isNumberPreset(row, column))
                    break;
            }

            winnerOne.mySudokuVector.at(row).at(column) = rand() % 9 + 1; 

            for(int attempt = 0; attempt < 10; attempt++){
                row = rand() % 9;
                column = rand() % 9;
                if(!init.isNumberPreset(row, column))
                    break;
            }

            winnerTwo.mySudokuVector.at(row).at(column) = rand() % 9 + 1; 

            tempCollection.push_back(winnerOne);
            tempCollection.push_back(winnerTwo);

            return tempCollection;
        }
};

class GeneticAlgorithm{    
    public: 
        vector<SudokuGrid> collectionOfGrids;
        Initialization initializer; 

        // Beginning portion of genetic algorithm with initial setup of first population
        void startTheGA(double mutation, double crossover, vector<SudokuGrid> restartGrids){
            SudokuGrid mySudokuGrid;

            // Hard coding in sudoku puzzle
            vector<vector<int>> mySudokuVector = {{0, 0, 3, 0, 2, 0, 6, 0, 0},
                                                 {9, 0, 0, 3, 0, 5, 0, 0, 1},
                                                 {0, 0, 1, 8, 0, 6, 4, 0, 0},
                                                 {0, 0, 8, 1, 0, 2, 9, 0, 0},
                                                 {7, 0, 0, 0, 0, 0, 0, 0, 8},
                                                 {0, 0, 6, 7, 0, 8, 2, 0, 0},
                                                 {0, 0, 2, 6, 0, 9, 5, 0, 0},
                                                 {8, 0, 0, 2, 0, 3, 0, 0, 9},
                                                 {0, 0, 5, 0, 1, 0, 3, 0, 0}};

            
            mySudokuGrid.setSudokuGrid(mySudokuVector);

           // mySudokuGrid.printSudokuGrid();

            initializer.getGivenNumbers(mySudokuGrid);
            vector<SudokuGrid> grids; 

            //cout << "-------------------- INITIAL GRIDS -------------------------" << endl << endl;

            grids = initializer.initalizeGrids(mySudokuVector);

            //cout << "------------------ END INITIAL GRIDS -----------------------" << endl << endl; 

            collectionOfGrids = grids; 

            doTheGA(mutation, crossover, restartGrids);
            
        }

        void doTheGA(double mutation, double crossover, vector<SudokuGrid> restartGrids){

            vector<SudokuGrid> oldGrids = collectionOfGrids;
            
            int generation{0};
            int restart{0};


            sort(oldGrids.begin(), oldGrids.end(), [](const SudokuGrid& A, const SudokuGrid& B){
                return A.fitness < B.fitness;
            });

            for(int runs = 0; runs < 1000; runs++){
                int currentTopFitness = oldGrids.at(0).fitness;
                generation++;

                vector<SudokuGrid> newGrids; 

                if(!restartGrids.empty())
                    for(int i = 0; i < restartGrids.size(); i++)
                        newGrids.push_back(restartGrids.at(i));


                newGrids.push_back(oldGrids.at(0));
                newGrids.push_back(oldGrids.at(1));

                oldGrids.erase(oldGrids.begin());
                oldGrids.erase(oldGrids.begin());

                while(newGrids.size() < 100){
                    SudokuGrid winnerOne;
                    SudokuGrid winnerTwo; 

                    winnerOne = Tournament::doTourney(oldGrids);
                    winnerTwo = Tournament::doTourney(oldGrids);

                    if(double(rand() % 101) < crossover){
                        vector<SudokuGrid> childrenCrossover = Crossover::doCrossover(winnerOne, winnerTwo);
                        vector<SudokuGrid> childrenMutated = Mutation::doMutate(childrenCrossover.at(0), childrenCrossover.at(1), initializer);
                        childrenMutated.at(0).setFitness(initializer.getFitnessValue(childrenMutated.at(0)));
                        childrenMutated.at(1).setFitness(initializer.getFitnessValue(childrenMutated.at(1)));
                        newGrids.push_back(childrenMutated.at(0));
                        newGrids.push_back(childrenMutated.at(1));
                    }
                    else
                        for(int grid = 0; grid < 2; grid++)
                            if(double(rand() % 101) < mutation){
                                vector<SudokuGrid> childrenMutated = Mutation::doMutate(winnerOne, winnerTwo, initializer);
                                childrenMutated.at(0).setFitness(initializer.getFitnessValue(childrenMutated.at(0)));
                                childrenMutated.at(1).setFitness(initializer.getFitnessValue(childrenMutated.at(1)));
                                newGrids.push_back(childrenMutated.at(0));
                                newGrids.push_back(childrenMutated.at(1));
                            }           
                }
                oldGrids = newGrids;

                sort(oldGrids.begin(), oldGrids.end(), [](const SudokuGrid& A, const SudokuGrid& B){
                    return A.fitness < B.fitness;
                    });

                if(currentTopFitness == oldGrids.at(0).fitness)
                    restart++; 

                cout << "Generation " << generation << endl;
                cout << "Best Fitness: " << oldGrids.at(0).fitness << endl;

                if(oldGrids.at(0).fitness == 0){
                    cout << "Answer found: " << endl;
                    oldGrids.at(0).printSudokuGrid();
                    break;
                }
                if(restart == 50){
                    for(int i = 0; i < 5; i++)
                        restartGrids.push_back(oldGrids.at(i));
                    sort(restartGrids.begin(), restartGrids.end(), [](const SudokuGrid& A, const SudokuGrid& B){
                        return A.fitness < B.fitness;
                    });
                    if(restartGrids.size() > 20)
                        for(int i = 0; i < 5; i++)
                            restartGrids.pop_back();

                    cout << "Restarting... ";
                    startTheGA(mutation, crossover, restartGrids);
                }

                    
            }
        }
};

int main(){ 

    // Seeding pseudo random number generator 
    timeval t1{};
    mingw_gettimeofday(&t1, nullptr);
    srand(t1.tv_usec * t1.tv_sec);

    GeneticAlgorithm GA;

    double mutation{};
    double crossover{};

    cin >> mutation;
    cin >> crossover;

    vector<SudokuGrid> restartGrids; 

    GA.startTheGA(mutation, crossover, restartGrids);
}
