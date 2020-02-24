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

<<<<<<< Updated upstream
	TestTarget = (0, 0, 1);
	TestPosition = (0, 0, 0);

	View2 = (TestTarget - TestPosition).Normalized();
	right2 = View2.Cross(up);
	right2.y = 0;
	right2.Normalize();
	this->up = defaultUp = right2.Cross(View2).Normalized();
=======
	accel = (0, 0, 0);
>>>>>>> Stashed changes
}

void Camera2::Update(double dt)
{
	static const float CAMERA_SPEED = 50.f;
	if(Application::IsKeyPressed(VK_LEFT)|| Application::IsKeyPressed('A'))
	{
<<<<<<< Updated upstream
		float yaw = (float)(-CAMERA_SPEED * dt);
		Mtx44 rotation;
		rotation.SetToRotation(yaw, 0, 1, 0);
		//position = rotation * position;
		//up = rotation * up;
		View2 = rotation * View2;
		TestTarget = position + View2;
=======
		if (Application::IsKeyPressed('A'))
		{
			float yaw = (float)(-CAMERA_SPEED * dt);
			Mtx44 rotation;
			rotation.SetToRotation(yaw, 0, 1, 0);
			//position = rotation * position;
			//up = rotation * up;

			view = rotation * view;
			target = position + view;
		}
		if (Application::IsKeyPressed('D'))
		{
			float yaw = (float)(CAMERA_SPEED * dt);
			Mtx44 rotation;
			rotation.SetToRotation(yaw, 0, 1, 0);
			//position = rotation * position;
			//up = rotation * up;

			view = rotation * view;
			target = position + view;
		}
		position += accel * dt;
		target = position + view;
		position += decel * dt;
		target = position + view;
		if (Application::IsKeyPressed('W'))
		{
			/*float pitch = (float)(-CAMERA_SPEED * dt);
			Vector3 view = (target - position).Normalized();
			Vector3 right = view.Cross(up);
			right.y = 0;
			right.Normalize();
			up = right.Cross(view).Normalized();
			Mtx44 rotation;
			rotation.SetToRotation(pitch, right.x, right.y, right.z);
			position = rotation * position;*/
			
		}
		if (Application::IsKeyPressed('S'))
		{
			/*float pitch = (float)(CAMERA_SPEED * dt);
			Vector3 view = (target - position).Normalized();
			Vector3 right = view.Cross(up);
			right.y = 0;
			right.Normalize();
			up = right.Cross(view).Normalized();
			Mtx44 rotation;
			rotation.SetToRotation(pitch, right.x, right.y, right.z);
			position = rotation * position;*/
			
		}
		if (Application::IsKeyPressed('N'))
		{
			Vector3 direction = target - position;
			if (direction.Length() > 5)
			{
				Vector3 view = (target - position).Normalized();
				position += view * (float)(10.f * dt);
			}
		}
		if (Application::IsKeyPressed('M'))
		{
			Vector3 view = (target - position).Normalized();
			position -= view * (float)(10.f * dt);
		}
>>>>>>> Stashed changes
	}
	if(Application::IsKeyPressed(VK_RIGHT) || Application::IsKeyPressed('D'))
	{
		float yaw = (float)(CAMERA_SPEED * dt);
		Mtx44 rotation;
		rotation.SetToRotation(yaw, 0, 1, 0);
		//position = rotation * position;
		//up = rotation * up;
		View2 = rotation * View2;
		TestTarget = position + View2;
	}
	if(Application::IsKeyPressed(VK_UP))
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
	if(Application::IsKeyPressed(VK_DOWN))
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
	if(Application::IsKeyPressed('R'))
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