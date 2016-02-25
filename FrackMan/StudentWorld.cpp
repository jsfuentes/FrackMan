#include "StudentWorld.h"
#include "Actor.h"
#include <string>
#include <random>
#include <algorithm>
using namespace std;

///////////////////////
////MAIN FUNCTIONS
//////////////////////
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
	m_BarrelsLeft = min(2 + static_cast<int>(getLevel()), 20);
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
	addActor(FrackMan_); //FrackMan is in array first so doesSomething first
	addActor(Boulder_, B);
	addActor(Oil_, m_BarrelsLeft);
	addActor(Gold_, G);
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::addActor(ObjectName objectName, int number)
{
	if (objectName == FrackMan_)
	{
		m_Actors.push_back(new FrackMan(this, 30, 60));
		return;
	}
	for (int i = 0; i < number; i++)
	{
		int x, y;
		if (objectName == Sonar_)
		{
			Object* sony = new SonarKit(this, 0, 60);
			m_Actors.push_back(sony);
		}
		if (objectName == Water_)
		{
			//Object* pooly = new WaterPool(this, )
		}
		else if (objectName == Boulder_) //object randomly placed over top of map
		{
			do {
				x = randInt(0, 60);
				y = randInt(20, 56);
			} while (withinMineShaft(y, x) || withinMineShaft(y, x + 4) || closeToObjects(x, y));
		//calls within MineShaft twice, once for the bottom left edge and again for the bottom right edge
			clearDirt(x, y, false);
			Object* boldy = new Boulder(this, x, y);
			m_Actors.push_back(boldy);
		}
		else //randomly placed objects over entire map
		{
			do {
				x = randInt(0, 60);
				y = randInt(0, 56);
			} while (withinMineShaft(y, x) || withinMineShaft(y, x + 4) || closeToObjects(x, y));
			if (objectName == Oil_)
			{
				Object* oily = new OilBarrel(this, x, y);
				m_Actors.push_back(oily);
			}
			else if (objectName == Gold_)
			{
				Object* goldy = new GoldNugget(this, x, y, false);
				m_Actors.push_back(goldy);
			}
		}
	}
}



int StudentWorld::move()
{
	double G = getLevel() * 25 + 300; //new Goodie chance is 1/G
	if (randInt(0, 10) == 0)//insertion
	{
		if (randInt(0, 1) == 0)
			addActor(Sonar_);
		else
			addActor(Water_);
	}
	for (vector<Object*>::iterator it = m_Actors.begin(); it != m_Actors.end(); it++) //action
	{
		(*it)->doSomething();
		if ((*it)->canDigThroughDirt()) //only the man can dig through dirt
		{
			clearDirt((*it)->getX(), (*it)->getY(), true);
			revealAllNearbyObjects((*it)->getX(), (*it)->getY(), 4);
		}
	}
	vector<Object*>::iterator it = m_Actors.begin(); 
	while(it != m_Actors.end()) //deletion
	{
		if (!(*it)->isAlive())
		{
			if ((*it)->needsToBePickedUpToFinishLevel())
				m_BarrelsLeft--;
			delete (*it);
			it = m_Actors.erase(it);
		}
		else
			it++;
	}
	if(m_BarrelsLeft == 0)
	{ 
		playSound(SOUND_FINISHED_LEVEL);
		return GWSTATUS_FINISHED_LEVEL;
	}
	else
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
///////////////////////////////////
/////HELPER FUNCTIONS
//////////////////////////////////
void StudentWorld::revealAllNearbyObjects(int x, int y, int radius)
{
	for (vector<Object*>::iterator it = m_Actors.begin(); it != m_Actors.end(); it++)
	{
		if (!(*it)->isVisible())
		{
			if (distanceBetween(*it, x, y) <= radius)
			{
				(*it)->setVisible(true);
			}
		}
	}
}

void StudentWorld::clearDirt(int x, int y, bool sound)
{
	for (int i = x; i < x + 4; i++)
	{
		for (int j = y; j < y + 4; j++)
		{
			if (m_Dirt[i][j] != nullptr)
			{
				if (sound)
				{
					playSound(SOUND_DIG);
				}
				delete m_Dirt[i][j];
				m_Dirt[i][j] = nullptr;
			}
		}
	}
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

Object* StudentWorld::findNearbyFrackMan(Object* a, int radius) const
{
	Object* MrFrack = *m_Actors.begin(); //always Frackman since he was added first
	return ((distanceBetween(MrFrack, a->getX(), a->getY())) <= radius ? MrFrack : nullptr);
}

bool StudentWorld::isDirtAt(int x, int y)
{
	if (m_Dirt[x][y] != nullptr)
		return true;
	else
		return false;
}

double StudentWorld::distanceBetween(Object* a1, int x, int y) const
{
	int dX = x - a1->getX();
	int dY = y - a1->getY();
	return sqrt(pow(dX, 2) + pow(dY, 2));
}
///////////////////////////
//PRIVATE OR UTILITY FUNCTIONS
//////////////////////////
Object* StudentWorld::objectCollided(Object* actor, int x, int y)//returns object if overlapping or null if not
{
	if (actor == nullptr)
		return nullptr;
	for (vector<Object*>::iterator it = m_Actors.begin(); it != m_Actors.end(); it++)
	{
		if (*it != actor) //if they arent the same object
		{
			if(distanceBetween(*it, x, y) < 4)
				return *it;
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
