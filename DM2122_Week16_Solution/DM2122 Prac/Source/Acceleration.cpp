#include "Acceleration.h"

Acceleration::Acceleration()
{
	isAccelerating = false;
	isDecelerating = false;
	velocityA = 0;
	velocityD = 0;
	hasStoppedA = false;
	hasStoppedD = false;
	acceleration = 0;
	lastTimeA = 0;
	lastTimeD = 0;
}

Acceleration::~Acceleration()
{
}

void Acceleration::SetisAccelerating(bool change)
{
	isAccelerating = change;
}

void Acceleration::SetisDecelerating(bool change)
{
	isDecelerating = change;
}

float Acceleration::returnAcceleration(double dt, float currentTime, float max)
{
	if (currentTime - lastTimeA > 0.5f && isAccelerating == true && velocityA != max) {
		velocityA++;
		lastTimeA = currentTime;
	}
	hasStoppedA = false;
	if (isAccelerating == false && hasStoppedA == false && velocityA != 0) {
		if (currentTime - lastTimeA > 0.5f) {
			velocityA--;
			lastTimeA = currentTime;
		}
		hasStoppedA = true;
	}
	acceleration += velocityA * dt;

	//return acceleration;
	return velocityA;
}

float Acceleration::returnDeceleration(double dt, float currentTime, float max)
{
	if (currentTime - lastTimeD > 0.5f && isDecelerating == true && velocityD != -max) {
		velocityD--;
		lastTimeD = currentTime;
	}
	hasStoppedD = false;
	if (isDecelerating == false && hasStoppedD == false && velocityD != 0) {
		if (currentTime - lastTimeD > 0.5f) {
			velocityD++;
			lastTimeD = currentTime;
		}
		hasStoppedD = true;
	}
	acceleration += velocityD * dt;

	//return acceleration;
	return velocityD;
}
