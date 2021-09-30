#pragma once

class Context
{
public:
	static Context* Get();
	static void Create();
	static void Delete();

private:
	Context();
	~Context();

public:
	void ResizeScreen();

	void Update();
	void Render();

	Matrix View();
	Matrix Projection();

	class Perspective* GetPerspective() { return perspective; }
	class Viewport* GetViewport() { return viewport; }
	class Camera* GetCamera() { return camera; }

	Color& Ambient() { return ambient; }
	Color& Specular() { return specular; }
	Vector3& Direction() { return direction; }
	Vector3& Position() { return position; }

	Plane* Culling() { return culling; }
	Plane& Culling(UINT index) { return culling[index]; }
	Plane& Clipping() { return clipping; }

	Color& MapRestrictArea() { return mapRestrictArea; }
	Vector2& MapSize() { return mapSize; }

	bool GetOnOff() { return onOff; }
	void SetOnOff(bool b) { this->onOff = b; }

private:
	static Context* instance;

private:
	class Perspective* perspective;
	class Viewport* viewport;
	class Camera* camera;

	Color ambient = Color(0, 0, 0, 1);
	Color specular = Color(1, 1, 1, 1);
	Vector3 direction = Vector3(-1, -1, 1);
	Vector3 position = Vector3(0, 0, 0);

	Plane culling[4];
	Plane clipping;

	Color mapRestrictArea = Color(0, 0, 0, 0);
	Vector2 mapSize = Vector2(0,0);

	bool onOff = false;
};