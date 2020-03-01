#pragma once
#include <string>

class Cars
{
private:
	float price;
	std::string name;

public:
	void setPrice(float p);
	float getPrice();


	Cars();
	~Cars();
};

