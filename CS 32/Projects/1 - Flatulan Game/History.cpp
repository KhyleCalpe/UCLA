// History implementation

#include <iostream>
#include "City.h"
#include "globals.h"
#include "History.h"
using namespace std;

History::History(int nRows, int nCols)			// Create a grid to account for the converted flatulans
	: m_nRows(nRows), m_nCols(nCols)
{
	for (int i = 0; i < nRows; i++)
		for (int j = 0; j < nCols; j++)
			m_grid[i][j] = 0;
}

bool History::record(int r, int c)
{
	if (r > m_nRows || c > m_nCols)							// Check if the r,c are out of bounds
		return false;
	m_grid[r - 1][c - 1]++;									// Fix the player position in history
	return true;
}

void History::display() const 

{
	// Position (row,col) in the city coordinate system is represented in
	// the array element grid[row-1][col-1]
	char grid[MAXROWS][MAXCOLS];
	int r, c;

	// Fill the grid with dots
	for (r = 0; r < m_nRows; r++)			
		for (c = 0; c < m_nCols; c++)
			grid[r][c] = '.';

	// Indicate each converted Flatulan's position
	for (int k = 0; k < m_nRows; k++)					// Check for each value in the new grid and print a symbol based on the amount
	{
		for (int l = 0; l < m_nCols; l++) 
		{
			if (m_grid[k][l] == 0)						
				grid[k][l] = '.';						// If there's no flatulans, the grid will print '.'
			else if (m_grid[k][l] >= 26)				
				grid[k][l] = 'Z';						// If there are equal to or more than 26 flatulans, print 'Z'
			else
				grid[k][l] = 64 + m_grid[k][l];			// Print out based on the ASCII chart
		}
	}

	// Draw the grid
	clearScreen();
	for (r = 0; r < m_nRows; r++)
	{
		for (c = 0; c < m_nCols; c++)
			cout << grid[r][c];
		cout << endl;
	}
	cout << endl;
}