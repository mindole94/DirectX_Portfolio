#pragma once
#include "Camera.h"

class Freedom : public Camera
{
public:
	Freedom();
	~Freedom();

	void Update() override;

	void Speed(float move, float rotation = 2.0f);
	void At(Vector3 at) { this->at = at; }
	Vector3 GetPosition() { return position; }

	//void SetShake(float shakeTime, float totalTime, float shakeScale);
	void SetShake(float totalTime, float shakeScale);
	void CameraShake();

private:
	float move = 50.0f;
	float rotation = 2.0f;

	float radian = 50.0f;
	float theta = 0.0f;
	float phi = 0.0f;
	float height = 20.0f;

	Vector3 at = Vector3(30.0f, 0.5f, 30.0f);
	Vector3 position = Vector3(0.0f, 0.0f, 0.0f);

	bool cameraShake = false;
	float shakeTime = 0.0f;
	float totalShakeTime = 0.0f;
	float shakeScale = 0.0f;
	float shakeForce = 0.0f;
};