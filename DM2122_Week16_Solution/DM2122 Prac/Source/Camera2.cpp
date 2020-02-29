#include "Camera2.h"
#include "Application.h"
#include "Mtx44.h"
#include "Utility.h"
#include "GLFW/glfw3.h"

extern GLFWwindow* m_window;

Camera2::Camera2()
{
}

Camera2::~Camera2()
{
}

void Camera2::Init(const Vector3& pos, const Vector3& target, const Vector3& up, int camera)
{
	this->camera = camera;
	this->position = defaultPosition = pos;
	this->target = defaultTarget = target;
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	this->up = defaultUp = right.Cross(view).Normalized();
}

void Camera2::Update(double dt)
{
	static const float CAMERA_SPEED = 50.f;

	if (camera == 0)
	{
		/*s

		/*if (Application::IsKeyPressed('A'))
		{
			float yaw = (float)(-CAMERA_SPEED * dt);
			Mtx44 rotation;
			rotation.SetToRotation(yaw, 0, 1, 0);
			position = rotation * position;
			up = rotation * up;
		}
		if (Application::IsKeyPressed('D'))
		{
			float yaw = (float)(CAMERA_SPEED * dt);
			Mtx44 rotation;
			rotation.SetToRotation(yaw, 0, 1, 0);
			position = rotation * position;
			up = rotation * up;
		}
		if (Application::IsKeyPressed('W'))
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
		if (Application::IsKeyPressed('S'))
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
		}*/
	}
	/*else if (camera == 1)
	{
		if (Application::IsKeyPressed(VK_LEFT))
		{
			float yaw = (float)(-CAMERA_SPEED * dt);
			Mtx44 rotation;
			rotation.SetToRotation(yaw, 0, 1, 0);
			position = rotation * position;
			up = rotation * up;
		}
		if (Application::IsKeyPressed(VK_RIGHT))
		{
			float yaw = (float)(CAMERA_SPEED * dt);
			Mtx44 rotation;
			rotation.SetToRotation(yaw, 0, 1, 0);
			position = rotation * position;
			up = rotation * up;
		}
		if (Application::IsKeyPressed(VK_UP))
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
		if (Application::IsKeyPressed(VK_DOWN))
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
	}*/
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

void Camera2::MouseControl()
{
	int width, height;
	glfwGetWindowSize(m_window, &width, &height);

	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up).Normalized();
	POINT mouseposition;
	GetCursorPos(&mouseposition);
	ShowCursor(false);

	int positionX = mouseposition.x;
	int positionY = mouseposition.y;

	if (MouseState)
	{
		lastX = positionX;
		lastY = positionY;
		MouseState = false;
	}

	float offsetx = positionX - lastX;
	float offsety = positionY - lastY;
	lastX = positionX;
	lastY = positionY;

	float sens = 0.2f; // sensitivity of mouse
	offsetx *= sens;
	offsety *= sens;

	yaw += offsetx; // for left and right 
	pitch -= offsety; // for up and down

	if (pitch > 89.0f) // to set the limit to how far the player can look around
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	Vector3 front;
	front.x = cos(Math::DegreeToRadian(yaw)) * cos(Math::DegreeToRadian(pitch));
	front.y = sin(Math::DegreeToRadian(pitch));
	front.z = sin(Math::DegreeToRadian(yaw)) * cos(Math::DegreeToRadian(pitch));

	view = front.Normalized();
	right.y = 0;
	right.Normalize();
	up = right.Cross(view).Normalized();
	right = view.Cross(up).Normalized();
	target = position + view;

	//SetCursorPos(width / 2, height / 2);
}
