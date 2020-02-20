#include "Camera2.h"
#include "Application.h"
#include "Mtx44.h"
#include "Utility.h"

Camera2::Camera2()
{
}

Camera2::~Camera2()
{
}

void Camera2::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	this->position = defaultPosition = pos;
	this->target = defaultTarget = target;
	view = (target - position).Normalized();
	right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	this->up = defaultUp = right.Cross(view).Normalized();
	height = pos.y;
}

void Camera2::Update(double dt)
{
	/*
	static const float CAMERA_SPEED = 50.f;
	if(Application::IsKeyPressed(VK_LEFT) || Application::IsKeyPressed('A'))
	{
		float yaw = (float)(-CAMERA_SPEED * dt);
		Mtx44 rotation;
		rotation.SetToRotation(yaw, 0, 1, 0);
		position = rotation * position;
		up = rotation * up;
	}
	if(Application::IsKeyPressed(VK_RIGHT) || Application::IsKeyPressed('D'))
	{
		float yaw = (float)(CAMERA_SPEED * dt);
		Mtx44 rotation;
		rotation.SetToRotation(yaw, 0, 1, 0);
		position = rotation * position;
		up = rotation * up;
	}
	if(Application::IsKeyPressed(VK_UP) || Application::IsKeyPressed('W'))
	{
		float pitch = (float)(-CAMERA_SPEED * dt);
		Vector3 view = (target - position).Normalized();
		Vector3 right = view.Cross(up);
		right.y = 0;
		right.Normalize();
		up = right.Cross(view).Normalized();
		Mtx44 rotation;
		rotation.SetToRotation(pitch, right.x, right.y, right.z);
		position = rotation * position;
	}
	if(Application::IsKeyPressed(VK_DOWN) || Application::IsKeyPressed('S'))
	{
		float pitch = (float)(CAMERA_SPEED * dt);
		Vector3 view = (target - position).Normalized();
		Vector3 right = view.Cross(up);
		right.y = 0;
		right.Normalize();
		up = right.Cross(view).Normalized();
		Mtx44 rotation;
		rotation.SetToRotation(pitch, right.x, right.y, right.z);
		position = rotation * position;
	}
	*/
	
	static const float CAMERA_SPEED = 40.f;
	view = (target - position).Normalized();
	right = view.Cross(up);
	Vector3 up2nd = Vector3(0, 1, 0);
	right.y = 0;
	float initalposy = position.y;

	if (Application::IsKeyPressed(VK_LEFT))
	{
		up = right.Cross(view).Normalized();
		float yaw = (float)(CAMERA_SPEED * dt * 2);
		Mtx44 rotation;
		rotation.SetToRotation(yaw, up2nd.x, up2nd.y, up2nd.z);
		view = rotation * view;
		target = position + view;
		Rotation2 += yaw;
	}
	if (Application::IsKeyPressed(VK_RIGHT))
	{
		up = right.Cross(view).Normalized();
		float yaw = (float)(-CAMERA_SPEED * dt * 2);
		Mtx44 rotation;
		rotation.SetToRotation(yaw, up2nd.x, up2nd.y, up2nd.z);
		view = rotation * view;
		target = position + view;
		Rotation2 += yaw;
	}
	if (Application::IsKeyPressed(VK_UP))
	{
		float pitch = (float)(CAMERA_SPEED * dt);
		Mtx44 rotation;
		rotation.SetToRotation(pitch, right.x, 0, right.z);
		view = rotation * view;
		up = rotation * up;
		target = position + view;
		Rotation += pitch;

	}
	if (Application::IsKeyPressed(VK_DOWN))
	{
		float pitch = (float)(-CAMERA_SPEED * dt);
		Mtx44 rotation;
		rotation.SetToRotation(pitch, right.x, 0, right.z);
		view = rotation * view;
		up = rotation * up;
		target = position + view;
		Rotation += pitch;
	}

	if (Application::IsKeyPressed('A'))
	{
		Vector3 newposition = position - right;
		bool nocollisionx = true;
		bool nocollisionz = true;
		bool nocollision = true;
		if (newposition.z <= -200 || newposition.z >= 200)
		{
			nocollisionz = false;
		}
		if (newposition.x <= -200 || newposition.x >= 200)
		{
			nocollisionx = false;
		}

		if (nocollisionx == true && nocollision == true)
		{
			position.x = newposition.x;
			target.x = position.x + view.x;
		}
		if (nocollisionz == true && nocollision == true)
		{
			position.z = newposition.z;
			target.x = position.x + view.x;

		}
		target = position + view;

	}
	if (Application::IsKeyPressed('D'))
	{
		Vector3 newposition = position + right;
		bool nocollisionx = true;
		bool nocollisionz = true;
		bool nocollision = true;
		if (newposition.z <= -200 || newposition.z >= 200)
		{
			nocollisionz = false;
		}
		if (newposition.x <= -200 || newposition.x >= 200)
		{
			nocollisionx = false;
		}

		if (nocollisionx == true && nocollision == true)
		{
			position.x = newposition.x;
			target.x = position.x + view.x;
		}
		if (nocollisionz == true && nocollision == true)
		{
			position.z = newposition.z;
			target.z = position.z + view.z;
		}

	}

	if (Application::IsKeyPressed('W'))
	{
		Vector3 newposition = position + view;
		bool nocollisionx = true;
		bool nocollisionz = true;
		bool nocollision = true;
		if (newposition.z <= -200 || newposition.z >= 200)
		{
			nocollisionz = false;
		}
		if (newposition.x <= -200 || newposition.x >= 200)
		{
			nocollisionx = false;
		}

		if (nocollisionx == true && nocollision == true)
		{
			position.x = newposition.x;
			target.x = position.x + view.x;
		}
		if (nocollisionz == true && nocollision == true)
		{
			position.z = newposition.z;
			target.z = position.z + view.z;

		}

		position.y = initalposy;
	}
	if (Application::IsKeyPressed('S'))
	{
		Vector3 newposition = position - view;
		bool nocollisionx = true;
		bool nocollisionz = true;
		bool nocollision = true;

		if (newposition.z <= -200 || newposition.z >= 200)
		{
			nocollisionz = false;
		}
		if (newposition.x <= -200 || newposition.x >= 200)
		{
			nocollisionx = false;
		}

		if (nocollisionx == true && nocollision == true)
		{
			position.x = newposition.x;
			target.x = position.x + view.x;
		}
		if (nocollisionz == true && nocollision == true)
		{
			position.z = newposition.z;
			target.z = position.z + view.z;

		}
		position.y = initalposy;
	}

	/*
	if(Application::IsKeyPressed('N'))
	{
		Vector3 direction = target - position;
		if(direction.Length() > 5)
		{
			Vector3 view = (target - position).Normalized();
			position += view * (float)(10.f * dt);
		}
	}
	if(Application::IsKeyPressed('M'))
	{
		Vector3 view = (target - position).Normalized();
		position -= view * (float)(10.f * dt);
	}
	*/

	if (Application::IsKeyPressed('R'))
	{
		Reset();
	}
}

void Camera2::Reset()
{
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;
}