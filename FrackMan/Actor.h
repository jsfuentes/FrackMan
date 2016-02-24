#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Object : public GraphObject
{
public:
	Object(StudentWorld* world, int imageID, int startX, int startY, Direction dir, double size, unsigned int depth);
	virtual ~Object() {};
	virtual void doSomething() = 0;
	virtual bool canDigThroughDirt() const { return false; };
	virtual bool canActorsPassThroughMe() const { return true; };
	StudentWorld* getWorld() { return m_world; };
	bool isAlive() { return m_isAlive; };
	void kill() { m_isAlive = false; };
private:
	StudentWorld* m_world;
	bool m_isAlive;
};

class Boulder : public Object
{
public:
	Boulder(StudentWorld* world, int startX, int startY);
	void doSomething();
	bool canActorsPassThroughMe() const { return false; };
private:
	bool isDirtBelow();
	bool isStable;
	int delayCounter;
};

class Dirt : public Object
{
public:
	Dirt(StudentWorld* world, int startX, int startY); //default deconstructor works but 
	~Dirt() {};
	void doSomething() {};
private:
};

class Agent : public Object
{
public:
	Agent(StudentWorld* world, int startX, int startY, Direction startDir,
		int imageID, unsigned int hitPoints);
private:
	unsigned int m_HP;
};

class FrackMan :public Agent
{
public:
	FrackMan(StudentWorld* world, int startX, int startY); //default deconstructor is adequete but
	~FrackMan() {};
	void doSomething();
	bool canDigThroughDirt() const{ return true; };
private:

};
#endif //Actor.h