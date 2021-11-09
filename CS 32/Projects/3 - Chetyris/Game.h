#ifndef GAME_INCLUDED
#define GAME_INCLUDED

#include "Piece.h"
#include "UserInterface.h"
#include "Well.h"
#include <string>
#include <queue>
#include <cmath>

class Game
{
  public:

	  // Constructor
    Game(int width, int height);

	  // Play the level
	void play();
    bool playOneLevel();
	  
	  // Display
    void displayPrompt(std::string s);
    void displayStatus();
	void pushRandomPiece(PieceType pt);
	void clearNextPiece();

  private:

	  // Game status variables 
    Well    m_well;
    Screen  m_screen;
    int     m_level;
	int	    m_score;
	int		m_rowsLeft;

	  // Game piece variables 
	queue<Piece*> m_pcs;
	Piece*		  m_piece;
	Piece*		  m_ptr;
	Timer		  m_timer;
	char		  m_input;

	  // Well variables
	int m_width;
	int m_height;
};

#endif // GAME_INCLUDED