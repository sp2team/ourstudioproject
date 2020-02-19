#include "Acceleration.h"

Acceleration::Acceleration()
{
	isAccelerating = false;
	velocity = 0;
	hasStopped = false;
	acceleration = 0;
	lastTime = 0;
}

Acceleration::~Acceleration()
{
}

void Acceleration::SetisAccelerating(bool change)
{
	isAccelerating = change;
}

float Acceleration::returnAcceleration(double dt, float currentTime)
{
	if (currentTime - lastTime > 0.5f && isAccelerating == true) {
		velocity++;
		lastTime = currentTime;
	}
	isAccelerating = false;
	hasStopped = false;
	if (isAccelerating == false && hasStopped == false && velocity != 0) {
		if (currentTime - lastTime > 0.5f) {
			velocity--;
			lastTime = currentTime;
		}
		hasStopped = true;
	}
	acceleration += velocity * dt;

	return acceleration;
}
