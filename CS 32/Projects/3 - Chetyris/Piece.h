#ifndef PIECE_INCLUDED
#define PIECE_INCLUDED

#include "UserInterface.h"
#include "Well.h"
using namespace std;

class Screen;
class Well;

enum PieceType {
    PIECE_I, PIECE_L, PIECE_J, PIECE_T, PIECE_O, PIECE_S, PIECE_Z,
    PIECE_VAPOR, PIECE_FOAM, PIECE_CRAZY, NUM_PIECE_TYPES
};

PieceType chooseRandomPieceType();

////////////////////////////////
// Piece base class functions //
////////////////////////////////

class Piece
{
public:

	  // Constructor, Destructor 
	Piece(Screen& screen, Well& well);
	virtual ~Piece();

	  // Movement
	void move();
	void skip();
	bool moveLeft();
	bool moveRight();
	bool moveDown();
	bool canMove(char ch);
	void rotate();
	bool canRotate();
	void doRotate();

	  // Mutators
	bool canDisplayPiece();
	void setPieceType(PieceType pt);
	void setPiece(char piece[PIECE_HEIGHT][PIECE_WIDTH]);
	void setBoxes(char piece[PIECE_HEIGHT][PIECE_WIDTH], int orientation);
	void transcribe();
	void displayXY(int x, int y);
	virtual void finishPiece();
	virtual void vaporize(Well& well);
	virtual void fillEmptySpaces(Well& well, int i, int j);

	  // Accessors
	PieceType accessPieceType();
	Well*	  accessWell();
	int		  accessX();
	int		  accessY();

private:

	  // Bounding box and piece orientations
	char m_piece[PIECE_WIDTH][PIECE_HEIGHT];
	char m_box_0[PIECE_HEIGHT][PIECE_WIDTH];
	char m_box_1[PIECE_HEIGHT][PIECE_WIDTH];
	char m_box_2[PIECE_HEIGHT][PIECE_WIDTH];
	char m_box_3[PIECE_HEIGHT][PIECE_WIDTH];
	int m_orientation;
	  
	  // Piece info
	PieceType m_pt;
	Screen*   m_screen;
	Well*     m_well;
	int		  m_x;	
	int		  m_y;
};

/////////////////////////////////
// Piece sub classes functions //
/////////////////////////////////

class Piece_T : public Piece
{
public:
	Piece_T(Screen& screen, Well& well);
	~Piece_T();
};

class Piece_L : public Piece
{
public:
	Piece_L(Screen& screen, Well& well);
	~Piece_L();
};

class Piece_J : public Piece
{
public:
	Piece_J(Screen& screen, Well& well);
	~Piece_J();
};

class Piece_O : public Piece
{
public:
	Piece_O(Screen& screen, Well& well);
	~Piece_O();
};

class Piece_S : public Piece
{
public:
	Piece_S(Screen& screen, Well& well);
	~Piece_S();
};

class Piece_Z : public Piece
{
public:
	Piece_Z(Screen& screen, Well& well);
	~Piece_Z();
};

class Piece_I : public Piece
{
public:
	Piece_I(Screen& screen, Well& well);
	~Piece_I();
};

class Piece_VAPOR : public Piece
{
public:
	Piece_VAPOR(Screen& screen, Well& well);
	~Piece_VAPOR();
	bool canVaporize(char ch);
	virtual void vaporize(Well& well);
private:
	Screen* m_screen;
	Well* m_well;
};

class Piece_FOAM : public Piece
{
public:
	Piece_FOAM(Screen& screen, Well& well);
	~Piece_FOAM();
	void fillEmptySpaces(Well& well, int i, int j);
	virtual void finishPiece();
	bool canFill(char ch);
private:
	Screen* m_screen;
	Well* m_well;
};

class Piece_CRAZY : public Piece
{
public:
	Piece_CRAZY(Screen& screen, Well& well);
	~Piece_CRAZY();
};

#endif // PIECE_INCLUDED