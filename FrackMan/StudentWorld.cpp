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
			if (withinMineShaft(row, column) || row >= 60)
				//the or is to fill the unused spaces with nullptrs
			{
				m_Dirt[column][row] = nullptr;
				continue;
			}
			m_Dirt[column][row] = new Dirt(this, column, row);
		}
	}
	addActor("FrackMan");
	addActor("Boulder", B);
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::addActor(string objectName, int number)
{
	if (objectName == "FrackMan")
	{
		m_Actors.push_back(new FrackMan(this, 30, 60));
		return;
	}
	for (int i = 0; i < number; i++)
	{
		int x, y;
		do {
			x = randInt(0, 60);
			y = randInt(20, 56);
		} while (withinMineShaft(y, x) && closeToObjects(x, y));
		if (objectName == "Boulder")
		{
			clearDirt(x, y);
			Object* boldy = new Boulder(this, x, y);
			m_Actors.push_back(boldy);
		}
	}
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
bool StudentWorld::closeToObjects(int x, int y)
{
	bool isTooClose = false;
	for (vector<Object*>::iterator it = m_Actors.begin(); it != m_Actors.end(); it++)
	{
		int dX = x - (*it)->getX();
		int dY = y - (*it)->getY();
		double distance = sqrt(dX^dX + dY^dY);
		isTooClose = ((distance <= 6) ? true : false);
	}
	return isTooClose;
}

bool StudentWorld::withinMineShaft(int row, int column)
{
	if (row >= 4 && row <= 59 && column >= 30 && column <= 33)
		return true;
	else
		return false;
}

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
