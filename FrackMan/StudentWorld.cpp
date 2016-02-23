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
	while (!m_Actors.empty())
	{
		delete m_Actors.back();
		m_Actors.pop_back();
	}
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
	addActor(new FrackMan(this, 30, 60));
	addActor(new Boulder(this, 10, 10));
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::addActor(Object* a)
{
	if (a != nullptr)
		m_Actors.push_back(a);
}

void StudentWorld::clearDirt(int x, int y)
{
	for (int i = x; i < x + 4; i++)
	{
		for (int j = y; j < y + 4; j++)
		{
			delete m_Dirt[i][j];
			m_Dirt[i][j] = nullptr;
		}
	}
}

int StudentWorld::move()
{
	// decLives();
	// return GWSTATUS_PLAYER_DIED;
	for (vector<Object*>::iterator it = m_Actors.begin(); it != m_Actors.end(); it++)
	{
		(*it)->doSomething();
		if ((*it)->canDigThroughDirt())
		{
			clearDirt((*it)->getX(), (*it)->getY());
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
	while (!m_Actors.empty())
	{
		delete m_Actors.back();
		m_Actors.pop_back();
	}
	
}
// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
