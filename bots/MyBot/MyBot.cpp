/**
* @file MyBot.cpp
* @author T B Ramkamal <EE18B153>
* @course CS6380 Assignment 2
* */

#include "Othello.h"
#include "OthelloBoard.h"
#include "OthelloPlayer.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <bits/stdc++.h>
using namespace std;
using namespace Desdemona;

#define INF 2000000

static int BOARD_SIZE = 8;

//////////////////////////////////////////////////////////////////////////////////////
// helper function to return 
list<Move> gVM(const Turn turn, const OthelloBoard& board)
{

    // OthelloBoard b = board;

    int filled[BOARD_SIZE+2][BOARD_SIZE+2] = {0};
    int outlined[BOARD_SIZE+2][BOARD_SIZE+2] = {0};
    int outline[BOARD_SIZE][BOARD_SIZE] = {0};

    list<Move> moves;

    for(int i=1; i<BOARD_SIZE+1; i++)
    {
        for(int j=1; j<BOARD_SIZE+1; j++)
        {
            if(board.get(i-1, j-1) != EMPTY)
            {
                filled[i][j] = 1;
                outlined[i-1][j-1] = outlined[i-1][j] = outlined[i-1][j+1] = outlined[i][j-1] = outlined[i][j] = outlined[1][j+1] = outlined[i+1][j-1] = outlined[i+1][j] = outlined[i+1][j+1] = 1;
            }
        }
    }

    for(int i=0; i<BOARD_SIZE; i++)
    {
        for(int j=0; j<BOARD_SIZE; j++)
        {
            outline[i][j] = outlined[i+1][j+1] - filled[i+1][j+1];
            if(outline[i][j])
            {
                if (board.validateMove(turn, i, j))
                    moves.push_back(Move(i, j));
            }
        }
    }

    return moves;

}

//////////////////////////////////////////////////////////////////////////////////////

class MyBot: public OthelloPlayer
{
    public:
        /**
         * Initialisation routines here
         */
        int moveNum;
        int weights[8][8];

        MyBot( Turn turn );
        virtual ~MyBot() {}

        int calcH(const OthelloBoard& board, Turn turn, int hVal);
        int abPruning(int alpha, int beta, const OthelloBoard& board, Turn turn, Turn local, int player, int kply, int hVal);

        // int eval(Turn turn, const OthelloBoard& board);
        // int evaluate(Turn turn, const OthelloBoard& board);

        /**
         * Play something 
         */
        virtual Move play( const OthelloBoard& board );
    private:
        

};

MyBot::MyBot( Turn turn ) :
    OthelloPlayer( turn )
{
    
    //pre-measured pre-processed values
    int temp[8][8] = {
        {150, -30, 20, 5, 5, 20, -30, 150},
        {-30, -50, -5, -5, -5, -5, -40, -30},
        {20, -5, 15, 5, 5, 15, -5, 20},
        {5, -5, 5, 0, 0, 5, -5, 5},
        {5, -5, 5, 0, 0, 5, -5, 5},
        {20, -5, 15, 5, 5, 15, -5, 20},
        {-30, -50, -5, -5, -5, -5, -50, -30},
        {150, -30, 20, 5, 5, 20, -30, 150}
    };
        
    for(int i = 0; i < BOARD_SIZE; i++) {
       for(int j = 0; j < BOARD_SIZE; j++) {
           weights[i][j] = temp[i][j];
       }
    }

}

Move MyBot::play( const OthelloBoard& board )
{
    OthelloBoard b = board;

    list<Move> moves = b.getValidMoves( turn );
    // list<Move> moves = gVM(turn, b);

    if (moves.size()==0)
    {
        return Move::pass();
    }

    Move *bestMove = NULL;

    int rCount = b.getRedCount();
    int bCount = b.getBlackCount();

    // cout<<bCount<<" "<<rCount<<" | ";

    moveNum = rCount + bCount - 3;

    // cout<<moveNum<<" move - size "<<moves.size()<<endl;

    int k;

    if (moveNum<=30)
        k = 2;
    else if (moveNum<=40)
        k = 4;
    else if (moveNum<=55)
        k = 6;
    else
        k = 8;

    // cout<<moveNum<<endl;
    int maxH = INT_MIN/2;
    int tempH;

    OthelloBoard boardCopy;

    for (list<Move>::iterator it1=moves.begin(); it1!=moves.end(); it1++)
    {
        boardCopy = b;
        boardCopy.makeMove(turn, *it1);
        // MAKE THE MOVE THEN DO THE CALCULATION
        tempH = abPruning(INT_MIN/2,INT_MAX/2, boardCopy, turn, other(turn), 2, k, 1);
        if (tempH>=maxH || it1==moves.begin())
        {
            maxH = tempH;
            bestMove = &(*it1);
        } 
    }
    return *bestMove;
}



int MyBot::abPruning(int alpha, int beta, const OthelloBoard& board, Turn turn, Turn local, int player, int kply, int hVal)
{

    OthelloBoard b = board;

    if (kply==0)
    {
        return calcH(b, turn, hVal);
        // return eval(turn, b);
    }
    
    else if (player==1)
    {

        list<Move> nextMoves = b.getValidMoves(local);
        // list<Move> nextMoves = gVM(local, b);

        if (nextMoves.size()==0)
        {   
            list<Move> oppMoves = b.getValidMoves(other(local));
            // list<Move> oppMoves = gVM(other(local), b);
            

            if (oppMoves.size()!=0)
            {
                OthelloBoard boardCopy;
                boardCopy = b;

                return abPruning(alpha, beta, boardCopy, turn, other(local), 2, kply-1, hVal); 
            }

            else
            {
                int rCount = b.getRedCount();
                int bCount = b.getBlackCount();

                if (bCount==rCount)
                    return 0;

                else if (bCount>rCount)
                    return (turn==BLACK)?INT_MAX/2:INT_MIN/2;

                else
                    return (turn==RED)?INT_MAX/2:INT_MIN/2;
            }
            
        }

        else
        {
            OthelloBoard boardCopy;
            int maxH = INT_MIN/2;
            int tempH;
            
            for (list<Move>::iterator it0 = nextMoves.begin(); it0!=nextMoves.end(); ++it0)
            {
                boardCopy = b;
                boardCopy.makeMove(local, *it0);
                tempH = abPruning(alpha, beta, boardCopy, turn, other(local), 2, kply-1, hVal);
                maxH = max(tempH, maxH);
                alpha = max(alpha, maxH);
                if (beta <= alpha)
                    break;
            }
            
            return maxH;

        }
    }
    

    else if (player==2)
    {
        // cout<<player<<local<<endl;
        list<Move> nextMoves = b.getValidMoves(local);
        // list<Move> nextMoves = gVM(local, b);

        if (nextMoves.size()==0)
        {   
            list<Move> oppMoves = b.getValidMoves(other(local));
            // list<Move> oppMoves = gVM(other(local), b);


            if (oppMoves.size()!=0)
            {
                OthelloBoard boardCopy;
                boardCopy = b;

                return abPruning(alpha, beta, boardCopy, turn, other(local), 1, kply-1, hVal); 
            }

            else
            {
                int rCount = b.getRedCount();
                int bCount = b.getBlackCount();

                if (bCount==rCount)
                    return 0;

                else if (bCount>rCount)
                    return (turn==BLACK)?INT_MAX/2:INT_MIN/2;

                else
                    return (turn==RED)?INT_MAX/2:INT_MIN/2;
            }
            
        }

        else
        {
            OthelloBoard boardCopy;
            int minH = INT_MAX/2;
            int tempH;
            
            for (list<Move>::iterator it2 = nextMoves.begin(); it2!=nextMoves.end(); ++it2)
            {
                boardCopy = b;
                boardCopy.makeMove(local, *it2);
                tempH = abPruning(alpha, beta, boardCopy, turn, other(local), 1, kply-1, hVal);
                minH = min(tempH, minH);
                beta = min(beta, minH);
                if (beta <= alpha)
                    break;
            }
            
            return minH;
        }   
    }

}

int MyBot::calcH(const OthelloBoard& board, Turn turn, int hVal)
{

    Turn opp = other(turn);
    
    // positioning

    /*
    int costs[8][8] = {
        {30, -5, 12, 8, 8, 12, -5, 30},
        {-5, -8, -5, 2, 2, -5, -8, -5},
        {12, -5, 4, 4, 4, 4, -5, -12},
        {8, 2, 4, 0, 0, 4, 2, 8},
        {8, 2, 4, 0, 0, 4, 2, 8},
        {12, -5, 4, 4, 4, 4, -5, -12},
        {-5, -8, -5, 2, 2, -5, -8, -5},
        {30, -5, 12, 8, 8, 12, -5, 30}
    };
    */

    int costs[8][8] = { { 20, -3, 11, 8, 8, 11, -3, 20 },
                    { -3, -7, -4, 1, 1, -4, -7, -3 },
                    { 11, -4, 2, 2, 2, 2, -4, 11 },
                    { 8, 1, 2, 0, 0, 2, 1, 8 },
                    { 8, 1, 2, 0, 0, 2, 1, 8 },
                    { 11, -4, 2, 2, 2, 2, -4, 11 },
                    { -3, -7, -4, 1, 1, -4, -7, -3 },
                    { 20, -3, 11, 8, 8, 11, -3, 20 } };


    int turnPos = 0;
    int oppPos = 0;

    int turnFrontTiles = 0;
    int oppFrontTiles = 0;

    int neighbours[][2] = {
        {-1, 0},
        {-1, 1},
        {0, 1},
        {1, 1},
        {1, 0},
        {1, -1},
        {0, -1},
        {-1, -1}
    };

    for(int j=0; j<8; j++) 
    {
        for (int i=0; i<8; i++)
        {
            if (board.get(i, j)==turn)
            {
                turnPos += costs[i][j];
            }
            else if (board.get(i, j)==opp)
            {
                oppPos += costs[i][j];
            }
            else
            {
                int in, jn;

                for (int k=0; k<8; k++)
                {
                    in = i + neighbours[k][0];
                    jn = j + neighbours[k][1];
                    if ((in>=0 && in<8) && (jn >=0 && jn<8))
                    // if (board.validPosition(in, jn))
                    {
                        (board.get(in, jn)==turn)?(++turnFrontTiles):(board.get(in, jn)==opp)?(++oppFrontTiles):1;
                    }
                }
            }
        }
    }

    float positioning = ((turnPos + oppPos)!=0)?( 1 * (float) (turnPos - oppPos)/(turnPos + oppPos)):0;

    
    // front tiles

    float frontTiles =  ((turnFrontTiles + oppFrontTiles)!=0)?(1 *  (float) turnFrontTiles / (turnFrontTiles + oppFrontTiles)):0;
    if (oppFrontTiles > turnFrontTiles)
        frontTiles = frontTiles - 1;

    // frontTiles *= 50;
    
    // coin parity

    int rCount = board.getRedCount();
    int bCount = board.getBlackCount();

    int turnCoin = (turn==BLACK)?bCount:rCount;
    int oppCoin = (turn==BLACK)?rCount:bCount;

    float coinParity = ((turnCoin + oppCoin)!=0)?(1 * (float) (turnCoin - oppCoin) / (turnCoin + oppCoin)):0;
    
    // corner positioning

    int myCorner =0, oppCorner = 0;

    if (board.get(0, 0)==EMPTY)
    {
        (board.get(1, 0)==turn)?(++myCorner):(board.get(1, 0)==opp)?(++oppCorner):1;
        (board.get(1, 1)==turn)?(++myCorner):(board.get(1, 1)==opp)?(++oppCorner):1;
        (board.get(0, 1)==turn)?(++myCorner):(board.get(0, 1)==opp)?(++oppCorner):1;
    }

    if (board.get(0, 7)==EMPTY)
    {   
        (board.get(1, 7)==turn)?(++myCorner):(board.get(1, 7)==opp)?(++oppCorner):1;
        (board.get(1, 6)==turn)?(++myCorner):(board.get(1, 6)==opp)?(++oppCorner):1;
        (board.get(0, 6)==turn)?(++myCorner):(board.get(0, 6)==opp)?(++oppCorner):1;
    }

    if (board.get(7, 0)==EMPTY)
    {   
        (board.get(7, 1)==turn)?(++myCorner):(board.get(7, 1)==opp)?(++oppCorner):1;
        (board.get(6, 1)==turn)?(++myCorner):(board.get(6, 1)==opp)?(++oppCorner):1;
        (board.get(6, 0)==turn)?(++myCorner):(board.get(6, 0)==opp)?(++oppCorner):1;
    }

    if (board.get(7, 7)==EMPTY)
    {   
        (board.get(7, 6)==turn)?(++myCorner):(board.get(7, 6)==opp)?(++oppCorner):1;
        (board.get(6, 7)==turn)?(++myCorner):(board.get(6, 7)==opp)?(++oppCorner):1;
        (board.get(6, 6)==turn)?(++myCorner):(board.get(6, 6)==opp)?(++oppCorner):1;
    }

    int cornerCloseness = oppCorner - myCorner;

    // corner occupancy

    int cornerPieces[][2] = {
        {0, 0},
        {0, 7},
        {7, 0},
        {7, 7}
    };

    int p, q;

    float turnTiles = 0;
    float oppTiles = 0;

    for(int c=0; c<4; c++)
    {
        p = cornerPieces[c][0];
        q = cornerPieces[c][1];
        if (board.get(p, q)==turn)
        {
            turnTiles += 1;
        }
        else if (board.get(p, q)==opp)
        {
            oppTiles += 1;
        }
        else
        {
            (board.validateMove(turn, p, q))?(++turnTiles):(board.validateMove(opp, p, q))?(++oppTiles):1;
        }
    }

    float cornerOccupancy = ((turnTiles+oppTiles)!=0)?(1.0 * (float) (turnTiles-oppTiles)/(turnTiles+oppTiles)):0;

    // mobility

    float turnMoves = board.getValidMoves(turn).size();
    float oppMoves = board.getValidMoves(opp).size();

    float mobility = ((turnMoves+oppMoves)!=0)?(1 * (float)(turnMoves-oppMoves)/(turnMoves+oppMoves)):0;

    // printf("Pos FT CP CC CO MOB ");
    // cout<<10000*positioning<<" "<<7500*frontTiles<<" "<<1000*coinParity<<" "<<275*cornerCloseness<<" "<<10000*cornerOccupancy<<" "<<10000*mobility<<endl;

    if (moveNum<=20)
    {
        return 10000 * positioning + 1000 * coinParity + 10000 * mobility;
        // return 10000 * coinParity;
    }
    else if (moveNum<=40)
    {
        return 10000 * coinParity + 300 * cornerCloseness + 10000 * cornerOccupancy + 10000 * mobility;    
        // return 
    }
    else if (moveNum<=50)
    {
        return 1000 * coinParity + 300 * cornerCloseness + 10000 * cornerOccupancy + 10000 * mobility;    
    }
    else
    {
        return 10000 * coinParity;
    }

    // cout<<(1 * positioning + 155 * frontTiles + 100 * coinParity + 3820 * cornerCloseness + 1000 * cornerOccupancy + 88 * mobility)<<" "; 
    
    return 10000 * positioning + 7500 * frontTiles + 1000 * coinParity + 275 * cornerCloseness + 10000 * cornerOccupancy + 10000 * mobility;
    
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


