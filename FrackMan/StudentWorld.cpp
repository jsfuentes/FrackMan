#include "StudentWorld.h"
#include "Actor.h"
#include <string>
#include <random>
#include <algorithm>
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
	int B = min((static_cast<int>(getLevel()) / 2) + 2, 6); //getLevel() returns unsigned int(could produce error for a HUGE level
	int G = max(5 - (static_cast<int>(getLevel()) / 2), 2);
	int L = min(2 + static_cast<int>(getLevel()), 20);
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
	for (int i = 0; i < B; i++)
	{
		int x = randInt(0, 60);
		int y = randInt(20, 56);
		clearDirt(x, y);
		Boulder* boldy = new Boulder(this, x, y);
	}
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

///////////////////////////
//UTILITY FUNCTIONS
//////////////////////////
int randInt(int min, int max) //copy pasted from project 1 utilities
{
	if (max < min)
		swap(max, min);
	static random_device rd;
	static mt19937 generator(rd());
	uniform_int_distribution<> distro(min, max);
	return distro(generator);
}
// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
