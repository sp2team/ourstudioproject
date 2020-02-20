#ifndef CAMERA_2_H
#define CAMERA_2_H

#include "Camera.h"

class Camera2 : public Camera
{
public:
	Vector3 defaultPosition;
	Vector3 defaultTarget;
	Vector3 defaultUp;

	float Rotation;
	float Rotation2;
	float height;
	bool mouseenabled;
	float sensitivity;

	Vector3 view;
	Vector3 right;

	Camera2();
	~Camera2();
	virtual void Init(const Vector3& pos, const Vector3& target, const Vector3& up);
	virtual void Update(double dt);
	virtual void Reset();
};

#endif