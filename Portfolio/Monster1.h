#pragma once

class Monster1
{
public:
	Monster1(Shader* shader);
	Monster1(Shader* shader, int num);
	~Monster1();

	void Update();
	void PreRender();
	void PreRender_Reflection();
	void Render();
	void AttackRender();

private:
	void Idle(int instance, float distance);
	void Battle(int instance, float distance);
	void Death(int instance);

	void CountTime(int instance);
	void AttackCheck(int instance);

	void AttackUpdate();

	bool CheckRestrictArea(Vector3 position);//위치제약 체크

public:
	void PlayerPosition(Vector3 pos) { playerPos = pos; }
	void PlayerForward(Vector3 forward) { playerForward = forward; }
	
	Vector3 GetPosition(UINT instance);
	void SetHeight(UINT instance, float height) { this->height[instance] = height; }

	ColliderObject* HitBox(int instance);
	ColliderObject* AttBox(int instance);
	
	void GetDamage(int instance, float damage);
	bool GetActiveAttBox(int instance);
	bool GetActiveHitBox(int instance);

	void SetKnockback(int instance, bool bKnockback);

	int Num() { return num; }

	void CheckHit(int instance, bool result);

private:
	class MonsterModel* monster;

	Shader* shader;

private:
	int num = 5;

	struct Monster1Info
	{
		float Hp = 5000.0f;
		float MaxHp = 5000.0f;
		float IdleTime = 0.0f;
		bool Search = false;

		Vector3 PatrolPos = Vector3(0.0f, 0.0f, 0.0f);
		float InvincibleTime = 0.0f;

		float DeathTime = 0.0f;
		int WaitCount = 0;
		bool Attacked = false;
	};

	struct AttackLight
	{
		ParticleSystem* Light;
		ParticleSystem* Explosion;

		bool bActived = false;
		Vector3 StartPosition = Vector3(0, 0, 0);
		Vector3 AttackPosition = Vector3(0.0f, 0.0f, 0.0f);
		bool CheckHit = false;
		float time = 0.0f;
	};

	enum Behavior
	{
		bIdle, bPatrol, bChase, bAttack, bWait, bKnockback, bDeath, Count
	};
	Behavior* behavior;
	
	Vector3 playerPos = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 playerForward = Vector3(0.0f, 0.0f, 0.0f);

	const float maxIdleTime = 3.0f;
	const float patrolDist = 50.0f;
	const float maxInvincibleTime = 1.0f;
	const float rotRatio = 0.05f;
	const float speed = 5.0f;

	float* height;

	Monster1Info* info;
	AttackLight* attackLight;
};
