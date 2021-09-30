#include "stdafx.h"
#include "AnimationDemo.h"

void AnimationDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(19, 0, 0);
	Context::Get()->GetCamera()->Position(1, 7, -19);


	shader = new Shader(L"21_Render.fxo");
	
	//Kachujin();
	Ssalissali();
}

void AnimationDemo::Update()
{
	/*static UINT instance = 20;
	static UINT clip = 0;
	static float speed = 1.0f;
	static float tweenTime = 1.0f;
	
	static bool blendMode = false;
	static float alpha = 0.0f;

	
	ImGui::Checkbox("BlendMode", &blendMode);
	if (blendMode == false)
	{
		ImGui::InputInt("Instance", (int *)&instance);
		ImGui::InputInt("Clip", (int *)&clip);
		clip %= 4;

		ImGui::InputFloat("Speed", &speed, 0.01f);
		ImGui::InputFloat("TweenTime", &tweenTime, 0.01f);

		if (ImGui::Button("Apply"))
			kachujin->PlayTweenMode(instance, clip, speed, tweenTime);
	}
	else
	{
		ImGui::InputInt("Instance", (int *)&instance);
		ImGui::SliderFloat("Alpha", &alpha, 0.0f, 2.0f);

		kachujin->SetBlendAlpha(instance, alpha);

		if (ImGui::Button("Apply"))
			kachujin->PlayBlendMode(instance, 0, 1, 2);
	}	

	if (kachujin != NULL) 
		kachujin->Update();*/
	ssalissali->Update();
}

void AnimationDemo::Render()
{
	/*if (kachujin != NULL)
		kachujin->Render();*/
	ssalissali->Render();
}

void AnimationDemo::Kachujin()
{
	kachujin = new ModelAnimator(shader);
	kachujin->ReadMesh(L"Kachujin/Mesh");
	kachujin->ReadMaterial(L"Kachujin/Mesh");
	kachujin->ReadClip(L"Kachujin/Sword And Shield Idle");
	kachujin->ReadClip(L"Kachujin/Sword And Shield Walk");
	kachujin->ReadClip(L"Kachujin/Sword And Shield Run");
	kachujin->ReadClip(L"Kachujin/Sword And Shield Slash");
	kachujin->ReadClip(L"Kachujin/Salsa Dancing");


	Model* weapon = new Model();
	weapon->ReadMesh(L"Weapon/Sword");
	weapon->ReadMaterial(L"Weapon/Sword");

	Transform transform;
	transform.Position(20.0f, -10.0f, -10.5f);
	transform.Scale(2.75f, 1.65f, 1.25f);

	kachujin->Pass(2);

	
	for (float x = -50; x <= 50; x += 2.5f)
	{
		Transform* transform = kachujin->AddTransform();
		transform->Position(x, 0, -5);
		transform->Scale(0.01f, 0.01f, 0.01f);
	}
	kachujin->UpdateTransforms();
}

void AnimationDemo::Ssalissali()
{
	ssalissali = new ModelAnimator(shader);
	ssalissali->ReadMesh(L"Ssalissali/Ssalissali");
	ssalissali->ReadMaterial(L"Ssalissali/Ssalissali");

	ssalissali->ReadClip(L"Ssalissali/clip/Atk01");
	ssalissali->ReadClip(L"Ssalissali/clip/Death");
	ssalissali->ReadClip(L"Ssalissali/clip/DownEnd");
	ssalissali->ReadClip(L"Ssalissali/clip/DownLoop");
	ssalissali->ReadClip(L"Ssalissali/clip/DownStart");
	ssalissali->ReadClip(L"Ssalissali/clip/Idle");
	ssalissali->ReadClip(L"Ssalissali/clip/Run");
	ssalissali->ReadClip(L"Ssalissali/clip/unarmedwait");
	ssalissali->ReadClip(L"Ssalissali/clip/Wait");
	ssalissali->ReadClip(L"Ssalissali/clip/Walk");

	for (float x = -50; x < 50; x += 10.0f)
	{
		Transform* transform = ssalissali->AddTransform();
		transform->Position(x, 0, -5);
		//transform->Scale(0.01f, 0.01f, 0.01f);
	}
	ssalissali->Pass(3);

	for (int i = 0; i < 10; i++)
	{
		ssalissali->PlayTweenMode(i, i);
	}
	ssalissali->UpdateTransforms();
}
