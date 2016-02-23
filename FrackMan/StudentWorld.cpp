#include "StudentWorld.h"
#include "Actor.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

StudentWorld::StudentWorld(std::string assetDir)
	: GameWorld(assetDir)
{}

StudentWorld::~StudentWorld()
{
	for (int column = 0; column < 64; column++)
	{
		for (int row = 0; row < 60; row++)
		{
			delete m_Dirt[column][row];
			m_Dirt[column][row] = nullptr;
		}
	}
	delete m_FrackMan;
	m_FrackMan = nullptr;
}
int StudentWorld::init()
{
	for (int column = 0; column < 64; column++)
	{
		for (int row = 0; row < 64; row++)
		{
			if (row >= 4 && row <= 59 && column >= 30 && column <= 33 || row >= 60)
				//the or is to fill the unused spaces with nullptrs
				//the intial ands are for the mine shaft
			{
				m_Dirt[column][row] = nullptr;
				continue;
			}
			m_Dirt[column][row] = new Dirt(this, column, row);
		}
	}
	m_FrackMan = new FrackMan(this, 30, 60);
	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	// decLives();
	// return GWSTATUS_PLAYER_DIED;
	m_FrackMan->doSomething();
	int manX = m_FrackMan->getX();
	int manY = m_FrackMan->getY();
	for (int i = manX; i < manX + 4; i++)
	{
		for (int j = manY; j < manY + 4; j++)
		{
			delete m_Dirt[i][j];
			m_Dirt[i][j] = nullptr;
		}
	}
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	for (int column = 0; column < 64; column++)
	{
		for (int row = 0; row < 60; row++)
		{
			delete m_Dirt[column][row];
			m_Dirt[column][row] = nullptr;
		}
	}
	delete m_FrackMan;
	m_FrackMan = nullptr;
}
// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
