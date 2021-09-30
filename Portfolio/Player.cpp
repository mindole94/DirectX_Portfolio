#include "stdafx.h"
#include "Player.h"

Player::Player(Shader * shader)
	: shader(shader)
{
	CreatePlayer();
	CreateWeapon();
	CreateColliders();
	CreateImage();
	CreateParticle();
}

Player::~Player()
{
	SafeDelete(colliderFireball);
	SafeDelete(trail);
	SafeDelete(fireballParticle);
	SafeDelete(footprintParticle);
	
	SafeDelete(skillTexture[0]);
	SafeDelete(skillRender[0]);
	SafeDelete(mpRender);
	SafeDelete(hpRender);
	SafeDelete(mpTexture);
	SafeDelete(hpTexture);

	SafeDelete(collidersAtt);
	SafeDelete(collidersHit);
	SafeDelete(weaponTransform);
	SafeDelete(weapon);
	SafeDelete(sapling);
}

void Player::Update()
{
	SettingVector();
	motion = sapling->GetCurrentClipIndex(0);
	sapling->GetAttachTransforms(0, worldBonesPosition);

	if (hp > 0)
	{
		//Idle
		if (ableIdle == true)
		{
			if (idleTrigger == false)
			{
				idleTrigger = true;

				SetIdle();
			}
		}

		if (isEvade == false && bSkill == false)
		{
			Move();
			Attack();
			Jump();
		}

		SuperJump();
		DashAttack();
		Yell();
		UpperCut();
		FireBall();

		Evade();

	}//if (hp > 0)

	autoHeal += Time::Delta();
	if (autoHeal > 5.0f)
	{
		autoHeal = 0.0f;
		hp += 30.0f;
		mp += 30.0f;
	}

	hp = Math::Clamp(hp, 0, maxHp);
	mp = Math::Clamp(mp, 0, maxMp);

	if (Keyboard::Get()->Press(0x39))
		hp -= 5;
	else if (Keyboard::Get()->Down(0x30))
		hp += 100;

	CoolTime();



	position.y = Math::Clamp(position.y, height, 100000.0f);

	sapling->GetTransform(0)->Position(position);
	sapling->GetTransform(0)->Rotation(rotation);

	sapling->UpdateTransforms();
	sapling->Update();

	AttachWeapon();

	ImageUpdate();
	CollidersUpdate();
	ParticleUpdate();
	//TODO : 지워라
	if (Keyboard::Get()->Down('P'))
	{
		std::cout << "Player Position : " << sapling->GetTransform(0)->GetPosition().x << " " << sapling->GetTransform(0)->GetPosition().y << " " << sapling->GetTransform(0)->GetPosition().z << std::endl;
		std::cout << "Player Scale : " << sapling->GetTransform(0)->GetScale().x << " " << sapling->GetTransform(0)->GetScale().y << " " << sapling->GetTransform(0)->GetScale().z << std::endl;
		std::cout << motion << std::endl;
		system("pause");
	}
}

void Player::PreRender()
{
	sapling->Pass(2);
	sapling->Render();

	weapon->Pass(1);
	weapon->Render();
}

void Player::Render()
{
	CollidersRender();
	
	sapling->Pass(10);
	sapling->Render();

	weapon->Pass(9);
	weapon->Render();

	ParticleRender();
	ImageRender();
}

int Player::CurrentSkill()
{
	if (isSkill[0] == true)//슈퍼점프
		return 0;
	else if (isSkill[1] == true)//돌격
		return 1;
	else if (isSkill[2] == true)//방깍
		return 2;
	else if (isSkill[3] == true)//어퍼컷
		return 3;
	else if (isSkill[4] == true)//파이어볼
		return 4;
	else//평타
		return 5;
}

int Player::Damage()
{
	if (cameraShake == true)//슈퍼점프
		return atk * 5;
	else if (isSkill[1] == true)//돌격
		return atk * 2;
	else if (isSkill[2] == true)//방깍
		return 0;
	else if (isSkill[3] == true)//어퍼컷
		return atk * 3;
	else if (isSkill[4] == true)//파이어볼
		return fireballScale * 5;
	else//평타
		return atk;
}

void Player::CreatePlayer()
{
	sapling = new ModelAnimator(shader);
	sapling->ReadMesh(L"sapling/sapling");
	sapling->ReadMaterial(L"sapling/sapling");

	//이동
	sapling->ReadClip(L"sapling/clip/Standing Idle");//0
	sapling->ReadClip(L"sapling/clip/Running");//1
	sapling->ReadClip(L"sapling/clip/Standing Jump");//2일반 점프

	//회피
	sapling->ReadClip(L"sapling/clip/Jump Back");//3무적 백점프

	//평타
	sapling->ReadClip(L"sapling/clip/Standing Melee Attack Downward");//4 평타 콤보1
	sapling->ReadClip(L"sapling/clip/Standing Melee Attack Kick");//5 평타 콤보2
	sapling->ReadClip(L"sapling/clip/Standing Melee Attack Backhand");//6 평타 콤보3

	//스킬
	sapling->ReadClip(L"sapling/clip/Mutant Jumping");//7
	sapling->ReadClip(L"sapling/clip/Sword And Shield Run");//8
	sapling->ReadClip(L"sapling/clip/Sword And Shield Slash");//9
	sapling->ReadClip(L"sapling/clip/Standing Taunt Battlecry");//10
	sapling->ReadClip(L"sapling/clip/Spell Cast");//11

	//댄스
	sapling->ReadClip(L"sapling/clip/Chicken Dance");//13

	Transform* transform = NULL;
	transform = sapling->AddTransform();
	transform->Position(0, 0, 0);
	transform->Scale(0.05f, 0.05f, 0.05f);

	sapling->UpdateTransforms();

	position = sapling->GetTransform(0)->GetPosition();
}

#pragma region default motion
void Player::Move()
{
	if (ableMove == false) return;
	
	isRun = true;

	if (Keyboard::Get()->Press('W'))
	{
		if (Keyboard::Get()->Press('A'))
		{
			Vector3 cforward = Context::Get()->GetCamera()->Forward();
			position += cFLeft * runSpeed * Time::Delta();

			float dot = D3DXVec3Dot(&cFLeft, &forward);
			Vector3 temp;
			D3DXVec3Cross(&temp, &forward, &cFLeft);

			float sign = temp.y >= 0.0f ? 1.0f : -1.0f;
			float theta = sign * acosf(dot);

			Vector3 rot;
			sapling->GetTransform(0)->Rotation(&rot);
			rot.y += theta / 30;

			rotation.y = rot.y;
		}
		else if (Keyboard::Get()->Press('D'))
		{
			position += cFRight * runSpeed * Time::Delta();

			float dot = D3DXVec3Dot(&cFRight, &forward);
			Vector3 temp;
			D3DXVec3Cross(&temp, &forward, &cFRight);

			float sign = temp.y >= 0.0f ? 1.0f : -1.0f;
			float theta = sign * acosf(dot);

			Vector3 rot;
			sapling->GetTransform(0)->Rotation(&rot);
			rot.y += theta / 30;

			rotation.y = rot.y;
		}
		else
		{
			position += cForward * runSpeed * Time::Delta();

			float dot = D3DXVec3Dot(&cForward, &forward);
			Vector3 temp;
			D3DXVec3Cross(&temp, &forward, &cForward);

			float sign = temp.y >= 0.0f ? 1.0f : -1.0f;
			float theta = sign * acosf(dot);

			Vector3 rot;
			sapling->GetTransform(0)->Rotation(&rot);
			rot.y += theta / 30;

			rotation.y = rot.y;
		}

		bFootprint = true;
	}
	else if (Keyboard::Get()->Press('S'))
	{
		if (Keyboard::Get()->Press('A'))
		{
			position += cBLeft * runSpeed * Time::Delta();

			float dot = D3DXVec3Dot(&cBLeft, &forward);
			Vector3 temp;
			D3DXVec3Cross(&temp, &forward, &cBLeft);

			float sign = temp.y >= 0.0f ? 1.0f : -1.0f;
			float theta = sign * acosf(dot);

			Vector3 rot;
			sapling->GetTransform(0)->Rotation(&rot);
			rot.y += theta / 30;

			rotation.y = rot.y;
		}
		else if (Keyboard::Get()->Press('D'))
		{
			position += cBRight * runSpeed * Time::Delta();

			float dot = D3DXVec3Dot(&cBRight, &forward);
			Vector3 temp;
			D3DXVec3Cross(&temp, &forward, &cBRight);

			float sign = temp.y >= 0.0f ? 1.0f : -1.0f;
			float theta = sign * acosf(dot);

			Vector3 rot;
			sapling->GetTransform(0)->Rotation(&rot);
			rot.y += theta / 30;

			rotation.y = rot.y;
		}
		else
		{
			position += cBackward * runSpeed * Time::Delta();

			float dot = D3DXVec3Dot(&cBackward, &forward);
			Vector3 temp;
			D3DXVec3Cross(&temp, &forward, &cBackward);

			float sign = temp.y >= 0.0f ? 1.0f : -1.0f;
			float theta = sign * acosf(dot);

			Vector3 rot;
			sapling->GetTransform(0)->Rotation(&rot);
			rot.y += theta / 30;

			rotation.y = rot.y;
		}

		bFootprint = true;
	}
	else if (Keyboard::Get()->Press('A'))
	{
		position += cLeft * runSpeed * Time::Delta();

		float dot = D3DXVec3Dot(&cLeft, &forward);
		Vector3 temp;
		D3DXVec3Cross(&temp, &forward, &cLeft);

		float sign = temp.y >= 0.0f ? 1.0f : -1.0f;
		float theta = sign * acosf(dot);

		Vector3 rot;
		sapling->GetTransform(0)->Rotation(&rot);
		rot.y += theta / 30;

		rotation.y = rot.y;

		bFootprint = true;
	}
	else if (Keyboard::Get()->Press('D'))
	{
		position += cRight * runSpeed * Time::Delta();

		float dot = D3DXVec3Dot(&cRight, &forward);
		Vector3 temp;
		D3DXVec3Cross(&temp, &forward, &cRight);

		float sign = temp.y >= 0.0f ? 1.0f : -1.0f;
		float theta = sign * acosf(dot);

		Vector3 rot;
		sapling->GetTransform(0)->Rotation(&rot);
		rot.y += theta / 30;

		rotation.y = rot.y;

		bFootprint = true;
	}
	else
	{
		runTrigger = false;

		isRun = false;

		bFootprint = false;
	}

	if (prevRun != isRun && ableRun == true)
	{
		Reset();
	}

	prevRun = isRun;

	if (motion == 1 && isRun == true)
	{
		if (Keyboard::Get()->Press(VK_SHIFT))
		{
			sapling->SetClipSpeed(1.5f);
			runSpeed = 30.0f;
		}
		else
		{
			sapling->SetClipSpeed(1.0f);
			runSpeed = 20.0f;
		}
	}

	if (runTrigger == false && isRun == true && ableRun == true)
	{
		runTrigger = true;
		ableIdle = false;
		ableJump = true;
		ableAttack[0] = true;

		SetRun();
	}
}

void Player::Attack()
{
	if (ableAttack[0] == false) return;

	//Attack
	if (Mouse::Get()->Down(0) && ableAttack[0] == true)
	{
		isAttack[0] = true;
		
		ableIdle = false;
		ableMove = true;
		ableRun = false;
		ableJump = false;
		ableAttack[0] = true;

		activeColliderAtt = true;

		if (attackTrigger[0] == false)
		{
			attackTrigger[0] = true;
			trail->Reset();

			SetAttack1();
		}
	}

	//Attack Release
	if (motion == 4 && isAttack[0])
	{
		if (hitTrigger == false)
		{
			bAttack = true;

			hitTrigger = true;
		}

		int frameCount = sapling->GetModel()->ClipByIndex(4)->FrameCount();
		int currFrame = sapling->GetCurrentFrameTweenMode();

		if (currFrame == 20)
		{
			trail->Delete(false);
			bSwordSpectrum = true;
		}

		if (currFrame == 33)
		{
			trail->Delete(true);
			bSwordSpectrum = false;
		}

		if (currFrame > frameCount - 20)
			ableAttack[1] = true;

		if (currFrame == frameCount - 1)
			Reset();
	}
	
	//Attack2
	if (Mouse::Get()->Down(0) && ableAttack[1] == true)
	{
		isAttack[1] = true;

		if (attackTrigger[1] == false)
		{
			attackTrigger[1] = true;
			hitTrigger = false;

			SetAttack2();
		}
	}

	//Attack2 Release
	if (motion == 5 && isAttack[1])
	{
		if (hitTrigger == false)
		{
			bAttack = true;

			hitTrigger = true;
		}

		int frameCount = sapling->GetModel()->ClipByIndex(5)->FrameCount();
		int currFrame = sapling->GetCurrentFrameTweenMode();

		if (currFrame > frameCount - 20)
			ableAttack[2] = true;

		if (currFrame == frameCount - 1)
			Reset();
	}

	//Attack3
	if (Mouse::Get()->Down(0) && ableAttack[2] == true)
	{
		isAttack[2] = true;

		if (attackTrigger[2] == false)
		{
			attackTrigger[2] = true;
			hitTrigger = false;
			trail->Reset();

			SetAttack3();
		}
	}

	//Attack3 Release
	if (motion == 6 && isAttack[2] == true)
	{
		if (hitTrigger == false)
		{
			bAttack = true;

			hitTrigger = true;
		}

		int frameCount = sapling->GetModel()->ClipByIndex(6)->FrameCount();
		int currFrame = sapling->GetCurrentFrameTweenMode();

		if (currFrame == 25)
		{
			trail->Delete(false);
			bSwordSpectrum = true;
		}

		if (currFrame == 35)
		{
			bSwordSpectrum = false;
		}

		if (currFrame == 45)
			trail->Delete(true);

		if (currFrame == frameCount - 1)
			Reset();
	}
}

void Player::Evade()
{
	if (isSkill[0] == true) return;

	if (Keyboard::Get()->Down('C') && curCoolTime[5] == 0.0f)
	{
		isEvade = true;

		curCoolTime[5] = 2.0f;

		if (evadeTrigger == false)
		{
			evadeTrigger = true;

			SetEvade();
		}
	}

	if (motion == 3 && isEvade == true)
	{
		if(isEvade == true)
			position += backward * runSpeed * Time::Delta();

		int frameCount = sapling->GetModel()->ClipByIndex(3)->FrameCount();
		int currFrame = sapling->GetCurrentFrameTweenMode();

		if (currFrame == frameCount - 1)
			Reset();
	}
}

void Player::Jump()
{
	//Jump
	if (Keyboard::Get()->Down(VK_SPACE) && ableJump == true)
	{
		isJump = true;
		isFall = false;

		ableIdle = false;
		ableMove = true;
		ableRun = false;
		ableAttack[0] = false;

		cameraRelease = true;

		if (jumpTrigger == false)
		{
			jumpTrigger = true;

			SetJump();
		}
	}

	//Jump Release
	if (motion == 2 && isJump == true)
	{
		int frameCount = sapling->GetModel()->ClipByIndex(2)->FrameCount();
		int currFrame = sapling->GetCurrentFrameTweenMode();

		if (currFrame > 11)
			isUp = true;

		if (currFrame > 24)
			isFall = true;

		if (currFrame == frameCount - 1)
			Reset();
	}
	
	if (isFall == true)
		position += down * runSpeed * Time::Delta();
	else if (isUp == true)
		position += up * runSpeed * Time::Delta();
}
#pragma endregion

#pragma region skill motion
void Player::SuperJump()
{
	if (cameraShake == true)
		cameraShake = false;

	if (Keyboard::Get()->Down('X') && curCoolTime[0] == 0.0f)
	{
		bSkill = true;
		isSkill[0] = true;
		isFall = false;

		cameraRelease = true;

		curCoolTime[0] = 5.0f;
		mp -= 1000;

		if (skillTrigger[0] == false)
		{
			skillTrigger[0] = true;

			SetSuperJump();
		}
	}

	if (motion == 7 && isSkill[0] == true)
	{
		int frameCount = sapling->GetModel()->ClipByIndex(7)->FrameCount();
		int currFrame = sapling->GetCurrentFrameTweenMode();
		
		if (currFrame > 35)
		{
			sapling->SetClipSpeed(1.0f);
			isUp = true;
		}

		if (currFrame > 50)
		{
			isFall = true;
			activeColliderAtt = true;
		}

		if (isFall == true && position.y < height + 0.1f)
		{
			sapling->SetCurrFrameTweenMode(0, frameCount - 20);
			
			cameraShake = true;
			
			Reset();
		}
		else if (currFrame > 60)
			sapling->SetCurrFrameTweenMode(0, 60);

	}

	if (isSkill[0] == false) return;

	if (isFall == true)
	{
		position += down * runSpeed * jumpAccel * Time::Delta();
		position += forward * runSpeed * 5.0f * Time::Delta();
	}
	else if (isUp == true)
	{
		jumpAccel += 0.02f;
		position += forward * runSpeed * 5.0f * Time::Delta();
		position += up * runSpeed * jumpAccel * Time::Delta();
	}
	
	jumpAccel = Math::Clamp(jumpAccel, 0.0f, 10.0f);
}

void Player::DashAttack()
{
	if (Keyboard::Get()->Down('Q') && curCoolTime[1] == 0.0f)
	{
		bSkill = true;
		isSkill[1] = true;

		curCoolTime[1] = 5.0f;
		mp -= 500;
		activeColliderAtt = true;

		if (skillTrigger[1] == false)
		{
			skillTrigger[1] = true;

			startPosition = position;

			SetDash();
		}
	}

	if (motion == 8 && isSkill[1] == true)
	{
		Vector3 posi = position - startPosition;
		float length = D3DXVec3Length(&posi);
		
		position += forward * 2.0f * runSpeed *  Time::Delta();
		position.y = height;

		if (length > 80.0f || damageMoment == true)
		{
			skillTrigger[1] = false;

			if (skillTrigger[2] == false)
			{
				skillTrigger[2] = true;

				SetSlash();
			}
		}
	}

	if (motion == 9 && isSkill[1] == true)
	{
		position.y = height;
		sapling->SetClipSpeed(3.0f);

		int frameCount = sapling->GetModel()->ClipByIndex(9)->FrameCount();
		int currFrame = sapling->GetCurrentFrameTweenMode();

		if (currFrame == frameCount - 1)
			Reset();
	}
}

void Player::Yell()
{
	if (Keyboard::Get()->Down('E') && curCoolTime[2] == 0.0f)
	{
		bSkill = true;
		isSkill[2] = true;

		curCoolTime[2] = 10.0f;
		mp -= 500;
		activeColliderAtt = true;

		if (skillTrigger[2] == false)
		{
			skillTrigger[2] = true;

			SetYell();
		}
	}

	if (motion == 10 && isSkill[2] == true)
	{
		int frameCount = sapling->GetModel()->ClipByIndex(10)->FrameCount();
		int currFrame = sapling->GetCurrentFrameTweenMode();

		if (currFrame == frameCount - 1)
			Reset();
	}
}
void Player::UpperCut()
{
	if (Keyboard::Get()->Down('1') && curCoolTime[3] == 0.0f)
	{
		bSkill = true;
		isSkill[3] = true;

		curCoolTime[3] = 3.0f;
		mp -= 300;
		activeColliderAtt = true;

		if (skillTrigger[3] == false)
		{
			skillTrigger[3] = true;

			SetAttack3();
		}
	}

	if (motion == 6 && isSkill[3] == true)
	{
		int frameCount = sapling->GetModel()->ClipByIndex(6)->FrameCount();
		int currFrame = sapling->GetCurrentFrameTweenMode();

		if (currFrame == frameCount - 1)
			Reset();
	}
}
void Player::FireBall()
{
	if (Keyboard::Get()->Press('2') && curCoolTime[4] == 0.0f)
	{
		bSkill = true;
		isSkill[4] = true;

		fireballTime += Time::Delta();

		fireballTime = Math::Clamp(fireballTime, 0.0f, 5.0f);
		fireballScale = (fireballTime * 25.0f) + 5.0f;
		
		fireballDirect = forward;
		fireballStartPosition = position;
		fireballRotation = rotation;

		Vector3 fScale = Vector3(0.0f, 0.0f, 0.0f);
		Quaternion quat(0.0f, 0.0f, 0.0f, 0.20f);

		Matrix defaultBonePosition = sapling->GetModel()->BoneByIndex(15)->Transform();
		Matrix frameBonePosition = sapling->BoneTransformByIndex(15);
		Matrix matrix = defaultBonePosition * frameBonePosition * sapling->GetTransform(0)->World();

		D3DXMatrixDecompose(&fScale, &quat, &fireballPosition, &matrix);

		if (skillTrigger[4] == false)
		{
			skillTrigger[4] = true;

			SetFireball();
		}
	}

	if (motion == 11 && isSkill[4] == true)
	{
		int frameCount = sapling->GetModel()->ClipByIndex(11)->FrameCount();
		int currFrame = sapling->GetCurrentFrameTweenMode();

		if (currFrame == 10)
			bFireball = true;

		if (Keyboard::Get()->Up('2'))//발사
		{
			mp -= 1000;

			shootFireball = true;
		}

		if (currFrame >= 20 && shootFireball == false)//불충전
		{
			sapling->SetPlayFrame(0, false);
			sapling->SetPauseFrame(0, true);
		}
		
		if (shootFireball == true)
		{
			sapling->SetPlayFrame(0, true);
			sapling->SetPauseFrame(0, false);
		}

		if (currFrame == frameCount - 1)
		{
			curCoolTime[4] = 5.0f;

			Reset();
		}
	}
}

#pragma endregion

#pragma region Set Animation
void Player::SetIdle()
{
	std::cout << "idle" << std::endl;
	sapling->PlayTweenMode(0, 0, 1.0f);
}

void Player::SetRun()
{
	std::cout << "Run" << std::endl;
	sapling->PlayTweenMode(0, 1, 1.0f);
}

void Player::SetJump()
{
	std::cout << "Jump" << std::endl;
	sapling->ResetFrame(0);
	sapling->PlayNormalMode(0, 2, 2.0f);
}

void Player::SetEvade()
{
	std::cout << "Evade" << std::endl;
	sapling->ResetFrame(0);
	sapling->PlayNormalMode(0, 3, 2.0f);
}

void Player::SetAttack1()
{
	std::cout << "Attack1" << std::endl;
	sapling->ResetFrame(0);
	sapling->PlayNormalMode(0, 4, 3.0f);

	mp += 100;
}

void Player::SetAttack2()
{
	std::cout << "Attack2" << std::endl;
	sapling->ResetFrame(0);
	sapling->PlayNormalMode(0, 5, 3.0f);

	mp += 100;
}

void Player::SetAttack3()
{
	std::cout << "Attack3" << std::endl;
	sapling->ResetFrame(0);
	sapling->PlayNormalMode(0, 6, 2.0f);

	mp += 100;
}

void Player::SetSuperJump()
{
	std::cout << "SuperJump" << std::endl;
	sapling->ResetFrame(0);
	sapling->PlayNormalMode(0, 7, 2.0f);
}

void Player::SetDash()
{
	std::cout << "Dash" << std::endl;
	sapling->ResetFrame(0);
	sapling->PlayNormalMode(0, 8, 2.0f);
}

void Player::SetSlash()
{
	std::cout << "Slash" << std::endl;
	sapling->ResetFrame(0);
	sapling->PlayNormalMode(0, 9, 2.0f);
}

void Player::SetYell()
{
	std::cout << "Yell" << std::endl;
	sapling->ResetFrame(0);
	sapling->PlayNormalMode(0, 10, 2.0f);
}

void Player::SetFireball()
{
	std::cout << "Fireball" << std::endl;
	sapling->ResetFrame(0);
	sapling->PlayNormalMode(0, 11, 1.0f);
}

#pragma endregion

void Player::CreateColliders()
{
	collidersHit = new ColliderObject();

	collidersHit->Init = new Transform();
	collidersHit->Init->Position(0.0f, 0.0f, 0.0f);
	collidersHit->Init->Scale(3, 8, 3);

	collidersHit->Transform = new Transform();
	collidersHit->Collider = new Collider(collidersHit->Transform, collidersHit->Init);

	//////////////////////////////////////////////////////////////////////////////////////////

	collidersAtt = new ColliderObject;

	collidersAtt->Init = new Transform();

	collidersAtt->Transform = new Transform();
	collidersAtt->Collider = new Collider(collidersAtt->Transform, collidersAtt->Init);

	//////////////////////////////////////////////////////////////////////////////////////////
	
	colliderFireball = new ColliderObject;

	colliderFireball->Init = new Transform();

	colliderFireball->Transform = new Transform();
	colliderFireball->Collider = new Collider(colliderFireball->Transform, colliderFireball->Init);
}

void Player::CollidersUpdate()
{
	Vector3 posi = position;
	posi.y += 4.0f;

	collidersHit->Init->Position(posi);
	collidersHit->Init->Rotation(rotation);
	//collidersHit->Init->Scale(3, 8, 3);

	collidersHit->Collider->Update();
	//////////////////////////////////////////////////////////////////////////////////////////

	if (isSkill[0] == true)
	{
		collidersAtt->Init->Position(0.0f, 0.0f, 0.0f);
		collidersAtt->Init->Scale(500.0f, 500.0f, 500.0f);
		collidersAtt->Init->Rotation(0.0f, 0.0f, 0.0f);
		collidersAtt->Transform->World(worldBonesPosition[0]);
	}
	else if (isSkill[1] == true && motion == 8)
	{
		collidersAtt->Init->Position(0.0f, 80.0f, -30.0f);
		collidersAtt->Init->Scale(100.0f, 100.0f, 50.0f);
		collidersAtt->Init->Rotation(0.0f, 0.0f, 0.0f);
		collidersAtt->Transform->World(worldBonesPosition[0]);
	}
	else if (isSkill[2] == true)
	{
		collidersAtt->Init->Position(0.0f, 0.0f, 0.0f);
		collidersAtt->Init->Scale(500.0f, 500.0f, 500.0f);
		collidersAtt->Init->Rotation(0.0f, 0.0f, 0.0f);
		collidersAtt->Transform->World(worldBonesPosition[0]);
	}
	else
	{
		collidersAtt->Init->Scale(5.0f, 25.0f, 5.0f);
		collidersAtt->Init->Position(0.0f, 24.0f, 0.0f);
		collidersAtt->Transform->World(weaponTransform->World() * worldBonesPosition[20]);
	}
	collidersAtt->Collider->Update();

	//////////////////////////////////////////////////////////////////////////////////////////
	
	float fs = fireballScale / 7;
	colliderFireball->Collider->GetTransform()->Position(fireballPosition);
	colliderFireball->Collider->GetTransform()->Scale(fs, fs, fs);
	colliderFireball->Collider->GetTransform()->Rotation(fireballRotation);
	colliderFireball->Collider->Update();
}

void Player::CollidersRender()
{
	if(showCollider == false) return;

	if (isEvade == true)
		collidersHit->Collider->Render(Color(1, 1, 0, 1));
	else
		collidersHit->Collider->Render();
	
	///////////////////////////////////////////////////////////

	if (activeColliderAtt == true)
		collidersAtt->Collider->Render(Color(1, 1, 0, 1));
	else
		collidersAtt->Collider->Render();

	///////////////////////////////////////////////////////////

	if (shootFireball == true)
		colliderFireball->Collider->Render(Color(1, 1, 0, 1));
	else if(bFireball == true)
		colliderFireball->Collider->Render();
}

void Player::CreateImage()
{
	hpTexture = new Texture(L"Red.png");

	hpRender = new Render2D();
	hpRender->GetTransform()->Scale(208.0f, 13.0f, 1);
	hpRender->GetTransform()->Position(490.0f, 167.0f, 0);
	hpRender->SRV(hpTexture->SRV());
	hpRender->Pass(1);

	mpTexture = new Texture(L"Blue.png");

	mpRender = new Render2D();
	mpRender->GetTransform()->Scale(208.0f, 13.0f, 1);
	mpRender->GetTransform()->Position(797.0f, 167.0f, 0);
	mpRender->SRV(mpTexture->SRV());
	mpRender->Pass(1);

	skillTexture[0] = new Texture(L"sapling/Icon/JusticeLeap_Tex.png");
	skillTexture[1] = new Texture(L"sapling/Icon/dash_Tex.png");
	skillTexture[2] = new Texture(L"sapling/Icon/Roar_Tex.png");
	skillTexture[3] = new Texture(L"sapling/Icon/ShockBlow_Tex.png");
	skillTexture[4] = new Texture(L"sapling/Icon/fireBall_Tex.png");
	skillTexture[5] = new Texture(L"sapling/Icon/Acrobat_Tex.png");

	float x = 410.0f;
	for (UINT i = 0; i < 6; i++)
	{
		skillRender[i] = new Render2D();
		skillRender[i]->GetTransform()->Scale(50.0f, 50.0f, 1);
		skillRender[i]->GetTransform()->Position(x, 115.0f, 0);
		skillRender[i]->SRV(skillTexture[i]->SRV());
		skillRender[i]->Pass(2);

		x += 80.0f;
		if (i == 2)
			x += 70.0f;
	}
}

void Player::ImageUpdate()
{
	float ratio = (float)hp / (float)maxHp;
	hpRender->ChangeColorPoint(ratio);
	
	ratio = (float)mp / (float)maxMp;
	mpRender->ChangeColorPoint(ratio);

	hpRender->Update();
	mpRender->Update();

	for (UINT i = 0; i < 6; i++)
		skillRender[i]->GetShader()->AsScalar("CoolTime")->SetFloat(curCoolTime[i] / coolTime[i]);

	for (UINT i = 0; i < 6; i++)
		skillRender[i]->Update();
}

void Player::ImageRender()
{
	mpRender->Render();
	hpRender->Render();

	for (UINT i = 0; i < 6; i++)
		skillRender[i]->Render();

	string str = to_string(hp) + " / " + to_string(maxHp);
	Gui::Get()->RenderText(445.0f, 545.0f, str);

	str = to_string(mp) + " / " + to_string(maxMp);
	Gui::Get()->RenderText(750.0f, 545.0f, str);

}

void Player::CreateWeapon()
{
	weapon = new ModelRender(shader);
	weapon->ReadMesh(L"Weapon/L_Dual17_SM");
	weapon->ReadMaterial(L"Weapon/L_Dual17_SM");
	
	weapon->AddTransform();

	weapon->UpdateTransforms();
	weapon->Pass(1);

	weaponTransform = new Transform();
	weaponTransform->Position(7.5f, 12.0f, 11.5f);
	weaponTransform->Rotation(0.0f, -1.0f, 2.25f);
	weaponTransform->Scale(3.0f, 3.0f, 3.0f);
}

void Player::AttachWeapon()
{
	weapon->GetTransform(0)->World(weaponTransform->World() *worldBonesPosition[20]);
	
	weapon->UpdateTransforms();
	weapon->Update();
}

void Player::CreateParticle()
{
	footprintParticle = new ParticleSystem(L"FootPrint");
	fireballParticle = new ParticleSystem(L"Fireball");
	
	trail = new TrailSystem(L"SwordTrail");
}

void Player::ParticleUpdate()
{
	if (bFootprint == true)
		footprintParticle->Add(position);

	if (bFireball == true)
	{
		if (shootFireball == true)
			fireballPosition += fireballDirect * 5.0f * runSpeed * Time::Delta();
		fireballParticle->Rotate(1.0f);
		fireballParticle->Add(fireballPosition);
		fireballParticle->Scale(fireballScale);
	}
	//////////////////////////////////////////////////////////////////////////////////////////

	if (bSwordSpectrum == true)
	{
		Vector3 targetPosition = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 rotation = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 scale = Vector3(0.0f, 0.0f, 0.0f);
		Quaternion quat(0.0f, 0.0f, 0.0f, 0.0f);

		Matrix defaultBonePosition = sapling->GetModel()->BoneByIndex(20)->Transform();
		Matrix frameBonePosition = sapling->BoneTransformByIndex(20);
		Matrix mat = defaultBonePosition * frameBonePosition * sapling->GetTransform(0)->World();

		D3DXMatrixDecompose(&scale, &quat, &targetPosition, &mat);

		trail->Add(collidersAtt->Collider->Bottom(), collidersAtt->Collider->Top());
	}

	trail->Update();
	//////////////////////////////////////////////////////////////////////////////////////////
	
	float length = D3DXVec3Length(&(fireballStartPosition - fireballPosition));
	
	if (length > 200.0f)
	{
		bFireball = false;
		fireballScale = 0.0f;
		fireballTime = 0.0f;
		shootFireball = false;
	}

	footprintParticle->Update();
	fireballParticle->Update();
}

void Player::ParticleRender()
{
	trail->Render();
	footprintParticle->Render();
	fireballParticle->Render();
}

void Player::SettingVector()
{
	position = sapling->GetTransform(0)->GetPosition();
	
	cForward = Context::Get()->GetCamera()->Forward(); cForward.y = 0.0f;
	D3DXVec3Normalize(&cForward, &cForward);

	cBackward = -cForward;

	cRight = Context::Get()->GetCamera()->Right(); cRight.y = 0.0f;
	D3DXVec3Normalize(&cRight, &cRight);

	cLeft = -cRight;

	cFRight = cForward + cRight;
	D3DXVec3Normalize(&cFRight, &cFRight);

	cBLeft = -cFRight;

	cFLeft = cForward + cLeft;
	D3DXVec3Normalize(&cFLeft, &cFLeft);

	cBRight = -cFLeft;

	forward = sapling->GetTransform(0)->Forward();
	std::cout << forward.x << " " << forward.y << " " << forward.z << std::endl;
	forward.x = -forward.x; forward.y = 0.0; forward.z = -forward.z;
	D3DXVec3Normalize(&forward, &forward);

	backward = sapling->GetTransform(0)->Forward();
	backward.y = 0.0;
	D3DXVec3Normalize(&backward, &backward);

	right = sapling->GetTransform(0)->Right();
	right.x = -right.x; right.y = 0.0; right.z = -right.z;
	D3DXVec3Normalize(&right, &right);

	left = sapling->GetTransform(0)->Right();
	left.y = 0.0f;
	D3DXVec3Normalize(&left, &left);
}

void Player::CoolTime()
{
	curCoolTime[0] -= Time::Delta();
	curCoolTime[1] -= Time::Delta();
	curCoolTime[2] -= Time::Delta();
	curCoolTime[3] -= Time::Delta();
	curCoolTime[4] -= Time::Delta();
	curCoolTime[5] -= Time::Delta();

	curCoolTime[0] = Math::Clamp(curCoolTime[0], 0.0f, 5.0f);
	curCoolTime[1] = Math::Clamp(curCoolTime[1], 0.0f, 5.0f);
	curCoolTime[2] = Math::Clamp(curCoolTime[2], 0.0f, 10.0f);
	curCoolTime[3] = Math::Clamp(curCoolTime[3], 0.0f, 3.0f);
	curCoolTime[4] = Math::Clamp(curCoolTime[4], 0.0f, 5.0f);
	curCoolTime[5] = Math::Clamp(curCoolTime[5], 0.0f, 2.0f);
}

void Player::Reset()
{
	std::cout << "Reset" << std::endl;
	//Current Status
	//isRun = false; 여기서 할필요없음
	isJump = false;
	isUp = false;
	isFall = true;
	//bAttack = false;
	isAttack[0] =  isAttack[1] = isAttack[2] = false;
	isEvade = false;
	bSkill = false;
	isSkill[0] = isSkill[1] = isSkill[2] = isSkill[3] = isSkill[4] = false;

	//Motion Trigger
	idleTrigger = false;
	runTrigger = false;
	jumpTrigger = false;
	attackTrigger[0] = attackTrigger[1] =  attackTrigger[2] = false;
	hitTrigger = false;
	evadeTrigger = false;
	skillTrigger[0] = skillTrigger[1] = skillTrigger[2] = skillTrigger[3] = skillTrigger[4] = false;

	//Able Motion
	ableIdle = true;
	ableMove = true;
	ableRun = true;
	ableJump = true;
	ableAttack[0] = true;
	ableAttack[1] = ableAttack[2] = false;

	damageMoment = false;
	
	jumpAccel = 0.0f;

	cameraRelease = false;

	activeColliderAtt = false;
	bSwordSpectrum = false;
}
