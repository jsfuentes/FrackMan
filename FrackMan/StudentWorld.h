#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>

class Dirt;
class Object;
class FrackMan;

int randInt(int min, int max); //at bottom of StudentWorld
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	enum ObjectName { FrackMan_, Boulder_, Oil_, Gold_, Sonar_, Water_};
	StudentWorld(std::string assetDir);
	~StudentWorld();
	virtual int init();
	virtual int move();
	virtual void cleanUp();
	
	void revealAllNearbyObjects(int x, int y, int radius);
	Object* findNearbyFrackMan(Object* a, int radius) const; //In man is within radius, return him if not null 
	void addActor(ObjectName objectName, int number = 1); // adds object base on enumerator
	bool canActorMoveTo(Object* a, int x, int y); //checks boundaries and boulders
	bool isDirtAt(int x, int y);
	bool isDirtAround(int x, int y); //different from dirt at as it checks the 4x4 square using x,y as bottom left
	void clearDirt(int x, int y, bool sound); //given the actors coordinates delete all dirt in 4x4
	double distanceBetween(Object* a1, int x, int y) const;

private:
	void setDisplayText();
	Object* objectCollided(Object* actor, int x, int y);
	bool withinMineShaft(int x, int y);
	bool closeToObjects(int x, int y);
	Dirt* m_Dirt[64][64];
	std::vector <Object*> m_Actors;
	FrackMan* m_FrackMan;
	int m_BarrelsLeft;
};

#endif // STUDENTWORLD_H_
