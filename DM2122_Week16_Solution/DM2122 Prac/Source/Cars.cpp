#include "Cars.h"

void Cars::setUnlocked(bool u)
{
	unlocked = u;
}

bool Cars::getUnlocked()
{
	return unlocked;
}

void Cars::setPrice(float p)
{
	price = p;
	unlocked = false;
}

float Cars::getPrice()
{
	return price;
}

Cars::Cars()
{
}

Cars::~Cars()
{
}
