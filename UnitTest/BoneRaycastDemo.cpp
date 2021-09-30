#include "stdafx.h"
#include "BoneRaycastDemo.h"

void BoneRaycastDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(20, 0, 0);
	Context::Get()->GetCamera()->Position(1, 36, -85);


	shader = new Shader(L"21_Render.fxo");
	sky = new CubeSky(L"Environment/GrassCube1024.dds");
	
	Mesh();
	Airplane();
	Kachujin();
	Weapon();

	BoneCollider();
}

void BoneRaycastDemo::Update()
{
	static float alpha = 0.0f;
	ImGui::SliderFloat("Alpha", &alpha, 0, 2);
	kachujin->SetBlendAlpha(3, alpha);

	sky->Update();

	cube->Update();
	grid->Update();
	cylinder->Update();
	sphere->Update();

	airplane->Update();
	kachujin->Update();


	static Vector3 position = Vector3(0, 0, 0);
	static Vector3 rotation = Vector3(0, 0, 0);
	static Vector3 scale = Vector3(1, -1, 1);

	ImGui::SliderFloat3("Weapon Position", position, -100, 100);
	ImGui::SliderFloat3("Weapon Rotation", rotation, -180, 180);
	ImGui::SliderFloat3("Weapon Scale", scale, -2, 2);

	weaponTransform.Position(position);
	weaponTransform.RotationDegree(rotation);
	weaponTransform.Scale(scale);


	for (UINT i = 0; i < 5; i++)
	{
		Matrix transform = kachujin->GetAttachTransform(i, 40);
		Transform* tempTrans = weapon->GetTransform(i);

		Matrix transform2 = weaponTransform.World();
		tempTrans->World(transform * transform2);
	}
	weapon->UpdateTransforms();
	weapon->Update();

	for (UINT i = 0; i < kachujin->GetTransformCount(); i++)
	{
		Matrix attach = kachujin->GetAttachTransform(i, 40);

		colliders[i]->Collider->GetTransform()->World(attach);
		colliders[i]->Collider->Update();
	}
	
	CheckIntersection();
}

void BoneRaycastDemo::Render()
{
	sky->Render();

	Pass(0, 1, 2);

	wall->Render();
	sphere->Render();

	brick->Render();
	cylinder->Render();

	stone->Render();
	cube->Render();

	floor->Render();
	grid->Render();

	airplane->Render();
	kachujin->Render();
	weapon->Render();

	for (UINT i = 0; i < kachujin->GetTransformCount(); i++)
	{
		if(i == collision)
			colliders[i]->Collider->Render(Color(1, 0, 0, 1));
		else
			colliders[i]->Collider->Render();
	}
		
}

void BoneRaycastDemo::Mesh()
{
	//Create Material
	{
		floor = new Material(shader);
		floor->DiffuseMap("Floor.png");
		//floor->SpecularMap("Floor_Specular.png");
		//floor->NormalMap("Floor_Normal.png");
		//floor->Specular(1, 1, 1, 20);

		stone = new Material(shader);
		stone->DiffuseMap("Stones.png");
		//stone->SpecularMap("Stones_Specular.png");
		//stone->NormalMap("Stones_Normal.png");
		//stone->Specular(1, 1, 1, 20);

		brick = new Material(shader);
		brick->DiffuseMap("Bricks.png");
		//brick->SpecularMap("Bricks_Specular.png");
		//brick->NormalMap("Bricks_Normal.png");
		//brick->Specular(1, 0.3f, 0.3f, 20);

		wall = new Material(shader);
		wall->DiffuseMap("Wall.png");
		//wall->SpecularMap("Wall_Specular.png");
		//wall->NormalMap("Wall_Normal.png");
		//wall->Specular(1, 1, 1, 20);
	}


	//Create Mesh
	{
		Transform* transform = NULL;

		cube = new MeshRender(shader, new MeshCube());
		transform = cube->AddTransform();
		transform->Position(0, 5, 0);
		transform->Scale(20, 10, 20);

		grid = new MeshRender(shader, new MeshGrid(5, 5));
		transform = grid->AddTransform();
		transform->Position(0, 0, 0);
		transform->Scale(12, 1, 12);

		cylinder = new MeshRender(shader, new MeshCylinder(0.5f, 3.0f, 20, 20));
		sphere = new MeshRender(shader, new MeshSphere(0.5f, 20, 20));
		for (UINT i = 0; i < 5; i++)
		{
			transform = cylinder->AddTransform();
			transform->Position(-30, 6, -15.0f + (float)i * 15.0f);
			transform->Scale(5, 5, 5);

			transform = cylinder->AddTransform();
			transform->Position(30, 6, -15.0f + (float)i * 15.0f);
			transform->Scale(5, 5, 5);


			transform = sphere->AddTransform();
			transform->Position(-30, 15.5f, -15.0f + (float)i * 15.0f);
			transform->Scale(5, 5, 5);

			transform = sphere->AddTransform();
			transform->Position(30, 15.5f, -15.0f + (float)i * 15.0f);
			transform->Scale(5, 5, 5);
		}
	}

	sphere->UpdateTransforms();
	cylinder->UpdateTransforms();
	cube->UpdateTransforms();
	grid->UpdateTransforms();

	meshes.push_back(sphere);
	meshes.push_back(cylinder);
	meshes.push_back(cube);
	meshes.push_back(grid);
}

void BoneRaycastDemo::Airplane()
{
	airplane = new ModelRender(shader);
	airplane->ReadMesh(L"B787/Airplane");
	airplane->ReadMaterial(L"B787/Airplane");

	Transform* transform = airplane->AddTransform();
	transform->Position(2.0f, 9.91f, 2.0f);
	transform->Scale(0.004f, 0.004f, 0.004f);
	airplane->UpdateTransforms();

	models.push_back(airplane);
}

void BoneRaycastDemo::Kachujin()
{
	kachujin = new ModelAnimator(shader);
	kachujin->ReadMesh(L"Kachujin/Mesh");
	kachujin->ReadMaterial(L"Kachujin/Mesh");
	kachujin->ReadClip(L"Kachujin/Sword And Shield Idle");
	kachujin->ReadClip(L"Kachujin/Sword And Shield Walk");
	kachujin->ReadClip(L"Kachujin/Sword And Shield Run");
	kachujin->ReadClip(L"Kachujin/Sword And Shield Slash");
	kachujin->ReadClip(L"Kachujin/Salsa Dancing");


	Transform* transform = NULL;

	transform = kachujin->AddTransform();
	transform->Position(0, 0, -30);
	transform->Scale(0.075f, 0.075f, 0.075f);
	kachujin->PlayTweenMode(0, 0, 1.0f);

	transform = kachujin->AddTransform();
	transform->Position(-15, 0, -30);
	transform->Scale(0.075f, 0.075f, 0.075f);
	kachujin->PlayTweenMode(1, 1, 1.0f);

	transform = kachujin->AddTransform();
	transform->Position(-30, 0, -30);
	transform->Scale(0.075f, 0.075f, 0.075f);
	kachujin->PlayTweenMode(2, 2, 0.75f);

	transform = kachujin->AddTransform();
	transform->Position(15, 0, -30);
	transform->Scale(0.075f, 0.075f, 0.075f);
	kachujin->PlayBlendMode(3, 0, 1, 2);
	kachujin->SetBlendAlpha(3, 1.5f);

	transform = kachujin->AddTransform();
	transform->Position(30, 0, -32.5f);
	transform->Scale(0.075f, 0.075f, 0.075f);
	kachujin->PlayTweenMode(4, 4, 0.75f);

	kachujin->UpdateTransforms();

	animators.push_back(kachujin);
}

void BoneRaycastDemo::Weapon()
{
	weapon = new ModelRender(shader);
	weapon->ReadMesh(L"Weapon/Sword");
	weapon->ReadMaterial(L"Weapon/Sword");

	for(UINT i = 0; i < 5; i++)
		weapon->AddTransform();

	models.push_back(airplane);
}

void BoneRaycastDemo::BoneCollider()
{
	UINT count = kachujin->GetTransformCount();
	colliders = new ColliderObject*[count];
	
	for (UINT i = 0; i < count; i++)
	{
		colliders[i] = new ColliderObject();

		colliders[i]->Init = new Transform();
		colliders[i]->Init->Position(0, 0, 0);
		colliders[i]->Init->Scale(10, 10, 10);

		colliders[i]->Transform = new Transform();
		colliders[i]->Collider = new Collider(colliders[i]->Transform, colliders[i]->Init);
	}
}

void BoneRaycastDemo::CheckIntersection()
{
	if (Mouse::Get()->Press(0) == false)
	{
		collision = -1;

		return;
	}		


	Matrix V = Context::Get()->View();
	Matrix P = Context::Get()->Projection();
	Viewport* Vp = Context::Get()->GetViewport();

	Vector3 mouse = Mouse::Get()->GetPosition();


	Matrix world;
	D3DXMatrixIdentity(&world);

	Vector3 n, f;
	mouse.z = 0.0f;
	Vp->Unproject(&n, mouse, world, V, P);

	mouse.z = 1.0f;
	Vp->Unproject(&f, mouse, world, V, P);

	Ray ray;
	ray.Position = n;
	ray.Direction = f - n;


	float distance = 0.0f;
	for (UINT i = 0; i < kachujin->GetTransformCount(); i++)
	{
		if (colliders[i]->Collider->Intersection(ray, &distance))
		{
			collision = i;

			break;
		}
	}
}

void BoneRaycastDemo::Pass(UINT mesh, UINT model, UINT anim)
{
	for (MeshRender* temp : meshes)
		temp->Pass(mesh);

	for (ModelRender* temp : models)
		temp->Pass(model);

	for (ModelAnimator* temp : animators)
		temp->Pass(anim);
}