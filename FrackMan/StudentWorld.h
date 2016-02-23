#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>

class Dirt;
class FrackMan;
class Object;

int randInt(int min, int max); //at bottom of StudentWorld
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir);
	~StudentWorld();
	virtual int init();
	virtual int move();
	virtual void cleanUp();
	void addActor(std::string objectName, int number = 1);
	void clearDirt(int x, int y); //given the actors coordinates delete all dirt in 4x4

private:
	Object* objectCollided(Object* actor, int x, int y);
	bool withinMineShaft(int x, int y);
	bool closeToObjects(int x, int y);
	Dirt* m_Dirt[64][64];
	std::vector <Object*> m_Actors;
};

#endif // STUDENTWORLD_H_
