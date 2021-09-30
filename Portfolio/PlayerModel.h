#pragma once

class PlayerModel
{
public:
	PlayerModel(Shader* shader);
	~PlayerModel();

	void Update();
	void PreRender();
	void PreRender_Reflection();
	void Render();
	void BoxRender();
	void EffectRender();

private:
	void Player();
	void Weapon();

public:
	ModelAnimator* GetPlayer() { return player; }

	void Position(float x, float y, float z);
	void Position(Vector3& vec);
	void Position(Vector3* vec);

	void Scale(float x, float y, float z);
	void Scale(Vector3& vec);
	void Scale(Vector3* vec);

	void Rotation(float x, float y, float z);
	void Rotation(Vector3& vec);
	void Rotation(Vector3* vec);

	void RotationDegree(float x, float y, float z);
	void RotationDegree(Vector3& vec);
	void RotationDegree(Vector3* vec);

	Vector3 Forward();
	Vector3 Backward();
	Vector3 Up();
	Vector3 Down();
	Vector3 Right();
	Vector3 Left();

	void World(Matrix& matrix);
	Matrix& World();

	Matrix BoneByIndex(UINT index) { return bones[index]; }
	Transform* WeaponTransform() { return weaponTransform; }

	void SetPosition(Vector3 pos) { this->position = pos; }

public:
	void Idle();
	void Run();
	void Jump();
	void Evade();
	void Attack1();
	void Attack2();
	void Attack3();

	void Skill1();
	void Skill2();
	void Skill3();
	void Skill4();
	void Skill5();

private:
	void CreateEffects();
	void UpdateEffects();
	void RenderEffects();

	void CreateColliders();

public:
	ColliderObject* WeaponBox() { return weaponBox; }
	ColliderObject* AttBox() { return attBox; }
	ColliderObject* HitBox() { return hitBox; }
	ColliderObject* FireBox() { return fireBox; }

	void SetActiveAttBox(bool active) { this->activeAttBox = active; }
	bool GetActiveAttBox() { return activeAttBox; }

	void SetActiveHitBox(bool active) { this->activeHitBox = active; }
	bool GetActiveHitBox() { return activeHitBox; }

	void SetActiveFireBox(bool active) { this->activeHitBox = active; }
	bool GetActiveFireBox() { return activeHitBox; }

	void AttColor(Color color) { attColor = color; }
	void HitColor(Color color) { hitColor = color; }
	void FireColor(Color color) { fireColor = color; }

	TrailSystem* SwordTrail() { return swordtrail; }

	void PlaySwordTrail(bool b) { bSwordTrail = b; }

	void SetSkillRange(Vector3* finalPos);

private:
	Shader* shader;

	ModelAnimator* player;
	ModelRender* weapon;

	Matrix bones[MAX_MODEL_TRANSFORMS];

	struct Footprint
	{
		ParticleSystem* Footprint;

		bool Use = false;
	};

	struct Fireball
	{
		ParticleSystem* Fireball;
		ParticleSystem* Explosion;

		Vector3 StartPosition = Vector3(0, 0, 0);
		Vector3 Position = Vector3(0, 0, 0);
		Vector3 Direct = Vector3(0, 0, 0);

		float Scale = 0.0f;
		float ExplsionTime = 0.0f;

		bool Use = false;
		bool Shoot = false;
		bool Boom = false;
	};

	struct Slash
	{
		ParticleSystem* Slash;

		bool Use = false;
	};

	struct Buff
	{
		ParticleSystem* Buff;

		bool Use = false;
	};

	struct SkillRange
	{
		Projector* SkillRange;

		bool Use = false;
	};

public:
	Footprint footprint;
	Fireball fireball;
	Slash slash;
	Buff buff;
	SkillRange skillRange;

private:
	TrailSystem* swordtrail = NULL;
	bool bSwordTrail = false;

	ColliderObject* weaponBox;
	ColliderObject* attBox;
	ColliderObject* hitBox;
	ColliderObject* fireBox;

	Color attColor = Color(0, 1, 0, 1);
	Color hitColor = Color(0, 1, 0, 1);
	Color fireColor = Color(0, 1, 0, 1);

	Transform* weaponTransform;

	bool activeAttBox = false;
	bool activeHitBox = true;
	bool activeFireBox = false;

	float testspeed = 1.0f;

	Vector3 position;
};