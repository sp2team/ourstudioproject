#pragma once
class Acceleration
{
private:
	bool isAccelerating;
	float velocity;
	bool hasStopped;
	float acceleration;
	float lastTime;
public:
	Acceleration();
	~Acceleration();

	void SetisAccelerating(bool change);
	float returnAcceleration(double dt, float currentTime);
};

