#include "Piece.h"

////////////////////////////////
// Piece base class functions //
////////////////////////////////

Piece::Piece(Screen& screen, Well& well)
	: m_x(INITIAL_X), m_y(INITIAL_Y), m_screen(&screen), m_well(&well), m_orientation(INITIAL_ORIENTATION)
{}

Piece::~Piece()
{
}

void Piece::move()
{
	// Display the piece's movement in the well
	accessWell()->clearContents(*m_screen);
	transcribe();
	accessWell()->displayContents(*m_screen);
}

void Piece::skip()
{
	// While space is available, move the piece down
	while (moveDown())
	{
		accessWell()->clearContents(*m_screen);
		transcribe();
	}
	accessWell()->displayContents(*m_screen);
}

bool Piece::moveLeft()
{
	// Move left if possible
	for (int i = 0; i < PIECE_HEIGHT; i++)
	{
		for (int j = 0; j < PIECE_WIDTH; j++)
		{
			if (m_piece[i][j] == PIECE)
			{
				if (!canMove(*(m_well->accessWellItem(m_x + j - 1, m_y + i))))
				{
					return false;
				}
				break;
			}
		}
	}
	m_x--;
	return true;
}

bool Piece::moveRight()
{
	// Move right if possible
	for (int i = 0; i < PIECE_HEIGHT; i++)
	{
		for (int j = PIECE_WIDTH; j > 0; j--)
		{
			if (m_piece[i][j - 1] == PIECE)
			{
				if (!canMove(*(m_well->accessWellItem(m_x + j, m_y + i))))
				{
					return false;
				}
				break;
			}
		}
	}
	m_x++;
	return true;
}

bool Piece::moveDown()
{
	// Move down if possible
	for (int i = 0; i < PIECE_WIDTH; i++)
	{
		for (int j = PIECE_HEIGHT - 1; j >= 0; j--)
		{
			if (m_piece[j][i] == PIECE)
			{
				if (!canMove(*(m_well->accessWellItem(m_x + i, m_y + j + 1))))
				{
					return false;
				}
				break;
			}
		}
	}
	m_y++;
	return true;
}

bool Piece::canMove(char ch)
{
	// Check for an empty space
	switch (ch)
	{
	case FILLED:
	case FOAM:
	case WALL:
		return false;
	case PIECE:
	case EMPTY:
	default:
		return true;
	}
}

void Piece::rotate()
{
	// Display the piece's rotation in the well
	accessWell()->clearContents(*m_screen);
	doRotate();
	transcribe();
	accessWell()->displayContents(*m_screen);
	if (m_orientation < 3)
		m_orientation++;
	else
		m_orientation -= 3;
}

bool Piece::canRotate()
{
	// Check the current orientation of the piece
	// Compare the next orientation of the piece in the well
	// If the next orientation's block collides with the well's pieces,
	// return false; otherwise, return true
	switch (m_orientation)
	{
	case 0:
		for (int i = 0; i < PIECE_HEIGHT; i++)
		{
			for (int j = 0; j < PIECE_WIDTH; j++)
			{
				if (m_box_1[i][j] == PIECE)
				{
					if (!canMove(*(m_well->accessWellItem(m_x + j, m_y + i))))
					{
						return false;
					}
				}
			}
		}
		return true;
	case 1:
		for (int i = 0; i < PIECE_HEIGHT; i++)
		{
			for (int j = 0; j < PIECE_WIDTH; j++)
			{
				if (m_box_2[i][j] == PIECE)
				{
					if (!canMove(*(m_well->accessWellItem(m_x + j, m_y + i))))
					{
						return false;
					}
				}
			}
		}
		return true;
	case 2:
		for (int i = 0; i < PIECE_HEIGHT; i++)
		{
			for (int j = 0; j < PIECE_WIDTH; j++)
			{
				if (m_box_3[i][j] == PIECE)
				{
					if (!canMove(*(m_well->accessWellItem(m_x + j, m_y + i))))
					{
						return false;
					}
				}
			}
		}
		return true;
	case 3:
		for (int i = 0; i < PIECE_HEIGHT; i++)
		{
			for (int j = 0; j < PIECE_WIDTH; j++)
			{
				if (m_box_0[i][j] == PIECE)
				{
					if (!canMove(*(m_well->accessWellItem(m_x + j, m_y + i))))
					{
						return false;
					}
				}
			}
		}
		return true;
	default:
		return true;
	}
}

void Piece::doRotate()
{
	// Rotate the piece
	switch (m_orientation)
	{
	case 0:
		setPiece(m_box_1);
		break;
	case 1:
		setPiece(m_box_2);
		break;
	case 2:
		setPiece(m_box_3);
		break;
	case 3:
		setPiece(m_box_0);
		break;
	default:
		break;
	}
}

bool Piece::canDisplayPiece()
{
	// Check if the piece at the initial position collides with anything 
	for (int i = 0; i < PIECE_WIDTH; i++)
	{
		for (int j = PIECE_HEIGHT - 1; j >= 0; j--)
		{
			if (m_piece[j][i] == PIECE)
			{
				if (!canMove(*(m_well->accessWellItem(INITIAL_X + i, INITIAL_Y + j))))
				{
					return false;
				}
				break;
			}
		}
	}
	return true;
}

void Piece::setPieceType(PieceType pt)
{
	m_pt = pt;
}

void Piece::setPiece(char piece[PIECE_HEIGHT][PIECE_WIDTH])
{
	// Fill the bounding box
	for (int i = 0; i < PIECE_HEIGHT; i++)
	{
		copy(piece[i], piece[i] + PIECE_WIDTH, m_piece[i]);
	}
}

void Piece::setBoxes(char piece[PIECE_HEIGHT][PIECE_WIDTH], int orientation)
{
	// Set the orientation boxes with the proper orientation pieces
	switch (orientation)
	{
	case 0:
		for (int i = 0; i < PIECE_HEIGHT; i++)
		{
			copy(piece[i], piece[i] + PIECE_WIDTH, m_box_0[i]);
		}
		break;
	case 1:
		for (int i = 0; i < PIECE_HEIGHT; i++)
		{
			copy(piece[i], piece[i] + PIECE_WIDTH, m_box_1[i]);
		}
		break;
	case 2:
		for (int i = 0; i < PIECE_HEIGHT; i++)
		{
			copy(piece[i], piece[i] + PIECE_WIDTH, m_box_2[i]);
		}
		break;
	case 3:
		for (int i = 0; i < PIECE_HEIGHT; i++)
		{
			copy(piece[i], piece[i] + PIECE_WIDTH, m_box_3[i]);
		}
		break;
	}
}

void Piece::transcribe()
{
	// Transcribe the piece into the well
	for (int i = 0; i < PIECE_HEIGHT; i++)
	{
		for (int j = 0; j < PIECE_WIDTH; j++)
		{
			if (m_piece[i][j] != EMPTY)
			{
				*(m_well->accessWellItem(m_x + j, m_y + i)) = m_piece[i][j];
			}
		}
	}
}

void Piece::displayXY(int x, int y)
{
	// Transcribe the piece into specific coordinates
	for (int i = 0; i < PIECE_HEIGHT; i++)
	{
		for (int j = 0; j < PIECE_WIDTH; j++)
		{
			if (m_piece[i][j] == PIECE)
			{
				m_screen->gotoXY(x + j, y + i);
				m_screen->printChar(PIECE);
			}
		}
	}
}

void Piece::finishPiece()
{
	// When a piece stops moving, change '#' to '$'
	for (int i = 0; i < PIECE_HEIGHT; i++)
	{
		for (int j = 0; j < PIECE_WIDTH; j++)
		{
			if (m_piece[i][j] == PIECE)
			{
				m_piece[i][j] = FILLED;
			}
		}
	}
	transcribe();
}

void Piece::vaporize(Well& well)
{
	return;
}

void Piece::fillEmptySpaces(Well & well, int i, int j)
{
	return;
}

PieceType Piece::accessPieceType()
{
	return m_pt;
}

int Piece::accessX()
{
	return m_x;
}

int Piece::accessY()
{
	return m_y;
}

Well* Piece::accessWell()
{
	return m_well;
}

/////////////////////////////////////
// Piece T derived class functions //
/////////////////////////////////////

Piece_T::Piece_T(Screen& screen, Well& well) : Piece(screen, well)
{
	char m_box_0[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', '#', ' ', ' ',
		'#', '#', '#', ' ',
		' ', ' ', ' ', ' ',
		' ', ' ', ' ', ' '
	};
	char m_box_1[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', '#', ' ', ' ',
		' ', '#', '#', ' ',
		' ', '#', ' ', ' ',
		' ', ' ', ' ', ' '
	};
	char m_box_2[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', ' ', ' ', ' ',
		'#', '#', '#', ' ',
		' ', '#', ' ', ' ',
		' ', ' ', ' ', ' '
	};
	char m_box_3[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', '#', ' ', ' ',
		'#', '#', ' ', ' ',
		' ', '#', ' ', ' ',
		' ', ' ', ' ', ' '
	};

	// Set the T piece and its orientations
	setPieceType(PIECE_T);
	setPiece(m_box_0);
	setBoxes(m_box_0, 0);
	setBoxes(m_box_1, 1);
	setBoxes(m_box_2, 2);
	setBoxes(m_box_3, 3);
}

Piece_T::~Piece_T()
{
}

/////////////////////////////////////
// Piece L derived class functions //
/////////////////////////////////////

Piece_L::Piece_L(Screen& screen, Well& well) : Piece(screen, well)
{
	char m_box_0[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', ' ', ' ', ' ',
		'#', '#', '#', ' ',
		'#', ' ', ' ', ' ',
		' ', ' ', ' ', ' '
	};
	char m_box_1[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', '#', '#', ' ',
		' ', ' ', '#', ' ',
		' ', ' ', '#', ' ',
		' ', ' ', ' ', ' '
	};
	char m_box_2[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', ' ', ' ', ' ',
		' ', ' ', '#', ' ',
		'#', '#', '#', ' ',
		' ', ' ', ' ', ' '
	};
	char m_box_3[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', ' ', ' ', ' ',
		' ', '#', ' ', ' ',
		' ', '#', ' ', ' ',
		' ', '#', '#', ' '
	};

	// Set the L piece and its orientations
	setPieceType(PIECE_L);
	setPiece(m_box_0);
	setBoxes(m_box_0, 0);
	setBoxes(m_box_1, 1);
	setBoxes(m_box_2, 2);
	setBoxes(m_box_3, 3);
}

Piece_L::~Piece_L()
{
}

/////////////////////////////////////
// Piece J derived class functions //
///////////////////////////////////// 

Piece_J::Piece_J(Screen& screen, Well& well) : Piece(screen, well)
{
	char m_box_0[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', ' ', ' ', ' ',
		' ', '#', '#', '#',
		' ', ' ', ' ', '#',
		' ', ' ', ' ', ' '
	};
	char m_box_1[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', ' ', ' ', ' ',
		' ', ' ', '#', ' ',
		' ', ' ', '#', ' ',
		' ', '#', '#', ' '
	};
	char m_box_2[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', ' ', ' ', ' ',
		' ', '#', ' ', ' ',
		' ', '#', '#', '#',
		' ', ' ', ' ', ' '
	};
	char m_box_3[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', '#', '#', ' ',
		' ', '#', ' ', ' ',
		' ', '#', ' ', ' ',
		' ', ' ', ' ', ' '
	};

	// Set the J piece and its orientations
	setPieceType(PIECE_J);
	setPiece(m_box_0);
	setBoxes(m_box_0, 0);
	setBoxes(m_box_1, 1);
	setBoxes(m_box_2, 2);
	setBoxes(m_box_3, 3);
}

Piece_J::~Piece_J()
{
}

/////////////////////////////////////
// Piece O derived class functions //
/////////////////////////////////////

Piece_O::Piece_O(Screen& screen, Well& well) : Piece(screen, well)
{
	char m_box_0[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		'#', '#', ' ', ' ',
		'#', '#', ' ', ' ',
		' ', ' ', ' ', ' ',
		' ', ' ', ' ', ' '
	};
	char m_box_1[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		'#', '#', ' ', ' ',
		'#', '#', ' ', ' ',
		' ', ' ', ' ', ' ',
		' ', ' ', ' ', ' '
	};
	char m_box_2[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		'#', '#', ' ', ' ',
		'#', '#', ' ', ' ',
		' ', ' ', ' ', ' ',
		' ', ' ', ' ', ' '
	};
	char m_box_3[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		'#', '#', ' ', ' ',
		'#', '#', ' ', ' ',
		' ', ' ', ' ', ' ',
		' ', ' ', ' ', ' '
	};

	// Set the O piece and its orientations
	setPieceType(PIECE_O);
	setPiece(m_box_0);
	setBoxes(m_box_0, 0);
	setBoxes(m_box_1, 1);
	setBoxes(m_box_2, 2);
	setBoxes(m_box_3, 3);
}

Piece_O::~Piece_O()
{
}

/////////////////////////////////////
// Piece S derived class functions //
/////////////////////////////////////

Piece_S::Piece_S(Screen& screen, Well& well) : Piece(screen, well)
{
	char m_box_0[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', ' ', ' ', ' ',
		' ', '#', '#', ' ',
		'#', '#', ' ', ' ',
		' ', ' ', ' ', ' '
	};
	char m_box_1[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', '#', ' ', ' ',
		' ', '#', '#', ' ',
		' ', ' ', '#', ' ',
		' ', ' ', ' ', ' '
	};
	char m_box_2[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', ' ', ' ', ' ',
		' ', '#', '#', ' ',
		'#', '#', ' ', ' ',
		' ', ' ', ' ', ' '
	};
	char m_box_3[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', '#', ' ', ' ',
		' ', '#', '#', ' ',
		' ', ' ', '#', ' ',
		' ', ' ', ' ', ' '
	};

	// Set the S piece and its orientations
	setPieceType(PIECE_S);
	setPiece(m_box_0);
	setBoxes(m_box_0, 0);
	setBoxes(m_box_1, 1);
	setBoxes(m_box_2, 2);
	setBoxes(m_box_3, 3);
}

Piece_S::~Piece_S()
{
}

/////////////////////////////////////
// Piece Z derived class functions //
/////////////////////////////////////

Piece_Z::Piece_Z(Screen& screen, Well& well) : Piece(screen, well)
{
	char m_box_0[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', ' ', ' ', ' ',
		'#', '#', ' ', ' ',
		' ', '#', '#', ' ',
		' ', ' ', ' ', ' '
	};
	char m_box_1[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', ' ', '#', ' ',
		' ', '#', '#', ' ',
		' ', '#', ' ', ' ',
		' ', ' ', ' ', ' '
	};
	char m_box_2[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', ' ', ' ', ' ',
		'#', '#', ' ', ' ',
		' ', '#', '#', ' ',
		' ', ' ', ' ', ' '
	};
	char m_box_3[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', ' ', '#', ' ',
		' ', '#', '#', ' ',
		' ', '#', ' ', ' ',
		' ', ' ', ' ', ' '
	};

	// Set the Z piece and its orientations
	setPieceType(PIECE_Z);
	setPiece(m_box_0);
	setBoxes(m_box_0, 0);
	setBoxes(m_box_1, 1);
	setBoxes(m_box_2, 2);
	setBoxes(m_box_3, 3);
}

Piece_Z::~Piece_Z()
{
}

/////////////////////////////////////
// Piece I derived class functions //
/////////////////////////////////////

Piece_I::Piece_I(Screen& screen, Well& well) : Piece(screen, well)
{
	char m_box_0[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', ' ', ' ', ' ',
		'#', '#', '#', '#',
		' ', ' ', ' ', ' ',
		' ', ' ', ' ', ' '
	};
	char m_box_1[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', '#', ' ', ' ',
		' ', '#', ' ', ' ',
		' ', '#', ' ', ' ',
		' ', '#', ' ', ' '
	};
	char m_box_2[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', ' ', ' ', ' ',
		'#', '#', '#', '#',
		' ', ' ', ' ', ' ',
		' ', ' ', ' ', ' '
	};
	char m_box_3[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', '#', ' ', ' ',
		' ', '#', ' ', ' ',
		' ', '#', ' ', ' ',
		' ', '#', ' ', ' '
	};

	// Set the I piece and its orientations
	setPieceType(PIECE_I);
	setPiece(m_box_0);
	setBoxes(m_box_0, 0);
	setBoxes(m_box_1, 1);
	setBoxes(m_box_2, 2);
	setBoxes(m_box_3, 3);
}

Piece_I::~Piece_I()
{
}

/////////////////////////////////////////
// Piece Vapor derived class functions //
/////////////////////////////////////////

Piece_VAPOR::Piece_VAPOR(Screen& screen, Well& well) : Piece(screen, well), m_screen(&screen), m_well(&well)
{
	char m_box_0[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', '#', '#', ' ',
		' ', ' ', ' ', ' ',
		' ', ' ', ' ', ' ',
		' ', ' ', ' ', ' '
	};
	char m_box_1[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', '#', '#', ' ',
		' ', ' ', ' ', ' ',
		' ', ' ', ' ', ' ',
		' ', ' ', ' ', ' '
	};	
	char m_box_2[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', '#', '#', ' ',
		' ', ' ', ' ', ' ',
		' ', ' ', ' ', ' ',
		' ', ' ', ' ', ' '
	};
	char m_box_3[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', '#', '#', ' ',
		' ', ' ', ' ', ' ',
		' ', ' ', ' ', ' ',
		' ', ' ', ' ', ' '
	};

	// Set the VAPOR piece and its orientations
	setPieceType(PIECE_VAPOR);
	setPiece(m_box_0);
	setBoxes(m_box_0, 0);
	setBoxes(m_box_1, 1);
	setBoxes(m_box_2, 2);
	setBoxes(m_box_3, 3);
}

Piece_VAPOR::~Piece_VAPOR()
{
}

bool Piece_VAPOR::canVaporize(char ch)
{
	// Check for filled spaces
	switch (ch)
	{
	case FILLED:
	case FOAM:
		return true;
	case PIECE:
	case WALL:
	case EMPTY:
	default:
		return false;
	}
}

void Piece_VAPOR::vaporize(Well& well)
{
	// Overload the vaporize() function in the Piece class
	// Check for the a 2x5 box with the VAPOR piece in the middle
	// Erase any pieces within the bounds of the box
	for (int i = 0; i < 2; i++)
	{
		for (int j = -2; j <= 2; j++)
		{
			if(accessY() + j < m_well->accessHeight() && accessY() + j >= 0)
			{
				if (canVaporize(*(m_well->accessWellItem(accessX() + i + 1, accessY() + j))))
				{
					*(m_well->accessWellItem(accessX() + i + 1, accessY() + j)) = EMPTY;
				}
			}
		}
	}
	m_well->displayContents(*m_screen);
}

////////////////////////////////////////
// Piece Foam derived class functions //
////////////////////////////////////////

Piece_FOAM::Piece_FOAM(Screen& screen, Well& well) : Piece(screen, well), m_screen(&screen), m_well(&well)
{
	char m_box_0[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', ' ', ' ', ' ',
		' ', '#', ' ', ' ',
		' ', ' ', ' ', ' ',
		' ', ' ', ' ', ' '
	};
	char m_box_1[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', ' ', ' ', ' ',
		' ', '#', ' ', ' ',
		' ', ' ', ' ', ' ',
		' ', ' ', ' ', ' '
	};
	char m_box_2[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', ' ', ' ', ' ',
		' ', '#', ' ', ' ',
		' ', ' ', ' ', ' ',
		' ', ' ', ' ', ' '
	};
	char m_box_3[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', ' ', ' ', ' ',
		' ', '#', ' ', ' ',
		' ', ' ', ' ', ' ',
		' ', ' ', ' ', ' '
	};

	// Set the FOAM piece and its orientations
	setPieceType(PIECE_FOAM);
	setPiece(m_box_0);
	setBoxes(m_box_0, 0);
	setBoxes(m_box_1, 1);
	setBoxes(m_box_2, 2);
	setBoxes(m_box_3, 3);
}

Piece_FOAM::~Piece_FOAM()
{
}

bool Piece_FOAM::canFill(char ch)
{
	// Check if a well element is empty
	switch (ch)
	{
	case EMPTY:
	case PIECE:
		return true;
	case FILLED:
	case FOAM:
	case WALL:
	default:
		return false;
	}
}

void Piece_FOAM::finishPiece()
{
	// Overload the finishPiece() function
	// Set the piece's characters to FOAM
	for (int i = 0; i < PIECE_HEIGHT; i++)
	{
		for (int j = 0; j < PIECE_WIDTH; j++)
		{
			if ((*(m_well->accessWellItem(i, j))) == PIECE)
			{
				*(m_well->accessWellItem(i, j)) = FOAM;
			}
		}
	}
}

void Piece_FOAM::fillEmptySpaces(Well & well, int i, int j)
{
	// Recursively fill a 5x5 box with FOAM piece in the middle
	if ((i == accessX() - 2 || i == accessX() + 4) || (j == accessY() - 2 || j == accessY() + 4))
		return;

	*(m_well->accessWellItem(i, j)) = FOAM;

	if (j > 0)
	{
		if (canFill(*(m_well->accessWellItem(i, j - 1))))
		{
			fillEmptySpaces(well, i, j - 1);
		}
	}
	if (canFill(*(m_well->accessWellItem(i - 1, j))))
	{
		fillEmptySpaces(well, i - 1, j);
	}
	if (canFill(*(m_well->accessWellItem(i, j + 1))))
	{
		fillEmptySpaces(well, i, j + 1);
	}
	if (canFill(*(m_well->accessWellItem(i + 1, j))))
	{
		fillEmptySpaces(well, i + 1, j);
	}

	m_well->displayContents(*m_screen);
}

/////////////////////////////////////////
// Piece Crazy derived class functions //
/////////////////////////////////////////

Piece_CRAZY::Piece_CRAZY(Screen& screen, Well& well) : Piece(screen, well)
{
	char m_box_0[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		'#', ' ', ' ', '#',
		' ', '#', '#', ' ',
		' ', ' ', ' ', ' ',
		' ', ' ', ' ', ' '
	};
	char m_box_1[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', ' ', ' ', '#',
		' ', ' ', '#', ' ',
		' ', ' ', '#', ' ',
		' ', ' ', ' ', '#'
	};
	char m_box_2[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		' ', ' ', ' ', ' ',
		' ', ' ', ' ', ' ',
		' ', '#', '#', ' ',
		'#', ' ', ' ', '#'
	};
	char m_box_3[PIECE_HEIGHT][PIECE_WIDTH] =
	{
		'#', ' ', ' ', ' ',
		' ', '#', ' ', ' ',
		' ', '#', ' ', ' ',
		'#', ' ', ' ', ' '
	};

	// Set the CRAZY piece and its orientations
	setPieceType(PIECE_CRAZY);
	setPiece(m_box_0);
	setBoxes(m_box_0, 0);
	setBoxes(m_box_1, 1);
	setBoxes(m_box_2, 2);
	setBoxes(m_box_3, 3);
}

Piece_CRAZY::~Piece_CRAZY()
{
}