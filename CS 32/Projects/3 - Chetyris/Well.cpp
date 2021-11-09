#include "Well.h"
#include "UserInterface.h"

Well::Well(int width, int height)
	: m_width(width), m_height(height), m_xWall(width + 2), m_yWall(height + 1)
{	
	// Check if the width and height are both within reason and within bounds
	if ((width < 8 || width > 12) || (height < 3 || height > 18))
	{
		exit(0);
	}

	// Assign a capacity to the dynamic array 
	m_well = new char*[m_yWall];
	for (int i = 0; i < m_yWall; i++)
	{
		m_well[i] = new char[m_xWall];
	}

	// Create an empty well
	for (int i = 0; i < m_yWall; i++)
	{
		for (int j = 0; j < m_xWall; j++)
		{
			if ((j == 0) || (j == m_xWall - 1) || (i == m_yWall - 1))
				m_well[i][j] = WALL;
			else
				m_well[i][j] = EMPTY;
		}
	}
}

Well::~Well()
{
	// Loop through the pointers and delete them and their values
	for (int i = 0; i < m_yWall; i++)
	{
		delete [] m_well[i];
	}
	delete [] m_well;
}

char* Well::accessWellItem(int x, int y)
{
	// Access a well element
	return &m_well[y][x];
}

int Well::accessHeight()
{
	return m_height;
}

void Well::display(Screen& screen, int x, int y)
{	
	// Display the well
	for (int i = 0; i < m_yWall; i++)
	{
		for (int j = 0; j < m_xWall; j++)
		{
			screen.gotoXY(j + x, i + y);
			screen.printChar(m_well[i][j]);
		}
	}
}

void Well::displayContents(Screen& screen)
{
	// Display the contents of the well
	for (int i = 0; i < m_yWall - 1; i++)
	{
		for (int j = 1; j < m_xWall - 1; j++)
		{
			screen.gotoXY(j, i);
			screen.printChar(m_well[i][j]);
		}
	}
}

void Well::clearContents(Screen & screen)
{
	// Clear the contents of the well
	for (int i = 0; i < m_yWall - 1; i++)
	{
		for (int j = 1; j < m_xWall - 1; j++)
		{
			if (m_well[i][j] == PIECE)
			{
				m_well[i][j] = EMPTY;
				screen.gotoXY(j, i);
				screen.printChar(EMPTY);
			}
		}
	}
}

void Well::clearWell(Screen & screen)
{
	// Clear the well
	for (int i = 0; i < m_yWall - 1; i++)
	{
		for (int j = 1; j < m_xWall - 1; j++)
		{
			if (m_well[i][j] != EMPTY)
			{
				m_well[i][j] = EMPTY;
				screen.gotoXY(j, i);
				screen.printChar(EMPTY);
			}
		}
	}
}

bool Well::isRowFilled(int y)
{
	// Check if row y in the well is filled
	for (int i = 1; i < m_xWall - 1; i++)
	{
		if (m_well[y][i] == EMPTY)
		{
			return false;
		}
	}
	return true;
}

void Well::clearRowFilled(int& score, int& rowsLeft)
{
	// Clear any filled rows and increment a counter of the cleared rows
	// Increase the game score based on the cleared rows
	// Decrease the amount of rows necessary to win
	int counter = 0;

	for (int i = m_height - 1; i > 0;)
	{
		if (isRowFilled(i))
		{
			dropDown(i);
			counter++;
		}
		else
			i--;
	}

	switch (counter)
	{
	case 1: 
		score += 100;
		break;
	case 2:
		score += 200;
		break;
	case 3:
		score += 400;
		break;
	case 4:
		score += 800;
		break;
	case 5:
		score += 1600;
		break;
	default:
		break;
	}

	if (counter > rowsLeft)
	{
		counter = rowsLeft;
	}

	rowsLeft -= counter;

}

void Well::dropDown(int y)
{
	// Drop the well's rows by one stopping at the y row
	for (int i = 1; i < m_xWall; i++)
	{
		for (int j = y; j > 0; j--)
		{
			m_well[j][i] = m_well[j - 1][i];
		}
	}
}