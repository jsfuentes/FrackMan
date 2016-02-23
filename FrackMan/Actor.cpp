#include "Actor.h"
#include "StudentWorld.h"

Object::Object(StudentWorld* world, int imageID, int startX, int startY, Direction dir, double size, unsigned int depth)
	:GraphObject(imageID, startX, startY, dir, size, depth), m_world(world) {};

Agent::Agent(StudentWorld* world, int startX, int startY, Direction startDir,
	int imageID, unsigned int hitPoints) : Object(world, imageID, startX, startY, startDir, 1.0, 0), m_HP(hitPoints)
{}

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
			{
				setDirection(up);
				break; //if not facing dir, wont move immediately 
			}
			if (getY() + 1 > 60)
			{
				moveTo(getX(), getY());
				break;

			}
			moveTo(getX(), getY() + 1);
			break;
		case KEY_PRESS_RIGHT:
			if (getDirection() != right)
			{
				setDirection(right);
				break;
			}
			if (getX() + 1 > 60)
			{
				moveTo(getX(), getY());
				break;
			}
			moveTo(getX() + 1, getY());
			break;
		case KEY_PRESS_LEFT:
			if (getDirection() != left)
			{
				setDirection(left);
				break;
			}
			if (getX() - 1 < 0)
			{
				moveTo(getX(), getY());
				break;
			}
			moveTo(getX() - 1, getY());
			break;
		case KEY_PRESS_DOWN:
			if (getDirection() != down)
			{
				setDirection(down);
				break;
			}
			if (getY() - 1 < 0)
			{
				moveTo(getX(), getY());
				break;
			}
			moveTo(getX(), getY() - 1);
			break;
		case KEY_PRESS_ESCAPE:
		case KEY_PRESS_SPACE:
		case KEY_PRESS_TAB:
		default:
			break;
		}

	}

} // Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp