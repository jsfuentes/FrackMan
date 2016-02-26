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
	bool activated = false;
	if (m_actOnPlayer)
		if (getWorld()->findNearbyFrackMan(this, 3) != nullptr)
			activated = true;
	if (m_temporary) //m_temporary ensures m_ticksLeft intialized
	{
		if(m_ticksLeft-- == 0)
			kill();
		if (m_actOnProtestor && getWorld()->findNearbyProtestor(this, 3) != nullptr)
			activated = true;
	}
	if (activated)
	{
		activate();
		getWorld()->increaseScore(m_ptsWhenAct);
		playSound();
		kill();
	}
}

WaterPool::WaterPool(StudentWorld* world, int startX, int startY):ActivatingObject(world, startX, 
	startY, IID_WATER_POOL, SOUND_GOT_GOODIE, true, false, true, 100) {}

void WaterPool::activate()
{
	getWorld()->give(StudentWorld::ObjectName::Water_);
}

SonarKit::SonarKit(StudentWorld* world, int startX, int startY): ActivatingObject(world, startX, 
	startY, IID_SONAR, SOUND_GOT_GOODIE, true, false, true, 75) {}

void SonarKit::activate()
{
	getWorld()->give(StudentWorld::ObjectName::Sonar_);
}

GoldNugget::GoldNugget(StudentWorld* world, int startX, int startY, bool temporary):ActivatingObject(
	world, startX, startY, IID_GOLD, temporary? SOUND_PROTESTER_FOUND_GOLD: SOUND_GOT_GOODIE, 
	!temporary, temporary, temporary, temporary ? 25 : 10) 
{
	if (temporary)
		setTicksToLive(100);
}

void GoldNugget::activate() 
{
	if (activatesOnPlayer())
		getWorld()->give(StudentWorld::ObjectName::Gold_);
	else
		getWorld()->give(StudentWorld::ObjectName::DroppedGold_, getWorld()->findNearbyProtestor(this, 3));
}

OilBarrel::OilBarrel(StudentWorld* world, int startX, int startY) : ActivatingObject(world,
	startX, startY, IID_BARREL, SOUND_FOUND_OIL, true, false, false, 1000) {}


Squirt::Squirt(StudentWorld* world, int startX, int startY, Direction startDir) : Object(world,
	IID_WATER_SPURT, startX, startY, startDir, 1.0, 1), m_distanceToTravel(4) {}

void Squirt::doSomething() 
{
	if (getWorld()->annoyAllNearbyAgents(this, 2, 3) != 0)
		kill();
	else if (m_distanceToTravel-- <= 0)
		kill();
	else
	{
		int x = getX();
		int y = getY();
		coordinatesIfMoved(getDirection(), x, y);
		if (getWorld()->canActorMoveTo(this, x, y))
		{
			moveTo(x, y);
		}
		else
			kill();
	}

}
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
			getWorld()->increaseScore(getWorld()->annoyAllNearbyAgents(this, 100, 3)*500);
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

Protester::Protester(StudentWorld* world, int startX, int startY, int imageID, int hitPoints,
	unsigned int score):Agent(world, startX, startY, left, imageID, hitPoints), m_StepsForward(randInt(8, 60)), 
	m_Leaving(false), m_MaxWaitingTime(max(0, 3 - (static_cast<int>(getWorld()->getLevel())/4))), 
	m_TimeSinceShout(15), m_TimeSincePerp(200), m_firstMoveToLeave(true), m_ScoreIfShot(score)
{
	m_CurrentWaitTime = m_MaxWaitingTime; //so immediately acts
}

void Protester::doSomething() 
{
	if (!isAlive())
		return;
	if (m_CurrentWaitTime >= m_MaxWaitingTime)
	{
		m_CurrentWaitTime = 0;
		if (m_Leaving)
		{
			if (getX() == 60 && getY() == 60)
			{
				kill();
				return;
			}
			else if (m_firstMoveToLeave)
				setDirection(static_cast<Direction>(getWorld()->determineFirstMoveTo(this, getX(), getY())));
			else
				setDirection(static_cast<Direction>(getWorld()->determineDirTo(getX(), getY())));
		}
		else
		{
			Object* MrFrack = getWorld()->findNearbyFrackMan(this, 4);
			if (MrFrack != nullptr && getWorld()->facingTowardFrackMan(this) && m_TimeSinceShout++ >= 15) //increments timesince shout here
			{
				getWorld()->playSound(SOUND_PROTESTER_YELL);
				MrFrack->annoy(2);
				m_TimeSinceShout = 0;
				return;
			}
			else if (tryToBeHardCore()) {}
			else if (MrFrack == nullptr)
			{
				Direction baseDir = getDirection();
				Direction toFrack = none;
				for (int dir = right; dir > 0; dir--)
				{
					setDirection(static_cast<Direction>(dir));
					if (getWorld()->facingTowardFrackMan(this))
					{
						toFrack = static_cast<Direction>(dir);
						break; //will be facing correct Direction
					}
					setDirection(baseDir);
				}
				if (toFrack != none)
				{
					m_StepsForward = 0;
				}
				else if (--m_StepsForward <= 0)
				{
					int x, y;
					do
					{
						Direction randDir = static_cast<Direction>(randInt(1, 4));
						setDirection(randDir);
						x = getX();
						y = getY();
						coordinatesIfMoved(randDir, x, y);
					} while (!getWorld()->canActorMoveTo(this, x, y));
					m_StepsForward = randInt(8, 60);
				}
				else if (m_TimeSincePerp++ >= 200)
				{
					int x, y;
					Direction initialDir = getDirection();
					x = getX();
					y = getY();
					coordinatesIfMoved(initialDir, x, y); //if it changes x, only care about the y's and viceversa
					if (x != getX())
					{
						bool canGoUp = getWorld()->canActorMoveTo(this, getX(), getY() + 1);
						bool canGoDown = getWorld()->canActorMoveTo(this, getX(), getY() - 1);
						if (canGoUp && canGoDown)
							setDirection(randInt(1, 2) == 1 ? up : down);
						else if (canGoUp)
							setDirection(up);
						else if (canGoDown)
							setDirection(down);
					}
					else if (y != getY())
					{
						bool canGoLeft = getWorld()->canActorMoveTo(this, getX() - 1, getY());
						bool canGoRight = getWorld()->canActorMoveTo(this, getX() + 1, getY());
						if (canGoLeft && canGoRight)
							setDirection(randInt(1, 2) == 1 ? left : right);
						else if (canGoLeft)
							setDirection(left);
						else if (canGoRight)
							setDirection(right);
					}
					if (initialDir != getDirection())
					{
						m_TimeSincePerp = 0;
						m_StepsForward = randInt(8, 60);
					}
				}
			}
		}
		int x, y;
		x = getX();
		y = getY();
		coordinatesIfMoved(getDirection(), x, y);
		if (getWorld()->canActorMoveTo(this, x, y))
			moveTo(x, y);
		else
			m_StepsForward = 0;
	}
	else
		m_CurrentWaitTime++;
}

bool Protester::annoy(int amount)
{
	if (!m_Leaving)
	{
		Agent::annoy(amount);
		if (getHP() <= 0)
		{
			m_Leaving = true;
			getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
			m_CurrentWaitTime = 3; //not 0 but it will act on the next tick for sure
			if (amount != 100)//not boulder which does its own increase score
				getWorld()->increaseScore(m_ScoreIfShot);
		}
		else
		{
			getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
			m_CurrentWaitTime = m_MaxWaitingTime - max(50, 100 - (static_cast<int>(getWorld()->getLevel()) * 10));
		}
		return true;
	}
	else
		return false;
}

void Protester::addGold()
{
	m_Leaving = true; 
}

RegularProtester::RegularProtester(StudentWorld* world, int startX, int startY): Protester(
	world, startX, startY, IID_PROTESTER, 5, 100) {}

HardcoreProtester::HardcoreProtester(StudentWorld* world, int startX, int startY) : Protester(
	world, startX, startY, IID_HARD_CORE_PROTESTER, 20, 250) {}

FrackMan::FrackMan(StudentWorld* world, int startX, int startY) : Agent(world, startX, startY, right,
	IID_PLAYER, 10), m_Gold(0), m_Sonar(1), m_Squirts(5) {}

void FrackMan::doSomething()
{
	if (!isAlive() || getHP() <= 0)
	{
		kill();
		return;
	}
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
				getWorld()->addActor(StudentWorld::ObjectName::Squirt_);
				getWorld()->playSound(SOUND_PLAYER_SQUIRT);
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
				getWorld()->addActor(StudentWorld::ObjectName::DroppedGold_);
				m_Gold--;
			}
			break;
		default:
			break;
		}
	}
} 
bool FrackMan::annoy(int amount)
{
	if (amount != 100 && getHP() <= 0)
		getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
	return Agent::annoy(amount);
}
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

void Object::coordinatesIfMoved(Direction dir, int& x, int& y)
{
	switch (dir)
	{
	case up:
		y += 1;
		break;
	case down:
		y -= 1;
		break;
	case left:
		x -= 1;
		break;
	case right:
		 x +=1;
		break;
	default:
		break;
	}
}