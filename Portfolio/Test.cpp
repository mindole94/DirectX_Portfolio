#include "stdafx.h"
#include "Test.h"
#include "Player.h"
#include "Monster1.h"
#include "Monster2.h"

void Test::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(20, 0, 0);

	shader = new Shader(L"Battle.fxo");

	player = new Player(shader);
	monster1 = new Monster1(shader, 1);
	monster2 = new Monster2(shader, 1);

	sky = new Sky(shader);
	sky->RealTime(false, Math::PI - 1e-6f);
}

void Test::Destroy()
{
	SafeDelete(sky);
	SafeDelete(monster2);
	SafeDelete(monster1);
	SafeDelete(player);
}

void Test::Update()
{
	Vector3 playerPosition = player->GetPosition();
	
	((Freedom *)Context::Get()->GetCamera())->At(playerPosition);
	monster1->PlayerPosition(playerPosition);
	monster2->PlayerPosition(playerPosition);

	monster1->SetHeight(0,0.0f);
	monster2->SetHeight(0,0.0f);
	monster2->Update();
	monster1->Update();
	player->Update();

	sky->GetPlayerPosition(playerPosition);
	sky->Update();

	if (sky->GetRealTime() == false)
	{
		static float theta = sky->GetTheta();
		ImGui::SliderFloat("Sky Time", &theta, -Math::PI, Math::PI);
		sky->SetTheta(theta);

		ImGui::Spacing(); ImGui::Separator();  ImGui::Spacing();
	}
}

void Test::PreRender()
{

}

void Test::Render()
{
	monster2->Render();
	monster1->Render();
	player->Render();
	player->EffectRender();

	sky->Pass(10);
	sky->Render();
}
