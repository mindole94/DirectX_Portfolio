#include "stdafx.h"
#include "PlayerModel.h"

PlayerModel::PlayerModel(Shader * shader)
	: shader(shader)
{
	Player();
	Weapon();

	CreateEffects();
	CreateColliders();
}

PlayerModel::~PlayerModel()
{
	SafeDelete(skillRange.SkillRange);
	SafeDelete(swordtrail);
	SafeDelete(buff.Buff);
	SafeDelete(slash.Slash);
	SafeDelete(fireball.Fireball);
	SafeDelete(fireball.Explosion);
	SafeDelete(footprint.Footprint);
	SafeDelete(fireBox);
	SafeDelete(hitBox);
	SafeDelete(attBox);
	SafeDelete(weaponBox);
	SafeDelete(weaponTransform);
	SafeDelete(weapon);
	SafeDelete(player);
}

void PlayerModel::Update()
{
	weaponBox->Collider->Update();
	attBox->Collider->Update();
	hitBox->Collider->Update();

	if (activeFireBox == true)
	{
		float scale = fireball.Scale;
		scale *= 0.1f;
		fireBox->Init->Scale(scale, scale, scale);
		fireBox->Init->Position(fireball.Position);
		fireBox->Collider->Update();
	}

	UpdateEffects();

	Position(position);

	{
		player->GetAttachTransforms(0, bones);
		Vector3 pos, rot;
		Position(&pos);
		Rotation(&rot);

		pos.y += 4.0f;

		Matrix mat;
		D3DXMatrixMultiply(&mat, &weaponTransform->World(), &bones[20]);

		weaponBox->Transform->World(mat);

		attBox->Transform->World(bones[0]);

		hitBox->Init->Position(pos);
		hitBox->Init->Rotation(rot);

		weapon->GetTransform(0)->World(mat);
		//weapon->GetTransform(0)->Scale(0.15f, 0.15f, 0.15f);
	}
	player->UpdateTransforms();
	weapon->UpdateTransforms();

	player->Update();
	weapon->Update();

}

void PlayerModel::PreRender()
{
	player->Pass(2);
	player->Render();

	weapon->Pass(1);
	weapon->Render();
}

void PlayerModel::PreRender_Reflection()
{
	player->Pass(16);
	player->Render();

	weapon->Pass(15);
	weapon->Render();
}

void PlayerModel::Render()
{
	player->Pass(6);
	player->Render();

	weapon->Pass(5);
	weapon->Render();
}

void PlayerModel::BoxRender()
{
	weaponBox->Collider->Render();
	attBox->Collider->Render(attColor);
	hitBox->Collider->Render(hitColor);

	if (activeFireBox == true)
		fireBox->Collider->Render(fireColor);
}

void PlayerModel::EffectRender()
{
	RenderEffects();

}

void PlayerModel::Player()
{
	player = new ModelAnimator(shader);
	player->ReadMesh(L"sapling/sapling");
	player->ReadMaterial(L"sapling/sapling");

	//이동
	player->ReadClip(L"sapling/clip/Standing Idle");//0
	player->ReadClip(L"sapling/clip/Running");//1
	player->ReadClip(L"sapling/clip/Standing Jump");//2일반 점프

	//회피
	player->ReadClip(L"sapling/clip/Jump Back");//3무적 백점프

	//평타
	player->ReadClip(L"sapling/clip/Standing Melee Attack Horizontal");//4 평타 콤보1
	player->ReadClip(L"sapling/clip/Standing Melee Combo Attack Ver. 3");//5 평타 콤보2
	player->ReadClip(L"sapling/clip/Standing Melee Attack 360 Low");//6 평타 콤보3

	//스킬
	player->ReadClip(L"sapling/clip/Standing Melee Attack Backhand");//7 스킬1
	player->ReadClip(L"sapling/clip/Spell Cast");//8 스킬2
	player->ReadClip(L"sapling/clip/Sword And Shield Run");//9 스킬3
	player->ReadClip(L"sapling/clip/Standing Taunt Battlecry");//10 스킬4
	player->ReadClip(L"sapling/clip/Mutant Run");//11 스킬5

	//댄스
	player->ReadClip(L"sapling/clip/Chicken Dance");//13

	Transform* transform = NULL;
	transform = player->AddTransform();
	transform->Position(0, 0, 0);
	transform->Scale(0.05f, 0.05f, 0.05f);
	player->PlayTweenMode(0, 0);
	player->UpdateTransforms();
}

void PlayerModel::Weapon()
{
	weapon = new ModelRender(shader);
	weapon->ReadMesh(L"Weapon/L_Dual17_SM");
	weapon->ReadMaterial(L"Weapon/L_Dual17_SM");

	weapon->AddTransform();

	weapon->UpdateTransforms();
	weapon->Pass(1);

	weaponTransform = new Transform();
	weaponTransform->Position(-0.616f, 3.093f, 3.875f);
	weaponTransform->RotationDegree(7.791f, 82.809f, -130.822f);
	weaponTransform->Scale(3.0f, 3.0f, 3.0f);
}

void PlayerModel::Position(float x, float y, float z)
{
	player->GetTransform(0)->Position(x, y, z);
}

void PlayerModel::Position(Vector3 & vec)
{
	player->GetTransform(0)->Position(vec);
}

void PlayerModel::Position(Vector3 * vec)
{
	player->GetTransform(0)->Position(vec);
}

void PlayerModel::Scale(float x, float y, float z)
{
	player->GetTransform(0)->Scale(x, y, z);
}

void PlayerModel::Scale(Vector3 & vec)
{
	player->GetTransform(0)->Scale(vec);
}

void PlayerModel::Scale(Vector3 * vec)
{
	player->GetTransform(0)->Scale(vec);
}

void PlayerModel::Rotation(float x, float y, float z)
{
	player->GetTransform(0)->Rotation(x, y, z);
}

void PlayerModel::Rotation(Vector3 & vec)
{
	player->GetTransform(0)->Rotation(vec);
}

void PlayerModel::Rotation(Vector3 * vec)
{
	player->GetTransform(0)->Rotation(vec);
}

void PlayerModel::RotationDegree(float x, float y, float z)
{
	player->GetTransform(0)->RotationDegree(x, y, z);
}

void PlayerModel::RotationDegree(Vector3 & vec)
{
	player->GetTransform(0)->RotationDegree(vec);

}

void PlayerModel::RotationDegree(Vector3 * vec)
{
	player->GetTransform(0)->RotationDegree(vec);
}

Vector3 PlayerModel::Forward()
{
	Vector3 vec = -player->GetTransform(0)->Forward();
	return vec;
}

Vector3 PlayerModel::Backward()
{
	Vector3 vec = player->GetTransform(0)->Forward();
	return vec;
}

Vector3 PlayerModel::Up()
{
	Vector3 vec = player->GetTransform(0)->Up();
	return vec;
}

Vector3 PlayerModel::Down()
{
	Vector3 vec = -player->GetTransform(0)->Up();
	return vec;
}

Vector3 PlayerModel::Right()
{
	Vector3 vec = player->GetTransform(0)->Right();
	return vec;
}

Vector3 PlayerModel::Left()
{
	Vector3 vec = -player->GetTransform(0)->Right();
	return vec;
}

void PlayerModel::World(Matrix & matrix)
{
	player->GetTransform(0)->World(matrix);
}

Matrix & PlayerModel::World()
{
	return player->GetTransform(0)->World();
}

void PlayerModel::Idle()
{
	player->PlayTweenMode(0, 0, 1.0f);
}

void PlayerModel::Run()
{
	player->PlayTweenMode(0, 1, 1.0f);
}

void PlayerModel::Jump()
{
	player->ResetFrame(0);
	player->PlayNormalMode(0, 2, 1.0f);
}

void PlayerModel::Evade()
{
	player->ResetFrame(0);
	player->PlayNormalMode(0, 3, 1.0f);
}

void PlayerModel::Attack1()
{
	player->ResetFrame(0);
	player->PlayNormalMode(0, 4, testspeed);
}

void PlayerModel::Attack2()
{
	player->ResetFrame(0);
	player->PlayNormalMode(0, 5, testspeed);
}

void PlayerModel::Attack3()
{ 
	player->ResetFrame(0);
	player->PlayNormalMode(0, 6, testspeed);
}

void PlayerModel::Skill1()
{
	player->ResetFrame(0);
	player->PlayNormalMode(0, 7, 1.0f);
}

void PlayerModel::Skill2()
{
	player->ResetFrame(0);
	player->PlayNormalMode(0, 8, 1.0f);
}

void PlayerModel::Skill3()
{
	player->ResetFrame(0);
	player->PlayNormalMode(0, 9, 2.0f);
}

void PlayerModel::Skill4()
{
	player->ResetFrame(0);
	player->PlayNormalMode(0, 10, 2.0f);
}

void PlayerModel::Skill5()
{
	player->ResetFrame(0);
	player->PlayNormalMode(0, 11, 2.0f);
}

void PlayerModel::CreateEffects()
{
	footprint.Footprint = new ParticleSystem(L"FootPrint");
	
	fireball.Fireball = new ParticleSystem(L"Fireball");
	fireball.Explosion = new ParticleSystem(L"FireballExplosion");

	slash.Slash = new ParticleSystem(L"SlashStar");
	slash.Slash->SetColor(Color(1, 0, 1, 1));

	buff.Buff = new ParticleSystem(L"Buff");
	buff.Buff->SetColor(Color(1, 0, 1, 1));

	swordtrail = new TrailSystem();

	skillRange.SkillRange = new Projector(shader, L"Red.png", 10, 100);
}

void PlayerModel::UpdateEffects()
{
	Vector3 pos;
	Position(&pos);

	if (footprint.Use == true)
		footprint.Footprint->Add(pos);

	if (fireball.Use == true && fireball.Shoot == false)
	{
		const float speed = 25.0f;
		activeFireBox = true;

		weapon->GetTransform(0)->Position(&fireball.Position);
		fireball.Scale += speed * Time::Delta();
		fireball.Scale = Math::Clamp(fireball.Scale, 0.0f, 50.0f);

		fireball.Fireball->Scale(fireball.Scale);
		fireball.Fireball->Add(fireball.Position);
	}
	else if (fireball.Use == true && fireball.Shoot == true)
	{
		const float speed = 1000.0f;
		fireColor = Color(1, 1, 0, 1);

		fireball.Position += fireball.Direct * speed * Time::Delta();
		
		fireball.Fireball->Add(fireball.Position);

		float length = D3DXVec3Length(&(fireball.Position - fireball.StartPosition));

		if (length > 150.0f)
		{
			fireball.Use = false;
			fireball.Shoot = false;
			fireball.Boom = true;

			activeFireBox = false;
			fireColor = Color(0, 1, 0, 1);
		}
	}

	if (fireball.Boom == true)
	{
		fireball.ExplsionTime += Time::Delta();
		fireball.Explosion->Scale(fireball.Scale * 5.0f);
		if (fireball.ExplsionTime < 2.0f)
		{
			fireball.Explosion->Add(fireball.Position);
		}
		else
		{
			fireball.Scale = 0.0f;
			fireball.Boom = false;
			fireball.ExplsionTime = 0.0f;
		}

	}

	if (slash.Use == true)
	{
		slash.Slash->Gravity(Forward());
		slash.Slash->Add(weaponBox->Collider->Top());
	}

	if (bSwordTrail == true)
		swordtrail->Add(weaponBox->Collider->Center(), weaponBox->Collider->Top());

	if (buff.Use == true)
	{
		Vector3 temp = hitBox->Collider->Bottom();
		float a = 2.0f;
		temp.x = Math::Random(temp.x - a, temp.x + a);
		temp.z = Math::Random(temp.z - a, temp.z + a);

		buff.Buff->Add(temp);
	}

	footprint.Footprint->Update();
	fireball.Fireball->Update();
	fireball.Explosion->Update();
	slash.Slash->Update();
	buff.Buff->Update();
	skillRange.SkillRange->Update();

	swordtrail->Update();
}

void PlayerModel::RenderEffects()
{
	footprint.Footprint->Render();
	fireball.Fireball->Render();
	fireball.Explosion->Render();
	slash.Slash->Render();
	buff.Buff->Render();
	skillRange.SkillRange->Render();

	swordtrail->Render();
}

void PlayerModel::CreateColliders()
{
	weaponBox = new ColliderObject;

	weaponBox->Init = new Transform();
	weaponBox->Init->Scale(5.0f, 25.0f, 5.0f);
	weaponBox->Init->Position(0.0f, 24.0f, 0.0f);

	weaponBox->Transform = new Transform();
	weaponBox->Collider = new Collider(weaponBox->Transform, weaponBox->Init);

	//////////////////////////////////////////////////////////////////////////////////////////

	hitBox = new ColliderObject();

	hitBox->Init = new Transform();
	hitBox->Init->Scale(3, 8, 3);

	hitBox->Transform = new Transform();
	hitBox->Collider = new Collider(hitBox->Transform, hitBox->Init);

	//////////////////////////////////////////////////////////////////////////////////////////

	attBox = new ColliderObject;

	attBox->Init = new Transform();

	attBox->Transform = new Transform();
	attBox->Collider = new Collider(attBox->Transform, attBox->Init);

	//////////////////////////////////////////////////////////////////////////////////////////

	fireBox = new ColliderObject;

	fireBox->Init = new Transform();

	fireBox->Transform = new Transform();
	fireBox->Collider = new Collider(fireBox->Transform, fireBox->Init);
}

void PlayerModel::SetSkillRange(Vector3 * finalPos)
{
	if (skillRange.Use == false)
	{
		Vector3 pos, rot;
		Vector3 forward = Forward() * 2000.0f;

		Position(&pos);
		Rotation(&rot);

		pos += forward;
		pos.y += 10.0f;

		skillRange.SkillRange->SetRotation(rot);
		skillRange.SkillRange->SetPosition(pos);
		skillRange.Use = true;

		*finalPos = pos;
	}
	else
	{
		Vector3 pos;

		Position(&pos);
		pos.y -= 100.0f;

		skillRange.SkillRange->SetPosition(pos);
		skillRange.Use = false;
	}
}
