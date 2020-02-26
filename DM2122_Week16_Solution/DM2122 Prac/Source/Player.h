#pragma once
#include "Cars.h"
#include <iostream>
#include <string>
#include <ostream>
#include <fstream>
#include <cstdint>

using namespace std;

class Player
{
private:
	float player1balance;
	float player2balance; 
	string file = "gamedata.txt";

public:
	Cars playerCar[8];

	void setPlayerOneBalance(float b);
	void setPlayerTwoBalance(float b);
	float getPlayerOneBalance();
	float getPlayerTwoBalance();
	bool alreadyHaveFile();
	void updateFile();
	


	Player();
	~Player();

};

