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

float Player::getPlayerOneBalance()
{
	return player1balance;
}

float Player::getPlayerTwoBalance()
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
		update << to_string(playerCar[counter].getUnlocked()) + "\n";
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
			if (stoi(line) == 1)
			{
				playerCar[0].setUnlocked(true);
			}
			else
			{
				playerCar[0].setUnlocked(false);
			}
		}
		if (lineCounter == DataLine + 4)
		{
			if (stoi(line) == 1)
			{
				playerCar[1].setUnlocked(true);
			}
			else
			{
				playerCar[1].setUnlocked(false);
			}
		}
		if (lineCounter == DataLine + 5)
		{
			playerCar[2].setUnlocked(stoi(line));
		}
		if (lineCounter == DataLine + 6)
		{
			playerCar[3].setUnlocked(stoi(line));
		}
		if (lineCounter == DataLine + 8)
		{
			playerCar[4].setUnlocked(stoi(line));
		}
		if (lineCounter == DataLine + 9)
		{
			playerCar[5].setUnlocked(stoi(line));
		}
		if (lineCounter == DataLine + 10)
		{
			playerCar[6].setUnlocked(stoi(line));
		}
		if (lineCounter == DataLine + 11)
		{
			playerCar[7].setUnlocked(stoi(line));
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
