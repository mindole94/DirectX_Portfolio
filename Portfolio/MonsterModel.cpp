#include "stdafx.h"
#include "MonsterModel.h"

MonsterModel::MonsterModel(Shader* shader, wstring monsterName, int num)
	: shader(shader)
{
	count = Math::Clamp(num, 0, MAX_MONSTER);
	name = monsterName;
	CreateMonster(monsterName, count);

	life = new bool[count];
	activeAttBox = new bool[count];
	activeHitBox = new bool[count];

	for (int i = 0; i < count; i++)
	{
		life[i] = true;
		activeAttBox[i] = false;
		activeHitBox[i] = true;
	}
}

MonsterModel::~MonsterModel()
{
	SafeDeleteArray(activeAttBox);
	SafeDeleteArray(activeHitBox);
	SafeDeleteArray(life);
	SafeDeleteArray(attBox);
	SafeDeleteArray(hitBox);
	SafeDelete(monster);
}

void MonsterModel::Update()
{
	if (name == L"Ssalissali")
	{
		for (int i = 0; i < count; i++)
		{
			if (life[i] == false) continue;

			Vector3 pos, rot;
			monster->GetTransform(i)->Position(&pos);
			pos.y += 3.0f;
			monster->GetTransform(i)->Rotation(&rot);

			hitBox[i]->Collider->GetTransform()->Position(pos);
			hitBox[i]->Collider->GetTransform()->Rotation(rot);
			hitBox[i]->Collider->Update();

			/////////////////////////////////////////////////////////////////////////////

			Matrix mat = monster->GetAttachTransform(i, 18);

			attBox[i]->Collider->GetTransform()->World(mat);
			attBox[i]->Collider->Update();
		}
	}
	else if (name == L"HealSpirit")
	{
		for (int i = 0; i < count; i++)
		{
			if (life[i] == false) continue;

			Vector3 pos, rot;
			monster->GetTransform(i)->Position(&pos);
			pos.y += 8.0f;
			monster->GetTransform(i)->Rotation(&rot);

			hitBox[i]->Collider->GetTransform()->Position(pos);
			hitBox[i]->Collider->GetTransform()->Rotation(rot);
			hitBox[i]->Collider->Update();

			/////////////////////////////////////////////////////////////////////////////

			attBox[i]->Collider->Update();
		}
	}

	monster->UpdateTransforms();
	monster->Update();
}

void MonsterModel::PreRender()
{
	monster->Pass(2);
	monster->Render();
}

void MonsterModel::PreRender_Reflection()
{
	monster->Pass(16);
	monster->Render();
}

void MonsterModel::Render()
{
	monster->Pass(6);
	monster->Render();
}

void MonsterModel::BoxRender()
{
	for (int i = 0; i < count; i++)
	{
		if (life[i] == false) continue;

		hitBox[i]->Collider->Render();
		attBox[i]->Collider->Render();
	}
}

void MonsterModel::CreateMonster(wstring monsterName, int num)
{
	monster = new ModelAnimator(shader);
	monster->ReadMesh(monsterName + L"/" + monsterName);
	monster->ReadMaterial(monsterName + L"/" + monsterName);

	monster->ReadClip(monsterName + L"/clip/Idle");//0

	monster->ReadClip(monsterName + L"/clip/Run");//1
	monster->ReadClip(monsterName + L"/clip/Atk01");//2
	monster->ReadClip(monsterName + L"/clip/Wait");//3

	monster->ReadClip(monsterName + L"/clip/Death");//4

	for (int instance = 0; instance < num; instance++)
	{
		Transform* transform = NULL;
		transform = monster->AddTransform();

		float ran = 250.0f;
		Vector3 posi = Math::RandomVec3(-ran, ran);

		while (CheckRestrictArea(posi) == true)
			posi = Math::RandomVec3(-ran, ran);

		if(monsterName == L"Ssalissali")
			posi.y = 0.0f;
		else
			posi.y = -4.0f;

		transform->Position(posi);
		//transform->Scale(0.2f, 0.2f, 0.2f);
		transform->Scale(0.3f, 0.3f, 0.3f);

		float rot = Math::Random(0.0f, 180.0f);
		transform->RotationDegree(0.0f, rot, 0.0f);
	}

	monster->UpdateTransforms();

	CreateColliders(num);
}

void MonsterModel::CreateColliders(int num)
{
	hitBox = new ColliderObject*[num];
	attBox = new ColliderObject*[num];

	if (name == L"Ssalissali")
	{
		for (int i = 0; i < num; i++)
		{
			hitBox[i] = new ColliderObject();

			hitBox[i]->Init = new Transform();
			hitBox[i]->Init->Scale(3, 5, 3);

			hitBox[i]->Transform = new Transform();
			hitBox[i]->Collider = new Collider(hitBox[i]->Transform, hitBox[i]->Init);

			/////////////////////////////////////////////////////////////////////////////

			attBox[i] = new ColliderObject();

			attBox[i]->Init = new Transform();
			attBox[i]->Init->Scale(8, 8, 8);

			attBox[i]->Transform = new Transform();
			attBox[i]->Collider = new Collider(attBox[i]->Transform, attBox[i]->Init);
		}
	}
	else if (name == L"HealSpirit")
	{
		for (int i = 0; i < num; i++)
		{
			hitBox[i] = new ColliderObject();

			hitBox[i]->Init = new Transform();
			hitBox[i]->Init->Scale(3, 3, 3);

			hitBox[i]->Transform = new Transform();
			hitBox[i]->Collider = new Collider(hitBox[i]->Transform, hitBox[i]->Init);

			/////////////////////////////////////////////////////////////////////////////

			attBox[i] = new ColliderObject();

			attBox[i]->Init = new Transform();
			attBox[i]->Init->Scale(2.5f, 2.5f, 2.5f);

			attBox[i]->Transform = new Transform();
			attBox[i]->Collider = new Collider(attBox[i]->Transform, attBox[i]->Init);
		}
	}
}

//@ brief : 제한구역안에 생성되었는가 체크
//@ return value : true -> 불가지역에 있음
bool MonsterModel::CheckRestrictArea(Vector3 position)
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

void MonsterModel::Position(int instance, float x, float y, float z)
{
	monster->GetTransform(instance)->Position(x, y, z);
}

void MonsterModel::Position(int instance, Vector3 & vec)
{
	monster->GetTransform(instance)->Position(vec);
}

void MonsterModel::Position(int instance, Vector3 * vec)
{
	monster->GetTransform(instance)->Position(vec);
}

void MonsterModel::Scale(int instance, float x, float y, float z)
{
	monster->GetTransform(instance)->Scale(x, y, z);
}

void MonsterModel::Scale(int instance, Vector3 & vec)
{
	monster->GetTransform(instance)->Scale(vec);
}

void MonsterModel::Scale(int instance, Vector3 * vec)
{
	monster->GetTransform(instance)->Scale(vec);
}

void MonsterModel::Rotation(int instance, float x, float y, float z)
{
	monster->GetTransform(instance)->Rotation(x, y, z);
}

void MonsterModel::Rotation(int instance, Vector3 & vec)
{
	monster->GetTransform(instance)->Rotation(vec);
}

void MonsterModel::Rotation(int instance, Vector3 * vec)
{
	monster->GetTransform(instance)->Rotation(vec);
}

void MonsterModel::RotationDegree(int instance, float x, float y, float z)
{
	monster->GetTransform(instance)->RotationDegree(x, y, z);
}

void MonsterModel::RotationDegree(int instance, Vector3 & vec)
{
	monster->GetTransform(instance)->RotationDegree(vec);

}

void MonsterModel::RotationDegree(int instance, Vector3 * vec)
{
	monster->GetTransform(instance)->RotationDegree(vec);
}

Vector3 MonsterModel::Forward(int instance)
{
	Vector3 vec = -monster->GetTransform(instance)->Forward();
	return vec;
}

Vector3 MonsterModel::Backward(int instance)
{
	Vector3 vec = monster->GetTransform(instance)->Forward();
	return vec;
}

Vector3 MonsterModel::Up(int instance)
{
	Vector3 vec = monster->GetTransform(instance)->Up();
	return vec;
}

Vector3 MonsterModel::Down(int instance)
{
	Vector3 vec = -monster->GetTransform(instance)->Up();
	return vec;
}

Vector3 MonsterModel::Right(int instance)
{
	Vector3 vec = monster->GetTransform(instance)->Right();
	return vec;
}

Vector3 MonsterModel::Left(int instance)
{
	Vector3 vec = -monster->GetTransform(instance)->Right();
	return vec;
}

void MonsterModel::World(int instance, Matrix & matrix)
{
	monster->GetTransform(instance)->World(matrix);
}

Matrix & MonsterModel::World(int instance)
{
	return monster->GetTransform(instance)->World();
}

void MonsterModel::Idle(int instance)
{
	monster->PlayTweenMode(instance, 0, 1.0f);
}

void MonsterModel::Run(int instance)
{
	monster->PlayTweenMode(instance, 1, 1.0f);
}

void MonsterModel::Attack(int instance)
{
	monster->ResetFrame(instance);
	monster->PlayNormalMode(instance, 2, 1.0f);
}

void MonsterModel::Wait(int instance)
{
	monster->ResetFrame(instance);
	monster->PlayNormalMode(instance, 3, 0.5f);
}
void MonsterModel::Death(int instance)
{
	monster->ResetFrame(instance);
	monster->PlayNormalMode(instance, 4, 1.0f);
}