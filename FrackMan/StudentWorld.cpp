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
	addActor("FrackMan"); //FrackMan is in array first so doesSomething first
	addActor("Boulder", B);
	addActor("Oil", L);
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
		if (objectName == "Boulder")
		{
			do {
				x = randInt(0, 60);
				y = randInt(20, 56);
			} while (withinMineShaft(y, x) || withinMineShaft(y, x + 4) || closeToObjects(x, y));
		//calls within MineShaft twice, once for the bottom left edge and again for the bottom right edge
			clearDirt(x, y);
			Object* boldy = new Boulder(this, x, y);
			m_Actors.push_back(boldy);
		}
		else 
		{
			do {
				x = randInt(0, 60);
				y = randInt(0, 56);
			} while (withinMineShaft(y, x) || withinMineShaft(y, x + 4) || closeToObjects(x, y));
			if (objectName == "Oil")
			{
				Object* oily = new OilBarrel(this, x, y);
				m_Actors.push_back(oily);
			}
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
	for (vector<Object*>::iterator it = m_Actors.begin(); it != m_Actors.end(); it++) //action
	{
		(*it)->doSomething();
		if ((*it)->canDigThroughDirt())
		{
			clearDirt((*it)->getX(), (*it)->getY());
		}
	}
	vector<Object*>::iterator it = m_Actors.begin(); 
	while(it != m_Actors.end())//deletion
	{
		if (!(*it)->isAlive())
		{
			delete (*it);
			it = m_Actors.erase(it);
		}
		else
			it++;
	}
	return GWSTATUS_CONTINUE_GAME;
}

bool StudentWorld::canActorMoveTo(Object* a, int x, int y)
{
	if (x > 60 || x < 0 || y > 60 || y < 0) //within bounds
			return false;
	else
	{
		Object* intersectingA = objectCollided(a, x, y);
		if (intersectingA == nullptr)
			return true;
		else
			return intersectingA->canActorsPassThroughMe();
	}
}

bool StudentWorld::isDirtAt(int x, int y)
{
	if (m_Dirt[x][y] != nullptr)
		return true;
	else
		return false;
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
//UTILITY FUNCTIONS (private or other)
//////////////////////////
Object* StudentWorld::objectCollided(Object* actor, int x, int y)//returns object if overlapping or null if not
{
	if (actor == nullptr)
		return nullptr;
	for (vector<Object*>::iterator it = m_Actors.begin(); it != m_Actors.end(); it++)
	{
		if (*it != actor) //if they arent the same object
		{
			int dX = abs(x - (*it)->getX());
			int dY = abs(y - (*it)->getY());
			if (dX < 4 && dY < 4)
			{
				return *it;
			}
		}
	}
	return nullptr;
}

bool StudentWorld::closeToObjects(int x, int y)
{
	bool isTooClose = false;
	for (vector<Object*>::iterator it = m_Actors.begin(); it != m_Actors.end(); it++)
	{
		if (distanceBetween(*it, x, y) <= 6)
			isTooClose = true;
	}
	return isTooClose;
}

double StudentWorld::distanceBetween(Object* a1, int x, int y)
{
	int dX = x - a1->getX();
	int dY = y - a1->getY();
	return sqrt(pow(dX, 2) + pow(dY, 2));
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
