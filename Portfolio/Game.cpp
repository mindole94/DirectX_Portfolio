#include "stdafx.h"
#include "Game.h"
#include "Player.h"
#include "Monster1.h"
#include "Monster2.h"

void Game::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(20, -180, 0);
	Context::Get()->GetCamera()->Position(0, 550, 715);
	
	shader = new Shader(L"48_Shadow.fxo");
	shadow = new Shadow(shader, Vector3(128, 0, 128), 65);

	sky = new CubeSky(L"Environment/GrassCube1024.dds", shader);

	terrain = new Terrain(shader, L"Terrain/Gray256.png");
	terrain->BaseMap(L"Terrain/Dirt3.png");

	player = new Player(shader);
	monster1 = new Monster1(shader);
	monster2 = new Monster2(shader);

	Vector3 playerPosition = player->GetPosition();
	((Freedom *)Context::Get()->GetCamera())->At(Vector3(playerPosition.x, playerPosition.y + 20.0f, playerPosition.z));
}

void Game::Destroy()
{
	SafeDelete(monster2);
	SafeDelete(monster1);
	SafeDelete(player);
	SafeDelete(terrain);

	SafeDelete(shader);

}

void Game::Update()
{
	{
		ImGui::Checkbox("Colliders Show", &showCollider);
		player->ShowCollider(showCollider);
		monster1->ShowCollider(showCollider);
		monster2->ShowCollider(showCollider);
	}

	//플레이어 위치
	Vector3 playerPosition = player->GetPosition();
	
	player->SetHeight(terrain->GetHeight(playerPosition));//플레이어 높이 설정

	for (int index = 0; index < MONSTER1_NUM; index++)
		monster1->SetHeight(index, terrain->GetHeight(monster1->GetPosition(index)));
	
	for (int index = 0; index < MONSTER2_NUM; index++)
		monster2->SetHeight(index, terrain->GetHeight(monster2->GetPosition(index)) - 3.0f);

	monster1->GetPlayerPosition(playerPosition);
	monster2->GetPlayerPosition(playerPosition);
		
	Vector3 cameraPosition = ((Freedom *)Context::Get()->GetCamera())->GetPosition();

	CheckFireball();
	CheckHit();
	CheckDamage();

	player->Update();

	monster2->Update();
	monster1->Update();

	CameraSetting();

	sky->Update();
	terrain->Update();

}

void Game::PreRender()
{
	shadow->PreRender();

	monster2->PreRender();
	monster1->PreRender();
	player->PreRender();
	
	terrain->Pass(3);
	terrain->Render();
}

void Game::Render()
{
	sky->Pass(4);
	sky->Render();

	terrain->Pass(8);
	terrain->Render();

	monster2->Render();
	monster1->Render();

	player->Render();
}

void Game::CheckHit()
{
	if (player->ActiveColliderAtt() == false) return;//공격중이 아닐때 리턴
	
	for (int index = 0; index < MONSTER1_NUM; index++)
	{
		if (monster1->ActiveColliderHit(index) == false) continue;//무적시간일 경우 스킵

		float distance = D3DXVec3Length(&(monster1->GetPosition(index) - player->GetPosition()));

		if (distance > 50.0f) continue;//거리가 멀경우 스킵

		bool hit = player->GetColliderAtt()->Collider->Intersection(monster1->GetColliderHit(index)->Collider);
		
		if (hit == true)
		{
			player->SetHitSucces(true);
			
			monster1->Damage(index, player->CurrentSkill(), player->Damage(), false);
		}
	}

	for (int index = 0; index < MONSTER2_NUM; index++)
	{
		if (monster2->ActiveColliderHit(index) == false) continue;//무적시간일 경우 스킵

		float distance = D3DXVec3Length(&(monster2->GetPosition(index) - player->GetPosition()));

		if (distance > 50.0f) continue;//거리가 멀경우 스킵

		bool hit = player->GetColliderAtt()->Collider->Intersection(monster2->GetColliderHit(index)->Collider);

		if (hit == true)
		{
			player->SetHitSucces(true);

			monster2->Damage(index, player->CurrentSkill(), player->Damage(), false);
		}
	}
}

void Game::CheckFireball()
{
	if (player->ActiveFireball() == false) return;//공격중이 아닐때 리턴

	for (int index = 0; index < MONSTER1_NUM; index++)
	{
		if (monster1->ActiveColliderHit(index) == false) continue;//무적시간일 경우 스킵

		float distance = D3DXVec3Length(&(monster1->GetPosition(index) - player->GetFireballPosition()));

		if (distance > 30.0f) continue;//거리가 멀경우 스킵

		bool hit = player->GetColliderFireball()->Collider->Intersection(monster1->GetColliderHit(index)->Collider);

		if (hit == true)
			monster1->Damage(index, player->CurrentSkill(), player->GetFireballScale() * 50.0f, true);
	}

	for (int index = 0; index < MONSTER2_NUM; index++)
	{
		if (monster2->ActiveColliderHit(index) == false) continue;//무적시간일 경우 스킵

		float distance = D3DXVec3Length(&(monster2->GetPosition(index) - player->GetFireballPosition()));

		if (distance > 30.0f) continue;//거리가 멀경우 스킵

		bool hit = player->GetColliderFireball()->Collider->Intersection(monster2->GetColliderHit(index)->Collider);

		if (hit == true)
			monster2->Damage(index, player->CurrentSkill(), player->GetFireballScale() * 50.0f, true);
	}
}

void Game::CheckDamage()
{
	if (player->ActiveEvade() == true) return;//플레이어가 회피중일때 리턴

	for (int index = 0; index < MONSTER1_NUM; index++)
	{
		if (monster1->ActiveColliderAtt(index) == false) continue;//몬스터가 공격중이 아닐때 스킵

		bool hit = monster1->GetColliderAtt(index)->Collider->Intersection(player->GetColliderHit()->Collider);

		if (hit == true)
		{
			int damage = Math::Random(-50, -100);

			player->SetHp(damage);
			monster1->SetActiveColliderHit(index, false);
		}
	}

	for (int index = 0; index < MONSTER2_NUM; index++)
	{
		if (monster2->ActiveColliderAtt(index) == false) continue;//몬스터가 공격중이 아닐때 스킵

		bool hit = monster2->GetColliderAtt(index)->Collider->Intersection(player->GetColliderHit()->Collider);

		if (hit == true)
		{
			int damage = Math::Random(-50, -100);

			player->SetHp(damage);
			monster2->SetActiveColliderHit(index, false);
		}
	}
}

void Game::CameraSetting()
{
	Vector3 playerPosition = player->GetPosition();

	if (player->CameraRelease() == true)
		((Freedom *)Context::Get()->GetCamera())->At(Vector3(playerPosition.x, terrain->GetHeight(playerPosition), playerPosition.z));
	else
		((Freedom *)Context::Get()->GetCamera())->At(Vector3(playerPosition.x, playerPosition.y, playerPosition.z));;

	if (player->CameraShake() == true)
		((Freedom *)Context::Get()->GetCamera())->SetShake(1.0f, 2.0f);
}