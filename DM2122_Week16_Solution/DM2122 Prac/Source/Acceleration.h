#pragma once
class Acceleration
{
private:
	bool isAccelerating;
	bool isDecelerating;

	bool hasStoppedA;
	bool hasStoppedD;

	float velocityA;
	float velocityD;

	float acceleration;

	float lastTimeA;
	float lastTimeD;
public:
	Acceleration();
	~Acceleration();

	void SetisAccelerating(bool change);
	void SetisDecelerating(bool change);
	float returnAcceleration(double dt, float currentTime);
	float returnDeceleration(double dt, float currentTime);
};

