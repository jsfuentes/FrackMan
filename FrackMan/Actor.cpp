#include "Actor.h"
#include "StudentWorld.h"

Object::Object(StudentWorld* world, int imageID, int startX, int startY, Direction dir, double size, unsigned int depth)
	:GraphObject(imageID, startX, startY, dir, size, depth), m_world(world), m_isAlive(true) {}

Agent::Agent(StudentWorld* world, int startX, int startY, Direction startDir,
	int imageID, unsigned int hitPoints) : Object(world, imageID, startX, startY, startDir, 1.0, 0)
	, m_HP(hitPoints) {}

ActivatingObject::ActivatingObject(StudentWorld* world, int startX, int startY, int imageID, 
	int soundToPlay, bool activateOnPlayer, bool activateOnProtester, bool initallyActive) :
	Object(world, imageID, startX, startY, right, 1.0, 2), m_soundToPlay(soundToPlay), 
	m_activateOnPlayer(activateOnPlayer), m_activateOnProtestor(activateOnProtester) {}

void ActivatingObject::playSound() 
{ 
	getWorld()->playSound(m_soundToPlay);
}

void ActivatingObject::doSomething() 
{
	if (!isAlive())
		return;
	if (m_activateOnPlayer)
	{
		Object* MrFrack = getWorld()->findNearbyFrackMan(this, 4);
		if (!isVisible() && MrFrack != nullptr)
		{
			setVisible(true);
		}
		MrFrack = getWorld()->findNearbyFrackMan(this, 3);
		if (MrFrack != nullptr)
		{
			activate();
			playSound();
			kill();
		}
	}
}

GoldNugget::GoldNugget(StudentWorld* world, int startX, int startY, bool temporary):ActivatingObject(
	world, startX, startY, IID_GOLD, temporary? SOUND_PROTESTER_FOUND_GOLD: SOUND_GOT_GOODIE, 
	!temporary, temporary, temporary)
{
	if (temporary)
		setVisible(true);
}

void GoldNugget::activate() 
{
	if (activatesOnPlayer)
		getWorld()->increaseScore(10);
	else
		getWorld()->increaseScore(25);
}

OilBarrel::OilBarrel(StudentWorld* world, int startX, int startY) : ActivatingObject(world,
	startX, startY, IID_BARREL, SOUND_FOUND_OIL, true, false, false) {}

void OilBarrel::activate()
{
	getWorld()->increaseScore(1000);
}

Boulder::Boulder(StudentWorld* world, int startX, int startY): Object(world, IID_BOULDER, 
	startX, startY, down, 1.0, 1), delayCounter(0), isStable(true)
{
	setVisible(true);
}

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

Dirt::Dirt(StudentWorld* world, int startX, int startY) : Object(world, IID_DIRT, startX, startY, right, .25, 3)
{
	setVisible(true);
};

FrackMan::FrackMan(StudentWorld* world, int startX, int startY) : Agent(world, startX, startY, right, IID_PLAYER, 10)
{
	setVisible(true);
};

void FrackMan::doSomething()
{
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
		case KEY_PRESS_SPACE:
		case KEY_PRESS_TAB:
		default:
			break;
		}

	}

} // Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp