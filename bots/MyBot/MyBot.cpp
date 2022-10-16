/*
* @file botTemplate.cpp
* @author Arun Tejasvi Chaganty <arunchaganty@gmail.com>
* @date 2010-02-04
* Template for users to create their own bots
*/

#include "Othello.h"
#include "OthelloBoard.h"
#include "OthelloPlayer.h"
#include <cstdlib>
#include <climits>
using namespace std;
using namespace Desdemona;

clock_t start,finish;
Turn Mycolour, OpponentColour;
int Myval, Myopponenetval;

int staticBoard[8][8] =  { { 10, -9,  2,  2,  2,  2, -3,  10},
                            {-9, -10, -2, -2, -2, -2, -10, -3},
                            { 2, -2,  1,  0,  0,  1, -2,  2},
                            { 2, -2,  0,  1,  1,  0, -2,  2},
                            { 2, -2,  0,  1,  1,  0, -2,  2},
                            { 2, -2,  1,  0,  0,  1, -2,  2},
                            {-9, -10, -2, -2, -2, -2, -10, -9},
                            { 10, -9,  2,  2,  2,  2, -9,  10}
                        };                  

bool compare(Move a, Move b) {
    return staticBoard[a.x][a.y] < staticBoard[b.x][b.y];
}

double BoardEvaluation(OthelloBoard board, Turn colour){

    int myBeads, oppBeads;
    float coinParity, mobility, corners, cornerclose;

    myBeads = oppBeads = 0;
    coinParity = mobility = corners = cornerclose = 0.0;

    //Piece Difference (Coin pairity)
    if(Mycolour == RED){
        myBeads = board.getRedCount();
        oppBeads = board.getBlackCount();
    }
    else{
        oppBeads = board.getRedCount();
        myBeads = board.getBlackCount();        
    }

    coinParity = 100*(myBeads - oppBeads)/(myBeads + oppBeads + 0.0);

    //Mobility
    int MyValidmoves = board.getValidMoves(Mycolour).size();
    int OppValidmoves = board.getValidMoves(OpponentColour).size();
    if(MyValidmoves + OppValidmoves != 0){
        mobility = 100*(MyValidmoves-OppValidmoves)/(MyValidmoves+OppValidmoves+ 0.0);
    }
    else{
        mobility = 0;
    }

    //Corner Captured and corner closeness
    int Mycorners, OppCorners;
    Mycorners = OppCorners = 0;
    myBeads = oppBeads = 0;
    if(board.get(0,0) == Mycolour) Mycorners++;
    else if(board.get(0,0) == OpponentColour) OppCorners++;
    else{
        if(board.get(0,1) == Mycolour) myBeads++;
        else if(board.get(0,1) == OpponentColour) oppBeads++;
        if(board.get(1,0) == Mycolour) myBeads++;
        else if(board.get(1,0) == OpponentColour) oppBeads++;
        if(board.get(1,1) == Mycolour) myBeads++;
        else if(board.get(1,1) == OpponentColour) oppBeads++;                
    }

    if(board.get(0,7) == Mycolour) Mycorners++;
    else if(board.get(0,7) == OpponentColour) OppCorners++;
    else{
        if(board.get(0,6) == Mycolour) myBeads++;
        else if(board.get(0,6) == OpponentColour) oppBeads++;
        if(board.get(1,6) == Mycolour) myBeads++;
        else if(board.get(1,6) == OpponentColour) oppBeads++;
        if(board.get(1,7) == Mycolour) myBeads++;
        else if(board.get(1,7) == OpponentColour) oppBeads++;                
    }

    if(board.get(7,0) == Mycolour) Mycorners++;
    else if(board.get(7,0) == OpponentColour) OppCorners++;
    else{
        if(board.get(7,1) == Mycolour) myBeads++;
        else if(board.get(7,1) == OpponentColour) oppBeads++;
        if(board.get(6,1) == Mycolour) myBeads++;
        else if(board.get(6,1) == OpponentColour) oppBeads++;
        if(board.get(6,0) == Mycolour) myBeads++;
        else if(board.get(6,0) == OpponentColour) oppBeads++;                
    }    

    if(board.get(7,7) == Mycolour) Mycorners++;
    else if(board.get(7,7) == OpponentColour) OppCorners++;    
    else{
        if(board.get(6,7) == Mycolour) myBeads++;
        else if(board.get(6,7) == OpponentColour) oppBeads++;
        if(board.get(6,6) == Mycolour) myBeads++;
        else if(board.get(6,6) == OpponentColour) oppBeads++;
        if(board.get(7,6) == Mycolour) myBeads++;
        else if(board.get(7,6) == OpponentColour) oppBeads++;                
    }

    if(Mycorners+OppCorners != 0) {
        corners = 100.0*(Mycorners-OppCorners)/(Mycorners+OppCorners+0.0);
    } else {
        corners = 0;
    }  

    if(myBeads+oppBeads != 0){
        cornerclose = (-1)*100.0*(myBeads-oppBeads)/(myBeads+oppBeads+0.0);
    }
    else{
        cornerclose = 0;
    }

    //Static Heuristic Function
    myBeads = oppBeads = 0;
    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            if(board.get(i,j) == Mycolour){
                myBeads += staticBoard[i][j];
            }
            else if(board.get(i,j) == OpponentColour){
                oppBeads += staticBoard[i][j];
            }
        }
    }
    double statHeuristic = myBeads - oppBeads;

    float stability = (10.1*coinParity)+(70.1*mobility)+(800.1*corners)+(100.1*cornerclose)+(10.1*statHeuristic);
    return stability;
}

double evaluation(Turn colour, Move move, OthelloBoard board, double alpha, double beta, int depth, int sign){

    finish = clock();
    float time_elapsed = (finish-start)/CLOCKS_PER_SEC;

    if(time_elapsed > 1.90) {
            return (-1)*1e15;
    }
    else if(depth == 0){
        if(sign == 1)
             return BoardEvaluation(board, colour);
        else
             return -BoardEvaluation(board, colour);
    }
    else{
        board.makeMove(colour,move);
        colour = other(colour);

        list<Move> moves = board.getValidMoves(colour);
        list<Move>::iterator it = moves.begin();

        double finalEval = -1e15;
        while(it != moves.end()){
            finalEval = max(finalEval, -evaluation(colour,*it,board,-beta,-alpha,depth-1,1-sign));
            alpha = max(alpha, finalEval);
            if(alpha >= beta) break;
            it++;
        }
        return finalEval;
    }
}

class MyBot: public OthelloPlayer
{
    public:
        /**
         * Initialisation routines here
         * This could do anything from open up a cache of "best moves" to
         * spawning a background processing thread. 
         */
        MyBot( Turn turn );

        /**
         * Play something 
         */
        virtual Move play( const OthelloBoard& board );
    private:
};

MyBot::MyBot( Turn turn )
    : OthelloPlayer( turn )
{
}

Move MyBot::play( const OthelloBoard& board )
{
    //Initialization
    list<Move> moves;
    list<Move>::iterator it,best,temp;
    OthelloBoard tempBoard;
    double best_eval, curr_eval, alpha, beta;

    //Clock Starts now
    start = clock();   
    
    Mycolour = turn;
    Myval = 1;
    OpponentColour = other(Mycolour);
    Myopponenetval = -1;

    tempBoard = board;
    moves = board.getValidMoves( turn );
    moves.sort(compare);
    best = temp = it = moves.begin();
    best_eval = alpha = -1e17;
    beta = -alpha;

    while(it != moves.end()){
        
        curr_eval = evaluation(turn, *it, tempBoard, alpha, beta, 5,1);
        if(curr_eval > best_eval){
            best = it;
            best_eval = curr_eval;
        }
        //Checking for '2 second' disqualification time
        finish = clock();
        if((float)(finish-start)/CLOCKS_PER_SEC > 1.90)
            return *best;
        it++;
    }
    return *best;
}

// The following lines are _very_ important to create a bot module for Desdemona

extern "C" {
    OthelloPlayer* createBot( Turn turn )
    {
        return new MyBot( turn );
    }

    void destroyBot( OthelloPlayer* bot )
    {
        delete bot;
    }
}


