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
	virtual void coordinatesIfMoved(Direction dir, int& x, int& y);
	virtual void addGold() {};
	virtual bool canDigThroughDirt() const { return false; };
	virtual bool canActorsPassThroughMe() const { return true; };
	virtual bool canPickThingsUp() const { return false; };
	virtual bool needsToBePickedUpToFinishLevel() const { return false; };
	virtual bool annoy(int amount) { return false; };
	StudentWorld* getWorld() { return m_world; };
	bool isAlive() { return m_isAlive; };
	void kill() { m_isAlive = false; };
private:
	StudentWorld* m_world;
	bool m_isAlive;
};

class Squirt : public Object
{
public:
	Squirt(StudentWorld* world, int startX, int startY, Direction startDir);
	virtual void doSomething();
private:
	int m_distanceToTravel;
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

class ActivatingObject : public Object
{
public:
	ActivatingObject(StudentWorld* world, int startX, int startY, int imageID,
		int soundToPlay, bool actOnPlayer, bool actOnProtester, bool initallyActive, 
		int ptsWhenAct = 0);
	virtual ~ActivatingObject() {};
	void playSound();
	virtual void doSomething(); //general actions to take(look for Frackman or Protestor)
	virtual void activate() {}; //specific actions to take
	void setTicksToLive(int ticksLeft) { m_ticksLeft = ticksLeft; };
	bool activatesOnPlayer() { return m_actOnPlayer; };
private:
	int m_ticksLeft;
	int m_soundToPlay;
	bool m_temporary;
	bool m_actOnPlayer;
	bool m_actOnProtestor;
	int m_ptsWhenAct;
};

class WaterPool : public ActivatingObject
{
public:
	WaterPool(StudentWorld* world, int startX, int startY);
	virtual void activate();
};

class SonarKit : public ActivatingObject
{
public:
	SonarKit(StudentWorld* world, int startX, int startY);
	virtual void activate();
};

class OilBarrel : public ActivatingObject
{
public:
	OilBarrel(StudentWorld* world, int startX, int startY);
	virtual bool needsToBePickedUpToFinishLevel() const { return true; };
private:
};

class GoldNugget : public ActivatingObject
{
public:
	GoldNugget(StudentWorld* world, int startX, int startY, bool temporary);
	virtual void activate();
};

class Agent : public Object
{
public:
	Agent(StudentWorld* world, int startX, int startY, Direction startDir,
		int imageID, unsigned int hitPoints);
	virtual ~Agent() {};
	virtual bool canPickThingsUp() const { return true; };
	virtual bool annoy(int amount) { m_HP -= amount;  return true; };
	int getHP() const { return m_HP; };
private:
	int m_HP;
};

class FrackMan :public Agent
{
public:
	FrackMan(StudentWorld* world, int startX, int startY); //default deconstructor is adequete but
	~FrackMan() {};
	void doSomething();
	bool canDigThroughDirt() const{ return true; }
	virtual bool annoy(int amount);
	virtual void addGold() { m_Gold++; };
	void addSonar() { m_Sonar++; };
	void addWater() { m_Squirts+=5; };
	int getGold() const { return m_Gold; };
	int getSonar() const { return m_Sonar; };
	int getWater() const { return m_Squirts; };
	
private:
	int m_Squirts;
	int m_Sonar;
	int m_Gold;
};

class Protester : public Agent
{
public:
	Protester(StudentWorld* world, int startX, int startY, int imageID,
		int hitPoints, unsigned int score);
	virtual ~Protester() {};
	virtual bool annoy(int amount);
	virtual void doSomething();
	virtual void addGold();
	virtual bool tryToBeHardCore() { return false; };
	virtual bool huntsFrackMan() const { return true; };
	virtual bool canPickThingsUp() const { return !m_Leaving; }
	// Set state to having given up protest
	void setMustLeaveOilField() { m_Leaving = true; };
	// Set number of ticks until next move
private:
	bool m_Leaving;
	bool m_firstMoveToLeave;
	int m_StepsForward;
	int m_MaxWaitingTime;
	int m_CurrentWaitTime;
	int m_TimeSincePerp;
	int m_TimeSinceShout;
	int m_ScoreIfShot; //also differentiater for hardcore and regular protester
};

class RegularProtester : public Protester
{
public:
	RegularProtester(StudentWorld* world, int startX, int startY);
};

class HardcoreProtester : public Protester
{
public:
	HardcoreProtester(StudentWorld* world, int startX, int startY);
	virtual bool TryToBeHardcore() { return true; };
	virtual void addGold() {};
};
#endif //Actor.h