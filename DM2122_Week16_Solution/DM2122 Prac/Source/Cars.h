#pragma once
#include <string>

class Cars
{
private:
	float price;
	std::string name;
	bool unlocked;

public:

	void setUnlocked(bool u);	
	bool getUnlocked();

	void setPrice(float p);
	float getPrice();


	Cars();
	~Cars();
};

