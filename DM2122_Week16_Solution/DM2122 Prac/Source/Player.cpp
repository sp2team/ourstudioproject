#include "Player.h"

void Player::setPlayerOneBalance(float b)
{
	player1balance = b;
	updateFile();
}

void Player::setPlayerTwoBalance(float b)
{
	player2balance = b;
	updateFile();
}

int Player::getPlayerOneBalance()
{
	return player1balance;
}

int Player::getPlayerTwoBalance()
{
	return player2balance;
}

bool Player::alreadyHaveFile()
{
	ifstream runTime(file);

	if (runTime.fail())
	{
		return false;
	}
	else
	{
		return true;
	}
}

void Player::updateFile()
{
	ofstream update(file);

	update << "Data\n";
	update << to_string(player1balance) + "\n";
	update << to_string(player2balance) + "\n";
	for (int counter = 0; counter <= 7; counter++)
	{
		update << to_string(playerCarUnlocked[counter]) + "\n";
	}
}

void Player::readFile()
{
	string line;
	int DataLine = 0;
	int lineCounter = 0;

	ifstream data(file);
	while (getline(data, line))
	{
		if (line == "Data")
		{
			DataLine = lineCounter;
		}
		if (lineCounter == DataLine + 1)
		{
			player1balance = stoi(line);
		}
		if (lineCounter == DataLine + 2)
		{
			player2balance = stoi(line);
		}
		if (lineCounter == DataLine + 3)
		{
			playerCarUnlocked[0] = stoi(line);
		}
		if (lineCounter == DataLine + 4)
		{
			playerCarUnlocked[1] = stoi(line);
		}
		if (lineCounter == DataLine + 5)
		{
			playerCarUnlocked[2] = stoi(line);
		}
		if (lineCounter == DataLine + 6)
		{
			playerCarUnlocked[3] = stoi(line);
		}
		if (lineCounter == DataLine + 7)
		{
			playerCarUnlocked[4] = stoi(line);
		}
		if (lineCounter == DataLine + 8)
		{
			playerCarUnlocked[5] = stoi(line);
		}
		if (lineCounter == DataLine + 9)
		{
			playerCarUnlocked[6] = stoi(line);
		}
		if (lineCounter == DataLine + 10)
		{
			playerCarUnlocked[7] = stoi(line);
		}

		lineCounter++;
	}
}

Player::Player()
{
	player1balance = 10000;
	player2balance = 50000;
	readFile();
}

Player::~Player()
{
	updateFile();
}
