#ifndef CAMERA_H
#define CAMERA_H

#include "ExportHeader.h"
#include "glm\glm.hpp"
#include <QtGui\QKeyEvent>

class ENGINE_SHARED Camera
{

public:
	Camera();
	~Camera();

	void UpdateView();
	void SetPosition(const glm::vec3& pos);
	void SetPosition(const float& x, const float& y, const float& z);
	void SetTarget(const glm::vec3& tar);
	void SetTarget(const float& x, const float& y, const float& z);
	void KeyPressedEvent(QKeyEvent* e);

	void Move_Foward(const float& speed);
	void Move_Sideways(const float& speed);
	void Move_Vertical(const float& step);
	void Rotate_Horizontal(const float& speed);
	void Rotate_Vertical(const float& speed);

	const glm::vec3& GetPosition() const { return Position; }
	const glm::mat4& GetView() const { return View; }
	const glm::mat4& GetInverseView();
	const glm::vec3& GetDirection() const { return Direction; }

private:
	glm::vec3 Position;
	glm::vec3 Up;
	glm::vec3 Direction;
	glm::mat4 View;

};

#endif