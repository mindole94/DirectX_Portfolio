#include "stdafx.h"
#include "Monster2.h"

Monster2::Monster2(Shader* shader)
	: shader(shader)
{
	Context::Get()->GetCamera()->RotationDegree(20, -180, 0);
	Context::Get()->GetCamera()->Position(0, 550, 715);

	shader = new Shader(L"21_Render.fxo");

	CreateMonster();

	for (int index = 0; index < MONSTER2_NUM; index++)
	{
		monsterInfo[index].position = monster->GetTransform(index)->GetPosition();
	}

}

Monster2::~Monster2()
{
	SafeDelete(monster);
	for (int index = 0; index < MONSTER2_NUM; index++)
	{
		SafeDelete(monsterInfo[index].CollidersAtt);
		SafeDelete(monsterInfo[index].CollidersHit);
		SafeDelete(monsterInfo[index].DeathParticle);
	}
}

void Monster2::Update()
{
	for (int index = 0; index < MONSTER2_NUM; index++)
	{
		if (monsterInfo[index].hp > 0)
		{
			monsterInfo[index].position = monster->GetTransform(index)->GetPosition();
			monsterInfo[index].position.y = monsterInfo[index].height;

			distance = D3DXVec3Length(&(playerPosition - monsterInfo[index].position));

			//탐색
			if (distance < (monsterInfo[index].search == true ? 100.0f : 30.0f))
				Follow(index);
			else
				Patrol(index);

			//TODO : 지워라
			monsterInfo[index].position.x = Math::Clamp(monsterInfo[index].position.x, 0.0f, 248.0f);
			monsterInfo[index].position.z = Math::Clamp(monsterInfo[index].position.z, 0.0f, 248.0f);

			monster->GetTransform(index)->Position(monsterInfo[index].position);


			if (monsterInfo[index].activeColliderHit == false)
			{
				monsterInfo[index].invincibleTime -= Time::Delta();

				if (monsterInfo[index].invincibleTime < 0.0f)
					monsterInfo[index].activeColliderHit = true;
			}
		}
		else
		{
			if (monsterInfo[index].deathTrigger == false)
			{
				Reset(index);

				monsterInfo[index].deathTrigger = true;

				SetDeath(index);
			}

			if (monsterInfo[index].duration > 10.0f) continue;

			int frameCount = monster->GetModel()->ClipByIndex(8)->FrameCount();
			int currFrame = monster->GetCurrentFrameTweenMode(index);

			if (currFrame == frameCount - 1)
			{
				monster->SetPlayFrame(index, false);
				monster->SetPauseFrame(index, true);

				monsterInfo[index].duration += Time::Get()->Delta();

				if (monsterInfo[index].duration > 1.0f)
				{
					float size = monsterInfo[index].scale.x;
					size -= Time::Delta() * 0.05f;
					monsterInfo[index].scale.x = size;

					monster->GetTransform(index)->Scale(size, size, size);

					monsterInfo[index].DeathParticle->Add(monsterInfo[index].position);
					monsterInfo[index].DeathParticle->Update();
				}
			}
		}

		monsterInfo[index].debuffTime -= Time::Delta();
		monsterInfo[index].debuffTime = Math::Clamp(monsterInfo[index].debuffTime, 0.0f, 100.0f);
	}

	monster->UpdateTransforms();
	monster->Update();
	CollidersUpdate();
	ParticleUpdate();
}

void Monster2::PreRender()
{
	monster->Pass(2);
	monster->Render();
}

void Monster2::Render()
{
	monster->Pass(10);
	monster->Render();
	CollidersRender();
	ParticleRender();
	//ImageRender();

	//죽으면 렌더링 안함
	for (int index = 0; index < MONSTER2_NUM; index++)
	{
		if (monsterInfo[index].hp > 0) continue;
		if (monsterInfo[index].duration > 10.0f) continue;

		if (monsterInfo[index].scale.x < 0.0f)
			monster->SetModelRender(index, false);

		monsterInfo[index].DeathParticle->Render();
	}
}

void Monster2::Damage(int index, int damageType, int damage, bool fireball)
{
	if (damageType == 0)
	{
		monsterInfo[index].isDown = true;
	}
	else if (damageType == 2)
	{
		monsterInfo[index].def -= 100;
		monsterInfo[index].debuffTime = 15.0f;
	}

	monsterInfo[index].hp -= damage - monsterInfo[index].def;
	monsterInfo[index].activeColliderHit = false;

	if (fireball == true)
		monsterInfo[index].invincibleTime = 0.5f;
	else
		monsterInfo[index].invincibleTime = 0.3f;
}

void Monster2::CreateMonster()
{
	monster = new ModelAnimator(shader);
	monster->ReadMesh(L"HealSpirit/HealSpirit");
	monster->ReadMaterial(L"HealSpirit/HealSpirit");

	monster->ReadClip(L"HealSpirit/clip/Idle");//0
	monster->ReadClip(L"HealSpirit/clip/run");//1

	monster->ReadClip(L"HealSpirit/clip/run");//2
	monster->ReadClip(L"HealSpirit/clip/atk01");//3
	monster->ReadClip(L"HealSpirit/clip/Wait");//4

	monster->ReadClip(L"HealSpirit/clip/DownStart");//5
	monster->ReadClip(L"HealSpirit/clip/DownLoop");//6
	monster->ReadClip(L"HealSpirit/clip/DownEnd");//7

	monster->ReadClip(L"HealSpirit/clip/Death");//8



	for (int index = 0; index < MONSTER2_NUM; index++)
	{
		Transform* transform = NULL;
		transform = monster->AddTransform();

		Vector3 posi = Math::RandomVec3(0.0f, 240.0f);
		
		transform->Position(posi);

		transform->Scale(0.2f, 0.2f, 0.2f);

		float rot = Math::Random(0.0f, 180.0f);
		transform->RotationDegree(0.0f, rot, 0.0f);

		float patX = Math::Random(posi.x - 30.0f, posi.x + 30.0f);
		float patZ = Math::Random(posi.z - 30.0f, posi.z + 30.0f);
		monsterInfo[index].patrolPosition = Vector3(patX, 0.0f, patZ);
		monsterInfo[index].scale = transform->GetScale();

		monsterInfo[index].DeathParticle = new ParticleSystem(L"StarUp");
		monsterInfo[index].GalaxyParticle = new ParticleSystem(L"Galaxy");

		PointLight light;
		light =
		{
		Color(1.0f, 0.0f, 0.0f, 1.0f), //Ambient
		Color(1.0f, 0.0f, 0.0f, 1.0f), //Diffuse
		Color(1.0f, 0.0f, 0.0f, 1.0f), //Specular
		Color(1.0f, 0.0f, 0.0f, 1.0f), //Emissive
		Vector3(patX, 10, patZ), 0.0f, 1.0f
		};
		Lighting::Get()->AddPointLight(light);
	}

	monster->UpdateTransforms();
	//monster->Pass(3);

	CreateColliders();
}

void Monster2::Patrol(int index)
{
	monsterInfo[index].search = false;

	Vector3 mPosi = monsterInfo[index].position;
	mPosi.y = 0.0f;

	float dis = D3DXVec3Length(&(monsterInfo[index].patrolPosition - mPosi));

	if (dis < 3.0f)
	{
		monsterInfo[index].duration += Time::Delta();

		if (monsterInfo[index].idleTrigger == false)
		{
			monsterInfo[index].idleTrigger = true;

			SetIdle(index);
		}

		if (monsterInfo[index].duration > 10.0f)
		{
			float patX = Math::Random(mPosi.x - 30.0f, mPosi.x + 30.0f);
			float patZ = Math::Random(mPosi.z - 30.0f, mPosi.z + 30.0f);
			monsterInfo[index].patrolPosition = Vector3(patX, 0.0f, patZ);

			Reset(index);
		}
	}
	else
	{
		if (monsterInfo[index].walkTrigger == false)
		{
			monsterInfo[index].walkTrigger = true;

			SetWalk(index);
		}

		//이동
		Vector3 dest = monsterInfo[index].patrolPosition - mPosi;
		D3DXVec3Normalize(&dest, &dest);

		monsterInfo[index].position += dest * 3.0f * Time::Delta();

		//회전
		float dot = D3DXVec3Dot(&monster->GetTransform(index)->Right(), &dest);
		Vector3 temp;
		D3DXVec3Cross(&temp, &monster->GetTransform(index)->Right(), &dest);

		float sign = temp.y >= 0.0f ? 1.0f : -1.0f;
		float theta = sign * acosf(dot);

		Vector3 rot;
		monster->GetTransform(index)->Rotation(&rot);
		rot.y += theta / 30;

		monster->GetTransform(index)->Rotation(rot);
	}
}

void Monster2::Follow(int index)
{
	monsterInfo[index].search = true;

	if (monsterInfo[index].isDown == true)//다운
	{
		//다운 step1
		if (monsterInfo[index].downStartTrigger == false)
		{
			monsterInfo[index].downStartTrigger = true;
			monsterInfo[index].duration = 0.0f;

			SetDownStart(index);
		}

		//다운 step2
		if (monster->GetCurrentClipIndex(index) == 5 && monsterInfo[index].downLoopTrigger == false)
		{
			int frameCount = monster->GetModel()->ClipByIndex(5)->FrameCount();
			int currFrame = monster->GetCurrentFrameTweenMode(index);

			if (currFrame == frameCount - 1)
			{
				if (monsterInfo[index].downLoopTrigger == false)
				{
					monsterInfo[index].downLoopTrigger = true;

					SetDownLoop(index);
				}
			}
		}

		//다운 step3
		if (monster->GetCurrentClipIndex(index) == 6 && monsterInfo[index].downEndTrigger == false)
		{
			monsterInfo[index].duration += Time::Delta();

			if (monsterInfo[index].duration > 5.0f)
			{
				if (monsterInfo[index].downEndTrigger == false)
				{
					monsterInfo[index].downEndTrigger = true;

					SetDownEnd(index);
				}
			}
		}

		//다운 end
		if (monster->GetCurrentClipIndex(index) == 7)
		{
			int frameCount = monster->GetModel()->ClipByIndex(7)->FrameCount();
			int currFrame = monster->GetCurrentFrameTweenMode(index);

			if (currFrame == frameCount - 1)
				Reset(index);
		}
	}
	else if(monsterInfo[index].bAttack == true)
	{
		PointLight& light = Lighting::Get()->GetPointLight(index);

		//공격
		if (monsterInfo[index].atkTrigger == false)
		{
			Reset(index);

			monsterInfo[index].atkTrigger = true;

			monsterInfo[index].playerPosition = playerPosition;
			monsterInfo[index].playerPosition.y += 5.0f;
			light.Position = monsterInfo[index].playerPosition;

			monsterInfo[index].galaxyPosition = playerPosition;
			monsterInfo[index].galaxyPosition.y += 90.0f;

			SetAttack(index);
		}

		if (monsterInfo[index].bAttack == true)
		{
			if (light.Range < 6.5f)
			{
				light.Range += 3.1f * Time::Delta();
			}
			else
			{
				monsterInfo[index].activeColliderAtt = true;
				monsterInfo[index].GalaxyParticle->Add(monsterInfo[index].galaxyPosition);
				monsterInfo[index].galaxyPosition.y -= 50.0f * Time::Delta();
			}
		}

		//공격후 대기
		if (monster->GetCurrentClipIndex(index) == 3 && monsterInfo[index].bAttack == true)
		{
			int frameCount = monster->GetModel()->ClipByIndex(3)->FrameCount();
			int currFrame = monster->GetCurrentFrameTweenMode(index);

			//공격 종료
			if (currFrame == frameCount - 1)
			{
				if (monsterInfo[index].waitTrigger == false)
				{
					monsterInfo[index].waitTrigger = true;

					SetWait(index);
				}
			}
		}

		//대기 종료
		if (monster->GetCurrentClipIndex(index) == 4)
		{
			if (monsterInfo[index].playerPosition.y - 14.0f > monsterInfo[index].galaxyPosition.y)
				Reset(index);
		}
	}
	else
	{
		//쫓아오기
		if (monsterInfo[index].runTrigger == false)
		{
			Reset(index);

			monsterInfo[index].runTrigger = true;

			SetRun(index);
		}


		Vector3 pPosi = playerPosition;
		pPosi.y = 0.0f;
		Vector3 mPosi = monsterInfo[index].position;
		mPosi.y = 0.0f;

		//이동
		Vector3 dest = pPosi - mPosi;
		D3DXVec3Normalize(&dest, &dest);

		monsterInfo[index].position += dest * 5.0f * Time::Delta();

		//회전
		float dot = D3DXVec3Dot(&monster->GetTransform(index)->Right(), &dest);
		Vector3 temp;
		D3DXVec3Cross(&temp, &monster->GetTransform(index)->Right(), &dest);

		float sign = temp.y >= 0.0f ? 1.0f : -1.0f;
		float theta = sign * acosf(dot);

		Vector3 rot;
		monster->GetTransform(index)->Rotation(&rot);
		rot.y += theta / 30;

		monster->GetTransform(index)->Rotation(rot);
	}

	if (distance < 10.0f) monsterInfo[index].bAttack = true;
}

void Monster2::SetIdle(int index)
{
	//std::cout << "Montser2 Idle" << std::endl;
	monster->PlayTweenMode(index, 0, 1.0f);
}

void Monster2::SetWalk(int index)
{
	//std::cout << "Montser2 Walk" << std::endl;
	monster->PlayTweenMode(index, 1, 1.0f);
}

void Monster2::SetRun(int index)
{
	//std::cout << "Montser2 Run" << std::endl;
	monster->PlayTweenMode(index, 2, 1.0f);
}

void Monster2::SetAttack(int index)
{
	//std::cout << "Montser2 Atk" << std::endl;
	monster->PlayNormalMode(index, 3, 1.0f);
}

void Monster2::SetWait(int index)
{
	//std::cout << "Montser2 Wait" << std::endl;
	monster->PlayNormalMode(index, 4, 0.5f);
}

void Monster2::SetDownStart(int index)
{
	//std::cout << "Montser2 DownStart" << std::endl;
	monster->PlayNormalMode(index, 5, 1.0f);
}

void Monster2::SetDownLoop(int index)
{
	//std::cout << "Montser2 DownLoop" << std::endl;
	monster->PlayNormalMode(index, 6, 1.0f);
}

void Monster2::SetDownEnd(int index)
{
	//std::cout << "Montser2 DownEnd" << std::endl;
	monster->PlayNormalMode(index, 7, 1.0f);
}

void Monster2::SetDeath(int index)
{
	//std::cout << "Montser2 Death" << std::endl;
	monster->PlayTweenMode(index, 8, 1.0f);
}

void Monster2::CreateColliders()
{
	for (int index = 0; index < MONSTER2_NUM; index++)
	{
		monsterInfo[index].CollidersHit = new ColliderObject();

		monsterInfo[index].CollidersHit->Init = new Transform();
		monsterInfo[index].CollidersHit->Init->Position(0.0f, 0.0f, 0.0f);
		monsterInfo[index].CollidersHit->Init->Scale(3, 3, 3);

		monsterInfo[index].CollidersHit->Transform = new Transform();
		monsterInfo[index].CollidersHit->Collider = new Collider(monsterInfo[index].CollidersHit->Transform, monsterInfo[index].CollidersHit->Init);

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////

		monsterInfo[index].CollidersAtt = new ColliderObject();

		monsterInfo[index].CollidersAtt->Init = new Transform();
		monsterInfo[index].CollidersAtt->Init->Position(0, 0, 0);
		monsterInfo[index].CollidersAtt->Init->Scale(3, 1, 3);

		monsterInfo[index].CollidersAtt->Transform = new Transform();
		monsterInfo[index].CollidersAtt->Collider = new Collider(monsterInfo[index].CollidersAtt->Transform, monsterInfo[index].CollidersAtt->Init);
	}
}

void Monster2::CollidersUpdate()
{
	Matrix worlds[MAX_MODEL_TRANSFORMS];
	for (UINT index = 0; index < MONSTER2_NUM; index++)
	{
		if (monsterInfo[index].hp < 0) continue;

		monster->GetAttachTransforms(index, worlds);

		Vector3 posi = monsterInfo[index].position;
		posi.y += 8.0f;
		Vector3 rot = monster->GetTransform(index)->GetRotation();

		monsterInfo[index].CollidersHit->Collider->GetTransform()->Position(posi);
		monsterInfo[index].CollidersHit->Collider->GetTransform()->Rotation(rot);
		monsterInfo[index].CollidersHit->Collider->Update();

		posi = monsterInfo[index].galaxyPosition;
		monsterInfo[index].CollidersAtt->Collider->GetTransform()->Position(posi);
		monsterInfo[index].CollidersAtt->Collider->Update();
	}
}

void Monster2::CollidersRender()
{
	if (showCollider == false) return;

	for (UINT index = 0; index < MONSTER2_NUM; index++)
	{
		if (monsterInfo[index].hp < 0) continue;

		if (monsterInfo[index].activeColliderHit == true)
			monsterInfo[index].CollidersHit->Collider->Render(Color(1, 0.5, 0.5, 1));
		else
			monsterInfo[index].CollidersHit->Collider->Render(Color(1, 0, 0, 1));

		if (monsterInfo[index].bAttack == false) continue;

		monsterInfo[index].CollidersAtt->Collider->Render(Color(1, 0, 0, 1));
	}
}

void Monster2::ParticleUpdate()
{
	for (UINT index = 0; index < MONSTER2_NUM; index++)
	{
		if (monsterInfo[index].bAttack == false) continue;

		monsterInfo[index].GalaxyParticle->Update();
	}
}

void Monster2::ParticleRender()
{
	for (UINT index = 0; index < MONSTER2_NUM; index++)
	{
		if (monsterInfo[index].bAttack == false) continue;

		monsterInfo[index].GalaxyParticle->Render();
	}
}

//void Monster2::CreateImage()
//{
//	hpBarTest = new Billboard(billShader, L"Red.png");
//	
//	for (int i = 0; i < MONSTER2_NUM; i++)
//	{
//		Transform* transform = hpBarTest->AddTransform();
//		Vector3 posi = monsterInfo[i].position;
//		posi.y += 11.0f;
//		transform->Position(posi);
//		transform->Scale(5.0f, 0.8f, 0.0f);
//	}
//
//	hpBarTest->Pass(3);
//	///////////////////////////////////////////////////////////
//
//	hpTexture = new Texture(L"Red.png");
//
//	///////////////////////////////////////////////////////////
//
//	/*hpTexture = new Texture(L"Red.png");
//
//	hpBar = new Render2D();
//	hpBar->GetTransform()->Scale(208.0f, 13.0f, 1);
//	hpBar->GetTransform()->Position(490.0f, 167.0f, 0);
//	
//	hpBuffer = new TextureBuffer(hpTexture->GetTexture());
//
//	textureShader->AsSRV("Input")->SetResource(hpBuffer->SRV());
//	textureShader->AsUAV("Output")->SetUnorderedAccessView(hpBuffer->UAV());
//
//
//	UINT width = hpBuffer->Width();
//	UINT height = hpBuffer->Height();
//	UINT arraySize = hpBuffer->ArraySize();
//
//	float x = ((float)width / 32.0f) < 1.0f ? 1.0f : ((float)width / 32.0f);
//	float y = ((float)height / 32.0f) < 1.0f ? 1.0f : ((float)height / 32.0f);
//
//	textureShader->Dispatch(0, 0, (UINT)ceilf(x), (UINT)ceilf(y), arraySize);
//
//	hpBar->SRV(hpBuffer->OutputSRV());*/
//}
//
//void Monster2::ImagePosition()
//{
//	for (int i = 0; i < MONSTER2_NUM; i++)
//	{
//		Vector3 posi = monsterInfo[i].position;
//		posi.y += 11.0f;
//
//		//hpBarTest->GetTransform(i)->Position(posi);
//		hpBarTest->GetTransform()->Position(posi);
//	}
//}
//
//void Monster2::ImageUpdate()
//{
//	ImagePosition();
//
//	hpBarTest->Update();
//}
//
//void Monster2::ImageRender()
//{
//	hpBarTest->Render();
//}

void Monster2::Reset(int index)
{
	//std::cout << "Monster2 Reset" << std::endl;
	//monsterInfo[index].bAttack = false;
	monsterInfo[index].isDown = false;

	monsterInfo[index].idleTrigger = false;
	monsterInfo[index].walkTrigger = false;
	monsterInfo[index].runTrigger = false;
	monsterInfo[index].atkTrigger = false;
	monsterInfo[index].waitTrigger = false;
	monsterInfo[index].downStartTrigger = false;
	monsterInfo[index].downLoopTrigger = false;
	monsterInfo[index].downEndTrigger = false;
	monsterInfo[index].deathTrigger = false;

	monsterInfo[index].duration = 0.0f;

	PointLight& light = Lighting::Get()->GetPointLight(index);
	light.Range = 0.0f;
}
