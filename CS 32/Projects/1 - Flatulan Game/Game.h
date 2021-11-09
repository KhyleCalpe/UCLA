	// Game interface

#ifndef GAME_INCLUDED
#define GAME_INCLUDED

class City;

class Game
{
public:
	// Constructor/destructor
	Game(int rows, int cols, int nFlatulans);
	~Game();

	// Mutators
	void play();

private:
	City* m_city;
};

#endif	// GAME_INCLUDED
