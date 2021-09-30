#pragma once
//#include "Systems/IExecute.h"

class Player
{
public:
	Player(Shader* shader);
	~Player();

	void Update();
	void PreRender();
	void Render();

	Vector3 GetPosition() { return position; }
	Vector3 GetFireballPosition() { return fireballPosition; }

	void SetHeight(float y) { height = y; }
	void SetHitSucces(bool hit) { damageMoment = hit; }

	auto GetColliderHit() -> ColliderObject* { return collidersHit; }
	auto GetColliderAtt() -> ColliderObject* { return collidersAtt; }
	auto GetColliderFireball() -> ColliderObject* { return colliderFireball; }

	int CurrentSkill();
	int Damage();

	void SetHp(int changeValue) { hp += changeValue; }

	bool CameraShake() { return cameraShake; }
	bool CameraRelease() { return cameraRelease; }

	bool ActiveColliderAtt() { return activeColliderAtt; }
	bool ActiveFireball() { return shootFireball; }
	bool ActiveEvade() { return isEvade; }
	
	float GetFireballScale() { return fireballScale; }

	void ShowCollider(bool show) { showCollider = show; }
	//Default Motion
private:
	void CreatePlayer();
	void Move();
	void Attack();
	void Evade();
	void Jump();

	//Skill Motion
	void SuperJump();//스킬0
	void DashAttack();//스킬1
	void Yell();//스킬2
	void UpperCut();//스킬3
	void FireBall();//스킬4

	//Default Animation
private:
	void SetIdle();
	void SetRun();
	void SetJump();
	void SetEvade();
	void SetAttack1();
	void SetAttack2();
	void SetAttack3();

	//Skill Animation
	void SetSuperJump();
	void SetDash();
	void SetSlash();
	void SetYell();
	void SetFireball();

private:
	void CreateImage();
	void ImageUpdate();
	void ImageRender();

	void CreateColliders();
	void CollidersUpdate();
	void CollidersRender();

private:
	void CreateWeapon();
	void AttachWeapon();

private:
	void CreateParticle();
	void ParticleUpdate();
	void ParticleRender();

private:
	void SettingVector();
	void CoolTime();
	void Reset();

private:
	Shader* shader;

	ModelAnimator* sapling = NULL;
	ModelRender* weapon = NULL;
	Transform* weaponTransform;

	ColliderObject* collidersHit;
	ColliderObject* collidersAtt;

	Matrix worldBonesPosition[MAX_MODEL_TRANSFORMS];

	bool showCollider = true;

private:
	int maxHp = 10000;
	int maxMp = 10000;
	int hp = 10000;
	int mp = 10000;
	int atk = 1000;
	float runSpeed = 20.0f;
	float jumpAccel = 0.0f;
	float height = 0.0f;
	float jumpSpeed = 20.0f;
	float autoHeal = 0.0f;

	//Current Status
	int motion = 0;
	bool isRun = false;
	bool prevRun = false;
	bool isJump = false;
	bool isUp = false;
	bool isFall = true;
	bool bAttack = false;
	bool isAttack[3] = { false, false, false };
	bool isEvade = false;
	bool bSkill = false;
	bool isSkill[5] = { false, false, false, false, false };

	//Motion Trigger
	bool idleTrigger = false;
	bool runTrigger = false;
	bool jumpTrigger = false;
	bool attackTrigger[3] = { false, false, false };
	bool hitTrigger = false;
	bool evadeTrigger = false;
	bool skillTrigger[5] = { false, false, false, false, false };

	//Able Motion
	bool ableIdle = true;
	bool ableMove = true;
	bool ableRun = true;
	bool ableJump = true;
	bool ableAttack1 = true;
	bool ableAttack2, ableAttack3 = false;
	bool ableAttack[3] = { true, false, false };

	//Time Setting
	float curCoolTime[6];//5 : 회피
	float coolTime[6] = { 5.0f, 5.0f, 10.0f, 3.0f, 5.0f, 2.0f };

	bool damageMoment = false;//대미지가 들어가는 순간

	bool activeColliderAtt = false;

	Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 rotation = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 startPosition = Vector3(0.0f, 0.0f, 0.0f);

	//월드
	Vector3 wForward	= Vector3(+0.0f, 0.0f, -1.0f);
	Vector3 wBackward	= Vector3(+0.0f, 0.0f, +1.0f);
	Vector3 wRight		= Vector3(+1.0f, 0.0f, +0.0f);
	Vector3 wLeft		= Vector3(-1.0f, 0.0f, +0.0f);

	//로컬
	Vector3 forward		= Vector3(+0.0f, +0.0f, -1.0f);
	Vector3 backward	= Vector3(+0.0f, +0.0f, +1.0f);
	Vector3 right		= Vector3(+1.0f, +0.0f, +0.0f);
	Vector3 left		= Vector3(-1.0f, +0.0f, +0.0f);
	Vector3 up			= Vector3(+0.0f, +1.0f, +0.0f);
	Vector3 down		= Vector3(+0.0f, -1.0f, +0.0f);
	
	//카메라
	Vector3 cForward;
	Vector3 cBackward;
	Vector3 cLeft;
	Vector3 cRight;
	Vector3 cFLeft;
	Vector3 cFRight;
	Vector3 cBLeft;
	Vector3 cBRight;

	//Camera
private:
	bool cameraShake = false;
	bool cameraRelease = false;

	//Image
private:
	Texture* hpTexture;
	Texture* mpTexture;

	Render2D * hpRender;
	Render2D * mpRender;

	Texture* skillTexture[6];
	Render2D* skillRender[6];

	//Particle
private:
	ParticleSystem* footprintParticle = NULL;
	ParticleSystem* fireballParticle = NULL;
	TrailSystem* trail = NULL;

	bool bFootprint = false;
	bool bFireball = false;
	bool bSwordSpectrum = false;

	//Fireball Info
private:
	ColliderObject* colliderFireball;

	float fireballTime = 0.0f;
	float fireballScale = 0.0f;
	bool shootFireball = false;

	Vector3 fireballStartPosition = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 fireballPosition = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 fireballRotation = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 fireballDirect = Vector3(0.0f, 0.0f, 0.0f);
};

//collidersAtt->Collider->GetTransform()->Position(fireballPosition);
//0 20~50