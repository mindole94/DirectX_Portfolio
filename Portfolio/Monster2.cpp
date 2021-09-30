#include "stdafx.h"
#include "Monster2.h"
#include "MonsterModel.h"

Monster2::Monster2(Shader* shader)
	: shader(shader)
{
	monster = new MonsterModel(shader, L"Ssalissali", num);

	info = new Monster2Info[num];
	behavior = new Behavior[num];
	height = new float[num];

	for (int i = 0; i < num; i++)
		behavior[i] = bIdle;
}

Monster2::Monster2(Shader * shader, int num)
	: shader(shader), num(num)
{
	monster = new MonsterModel(shader, L"Ssalissali", num);

	info = new Monster2Info[num];
	behavior = new Behavior[num];
	height = new float[num];

	for (int i = 0; i < num; i++)
		behavior[i] = bIdle;
}

Monster2::~Monster2()
{
	SafeDeleteArray(height);
	SafeDeleteArray(behavior);
	SafeDeleteArray(info);
	SafeDelete(monster);
}

void Monster2::Update()
{
	for (int i = 0; i < num; i++)
	{
		if (monster->Life(i) == false) continue;

		//hp 0이하 사망
		if (info[i].Hp <= 0.0f)
			Death(i);
		else
		{
			Vector3 pos;
			monster->GetMonster()->GetTransform(i)->Position(&pos);

			//몬스터와 플레이어의 거리
			float dist = D3DXVec3Length(&(playerPos - pos));

			//플레이어와의 거리가 추적 중이면 60, 아니면 30의 범위
			if (dist > (info[i].Search == true ? 60.0f : 30.0f))
				Idle(i, dist);
			else
				Battle(i, dist);

			CountTime(i);

			//몬스터 높이 설정
			monster->GetMonster()->GetTransform(i)->Position(&pos);
			pos.y = height[i];
			monster->GetMonster()->GetTransform(i)->Position(pos);
		}
	}

	monster->Update();
}

void Monster2::PreRender()
{
	monster->PreRender();
}

void Monster2::PreRender_Reflection()
{
	monster->PreRender_Reflection();
}

void Monster2::Render()
{
	monster->BoxRender();
	monster->Render();
}

void Monster2::Idle(int instance, float distance)
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

void Monster2::Battle(int instance, float distance)
{
	info[instance].Search = true;

	//Idle 혹은 Patrol Mode중이라면 Chase Mode
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
			//플레이어와의 거리가 5이하면 Attack Mode
			if (distance < 5.0f)
			{
				monster->Attack(instance);
				behavior[instance] = bAttack;
			}
			else
			{
				//이동 구현
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

			if (info[instance].Attacked == true)
				monster->SetActiveAttBox(instance, false);

			//공격박스 활성화
			if (currFrame == 23 && info[instance].Attacked == false)
			{
				info[instance].Attacked = true;
				monster->SetActiveAttBox(instance, true);
			}

			//Attack 종료시 Wait 모드
			if (currFrame > frameCount - 3)
			{
				monster->Wait(instance);
				behavior[instance] = bWait;
				info[instance].Attacked = false;
			}
		}
		//Wait 모드
		else if (behavior[instance] == bWait)
		{
			int frameCount = monster->GetMonster()->GetModel()->ClipByIndex(3)->FrameCount();
			int currFrame = monster->GetMonster()->GetCurrentFrameTweenMode(instance);

			//Wait Mode 종료
			if (currFrame > frameCount - 3)
			{
				//거리에 따라 Chase 혹은 Attack Mode
				if (distance < 5.0f)
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

void Monster2::Death(int instance)
{
	if (behavior[instance] != bDeath)
	{
		monster->Death(instance);
		behavior[instance] = bDeath;
	}

	int frameCount = monster->GetMonster()->GetModel()->ClipByIndex(4)->FrameCount();
	int currFrame = monster->GetMonster()->GetCurrentFrameTweenMode(instance);

	Color color = monster->GetMonster()->GetColor(instance);
	//피격시의 색변화 초기화
	color.r = 0.0f;

	//Disovle 시작
	if (currFrame > frameCount - 3)
	{
		monster->GetMonster()->PauseAnimation(instance);

		info[instance].DeathTime += 0.5f * Time::Get()->Delta();
		color.g = info[instance].DeathTime;

		//화면에서 감춤
		if (color.g >= Math::PI * 0.5f)
		{
			monster->Scale(instance, 0, 0, 0);
			monster->Life(instance, false);
		}
	}

	monster->GetMonster()->SetColor(instance, color);
}

void Monster2::CountTime(int instance)
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

bool Monster2::CheckRestrictArea(Vector3 position)
{
	Color restricted = Context::Get()->MapRestrictArea();
	Vector2 mapSize = Context::Get()->MapSize();

	//restriected가  UV였는데 WPosition으로 변환, z축은 뒤집어야함
	restricted.r = (mapSize.x * restricted.r) - (mapSize.x * 0.5f);		//좌
	restricted.g = (mapSize.x * restricted.g) - (mapSize.x * 0.5f);		//우
	restricted.b = -((mapSize.y * restricted.b) - (mapSize.y * 0.5f));	//상
	restricted.a = -((mapSize.y * restricted.a) - (mapSize.y * 0.5f));	//하

	bool b = true;
	b &= position.x > restricted.r;
	b &= position.x < restricted.g;
	b &= position.z < restricted.b;
	b &= position.z > restricted.a;

	return b;
}

Vector3 Monster2::GetPosition(int instance)
{
	Vector3 vec;
	monster->Position(instance, &vec);
	return vec;
}

ColliderObject * Monster2::HitBox(int instance)
{
	return monster->HitBox(instance);
}

ColliderObject * Monster2::AttBox(int instance)
{
	return monster->AttBox(instance);
}

void Monster2::GetDamage(int instance, float damage)
{
	monster->SetActiveHitBox(instance, false);

	Color color = monster->GetMonster()->GetColor(instance);
	color.r = 1.0f;
	monster->GetMonster()->SetColor(instance, color);

	info[instance].InvincibleTime = maxInvincibleTime;
	info[instance].Hp -= damage;
}

bool Monster2::GetActiveAttBox(int instance)
{
	return monster->GetActiveAttBox(instance);
}

bool Monster2::GetActiveHitBox(int instance)
{
	return monster->GetActiveHitBox(instance);
}

void Monster2::SetKnockback(int instance, bool bKnockback)
{
	if (bKnockback == true)
		behavior[instance] = this->bKnockback;
}
