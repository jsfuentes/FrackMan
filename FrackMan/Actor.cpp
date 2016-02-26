#include "Actor.h"
#include "StudentWorld.h"
#include <algorithm>
using namespace std;

Object::Object(StudentWorld* world, int imageID, int startX, int startY, Direction dir, double size, unsigned int depth)
	:GraphObject(imageID, startX, startY, dir, size, depth), m_world(world), m_isAlive(true) 
{
	setVisible(true);
}



ActivatingObject::ActivatingObject(StudentWorld* world, int startX, int startY, int imageID, 
	int soundToPlay, bool actOnPlayer, bool actOnProtester, bool initallyActive, int ptsWhenAct) : 
	Object(world, imageID, startX, startY, right, 1.0, 2), m_soundToPlay(soundToPlay), 
	m_actOnPlayer(actOnPlayer), m_actOnProtestor(actOnProtester), m_ptsWhenAct(ptsWhenAct)
{
	if (initallyActive)
	{		
		m_temporary = true;
		setTicksToLive(max(100, 300-10* (static_cast<int>(getWorld()->getLevel()))));
	}
	else
		setVisible(false);
}

void ActivatingObject::playSound() 
{ 
	getWorld()->playSound(m_soundToPlay);
}

void ActivatingObject::doSomething() 
{
	if (!isAlive())
		return;
	if (m_actOnPlayer)
	{
		Object* MrFrack = getWorld()->findNearbyFrackMan(this, 3);
		if (MrFrack != nullptr)
		{
			activate();
			getWorld()->increaseScore(m_ptsWhenAct);
			playSound();
			kill();
		}
	}
	if (m_temporary) //m_temporary ensures m_ticksLeft intialized
	{
		if(m_ticksLeft-- == 0)
			kill();
	}
}

WaterPool::WaterPool(StudentWorld* world, int startX, int startY):ActivatingObject(world, startX, 
	startY, IID_WATER_POOL, SOUND_GOT_GOODIE, true, false, true, 100) {}

void WaterPool::activate()
{
	getWorld()->giveFrackMan(StudentWorld::ObjectName::Water_);
}

SonarKit::SonarKit(StudentWorld* world, int startX, int startY): ActivatingObject(world, startX, 
	startY, IID_SONAR, SOUND_GOT_GOODIE, true, false, true, 75) {}

void SonarKit::activate()
{
	getWorld()->giveFrackMan(StudentWorld::ObjectName::Sonar_);
}

GoldNugget::GoldNugget(StudentWorld* world, int startX, int startY, bool temporary):ActivatingObject(
	world, startX, startY, IID_GOLD, temporary? SOUND_PROTESTER_FOUND_GOLD: SOUND_GOT_GOODIE, 
	!temporary, temporary, temporary, temporary ? 25 : 10) {}

void GoldNugget::activate() 
{
	if (activatesOnPlayer())
		getWorld()->giveFrackMan(StudentWorld::ObjectName::Gold_);
	else
	{

	}
}

OilBarrel::OilBarrel(StudentWorld* world, int startX, int startY) : ActivatingObject(world,
	startX, startY, IID_BARREL, SOUND_FOUND_OIL, true, false, false, 1000) {}


Boulder::Boulder(StudentWorld* world, int startX, int startY): Object(world, IID_BOULDER, 
	startX, startY, down, 1.0, 1), delayCounter(0), isStable(true) {}

void Boulder::doSomething() 
{
	if (isStable)
	{
		isStable = isDirtBelow();
	}
	else
	{
		if (delayCounter++ > 29) //delayCounter <30 && not stable means falling
		{
			if(delayCounter == 31) //just started falling(because iterators delayCounter before this(in if)
				getWorld()->playSound(SOUND_FALLING_ROCK);
			if (getWorld()->canActorMoveTo(this, getX(), getY() - 1) && !isDirtBelow())
				moveTo(getX(), getY() - 1);
			else
				kill();
		}
	}
}

bool Boulder::isDirtBelow()
{
	bool isDirtBelow = false;
	for (int i = 0; i < 4; i++) //for all 4 spaces below boulder
		if (getWorld()->isDirtAt(getX() + i, getY() - 1))
			isDirtBelow = true;
	return isDirtBelow;
}

Dirt::Dirt(StudentWorld* world, int startX, int startY) : Object(world, IID_DIRT, startX, startY, 
	right, .25, 3) {}

Agent::Agent(StudentWorld* world, int startX, int startY, Direction startDir,
	int imageID, unsigned int hitPoints) : Object(world, imageID, startX, startY, startDir, 1.0, 0)
	, m_HP(hitPoints) {}

Protester::Protester(StudentWorld* world, int startX, int startY, int imageID, unsigned int hitPoints,
	unsigned int score):Agent(world, startX, startY, left, imageID, hitPoints), m_StepsForward(randInt(8, 60)), 
	m_Leaving(false), m_MaxWaitingTime(max(0, 3 - (static_cast<int>(getWorld()->getLevel())/4)))
{
	m_CurrentWaitTime = m_MaxWaitingTime; //so immediately acts
}

void Protester::doSomething() 
{
	if (m_CurrentWaitTime >= m_MaxWaitingTime)
	{
		Object* MrFrack = getWorld()->findNearbyFrackMan(this, 3);
		if (MrFrack != nullptr && getWorld()->facingTowardFrackMan(this))
		{
			cout << "KLJLKSDJFL";
		}
		else if (getWorld()->facingTowardFrackMan(this))
			cout << "GET HIM";
	}
	else
		m_CurrentWaitTime++;
}

RegularProtester::RegularProtester(StudentWorld* world, int startX, int startY): Protester(
	world, startX, startY, IID_PROTESTER, 5, 100) {}



FrackMan::FrackMan(StudentWorld* world, int startX, int startY) : Agent(world, startX, startY, right,
	IID_PLAYER, 10), m_Gold(0), m_Sonar(1), m_Squirts(5) {}

void FrackMan::doSomething()
{
	if (!isAlive())
		return;
	int ch;
	if (getWorld()->getKey(ch) == true)
	{
		switch (ch)
		{
		case KEY_PRESS_UP:
			if (getDirection() != up)
				setDirection(up);
			else if (getWorld()->canActorMoveTo(this, getX(), getY() + 1))
				moveTo(getX(), getY() + 1);
			else
				moveTo(getX(), getY());
			break;
		case KEY_PRESS_RIGHT:
			if (getDirection() != right)
				setDirection(right);
			else if (getWorld()->canActorMoveTo(this, getX() + 1, getY()))
				moveTo(getX() + 1, getY());
			else
				moveTo(getX(), getY());
			break;
		case KEY_PRESS_LEFT:
			if (getDirection() != left)
				setDirection(left);
			else if (getWorld()->canActorMoveTo(this, getX() - 1, getY()))
				moveTo(getX() - 1, getY());
			else
				moveTo(getX(), getY());
			break;
		case KEY_PRESS_DOWN:
			if (getDirection() != down)
				setDirection(down);
			else if (getWorld()->canActorMoveTo(this, getX(), getY() - 1))
				moveTo(getX(), getY() - 1);
			else
				moveTo(getX(), getY());
			break;
		case KEY_PRESS_ESCAPE:
			kill();
			getWorld()->decLives();
			break;
		case KEY_PRESS_SPACE:
			if (m_Squirts > 0)
			{
				m_Squirts--;
			}
			break;
		case 'Z':
		case 'z':
			if (m_Sonar > 0)
			{
				getWorld()->revealAllNearbyObjects(getX(), getY(), 12);
				m_Sonar--;
			}
			break;
		case KEY_PRESS_TAB:
			if (m_Gold > 0)
			{
				m_Gold--;
			}
			break;
		default:
			break;
		}

	}

} // Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp