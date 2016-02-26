#include "StudentWorld.h"
#include "Actor.h"
#include <string>
#include <random>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <queue>
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
	int currentLevel = static_cast<int>(getLevel());
	int B = min((currentLevel / 2) + 2, 6); //getLevel() returns unsigned int(could produce error for a HUGE level
	int G = max(5 - (currentLevel) / 2, 2);
	m_BarrelsLeft = min(2 + currentLevel, 20);
	m_ProtestorsAddWaitTime = max(25, 200 - currentLevel);
	m_MaxProtestors = min(15.0, 2 + (currentLevel * 1.5)); //double truncated but its k
	m_currentProtestors = 0;
	m_currentTimeSinceAdd = -1;
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
		if (objectName == RegularProtester_)
		{
			Object* regPro = new RegularProtester(this, 60, 60);
			m_Actors.push_back(regPro);
		}
		else if (objectName == Squirt_)
		{ 
			x = m_FrackMan->getX();
			y = m_FrackMan->getY();
			for (int i = 0; i < 4; i++)
			m_FrackMan->coordinatesIfMoved(m_FrackMan->getDirection(), x, y);
			Object* squirty = new Squirt(this, x, y, m_FrackMan->getDirection());
			if (canActorMoveTo(squirty, x, y))
				m_Actors.push_back(squirty);
			else
				delete squirty;
		}
		else if (objectName == DroppedGold_)
		{
			Object* doppy = new GoldNugget(this, m_FrackMan->getX(), m_FrackMan->getY(), true);
			m_Actors.push_back(doppy);
		}
		else if (objectName == Sonar_)
		{
			Object* sony = new SonarKit(this, 0, 60);
			m_Actors.push_back(sony);
		}
		else if (objectName == Water_)
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
	setDisplayText(); //text updating
	int G = getLevel() * 25 + 300; //new Goodie chance is 1/G
	if (m_currentTimeSinceAdd == -1 || m_currentTimeSinceAdd++ >= m_ProtestorsAddWaitTime) //increments here after evaluated
		if (m_currentProtestors < m_MaxProtestors)
		{
			int probOfHardcore = min(90, (static_cast<int>(getLevel()) * 10) + 30);
			if (randInt(1, 100) <= probOfHardcore)
			{
				cout << "NOOOB";
			}
			else
				addActor(RegularProtester_);
			m_currentTimeSinceAdd = 0;
		}
	if (randInt(1, 100) == 1)//insertion, DONT FORGET TO CHANGE THIS TO G
	{
		if (randInt(1, 5) == 1)
			addActor(Sonar_);
		else
			addActor(Water_);
	}
	vector<Object*>::iterator it = m_Actors.begin();//action
	for (; it != m_Actors.end(); it++)
	{
		(*it)->doSomething();
		if (!m_FrackMan->isAlive())
			return GWSTATUS_PLAYER_DIED; 
	}
	clearDirt(m_FrackMan->getX(), m_FrackMan->getY(), true);
	m_FrackMan->doSomething(); 
	revealAllNearbyObjects(m_FrackMan->getX(), m_FrackMan->getY(), 4);
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
int StudentWorld::determineFirstMoveTo(Object* p1, int x, int y, bool toFrackMan)
{
	for (int i = 0; i < 64; i++)
		for (int j = 0; j < 64; j++)
			m_DistanceToExit[i][j] = -1; // fill maze solving with -1 to indicate unsolved
	struct Coord
	{
		Coord(int x, int y) {
			m_x = x; 
			m_y = y;
		};
		int m_x;
		int m_y;
	};
	queue<Coord> qC;
	Coord c(60, 60);
	m_DistanceToExit[60][60] = 0;
	qC.push(c);
	int stepsToExit = 0;
	int nextRound = 0;
	int currentRound = 1;
	bool newRound = true;
	while (!qC.empty())
	{
		if (--currentRound == 0)
		{
			currentRound = nextRound;
			nextRound = 0;
			stepsToExit++;
			newRound = true;
		}
		Coord currentC = qC.front();
		qC.pop();
		for (int dir = GraphObject::Direction::right; dir > 0; dir--)
		{
			int currentX = currentC.m_x;
			int currentY = currentC.m_y;
			m_FrackMan->coordinatesIfMoved(static_cast<GraphObject::Direction>(dir), currentX, currentY);
			if (canActorMoveTo(p1, currentX, currentY) && (m_DistanceToExit[currentX][currentY] == -1))
			{
				if (newRound)
					currentRound++;
				else
					nextRound++;
				qC.push(Coord(currentX, currentY));
				m_DistanceToExit[currentX][currentY] = stepsToExit;
			}
		}
		if (newRound)
			newRound = false;
	}
	/*for (int j = 63; j >= 0; j--)
	{
		for (int i = 0; i < 64; i++)	
			cout << m_DistanceToExit[i][j] << " ";
		cout << endl;
	}*/
	return determineDirTo(x, y);
}

int StudentWorld::determineDirTo(int x, int y, bool toFrackMan)
{
	int lowestDistance = m_DistanceToExit[x][y];
	int bestDir = 0;
	for (int dir = GraphObject::Direction::right; dir > 0; dir--)
	{
		int tempX = x;
		int tempY = y;
		m_FrackMan->coordinatesIfMoved(static_cast<GraphObject::Direction>(dir), tempX, tempY);
		if (m_DistanceToExit[tempX][tempY] != -1 && m_DistanceToExit[tempX][tempY] <= lowestDistance)
		{
			lowestDistance = m_DistanceToExit[tempX][tempY];
			bestDir = dir;
		}
	}
	return bestDir;
}

int StudentWorld::annoyAllNearbyAgents(Object* annoyer, int points, int radius, bool frackerIsImmune)
{
	int counter = 0;
	if (!frackerIsImmune && annoyer != m_FrackMan)
		if (distanceBetween(m_FrackMan, annoyer->getX(), annoyer->getY()) < radius)
			m_FrackMan->annoy(points);
	for (vector<Object*>::iterator it = m_Actors.begin(); it != m_Actors.end(); it++)
	{
		if (*it != annoyer && distanceBetween(*it, annoyer->getX(), annoyer->getY()) < radius)
			if ((*it)->annoy(points))
				counter++;
	}
	return counter;
}

bool StudentWorld::facingTowardFrackMan(Object* a)
{
	GraphObject::Direction dir = a->getDirection();
	switch (dir)
	{
	case GraphObject::Direction::down:
		for (int i = 0; canActorMoveTo(a, a->getX(), a->getY() - i); i++) //can Actor Move To checks if location within bounds
			if (distanceBetween(m_FrackMan, a->getX(), a->getY() - i) < 4)
				return true;
		return false;
		break;
	case GraphObject::Direction::up:
		for (int i = 0; canActorMoveTo(a, a->getX(), a->getY() + i); i++) //can Actor Move To checks if location within bounds
			if (distanceBetween(m_FrackMan, a->getX(), a->getY() + i) < 4)
				return true;
		return false;
		break;
	case GraphObject::Direction::right:
		for (int i = 0; canActorMoveTo(a, a->getX() + i, a->getY()); i++) //can Actor Move To checks if location within bounds
			if (distanceBetween(m_FrackMan, a->getX() + i, a->getY()) < 4)
				return true;
		return false;
		break;
	case GraphObject::Direction::left:
		for (int i = 0; canActorMoveTo(a, a->getX() - i, a->getY()); i++) //can Actor Move To checks if location within bounds
			if (distanceBetween(m_FrackMan, a->getX() - i, a->getY()) < 4)
				return true;
		return false;
		break;
	default:
		return false;
		break;
	}
}

void StudentWorld::give(ObjectName objectName, Object* reciever)
{
	if (objectName == Gold_)
		m_FrackMan->addGold();
	else if (objectName == Sonar_)
		m_FrackMan->addSonar();
	else if (objectName == Water_)
		m_FrackMan->addWater();
	else if (objectName == DroppedGold_ && reciever != nullptr)
		reciever->addGold();
}

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
		if (intersectingA != nullptr && !intersectingA->canActorsPassThroughMe())
			return false;
		if (!a->canDigThroughDirt() && isDirtAround(x, y))
			return false;
		return true;
	}
}

Object* StudentWorld::findNearbyFrackMan(Object* a, int radius) const
{
	return ((distanceBetween(m_FrackMan, a->getX(), a->getY())) <= radius ? m_FrackMan : nullptr);
}

Object* StudentWorld::findNearbyProtestor(Object* a, int radius)
{
	for (vector<Object*>::iterator it = m_Actors.begin(); it != m_Actors.end(); it++)
	{
		if ((*it)->canPickThingsUp() && distanceBetween(*it, a->getX(), a->getY()) < radius) //leaving protestors can pick things up
			return *it; //returns first protestor it finds, so only he gets the gold
	}
	return nullptr;
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
