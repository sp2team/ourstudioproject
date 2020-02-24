#include "Player.h"

void Player::setBalance(float b)
{
	balance = b;
}

float Player::getBalance()
{
	return balance;
}

Player::Player()
{
	balance = 200.0f;
}

Player::~Player()
{
}
