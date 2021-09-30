#pragma once

#define MAX_MONSTER 500

class MonsterModel
{
public:
	MonsterModel(Shader* shader, wstring monsterName, int num);
	~MonsterModel();

	void Update();
	void PreRender();
	void PreRender_Reflection();
	void Render();
	void BoxRender();

	ColliderObject* AttBox(int instance) { return attBox[instance]; }
	ColliderObject* HitBox(int instance) { return hitBox[instance]; }

	void SetActiveAttBox(int instance, bool active) { this->activeAttBox[instance] = active; }
	bool GetActiveAttBox(int instance) { return activeAttBox[instance]; }

	void SetActiveHitBox(int instance, bool active) { this->activeHitBox[instance] = active; }
	bool GetActiveHitBox(int instance) { return activeHitBox[instance]; }

	void Life(int instance, bool life) { this->life[instance] = life; }
	bool Life(int instance) { return life[instance]; }

private:
	void CreateMonster(wstring monsterName,int num);

	void CreateColliders(int num);

	bool CheckRestrictArea(Vector3 position);//위치제약 체크

public:
	ModelAnimator* GetMonster() { return monster; }

	void Position(int instance, float x, float y, float z);
	void Position(int instance, Vector3& vec);
	void Position(int instance, Vector3* vec);

	void Scale(int instance, float x, float y, float z);
	void Scale(int instance, Vector3& vec);
	void Scale(int instance, Vector3* vec);

	void Rotation(int instance, float x, float y, float z);
	void Rotation(int instance, Vector3& vec);
	void Rotation(int instance, Vector3* vec);

	void RotationDegree(int instance, float x, float y, float z);
	void RotationDegree(int instance, Vector3& vec);
	void RotationDegree(int instance, Vector3* vec);

	Vector3 Forward(int instance);
	Vector3 Backward(int instance);
	Vector3 Up(int instance);
	Vector3 Down(int instance);
	Vector3 Right(int instance);
	Vector3 Left(int instance);

	void World(int instance, Matrix& matrix);
	Matrix& World(int instance);

public:
	void Idle(int instance);
	void Run(int instance);
	void Attack(int instance);
	void Wait(int instance);
	void Death(int instance);

private:
	Shader* shader;
	
	ModelAnimator* monster;

	ColliderObject** attBox;
	ColliderObject** hitBox;

	bool* activeAttBox;
	bool* activeHitBox;

	int count = 0;
	wstring name = L"";
	bool* life;

	Color attColor = Color(0, 1, 0, 1);
	Color hitColor = Color(0, 1, 0, 1);
};