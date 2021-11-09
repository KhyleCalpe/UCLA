#include "Game.h"

const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT = 25;

const int WELL_X = 0;
const int WELL_Y = 0;

const int PROMPT_Y = 20;
const int PROMPT_X = 0;

const int SCORE_X = 16;
const int SCORE_Y = 8;

const int ROWS_LEFT_X = 16;
const int ROWS_LEFT_Y = 9;

const int LEVEL_X = 16;
const int LEVEL_Y = 10;

const int NEXT_PIECE_TITLE_X = 16;
const int NEXT_PIECE_TITLE_Y = 3;

const int NEXT_PIECE_X = 16;
const int NEXT_PIECE_Y = 4;

Game::Game(int width, int height)
	: m_well(width, height), m_screen(SCREEN_WIDTH, SCREEN_HEIGHT), m_level(1), m_score(0), m_rowsLeft(5), m_width(width), m_height(height)
{}

void Game::play()
{
    m_well.display(m_screen, WELL_X, WELL_Y);
    displayStatus();  //  score, rows left, level
    displayPrompt("Press the Enter key to begin playing Chetyris!");
    waitForEnter();  // [in UserInterface.h]

    for(;;)
    {
        if ( ! playOneLevel())
            break;
		displayPrompt("Good Job! Press the Enter key to begin playing Chetyris!");
		waitForEnter();
		m_well.clearWell(m_screen);
		m_level++;
		m_rowsLeft = m_level * 5;
		displayStatus();
    }
    displayPrompt("Game Over!  Press the Enter key to exit!");
    waitForEnter();
}

void Game::displayPrompt(std::string s)     
{
    m_screen.gotoXY(PROMPT_X, PROMPT_Y);
    m_screen.printStringClearLine(s);   // overwrites previous text
    m_screen.refresh();
}

void Game::displayStatus()
{
	m_screen.gotoXY(SCORE_X, SCORE_Y);
	m_screen.printString("Score:           " + to_string(m_score));
	m_screen.gotoXY(ROWS_LEFT_X, ROWS_LEFT_Y);
	m_screen.printString("Rows left:       " + to_string(m_rowsLeft));
	m_screen.gotoXY(LEVEL_X, LEVEL_Y);
	m_screen.printString("Level:           " + to_string(m_level));
}

void Game::pushRandomPiece(PieceType pt)
{
	switch (pt)
	{
	case PIECE_I:
		m_pcs.push(new Piece_I(m_screen, m_well));
		break;
	case PIECE_L:
		m_pcs.push(new Piece_L(m_screen, m_well));
		break;
	case PIECE_J:
		m_pcs.push(new Piece_J(m_screen, m_well));
		break;
	case PIECE_T:
		m_pcs.push(new Piece_T(m_screen, m_well));
		break;
	case PIECE_O:
		m_pcs.push(new Piece_O(m_screen, m_well));
		break;
	case PIECE_S:
		m_pcs.push(new Piece_S(m_screen, m_well));
		break;
	case PIECE_Z:
		m_pcs.push(new Piece_Z(m_screen, m_well));
		break;
	case PIECE_VAPOR: 
		m_pcs.push(new Piece_VAPOR(m_screen, m_well));
		break;
	case PIECE_FOAM: 
		m_pcs.push(new Piece_FOAM(m_screen, m_well));
		break;
	case PIECE_CRAZY: 
		m_pcs.push(new Piece_CRAZY(m_screen, m_well));
		break;
	case NUM_PIECE_TYPES:
		pushRandomPiece(chooseRandomPieceType());
		break;
	}
}

void Game::clearNextPiece()
{
	// Clear the next piece spaces
	for (int i = NEXT_PIECE_X; i < NEXT_PIECE_X + PIECE_WIDTH; i++)
	{
		for (int j = NEXT_PIECE_Y; j < NEXT_PIECE_Y + PIECE_HEIGHT; j++)
		{
			m_screen.gotoXY(i, j);
			m_screen.printChar(EMPTY);
		}
	}
}

bool Game::playOneLevel()
{
	// Initialize the level by pushing two pieces into the queue
	pushRandomPiece(chooseRandomPieceType());
	pushRandomPiece(chooseRandomPieceType());

	// Look at piece at the front of the queue and display it in the well
	m_piece = m_pcs.front();
	m_piece->transcribe();
	m_piece->accessWell()->displayContents(m_screen);

	// Display the initial status of the game
	displayStatus();
	m_screen.gotoXY(NEXT_PIECE_TITLE_X, NEXT_PIECE_TITLE_Y);	// Print out the level status based on coordinates
	m_screen.printString("Next piece:");
	m_pcs.back()->displayXY(NEXT_PIECE_X, NEXT_PIECE_Y);

	// While the rows needed to be cleared is more than 0, keep playing the game
	while (m_rowsLeft > 0)
	{	
		// Set the skipping mechanism to false (for dropping down immediately)
		bool skip = false;
		bool stop = false;
		// Keep moving the piece while the timer permits
		while (m_timer.elapsed() < fmax(1000 - (100 * (m_level - 1)), 100))
		{
			// Move the piece according to user input
			if (getCharIfAny(m_input))
			{
				switch (m_input)
				{
				case ARROW_LEFT:
					if (m_piece->accessPieceType() == PIECE_CRAZY)
					{
						if (m_piece->moveRight())
						{
							m_piece->move();
						}
					}
					else
					{
						if (m_piece->moveLeft())
						{
							m_piece->move();
						}
					}
					break;
				case ARROW_RIGHT:
					if (m_piece->accessPieceType() == PIECE_CRAZY)
					{
						if (m_piece->moveLeft())
						{
							m_piece->move();
						}
					}
					else
					{
						if (m_piece->moveRight())
						{
							m_piece->move();
						}
					}
					break;
				case ARROW_DOWN:
					if (m_piece->moveDown())
					{
						m_piece->move();
						m_timer.start();
						break;
					}
					else
					{
						stop = true;
						break;
					}
					break;
				case ARROW_UP:
					if (m_piece->canRotate())
					{
						m_piece->rotate();
					}
					break;
				case ' ':
					m_piece->skip();
					skip = true;
					break;
				case 'Q':
				case 'q':
					return false;
				}
			}
			if (skip || stop)
				break;
		}

		// If the timer ends without user input, move down if possible
		if (m_piece->moveDown())
		{
			m_piece->move();
			m_timer.start();
		}
		else
		{
			// Set the piece's elements to '$'
			m_piece->finishPiece();

			// Implement special piece functions
			m_piece->vaporize(m_well);
			m_piece->fillEmptySpaces(m_well, m_piece->accessX() + 1, m_piece->accessY());

			// Check and clear filled rows
			m_well.clearRowFilled(m_score, m_rowsLeft);
			
			// Check if the game can display the next piece at its initial position
			if (!m_pcs.back()->canDisplayPiece())
				break;
			
			// Delete the current piece
			m_ptr = m_piece;
			delete m_ptr;

			// Display the next piece and restart the timer
			m_pcs.pop();
			pushRandomPiece(chooseRandomPieceType());
			m_piece = m_pcs.front();
			m_piece->transcribe();
			m_piece->accessWell()->displayContents(m_screen);
			m_timer.start();
		}

		// Display the next piece outside of the well
		displayStatus();
		clearNextPiece();
		m_screen.gotoXY(NEXT_PIECE_TITLE_X, NEXT_PIECE_TITLE_Y);	// Print out the level status based on coordinates
		m_screen.printString("Next piece:");
		m_pcs.back()->displayXY(NEXT_PIECE_X, NEXT_PIECE_Y);

		// If the user clears the required rows, finish the round
		if (m_rowsLeft == 0)
		{
			m_pcs.pop();
			m_pcs.pop();
			return true;
		}
	}
	
    return false;
}