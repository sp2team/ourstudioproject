#pragma once
#include "Cars.h"

class Player
{
private:
	float balance;
	

public:
	Cars playerCar[4];

	void setBalance(float b);
	float getBalance();



	Player();
	~Player();

};

