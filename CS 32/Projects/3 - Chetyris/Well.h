#ifndef WELL_INCLUDED
#define WELL_INCLUDED

#include <vector>
using namespace std;

// Global constants

const char PIECE = '#';
const char FILLED = '$';
const char FOAM = '*';
const char WALL = '@';
const char EMPTY = ' ';

const int INITIAL_ORIENTATION = 0;
const int PIECE_WIDTH = 4;
const int PIECE_HEIGHT = 4;
const int INITIAL_X = 4;
const int INITIAL_Y = 0;

class Screen;

class Well
{
	public:

		  // Constructor
		Well(int width, int height);
		~Well();

		  // Accessors
		char* accessWellItem(int x, int y);
		int   accessHeight();

		  // Mutators
		void  display(Screen& screen, int x, int y);
		void  displayContents(Screen& screen);
		void  clearContents(Screen& screen);
		void  clearWell(Screen& screen);
		bool  isRowFilled(int y);
		void  clearRowFilled(int& score, int& rowsLeft);
		void  dropDown(int y);

	private:
		char** m_well;
		int	   m_width;
		int	   m_height;
		int    m_yWall;
		int    m_xWall;
};

#endif // WELL_INCLUDED
