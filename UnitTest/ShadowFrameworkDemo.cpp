#include "stdafx.h"
#include "ShadowFrameworkDemo.h"

void ShadowFrameworkDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(21, 0, 0);
	Context::Get()->GetCamera()->Position(126, 35, 39);


	shader = new Shader(L"48_Shadow.fxo");
	shadow = new Shadow(shader, Vector3(128, 0, 128), 65);


	sky = new CubeSky(L"Environment/GrassCube1024.dds", shader);

	terrain = new Terrain(shader, L"Terrain/Gray256.png");
	terrain->BaseMap(L"Terrain/Dirt3.png");

	Mesh();
	Airplane();

	Sapling();
	Weapon();

	PointLights();
	SpotLights();

	textureShader = new Shader(L"24_Texture.fxo");
	HP();
}

void ShadowFrameworkDemo::Destroy()
{

}

void ShadowFrameworkDemo::Update()
{
	sky->Update();

	cube->Update();
	cylinder->Update();
	sphere->Update();

	airplane->Update();
	sapling->Update();


	Matrix worlds[MAX_MODEL_TRANSFORMS];
	for (UINT i = 0; i < sapling->GetTransformCount(); i++)
	{
		sapling->GetAttachTransforms(i, worlds);
		weapon->GetTransform(i)->World(weaponTransform->World() * worlds[40]);
	}
	weapon->UpdateTransforms();
	weapon->Update();

	terrain->Update();

	float ratio = 1.0f;
	hpBar->ChangeColorPoint(ratio);
	hpBar->Update();
}

void ShadowFrameworkDemo::PreRender()
{
	shadow->PreRender();

	Pass(0, 1, 2);
	wall->Render();
	sphere->Render();

	brick->Render();
	cylinder->Render();

	stone->Render();
	cube->Render();

	airplane->Render();
	sapling->Render();
	weapon->Render();

	terrain->Pass(3);
	terrain->Render();
}

void ShadowFrameworkDemo::Render()
{

	sky->Pass(4);
	sky->Render();

	hpBar->Render();
	Pass(5, 6, 7);
	wall->Render();
	sphere->Render();

	brick->Render();
	cylinder->Render();

	stone->Render();
	cube->Render();

	airplane->Render();
	sapling->Render();
	weapon->Render();

	terrain->Pass(8);
	terrain->Render();
}

void ShadowFrameworkDemo::PostRender()
{

}

void ShadowFrameworkDemo::Mesh()
{
	//Create Material
	{
		stone = new Material(shader);
		stone->DiffuseMap("Stones.png");
		stone->NormalMap("Stones_Normal.png");
		stone->Specular(1, 1, 1, 20);
		stone->SpecularMap("Stones_Specular.png");
		stone->Emissive(0.15f, 0.15f, 0.15f, 0.3f);

		brick = new Material(shader);
		brick->DiffuseMap("Bricks.png");
		brick->NormalMap("Bricks_Normal.png");
		brick->Specular(1, 0.3f, 0.3f, 20);
		brick->SpecularMap("Bricks_Specular.png");
		brick->Emissive(0.15f, 0.15f, 0.15f, 0.3f);

		wall = new Material(shader);
		wall->DiffuseMap("Wall.png");
		wall->NormalMap("Wall_Normal.png");
		wall->Specular(1, 1, 1, 20);
		wall->SpecularMap("Wall_Specular.png");
		wall->Emissive(0.15f, 0.15f, 0.15f, 0.3f);
	}


	//Create Mesh
	{
		Transform* transform = NULL;

		cube = new MeshRender(shader, new MeshCube());
		transform = cube->AddTransform();
		transform->Position(128, 0, 128);
		transform->Scale(20, 10, 20);
		SetTransform(transform);

		cylinder = new MeshRender(shader, new MeshCylinder(0.5f, 3.0f, 20, 20));
		sphere = new MeshRender(shader, new MeshSphere(0.5f, 20, 20));
		for (UINT i = 0; i < 5; i++)
		{
			transform = cylinder->AddTransform();
			transform->Position(128 - 30, 6, 128 - 15.0f + (float)i * 15.0f);
			transform->Scale(5, 5, 5);
			SetTransform(transform);

			transform = cylinder->AddTransform();
			transform->Position(128 + 30, 6, 128 - 15.0f + (float)i * 15.0f);
			transform->Scale(5, 5, 5);
			SetTransform(transform);


			transform = sphere->AddTransform();
			transform->Position(128 - 30, 15.5f, 128 - 15.0f + (float)i * 15.0f);
			transform->Scale(5, 5, 5);
			SetTransform(transform);

			transform = sphere->AddTransform();
			transform->Position(128 + 30, 15.5f, 128 - 15.0f + (float)i * 15.0f);
			transform->Scale(5, 5, 5);
			SetTransform(transform);
		}
	}

	sphere->UpdateTransforms();
	cylinder->UpdateTransforms();
	cube->UpdateTransforms();

	meshes.push_back(sphere);
	meshes.push_back(cylinder);
	meshes.push_back(cube);
}

void ShadowFrameworkDemo::Airplane()
{
	airplane = new ModelRender(shader);
	airplane->ReadMesh(L"B787/Airplane");
	airplane->ReadMaterial(L"B787/Airplane");

	Transform* transform = airplane->AddTransform();
	transform->Position(128 + 2.0f, 9.91f, 128 + 2.0f);
	transform->Scale(0.004f, 0.004f, 0.004f);
	SetTransform(transform);
	airplane->UpdateTransforms();

	models.push_back(airplane);
}

void ShadowFrameworkDemo::Sapling()
{
	sapling = new ModelAnimator(shader);
	sapling->ReadMesh(L"Sapling/Sapling");
	sapling->ReadMaterial(L"Sapling/Sapling");
	sapling->ReadClip(L"Sapling/clip/Chicken Dance");
	sapling->ReadClip(L"Sapling/clip/Standing Melee Attack 360");
	sapling->ReadClip(L"Sapling/clip/Standing Melee Attack Backhand");
	sapling->ReadClip(L"Sapling/clip/Standing Melee Attack Downward");
	sapling->ReadClip(L"Sapling/clip/Standing Idle");


	Transform* transform = NULL;

	transform = sapling->AddTransform();
	transform->Position(128 + 0, 0, 128 - 30);
	transform->Scale(0.075f, 0.075f, 0.075f);
	SetTransform(transform);
	sapling->PlayTweenMode(0, 0, 1.0f);

	transform = sapling->AddTransform();
	transform->Position(128 - 15, 0, 128 - 30);
	transform->Scale(0.075f, 0.075f, 0.075f);
	SetTransform(transform);
	sapling->PlayTweenMode(1, 1, 1.0f);

	transform = sapling->AddTransform();
	transform->Position(128 - 30, 0, 128 - 30);
	transform->Scale(0.075f, 0.075f, 0.075f);
	SetTransform(transform);
	sapling->PlayTweenMode(2, 2, 0.75f);

	transform = sapling->AddTransform();
	transform->Position(128 + 15, 0, 128 - 30);
	transform->Scale(0.075f, 0.075f, 0.075f);
	SetTransform(transform);
	sapling->PlayBlendMode(3, 0, 1, 2);
	sapling->SetBlendAlpha(3, 1.5f);

	transform = sapling->AddTransform();
	transform->Position(128 + 30, 0, 128 - 32.5f);
	transform->Scale(0.075f, 0.075f, 0.075f);
	SetTransform(transform);
	sapling->PlayTweenMode(4, 4, 0.75f);

	sapling->UpdateTransforms();

	animators.push_back(sapling);
}

void ShadowFrameworkDemo::Weapon()
{
	weapon = new ModelRender(shader);
	weapon->ReadMesh(L"Weapon/Sword");
	weapon->ReadMaterial(L"Weapon/Sword");

	UINT count = sapling->GetTransformCount();
	for (UINT i = 0; i < count; i++)
		weapon->AddTransform();

	weapon->UpdateTransforms();
	models.push_back(weapon);

	weaponTransform = new Transform();
	weaponTransform->Position(-2.9f, 1.45f, -6.45f);
	weaponTransform->Scale(0.5f, 0.5f, 0.75f);
	weaponTransform->Rotation(0, 0, 1);
}

void ShadowFrameworkDemo::PointLights()
{
	PointLight light;
	light =
	{
		Color(0.0f, 0.0f, 0.0f, 1.0f), //Ambient
		Color(0.0f, 0.0f, 1.0f, 1.0f), //Diffuse
		Color(0.0f, 0.0f, 0.7f, 1.0f), //Specular
		Color(0.0f, 0.0f, 0.7f, 1.0f), //Emissive
		Vector3(128 - 30, 10, 128 - 30), 5.0f, 0.9f
	};
	Lighting::Get()->AddPointLight(light);

	light =
	{
		Color(0.0f, 0.0f, 0.0f, 1.0f),
		Color(1.0f, 0.0f, 0.0f, 1.0f),
		Color(0.6f, 0.2f, 0.0f, 1.0f),
		Color(0.6f, 0.3f, 0.0f, 1.0f),
		Vector3(128 + 15, 10, 128 - 30), 10.0f, 0.3f
	};
	Lighting::Get()->AddPointLight(light);


}

void ShadowFrameworkDemo::SpotLights()
{
	SpotLight light;
	light =
	{
		Color(0.3f, 1.0f, 0.0f, 1.0f),
		Color(0.7f, 1.0f, 0.0f, 1.0f),
		Color(0.3f, 1.0f, 0.0f, 1.0f),
		Color(0.3f, 1.0f, 0.0f, 1.0f),
		Vector3(128 - 15, 20, 128 - 30), 25.0f,
		Vector3(0, -1, 0), 30.0f, 0.9f
	};
	Lighting::Get()->AddSpotLight(light);

	light =
	{
		Color(1.0f, 0.2f, 0.9f, 1.0f),
		Color(1.0f, 0.2f, 0.9f, 1.0f),
		Color(1.0f, 0.2f, 0.9f, 1.0f),
		Color(1.0f, 0.2f, 0.9f, 1.0f),
		Vector3(128 + 0, 20, 128 - 30), 30.0f,
		Vector3(0, -1, 0), 40.0f, 0.9f
	};
	Lighting::Get()->AddSpotLight(light);
}

void ShadowFrameworkDemo::HP()
{
	//HP Bar
	hpTexture = new Texture(L"Red.png");

	hpBar = new Render2D();
	hpBar->GetTransform()->Scale(208.0f, 13.0f, 1);
	hpBar->GetTransform()->Position(490.0f, 167.0f, 0);
	hpBar->Pass(1);

	hpBuffer = new TextureBuffer(hpTexture->GetTexture());

	textureShader->AsSRV("Input")->SetResource(hpBuffer->SRV());
	textureShader->AsUAV("Output")->SetUnorderedAccessView(hpBuffer->UAV());


	UINT width = hpBuffer->Width();
	UINT height = hpBuffer->Height();
	UINT arraySize = hpBuffer->ArraySize();

	float x = ((float)width / 32.0f) < 1.0f ? 1.0f : ((float)width / 32.0f);
	float y = ((float)height / 32.0f) < 1.0f ? 1.0f : ((float)height / 32.0f);

	textureShader->Dispatch(0, 0, (UINT)ceilf(x), (UINT)ceilf(y), arraySize);

	hpBar->SRV(hpBuffer->OutputSRV());
}

void ShadowFrameworkDemo::Pass(UINT mesh, UINT model, UINT anim)
{
	for (MeshRender* temp : meshes)
		temp->Pass(mesh);

	for (ModelRender* temp : models)
		temp->Pass(model);

	for (ModelAnimator* temp : animators)
		temp->Pass(anim);
}

void ShadowFrameworkDemo::SetTransform(Transform * transform)
{
	Vector3 position;
	transform->Position(&position);

	Vector3 scale;
	transform->Scale(&scale);

	float y = terrain->GetVerticalRaycast(position);
	position.y += y + scale.y * 0.5f;
	transform->Position(position);
}

