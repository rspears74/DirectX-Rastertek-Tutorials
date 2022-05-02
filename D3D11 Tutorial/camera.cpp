#include "camera.h"

Camera::Camera()
{
	_positionX = 0.0f;
	_positionY = 0.0f;
	_positionZ = 0.0f;
	_rotationX = 0.0f;
	_rotationY = 0.0f;
	_rotationZ = 0.0f;
}

Camera::Camera(const Camera& other) {}

Camera::~Camera() {}

void Camera::SetPosition(float x, float y, float z)
{
	_positionX = x;
	_positionY = y;
	_positionZ = z;

	return;
}

void Camera::SetRotation(float x, float y, float z)
{
	_rotationX = x;
	_rotationY = y;
	_rotationZ = z;

	return;
}

XMFLOAT3 Camera::GetPosition()
{
	return XMFLOAT3(_positionX, _positionY, _positionZ);
}

XMFLOAT3 Camera::GetRotation()
{
	return XMFLOAT3(_rotationX, _rotationY, _rotationZ;
}

void Camera::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	//setup the vector that points upward
	up.x = 0.0f;
	up.y = 1.0f;
	up.x = 0.0f;

	//load it into an XMVECTOR struct
	upVector = XMLoadFloat3(&up);

	//setup the position of the camera in the world
	position.x = _positionX;
	position.y = _positionY;
	position.z = _positionZ;

	//load it into an XMVECTOR struct
	positionVector = XMLoadFloat3(&position);

	//setup where the camera is looking by default
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	//load it into an XMVECTOR struct
	lookAtVector = XMLoadFloat3(&lookAt);

	//set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians
	pitch = _rotationX * 0.0174532925f;
	yaw = _rotationY * 0.0174532925f;
	roll = _rotationZ * 0.0174532925f;

	//create the rotation matrix from the yaw, pitch, and roll values
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	//transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin
	lookAtVector = XMVector2TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	//translate the rotated camera position to the location of the viewer
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	//finally create the view matrix from the three updated vectors
	_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	return;
}

void Camera::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = _viewMatrix;
	return;
}