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
using namespace std;
using namespace Desdemona;


class MyBot: public OthelloPlayer
{
    public:
        /**
         * Initialisation routines here
         * This could do anything from open up a cache of "best moves" to
         * spawning a background processing thread. 
         */
        MyBot(Turn turn);

        /**
         * Play something 
         */
        virtual Move play(const OthelloBoard& board);
        virtual Move get_opt_move(const OthelloBoard& board);
        virtual int alphabeta(const OthelloBoard& board, int A, int B, int depth);
        virtual int score(const OthelloBoard& board);
    private:
};

MyBot::MyBot(Turn turn)
    : OthelloPlayer(turn)
{
}

Move MyBot::play(const OthelloBoard& board)
{
    Move play_move = get_opt_move(board);
    return play_move;
}

Move MyBot::get_opt_move(const OthelloBoard& board)
{
    int A = -1000;
    int B = 1000;
    int depth = 1;
    
    list<Move> valid_moves = board.getValidMoves(turn);
    list<Move>::iterator it = valid_moves.begin();
    Move optimal_move = *it;
    for (int i=1;it != valid_moves.end();++it,++i){
        OthelloBoard new_board = board;
        new_board.makeMove(turn,*it);
        int new_A = alphabeta(new_board,A,B,depth);
        if (new_A>A) {
            A=new_A;
            optimal_move = *it;
        }
    }
    return optimal_move;
}
int MyBot::alphabeta(const OthelloBoard& board, int A, int B, int depth)
{
    // OthelloBoard& now_board = board
    if (depth==4) {
        return score(board);
    }
    else if (depth%2==0) {
        depth++;
        Turn turn_max = (turn==RED)?RED:BLACK;
        list<Move> valid_moves = board.getValidMoves(turn_max);
        list<Move>::iterator it;
        for (it = valid_moves.begin();it != valid_moves.end();++it){
            OthelloBoard new_board = board;
            new_board.makeMove(turn_max,*it);
            A = std::max(A,alphabeta(new_board,A,B,depth));
            if (A>B) return B;
        }
        return A;
    }
    else if (depth%2==1) {
        depth++;
        Turn turn_min = (turn==RED)?BLACK:RED;
        list<Move> valid_moves = board.getValidMoves(turn_min);
        list<Move>::iterator it;
        for (it = valid_moves.begin();it != valid_moves.end();++it){
            OthelloBoard new_board = board;
            new_board.makeMove(turn_min,*it);
            B = std::min(B,alphabeta(new_board,A,B,depth));
            if (A>B) return A;
        }
        return B;
    }
    return 0;
}

int MyBot::score(const OthelloBoard& board)
{
    Turn turn_opp = (turn==RED)?BLACK:RED;
    int total_occupied = board.getBlackCount()+board.getRedCount();
    int heur[8][8] = {{8,-3,5,5,5,5,-3,8},
    			{-3,-7,1,1,1,1,-7,-3},
    			{5,1,3,3,3,3,1,5},
    			{5,1,3,3,3,3,1,5},
	               {5,1,3,3,3,3,1,5},
	               {5,1,3,3,3,3,1,5},
	               {-3,-7,1,1,1,1,-7,-3},
	               {8,-3,5,5,5,5,-3,8}};
    if (total_occupied>16){
    	int heur[8][8] = {{8,-5,5,5,5,5,-5,8},
			{-5,-7,1,1,1,1,-7,-5},
			{5,-1,1,1,1,1,-1,5},
			{5,-1,1,1,1,1,-1,5},
		       {5,-1,1,1,1,1,-1,5},
		       {5,-1,1,1,1,1,-1,5},
		       {-5,-7,-1,-1,-1,-1,-7,-5},
		       {8,-5,5,5,5,5,-5,8}};
    };
    int sum = 0;
    for(int r=0;r<8;r++){
        for(int c=0;c<8;c++){
            if (board.get(r,c)==turn) sum+=heur[r][c];
            if (board.get(r,c)==turn_opp) sum-=heur[r][c];
        }
    }
    return sum;
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



