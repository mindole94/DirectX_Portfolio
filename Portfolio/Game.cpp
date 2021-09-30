#include "stdafx.h"
#include "Game.h"
#include "Map.h"
#include "Player.h"
#include "Monster1.h"
#include "Monster2.h"

void Game::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(20, 0, 0);

	shader = new Shader(L"Battle.fxo");
	shadow = new Shadow(shader, Vector3(128, 0, 128), 130);

	map = new Map(shader);

	Context::Get()->MapSize() = Vector2(map->GetTerrain()->GetWidth(), map->GetTerrain()->GetHeight());
	Context::Get()->MapRestrictArea() = map->GetWater()->GetWaterRangeUV();

	player = new Player(shader);
	monster1 = new Monster1(shader, 30);
	monster2 = new Monster2(shader, 30);

	//PostEffect
	float width = D3D::Width(), height = D3D::Height();
	renderTarget = new RenderTarget((UINT)width, (UINT)height);
	depthStencil = new DepthStencil((UINT)width, (UINT)height);
	viewport = new Viewport(width, height);
	postEffect = new PostEffect(L"00_PostEffect.fxo");

	dissolveMap = new Texture(L"Dissolve/dissolve1.png");
	sDissolveMap = shader->AsSRV("DissolveMap");

	Context::Get()->Ambient() = Color(1, 1, 1, 1);

	weaponBox = new ColliderObject;

	weaponBox->Init = new Transform();
	weaponBox->Init->Scale(5.0f, 25.0f, 5.0f);
	weaponBox->Init->Position(0.0f, 24.0f, 0.0f);

	weaponBox->Transform = new Transform();
	weaponBox->Collider = new Collider(weaponBox->Transform, weaponBox->Init);

}

void Game::Destroy()
{
	SafeDelete(weaponBox);
	SafeDelete(dissolveMap);

	SafeDelete(renderTarget);
	SafeDelete(viewport);
	SafeDelete(depthStencil);
	SafeDelete(postEffect);

	SafeDelete(monster2);
	SafeDelete(monster1);
	SafeDelete(player);

	SafeDelete(map);
	SafeDelete(shadow)

	SafeDelete(shader);
}

void Game::Update()
{
	//////////////////////////////////////////////////////////////////////////////////////////
	if (Keyboard::Get()->Press('P'))
	{
		player->Full();
		std::cout << "Player : " << player->GetPosition().x << " " << player->GetPosition().y << " " << player->GetPosition().z << std::endl;
	}
	weaponBox->Collider->Update();

	//////////////////////////////////////////////////////////////////////////////////////////

	Vector3 playerPosition = player->GetPosition();
	//////////////////////////////////////////////////////////////////////////////////////////
	weaponBox->Transform->Position(playerPosition);
	//////////////////////////////////////////////////////////////////////////////////////////

	CameraSetting(playerPosition);
	Interact();
	ImGuiSetting();

	monster1->PlayerPosition(playerPosition);
	monster2->PlayerPosition(playerPosition);

	{
		float y = map->GetTerrain()->GetHeight(playerPosition);
		player->SetHeight(y);

		for (int i = 0; i < monster1->Num(); i++)
		{
			Vector3 pos = monster1->GetPosition(i);
			y = map->GetTerrain()->GetHeight(pos) - 3.0f;
			monster1->SetHeight(i, y);
		}

		for (int i = 0; i < monster2->Num(); i++)
		{
			Vector3 pos = monster2->GetPosition(i);
			y = map->GetTerrain()->GetHeight(pos);
			monster2->SetHeight(i, y);
		}
	}

	map->GetPlayerPosition(playerPosition);
	map->Update();

	shadow->Position(playerPosition);
	
	monster2->Update();
	monster1->Update();
	player->Update();

	if (player->IsBuff() == true)
		postEffect->PlayAfterImage(true);
	else
		postEffect->PlayAfterImage(false);

	postEffect->Update();
}

void Game::PreRender()
{
	//PreRender
	{
		//Depth
		{
			shadow->PreRender();

			monster2->PreRender();
			monster1->PreRender();
			player->PreRender();

			map->PreRender();
		}

		//Reflection
		{
			map->PreRender_Reflection_Water();

			monster2->PreRender_Reflection();
			monster1->PreRender_Reflection();
			player->PreRender_Reflection();
		}

		//Refraction
		{
			map->PreRender_Refraction_Water();

			monster2->Render();
			monster1->Render();
			player->Render();

			weaponBox->Collider->Render();
		}

		map->GetWater()->PreRender_End();

		//Sea
		if(map->BSea() == true)
		{
			map->PreRender_Reflection_Sea();

			map->PreRender_Refraction_Sea();

			map->GetSea()->PreRender_End();
		}
	}

	//Bloom
	viewport->RSSetViewport();
	{
		renderTarget->PreRender(depthStencil);

		map->Render();

		monster2->Render();
		monster1->Render();
		player->Render();
		weaponBox->Collider->Render();
		player->EffectRender();
		monster1->AttackRender();
	}
	postEffect->PreRender_Bloom(renderTarget, depthStencil, viewport);
	
	//AfterImage
	viewport->RSSetViewport();
	{
		renderTarget->PreRender(depthStencil);

		player->Render();
	}
	postEffect->PreRender_AfterImage(renderTarget, depthStencil, viewport);
}

void Game::Render()
{
	sDissolveMap->SetResource(dissolveMap->SRV());

	postEffect->Render();
	
	if (onUI == true)
		player->UIRender();

	//player->Render();
	//map->Render();

	////PostEffect Off
	//{
	//	sky->Pass(10);
	//	sky->Render();

	//	terrain->Render();

	//	monster2->Render();
	//	monster1->Render();
	//	player->Render();
	//}
}

void Game::Interact()
{
	PlayerDamaged();

	InteractSkill2();

	if (player->GetActiveAttBox() == false) return;

	switch (player->CurrentSkill())
	{
	case 0: InteractDefault(); break;
	case 1: InteractSkill1(); break;
	case 3: InteractSkill3(); break;
	case 5: InteractSkill5(); break;
	}
}

void Game::InteractDefault()
{
	float damage = player->Damage();

	for (int i = 0; i < monster1->Num(); i++)
	{
		float distance = D3DXVec3Length(&(monster1->GetPosition(i) - player->GetPosition()));
		
		if (distance > 12.0f) continue;

		if (monster1->GetActiveHitBox(i) == false) continue;
		
		bool hit = player->AttBox()->Collider->Intersection(monster1->HitBox(i)->Collider);
		
		if (hit == true)
		{
			monster1->GetDamage(i, damage);

			cameraShake = true;
		}
	}

	for (int i = 0; i < monster2->Num(); i++)
	{
		float distance = D3DXVec3Length(&(monster2->GetPosition(i) - player->GetPosition()));

		if (distance > 12.0f) continue;

		if (monster2->GetActiveHitBox(i) == false) continue;

		bool hit = player->AttBox()->Collider->Intersection(monster2->HitBox(i)->Collider);

		if (hit == true)
		{
			monster2->GetDamage(i, damage);

			cameraShake = true;
		}
	}
}

void Game::InteractSkill1()
{
	float damage = player->Damage() * 1.5f;

	for (int i = 0; i < monster1->Num(); i++)
	{
		float distance = D3DXVec3Length(&(monster1->GetPosition(i) - player->GetPosition()));

		if (distance > 12.0f) continue;

		if (monster1->GetActiveHitBox(i) == false) continue;

		bool hit = player->AttBox()->Collider->Intersection(monster1->HitBox(i)->Collider);

		if (hit == true)
		{
			monster1->GetDamage(i, damage);

			cameraShake = true;
		}
	}

	for (int i = 0; i < monster2->Num(); i++)
	{
		float distance = D3DXVec3Length(&(monster2->GetPosition(i) - player->GetPosition()));

		if (distance > 12.0f) continue;

		if (monster2->GetActiveHitBox(i) == false) continue;

		bool hit = player->AttBox()->Collider->Intersection(monster2->HitBox(i)->Collider);

		if (hit == true)
		{
			monster2->GetDamage(i, damage);

			cameraShake = true;
		}
	}
}

void Game::InteractSkill2()
{
	if (player->GetActiveFireBox() == false) return;

	float damage = player->Damage() * 2.0f;

	Vector3 pos;
	player->FireBox()->Init->Position(&pos);

	for (int i = 0; i < monster1->Num(); i++)
	{
		float distance = D3DXVec3Length(&(monster1->GetPosition(i) - pos));

		if (distance > 15.0f) continue;

		if (monster1->GetActiveHitBox(i) == false) continue;

		bool hit = player->FireBox()->Collider->Intersection(monster1->HitBox(i)->Collider);

		if (hit == true)
			monster1->GetDamage(i, damage);
	}

	for (int i = 0; i < monster2->Num(); i++)
	{
		float distance = D3DXVec3Length(&(monster2->GetPosition(i) - pos));

		if (distance > 15.0f) continue;

		if (monster2->GetActiveHitBox(i) == false) continue;

		bool hit = player->FireBox()->Collider->Intersection(monster2->HitBox(i)->Collider);

		if (hit == true)
			monster2->GetDamage(i, damage);
	}
}

void Game::InteractSkill3()
{
	Vector3 forward;
	forward = player->Forward();

	monster1->PlayerForward(forward);
	monster2->PlayerForward(forward);

	for (int i = 0; i < monster1->Num(); i++)
	{
		float distance = D3DXVec3Length(&(monster1->GetPosition(i) - player->GetPosition()));
		
		if (distance > 7.0f) continue;

		if (monster1->GetActiveHitBox(i) == false) continue;

		bool hit = player->AttBox()->Collider->Intersection(monster1->HitBox(i)->Collider);

		if (hit == true)
			monster1->SetKnockback(i, true);
	}

	for (int i = 0; i < monster2->Num(); i++)
	{
		float distance = D3DXVec3Length(&(monster2->GetPosition(i) - player->GetPosition()));

		if (distance > 7.0f) continue;

		if (monster2->GetActiveHitBox(i) == false) continue;

		bool hit = player->AttBox()->Collider->Intersection(monster2->HitBox(i)->Collider);

		if (hit == true)
			monster2->SetKnockback(i, true);
	}
}

void Game::InteractSkill5()
{
	float damage = player->Damage() * 2.0f;

	for (int i = 0; i < monster1->Num(); i++)
	{
		float distance = D3DXVec3Length(&(monster1->GetPosition(i) - player->GetPosition()));

		if (monster1->GetActiveHitBox(i) == false) continue;

		bool hit = player->AttBox()->Collider->Intersection(monster1->HitBox(i)->Collider);

		if (hit == true)
		{
			monster1->GetDamage(i, damage);

			cameraShake = true;
		}
	}

	for (int i = 0; i < monster2->Num(); i++)
	{
		float distance = D3DXVec3Length(&(monster2->GetPosition(i) - player->GetPosition()));

		if (monster2->GetActiveHitBox(i) == false) continue;

		bool hit = player->AttBox()->Collider->Intersection(monster2->HitBox(i)->Collider);

		if (hit == true)
		{
			monster2->GetDamage(i, damage);

			cameraShake = true;
		}
	}
}

void Game::PlayerDamaged()
{
	for (int i = 0; i < monster1->Num(); i++)
	{
		if (monster1->GetActiveAttBox(i) == false) continue;

		bool hit = player->HitBox()->Collider->Intersection(monster1->AttBox(i)->Collider);

		if (hit == true)
		{
			player->ReceivedDamage(50.0f);
			monster1->CheckHit(i, true);
		}
	}

	for (int i = 0; i < monster2->Num(); i++)
	{
		if (monster2->GetActiveAttBox(i) == false) continue;

		bool hit = player->HitBox()->Collider->Intersection(monster2->AttBox(i)->Collider);

		if (hit == true)
			player->ReceivedDamage(50.0f);
	}
}

void Game::CameraSetting(Vector3 pos)
{
	if (cameraShake == true)
	{
		((Freedom *)Context::Get()->GetCamera())->SetShake(0.5f, 1.0f);
		cameraShake = false;
	}
	else
		((Freedom *)Context::Get()->GetCamera())->At(pos);
}

void Game::ImGuiSetting()
{
	if (Keyboard::Get()->Down('O'))
	{
		bool b = Context::Get()->GetOnOff();
		b = !b;
		Context::Get()->SetOnOff(b);
	}
	
	if (Context::Get()->GetOnOff() == true)
	{
		ImGui::Begin("Setting");
		{
			string str = to_string(ImGui::GetIO().Framerate);
			ImGui::Text(str.c_str()); ImGui::Spacing();
			
			ImGui::Spacing(); ImGui::Separator();  ImGui::Spacing();

			ImGui::Text("Wire Frame"); ImGui::Spacing();

			static bool bSea = map->BSea();
			ImGui::Checkbox("Be Sea", &bSea);
			map->BSea(bSea);

			static bool seaPass = false;
			ImGui::Checkbox("WireFrame Sea", &seaPass);
			map->SeaPass(seaPass == false ? 24 : 25);

			static bool terrainPass = false;
			ImGui::Checkbox("WireFrame Terrain", &terrainPass);
			map->TerrainPass(terrainPass == false ? 8 : 9);

			ImGui::Spacing(); ImGui::Separator();  ImGui::Spacing();

			ImGui::Text("Time"); ImGui::Spacing();

			static bool realTime = map->GetSky()->GetRealTime();
			ImGui::Checkbox("Real Time##Sky", &realTime);
			map->GetSky()->SetRealTime(realTime);

			static float timeFactor = map->GetSky()->GetTimeFactor();
			ImGui::SliderFloat("Time Facotr##Sky", &timeFactor, 0.0f, 5.0f);
			map->GetSky()->SetTimeFactor(timeFactor);

			float theta = map->GetSky()->GetTheta();
			ImGui::SliderFloat("Sky Time", &theta, -Math::PI, Math::PI);
			if (map->GetSky()->GetRealTime() == false)
				map->GetSky()->SetTheta(theta);

			float plantShake = map->GetPlantShake();
			ImGui::SliderFloat("Plant Shake", &plantShake, 0.0f, 2.0f * Math::PI);
			map->SetPlantShake(plantShake);

			ImGui::Spacing(); ImGui::Separator();  ImGui::Spacing();

			ImGui::Text("UI"); ImGui::Spacing();
			ImGui::Checkbox("On UI", &onUI);

			ImGui::Spacing(); ImGui::Separator();  ImGui::Spacing();

			ImGui::Text("Camera"); ImGui::Spacing();

			bool bFreeMode = ((Freedom *)Context::Get()->GetCamera())->GetFreeMode();
			ImGui::Checkbox("Free Camera", &bFreeMode);
			((Freedom *)Context::Get()->GetCamera())->SetFreeMode(bFreeMode);
		}
		ImGui::End();
	}

}
