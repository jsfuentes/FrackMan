#include "Actor.h"
#include "StudentWorld.h"

Object::Object(StudentWorld* world, int imageID, int startX, int startY, Direction dir, double size, unsigned int depth)
	:GraphObject(imageID, startX, startY, dir, size, depth), m_world(world) {};

Agent::Agent(StudentWorld* world, int startX, int startY, Direction startDir,
	int imageID, unsigned int hitPoints) : Object(world, imageID, startX, startY, startDir, 1.0, 0), m_HP(hitPoints)
{}

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
			if(getWorld()->canActorMoveTo(this, getX(), getY() -1) && !isDirtBelow())
				moveTo(getX(), getY() - 1);
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