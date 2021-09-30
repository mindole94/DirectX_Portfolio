#include "stdafx.h"
#include "Monster1.h"
#include "MonsterModel.h"

Monster1::Monster1(Shader* shader)
	: shader(shader)
{
	monster = new MonsterModel(shader, L"HealSpirit", num);

	info = new Monster1Info[num];
	behavior = new Behavior[num];
	height = new float[num];
	attackLight = new AttackLight[num];

	for (int i = 0; i < num; i++)
		behavior[i] = bIdle;
}

Monster1::Monster1(Shader * shader, int num)
	: shader(shader), num(num)
{
	monster = new MonsterModel(shader, L"HealSpirit", num);

	info = new Monster1Info[num];
	behavior = new Behavior[num];
	height = new float[num];
	attackLight = new AttackLight[num];

	for (int i = 0; i < num; i++)
		behavior[i] = bIdle;

	for (int i = 0; i < num; i++)
	{
		attackLight[i].Light = new ParticleSystem(L"Galaxy");
		attackLight[i].Explosion = new ParticleSystem(L"GalaxyExplosion");

		PointLight light;
		light =
		{
		Color(1.0f, 0.0f, 0.0f, 1.0f), //Ambient
		Color(1.0f, 0.0f, 0.0f, 1.0f), //Diffuse
		Color(1.0f, 0.0f, 0.0f, 1.0f), //Specular
		Color(1.0f, 0.0f, 0.0f, 1.0f), //Emissive
		Vector3(0.0f, 0.0f, 0.0f), 0.0f, 1.0f
		};
		Lighting::Get()->AddPointLight(light);
	}
}

Monster1::~Monster1()
{
	for (int i = 0; i < num; i++)
	{
		SafeDelete(attackLight[i].Light);
		SafeDelete(attackLight[i].Explosion);
	}

	SafeDeleteArray(height);
	SafeDeleteArray(attackLight);
	SafeDeleteArray(behavior);
	SafeDeleteArray(info);
	SafeDelete(monster);
}

void Monster1::Update()
{
	for (int i = 0; i < num; i++)
	{
		if (monster->Life(i) == false) continue;

		//hp 0���� ���
		if (info[i].Hp <= 0.0f)
			Death(i);
		else
		{
			Vector3 pos;
			monster->GetMonster()->GetTransform(i)->Position(&pos);

			//���Ϳ� �÷��̾��� �Ÿ�
			float dist = D3DXVec3Length(&(playerPos - pos));

			//�÷��̾���� �Ÿ��� ���� ���̸� 60, �ƴϸ� 30�� ����
			if (dist > (info[i].Search == true ? 60.0f : 30.0f))
				Idle(i, dist);
			else
				Battle(i, dist);

			CountTime(i);
			AttackCheck(i);

			//���� ���� ����
			monster->GetMonster()->GetTransform(i)->Position(&pos);
			pos.y = height[i];
			monster->GetMonster()->GetTransform(i)->Position(pos);
		}
	}

	AttackUpdate();
	monster->Update();
}

void Monster1::PreRender()
{
	monster->PreRender();
}

void Monster1::PreRender_Reflection()
{
	monster->PreRender_Reflection();
}

void Monster1::Render()
{
	monster->BoxRender();
	monster->Render();
}

void Monster1::Idle(int instance, float distance)
{
	info[instance].Search = false;

	if (behavior[instance] == bIdle || behavior[instance] == bPatrol)
	{
		if (behavior[instance] == bIdle)
		{

			info[instance].IdleTime += Time::Delta();

			if (info[instance].IdleTime > maxIdleTime)
			{
				info[instance].IdleTime = 0.0f;

				Vector3 pos;
				monster->GetMonster()->GetTransform(instance)->Position(&pos);

				float patX = Math::Random(pos.x - patrolDist, pos.x + patrolDist);
				float patZ = Math::Random(pos.z - patrolDist, pos.z + patrolDist);
				
				info[instance].PatrolPos = Vector3(patX, 0.0f, patZ);

				while (CheckRestrictArea(info[instance].PatrolPos) == true)
				{
					patX = Math::Random(pos.x - patrolDist, pos.x + patrolDist);
					patZ = Math::Random(pos.z - patrolDist, pos.z + patrolDist);
					info[instance].PatrolPos = Vector3(patX, 0.0f, patZ);
				}

				monster->Run(instance);

				behavior[instance] = bPatrol;
			}
		}
		else if (behavior[instance] == bPatrol)
		{
			Vector3 pos, temp;
			monster->GetMonster()->GetTransform(instance)->Position(&pos);
			temp = pos;
			temp.y = 0.0f;

			float dist = D3DXVec3Length(&(info[instance].PatrolPos - temp));

			if (dist < 3.0f)
			{
				monster->Idle(instance);
				behavior[instance] = bIdle;
			}
			else
			{
				Vector3 direct = info[instance].PatrolPos - temp;
				D3DXVec3Normalize(&direct, &direct);
				pos += direct * speed * Time::Delta();

				float dot = D3DXVec3Dot(&monster->Right(instance), &direct);
				Vector3 temp;
				D3DXVec3Cross(&temp, &monster->Right(instance), &direct);

				float sign = temp.y >= 0.0f ? 1.0f : -1.0f;
				float theta = sign * acosf(dot);

				Vector3 rot;
				monster->GetMonster()->GetTransform(instance)->Rotation(&rot);
				rot.y += theta * rotRatio;

				monster->GetMonster()->GetTransform(instance)->Position(pos);
				monster->GetMonster()->GetTransform(instance)->Rotation(rot);
			}
		}
	}
	else
	{
		if (behavior[instance] == bChase)
		{
			int frameCount = monster->GetMonster()->GetModel()->ClipByIndex(1)->FrameCount();
			int currFrame = monster->GetMonster()->GetCurrentFrameTweenMode(instance);

			if (currFrame > frameCount - 3)
			{
				monster->Idle(instance);
				behavior[instance] = bIdle;
			}
		}
		else if (behavior[instance] == bAttack)
		{
			int frameCount = monster->GetMonster()->GetModel()->ClipByIndex(2)->FrameCount();
			int currFrame = monster->GetMonster()->GetCurrentFrameTweenMode(instance);

			if (currFrame > frameCount - 3)
			{
				monster->Idle(instance);
				behavior[instance] = bIdle;
			}
		}
		else if (behavior[instance] == bWait)
		{
			int frameCount = monster->GetMonster()->GetModel()->ClipByIndex(3)->FrameCount();
			int currFrame = monster->GetMonster()->GetCurrentFrameTweenMode(instance);

			if (currFrame > frameCount - 3)
			{
				monster->Idle(instance);
				behavior[instance] = bIdle;
			}
		}
	}
}

void Monster1::Battle(int instance, float distance)
{
	info[instance].Search = true;

	//Idle Ȥ�� Patrol Mode���̶�� Chase Mode
	if (behavior[instance] == bIdle || behavior[instance] == bPatrol)
	{
		behavior[instance] = bChase;
		monster->Run(instance);
	}
	else
	{
		//Chase Mode
		if (behavior[instance] == bChase)
		{
			//�÷��̾���� �Ÿ��� 20���ϸ� Attack Mode
			if (distance < 20.0f)
			{
				monster->Attack(instance);
				behavior[instance] = bAttack;
			}
			else
			{
				//�̵� ����
				Vector3 pos, direct;
				monster->GetMonster()->GetTransform(instance)->Position(&pos);
				direct = playerPos - pos;

				D3DXVec3Normalize(&direct, &direct);
				pos += direct * speed * Time::Delta();

				float dot = D3DXVec3Dot(&monster->Right(instance), &direct);
				Vector3 temp;
				D3DXVec3Cross(&temp, &monster->Right(instance), &direct);

				float sign = temp.y >= 0.0f ? 1.0f : -1.0f;
				float theta = sign * acosf(dot);

				Vector3 rot;
				monster->GetMonster()->GetTransform(instance)->Rotation(&rot);
				rot.y += theta * rotRatio;

				monster->GetMonster()->GetTransform(instance)->Position(pos);
				monster->GetMonster()->GetTransform(instance)->Rotation(rot);
			}
		}
		//Attack Mode
		else if (behavior[instance] == bAttack)
		{
			int frameCount = monster->GetMonster()->GetModel()->ClipByIndex(2)->FrameCount();
			int currFrame = monster->GetMonster()->GetCurrentFrameTweenMode(instance);

			//���ݹڽ� Ȱ��ȭ 40������
			if (currFrame == 40 && info[instance].Attacked == false)
			{
				monster->SetActiveAttBox(instance, true);

				info[instance].Attacked = true;
				attackLight[instance].AttackPosition = playerPos;
			}

			//Attack ����� Wait ���
			if (currFrame > frameCount - 3)
			{
				monster->Wait(instance);
				behavior[instance] = bWait;
			}
		}
		//Wait ���
		else if (behavior[instance] == bWait)
		{
			int frameCount = monster->GetMonster()->GetModel()->ClipByIndex(3)->FrameCount();
			int currFrame = monster->GetMonster()->GetCurrentFrameTweenMode(instance);

			//Wait Mode ����
			if (currFrame > frameCount - 3)
			{
				info[instance].WaitCount++;

				//�Ÿ��� ���� Chase Ȥ�� Attack Mode
				if (info[instance].WaitCount == 2)
				{
					info[instance].WaitCount = 0;

					if (distance < 20.0f)
					{
						monster->Attack(instance);
						behavior[instance] = bAttack;
					}
					else
					{
						monster->Run(instance);
						behavior[instance] = bChase;
					}
				}
			}
		}
		else if (behavior[instance] == bKnockback)
		{
			Vector3 pos;
			monster->GetMonster()->GetTransform(instance)->Position(&pos);
			pos += 70.0f * playerForward;

			monster->GetMonster()->GetTransform(instance)->Position(pos);
			behavior[instance] = bChase;
		}
	}
}

void Monster1::Death(int instance)
{
	if (behavior[instance] != bDeath)
	{
		monster->Death(instance);
		behavior[instance] = bDeath;
	}

	int frameCount = monster->GetMonster()->GetModel()->ClipByIndex(4)->FrameCount();
	int currFrame = monster->GetMonster()->GetCurrentFrameTweenMode(instance);

	Color color = monster->GetMonster()->GetColor(instance);
	//�ǰݽ��� ����ȭ �ʱ�ȭ
	color.r = 0.0f;

	//Disovle ����
	if (currFrame > frameCount - 3)
	{
		monster->GetMonster()->PauseAnimation(instance);

		info[instance].DeathTime += 0.5f * Time::Get()->Delta();
		color.g = info[instance].DeathTime;
		
		//ȭ�鿡�� ����
		if (color.g >= Math::PI * 0.5f)
		{
			monster->Scale(instance, 0, 0, 0);
			monster->Life(instance, false);
		}
	}

	monster->GetMonster()->SetColor(instance, color);
}

void Monster1::CountTime(int instance)
{
	if (monster->GetActiveHitBox(instance) == true) return;

	info[instance].InvincibleTime -= Time::Delta();

	if (info[instance].InvincibleTime < 0.0f)
	{
		Color color = monster->GetMonster()->GetColor(instance);
		color.r = 0.0f;
		monster->GetMonster()->SetColor(instance, color);

		info[instance].InvincibleTime = 0.0f;
		monster->SetActiveHitBox(instance, true);
	}
}

void Monster1::AttackCheck(int instance)
{
	if (info[instance].Attacked == false) return;
	
	PointLight& light = Lighting::Get()->GetPointLight(instance);

	//ó�� ���Խ�
	if (attackLight[instance].bActived == false)
	{
		attackLight[instance].bActived = true;
		attackLight[instance].AttackPosition.y += 100.0f;
		attackLight[instance].StartPosition = attackLight[instance].AttackPosition;

		monster->SetActiveAttBox(instance, true);

		light.Position = playerPos;
		light.Range = 1.5f;
	}

	//Udpate ����
	if (attackLight[instance].bActived == true)
	{
		attackLight[instance].AttackPosition.y -= 50.0f * Time::Delta();
		attackLight[instance].Light->Add(attackLight[instance].AttackPosition);

		monster->AttBox(instance)->Init->Position(attackLight[instance].AttackPosition);
	}

	//�÷��̾�� ���߽�
	if (attackLight[instance].CheckHit == true)
	{
		monster->SetActiveAttBox(instance, false);
		attackLight[instance].CheckHit = false;
	}

	//������ - ������ġ
	float distance = attackLight[instance].AttackPosition.y - light.Position.y;

	//���� �浹�� explosion particle
	if (distance < 0.0f)
	{

		attackLight[instance].Explosion->Add(light.Position);
		attackLight[instance].time += Time::Delta();
	}

	//���� ���� ����
	if (attackLight[instance].time > 1.0f)
	{
		monster->SetActiveAttBox(instance, false);
		attackLight[instance].CheckHit = false;
		attackLight[instance].bActived = false;
		attackLight[instance].time = 0.0f;
		info[instance].Attacked = false;

		light.Range = 0.0f;
	}
}

void Monster1::AttackUpdate()
{
	for (int i = 0; i < num; i++)
	{
		attackLight[i].Light->Update();
		attackLight[i].Explosion->Update();
	}
}

bool Monster1::CheckRestrictArea(Vector3 position)
{
	Color restricted = Context::Get()->MapRestrictArea();
	Vector2 mapSize = Context::Get()->MapSize();

	//restriected��  UV���µ� WPosition���� ��ȯ, z���� ���������
	restricted.r = (mapSize.x * restricted.r) - (mapSize.x * 0.5f);		//��
	restricted.g = (mapSize.x * restricted.g) - (mapSize.x * 0.5f);		//��
	restricted.b = -((mapSize.y * restricted.b) - (mapSize.y * 0.5f));	//��
	restricted.a = -((mapSize.y * restricted.a) - (mapSize.y * 0.5f));	//��

	bool b = true;
	b &= position.x > restricted.r;
	b &= position.x < restricted.g;
	b &= position.z < restricted.b;
	b &= position.z > restricted.a;

	return b;
}

void Monster1::AttackRender()
{
	for (int i = 0; i < num; i++)
	{
		attackLight[i].Explosion->Render();
		attackLight[i].Light->Render();
	}
}

Vector3 Monster1::GetPosition(UINT instance)
{
	Vector3 pos;
	monster->Position(instance, &pos);

	return pos;
}

ColliderObject * Monster1::HitBox(int instance)
{
	return monster->HitBox(instance);
}

ColliderObject * Monster1::AttBox(int instance)
{
	return monster->AttBox(instance);
}

void Monster1::GetDamage(int instance, float damage)
{
	monster->SetActiveHitBox(instance, false);

	Color color = monster->GetMonster()->GetColor(instance);
	color.r = 1.0f;
	monster->GetMonster()->SetColor(instance, color);

	info[instance].InvincibleTime = maxInvincibleTime;
	info[instance].Hp -= damage;
}

bool Monster1::GetActiveAttBox(int instance)
{
	return monster->GetActiveAttBox(instance);
}

bool Monster1::GetActiveHitBox(int instance)
{
	return monster->GetActiveHitBox(instance);
}

void Monster1::SetKnockback(int instance, bool bKnockback)
{
	if (bKnockback == true)
		behavior[instance] = this->bKnockback;
}

void Monster1::CheckHit(int instance, bool result)
{
	attackLight[instance].CheckHit = result;
}
