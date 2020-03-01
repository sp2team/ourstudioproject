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
	int player1balance;
	int player2balance;
	string file = "gamedata.txt";

public:
	Cars playerCar[8];
	bool playerCarUnlocked[8];
	int player1selectedcar;
	int player2selectedcar;

	int player1Texture;
	int player2Texture;

	void setPlayerOneBalance(float b);
	void setPlayerTwoBalance(float b);
	int getPlayerOneBalance();
	int getPlayerTwoBalance();
	bool alreadyHaveFile();
	void updateFile();
	void readFile();
	


	Player();
	~Player();

};

