#include "StudentWorld.h"
#include "Actor.h"
#include <string>
#include <random>
#include <algorithm>
#include <sstream>
#include <iomanip>
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
	delete m_FrackMan;
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
	addActor(FrackMan_); 
	addActor(Boulder_, B);
	addActor(Oil_, m_BarrelsLeft);
	addActor(Gold_, G);
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::addActor(ObjectName objectName, int number)
{
	if (objectName == FrackMan_)
	{
		m_FrackMan = new FrackMan(this, 30, 60);
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
			do {
				x = randInt(0, 60); //60 because images location decided by bottomleft corner
				y = randInt(0, 60);
			} while (isDirtAround(x, y));
			Object* pooly = new WaterPool(this, x, y);
			m_Actors.push_back(pooly);
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
	setDisplayText();
	int G = getLevel() * 25 + 300; //new Goodie chance is 1/G
	if (randInt(1, G) == 1)//insertion
	{
		if (randInt(1, 5) == 1)
			addActor(Sonar_);
		else
			addActor(Water_);
	}
	clearDirt(m_FrackMan->getX(), m_FrackMan->getY(), true);
	m_FrackMan->doSomething(); //action
	revealAllNearbyObjects(m_FrackMan->getX(), m_FrackMan->getY(), 4);
	vector<Object*>::iterator it = m_Actors.begin();
	for (; it != m_Actors.end(); it++)
		(*it)->doSomething();
	it = m_Actors.begin(); //deletion
	while(it != m_Actors.end()) 
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
	delete m_FrackMan;
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
	return ((distanceBetween(m_FrackMan, a->getX(), a->getY())) <= radius ? m_FrackMan : nullptr);
}

bool StudentWorld::isDirtAt(int x, int y)
{
	if (m_Dirt[x][y] != nullptr)
		return true;
	else
		return false;
}

bool StudentWorld::isDirtAround(int x, int y)
{
	for (int i = x; i < x + 4 ; i++)
	{
		for (int j = y; j < y + 4; j++)
		{
			if (m_Dirt[i][j] != nullptr)
			{
				return true;
			}
		}
	}
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
void StudentWorld::setDisplayText()
{
	ostringstream oss;
	oss.setf(ios::fixed);
	oss.precision(0);
	oss.fill('0');
	oss << "Scr: " << setw(6) << getScore();
	oss.fill(' ');
	oss << "  Lvl: " << setw(2) << getLevel();
	oss << "  Lives: " << getLives();
	oss << "  Hlth: " << setw(3) << m_FrackMan->getHP() * 10 << "%";
	oss << "  Wtr: " << setw(2) << m_FrackMan->getWater();
	oss << "  Gld: " << setw(2) << m_FrackMan->getGold();
	oss << "  Sonar: " << setw(2) << m_FrackMan->getSonar();
	oss << "  Oil Left: " << setw(2) << m_BarrelsLeft;
	string s = oss.str();
	setGameStatText(s); // calls our provided GameWorld::setGameStatText
}

Object* StudentWorld::objectCollided(Object* actor, int x, int y)//returns object if overlapping or null if not
{
	if (actor == nullptr)
		return nullptr;
	if (m_FrackMan != actor && distanceBetween(m_FrackMan, x, y) < 4)
		return m_FrackMan;
	for (vector<Object*>::iterator it = m_Actors.begin(); it != m_Actors.end(); it++)
	{
		if (*it != actor && distanceBetween(*it, x, y) < 4)
				return *it;
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
