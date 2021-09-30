#pragma once

class Player
{
public:
	Player(Shader* shader);
	~Player();

	void Update();
	void PreRender();
	void PreRender_Reflection();
	void Render();
	void UIRender();
	void EffectRender();

public:
	Vector3 GetPosition() { return position; }
	Vector3 Forward();

	float Damage() { return atk; }

	UINT CurrentSkill() { return currSkill; }

	void SetHeight(float height) { this->height = height; }

	bool IsBuff() { return buff; }

	void ReceivedDamage(float damage);// { hp -= damage; }

	void Full() { hp = mp = 10000.0f; }

private:
	void Move();
	void Attack();
	void Jump();
	void Evade();
	
	void Skill();
	void Skill1();
	void Skill2();
	void Skill3();
	void Skill4();
	void Skill5();

	void CountTime();
	void SetDirection();
	void CanReset();

private:
	void CreateUI();
	void UpdateUI();
	void RenderUI();

	void ColliderUpdate();

public:
	ColliderObject* HitBox();
	ColliderObject* AttBox();
	ColliderObject* FireBox();

	bool GetActiveAttBox();
	bool GetActiveHitBox();
	bool GetActiveFireBox();

private:
	class PlayerModel* player = NULL;
	Shader* shader;

	Vector3 position;
	Vector3 rotation;

private:
	enum Behavior
	{
		bIdle, bMove, bAttack, bJump, bEvade, bSkill, bReact, Count
	};
	Behavior behavior;

private:
	bool canMove = true;
	bool canAttack[4] = { true, true, false, false };// 0 : All Attack, 1 : Attack Combo1, 2 : Attack Combo2, 3 : Attack Combo3
	bool canJump = true;
	bool canEvade = true;
	bool canSkill = true;

private:
	float hp = 10000.0f;
	float mp = 10000.0f;
	float atk = 1000.0f;
	const float maxHp = 10000.0f;
	const float maxMp = 10000.0f;
	const float consumeMana[6] = { 500.0f, 100.0f, 500.0f, 300.0f, 500.0f, 100.0f };
	const float defaultAtk = 200.0f;
	int comboInterval = 10;

	const float coolTime[6] = { 2.0f, 1.0f, 5.0f, 1.0f, 10.0f, 1.0f };// 0 : 회피
	float rCoolTime[6] = { 0.0f };
	UINT currSkill = 0;

	bool buff = false;
	const float buffTime = 30.0f;
	float rBuffTime = 0.0f;

	float rInvincibleTime = 0.0f;
	const float invincibleTime = 1.0f;

	float height = 0.0f;
	float rotRatio = 0.05f;

private://UI
	Texture* texture[9];// 0 : hp, 1 : mp, 2 : 회피, 3 ~ 7 : 스킬, 8 : 버프
	Render2D* render2D[9];

private://Camera
	Vector3 cForward;
	Vector3 cBackward;
	Vector3 cLeft;
	Vector3 cRight;
	Vector3 cFLeft;
	Vector3 cFRight;
	Vector3 cBLeft;
	Vector3 cBRight;
};