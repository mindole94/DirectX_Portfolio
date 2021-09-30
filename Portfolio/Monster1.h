#pragma once

#define MONSTER1_NUM 1

class Monster1
{
public:
	Monster1(Shader* shader);
	~Monster1();

	void Update();
	void PreRender();
	void Render();

	Vector3 GetPosition(int index) { return monsterInfo[index].position; }
	
	void SetHeight(int index, float y) { monsterInfo[index].height = y; }
	
	auto GetColliderHit(int index) -> ColliderObject* { return monsterInfo[index].CollidersHit; }
	auto GetColliderAtt(int index) -> ColliderObject* { return monsterInfo[index].CollidersAtt; }

	void GetPlayerPosition(Vector3 position) { playerPosition = position; }

	void Damage(int index, int damageType, int damage, bool fireball);

	bool ActiveColliderHit(int index) { return monsterInfo[index].activeColliderHit; }
	bool ActiveColliderAtt(int index) { return monsterInfo[index].activeColliderAtt; }

	void SetActiveColliderHit(int index, bool be) { monsterInfo[index].activeColliderAtt = be; }
	
	void ShowCollider(bool show) { showCollider = show; }

private:
	void CreateMonster();
	void Patrol(int index);
	void Follow(int index);

private:
	void SetIdle(int index);
	void SetWalk(int index);
	void SetRun(int index);
	void SetAttack(int index);
	void SetWait(int index);
	void SetDownStart(int index);
	void SetDownLoop(int index);
	void SetDownEnd(int index);
	void SetDeath(int index);

private:
	void CreateColliders();
	void CollidersUpdate();
	void CollidersRender();

private:
	void Reset(int index);

private:
	Shader* shader;

	ModelAnimator* monster = NULL;

private:
	struct MontserInfo
	{
		Vector3 position;
		Vector3 patrolPosition;//패트롤 목표
		Vector3 scale;
		float height;

		bool search;//탐색 on/off
		
		int hp = 8000;
		int maxHp = 8000;
		int def = 100;

		ColliderObject* CollidersHit;//피격박스
		ColliderObject* CollidersAtt;//공격박스
		ParticleSystem* DeathParticle = NULL;

		bool activeColliderHit = true;
		bool activeColliderAtt = false;

		bool isAttack = false;
		bool isDown = false;

		bool idleTrigger = false;
		bool walkTrigger = false;
		bool runTrigger = false;
		bool atkTrigger = false;
		bool waitTrigger = false;
		bool downStartTrigger = false;
		bool downLoopTrigger = false;
		bool downEndTrigger = false;
		bool deathTrigger = false;

		float duration = 0.0f;
		float debuffTime = 0.0f;
		float invincibleTime = 0.0f;
	}monsterInfo[MONSTER1_NUM];

	Vector3 playerPosition = Vector3(0.0f, 0.0f, 0.0f);
	float distance = 0.0f;

	bool showCollider = true;
};
