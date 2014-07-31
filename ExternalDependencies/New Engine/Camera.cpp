#include "Camera.h"
#include <QtGui\QKeyEvent>
#include "glm\gtx\transform.hpp"

#ifndef PI
#define PI 3.1415926535f
#endif

using glm::vec3;
using glm::vec4;
using glm::mat4;

Camera::Camera()
{
	Position = glm::vec3(0,0,-8.0f);
	Direction = glm::vec3(0,0,1);
	Up = glm::vec3(0,1,0);
}

Camera::~Camera()
{

}

void Camera::SetPosition(const vec3& pos)
{
	Position = pos;
	UpdateView();
}

void Camera::SetPosition(const float& x, const float& y, const float& z)
{
	Position = vec3(x,y,z);
	UpdateView();
}

void Camera::SetTarget(const vec3& tar)
{
	Direction = glm::normalize(tar);
	UpdateView();
}

void Camera::SetTarget(const float& x, const float& y, const float& z)
{
	Direction = glm::normalize(vec3(x,y,z));
	UpdateView();
}

void Camera::KeyPressedEvent(QKeyEvent* e)
{
	switch (e->key())
	{
		case Qt::Key_W:
			Move_Foward(0.1f);
			break;
		case Qt::Key_A:
			Move_Sideways(0.1f);
			break;
		case Qt::Key_S:
			Move_Foward(-0.1f);
			break;
		case Qt::Key_D:
			Move_Sideways(-0.1f);
			break;
		case Qt::Key_F:
			Move_Vertical(-0.1f);
			break;
		case Qt::Key_R:
			Move_Vertical(0.1f);
			break;
		default:
			e->ignore();
			break;
	}
}

void Camera::Move_Foward(const float& speed)
{
	Position += (speed * Direction);
	UpdateView();
}

void Camera::Move_Sideways(const float& speed)
{
	vec3 cross = glm::normalize(glm::cross(Up, Direction));
	Position += (cross * speed);
	UpdateView();
}

void Camera::Move_Vertical(const float& step)
{
	Position += (step * Up);
	UpdateView();
}

void Camera::Rotate_Horizontal(const float& speed)
{
	mat4 rotation = glm::rotate(speed, Up);
	Direction = vec3(rotation * vec4(Direction,1));
	UpdateView();
}

void Camera::Rotate_Vertical(const float& speed)
{
	vec3 cross = glm::cross(Direction, Up);
	mat4 rotation = glm::rotate(speed, cross);
	Direction = vec3(rotation * vec4(Direction,0));
	UpdateView();

}

void Camera::UpdateView()
{
	View = glm::lookAt(Position, Position + Direction, Up);
}

const glm::mat4& Camera::GetInverseView()
{
	return glm::inverse(View); 
}