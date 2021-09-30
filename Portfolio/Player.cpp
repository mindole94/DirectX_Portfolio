#include "stdafx.h"
#include "Player.h"
#include "PlayerModel.h"

Player::Player(Shader * shader)
	: shader(shader)
{
	player = new PlayerModel(shader);

	CreateUI();
}

Player::~Player()
{
	for (UINT i = 0; i < 8; i++)
	{
		SafeDelete(texture[i]);
		SafeDelete(render2D[i]);
	}

	SafeDelete(player);
}

void Player::Update()
{
	if (hp >= 0)
	{
		SetDirection();

		if (behavior == bIdle)
			player->Idle();
		
		if (((Freedom *)Context::Get()->GetCamera())->GetFreeMode() == false)
		{
			Move();
			Attack();
			Jump();
			Evade();
			Skill();
		}
		else
		{
			CanReset();
			behavior = bIdle;
			player->PlaySwordTrail(false);
			player->SwordTrail()->Delete(true);
		}

		ColliderUpdate();
	}

	CountTime();

	UpdateUI();

	player->SetPosition(position);
	player->Rotation(rotation);
	player->Update();
}

void Player::PreRender()
{
	player->PreRender();
}

void Player::PreRender_Reflection()
{
	player->PreRender_Reflection();
}

void Player::Render()
{
	player->Render();
	player->BoxRender();

}

void Player::UIRender()
{
	RenderUI();
}

void Player::EffectRender()
{
	player->EffectRender();
	player->BoxRender();

}

Vector3 Player::Forward()
{
	return player->Forward();
}

void Player::ReceivedDamage(float damage)
{
	hp -= damage;

	Color color = player->GetPlayer()->GetColor(0);
	color.r = 1.0f;
	player->GetPlayer()->SetColor(0, color);
	rInvincibleTime = 0.5f;

	player->SetActiveHitBox(false);
}

void Player::Move()
{
	if (canMove == false) return;

	static bool trigger = false;

	if (Keyboard::Get()->Press('W') || Keyboard::Get()->Press('A') ||
		Keyboard::Get()->Press('S') || Keyboard::Get()->Press('D'))
		behavior = bMove;
	else
		behavior = bIdle;

	if (behavior == bMove)
		player->footprint.Use = true;
	else
	{
		trigger = false;
		player->footprint.Use = false;
		return;
	}

	Vector3 forward = Forward();

	float speed = 20.0f;

	if (Keyboard::Get()->Press(VK_SHIFT))
		speed *= 20.0f;

	if (Keyboard::Get()->Press('W'))
	{
		if (Keyboard::Get()->Press('A'))
		{
			Vector3 cforward = Context::Get()->GetCamera()->Forward();
			position += cFLeft * speed * Time::Delta();

			float dot = D3DXVec3Dot(&cFLeft, &forward);
			Vector3 temp;
			D3DXVec3Cross(&temp, &forward, &cFLeft);

			float sign = temp.y >= 0.0f ? 1.0f : -1.0f;
			float theta = sign * acosf(dot);

			rotation.y += theta * rotRatio;
		}
		else if (Keyboard::Get()->Press('D'))
		{
			position += cFRight * speed * Time::Delta();

			float dot = D3DXVec3Dot(&cFRight, &forward);
			Vector3 temp;
			D3DXVec3Cross(&temp, &forward, &cFRight);

			float sign = temp.y >= 0.0f ? 1.0f : -1.0f;
			float theta = sign * acosf(dot);

			rotation.y += theta * rotRatio;
		}
		else
		{
			position += cForward * speed * Time::Delta();

			float dot = D3DXVec3Dot(&cForward, &forward);
			Vector3 temp;
			D3DXVec3Cross(&temp, &forward, &cForward);

			float sign = temp.y >= 0.0f ? 1.0f : -1.0f;
			float theta = sign * acosf(dot);

			rotation.y += theta * rotRatio;
		}
	}
	else if (Keyboard::Get()->Press('S'))
	{
		if (Keyboard::Get()->Press('A'))
		{
			position += cBLeft * speed * Time::Delta();

			float dot = D3DXVec3Dot(&cBLeft, &forward);
			Vector3 temp;
			D3DXVec3Cross(&temp, &forward, &cBLeft);

			float sign = temp.y >= 0.0f ? 1.0f : -1.0f;
			float theta = sign * acosf(dot);

			rotation.y += theta * rotRatio;
		}
		else if (Keyboard::Get()->Press('D'))
		{
			position += cBRight * speed * Time::Delta();

			float dot = D3DXVec3Dot(&cBRight, &forward);
			Vector3 temp;
			D3DXVec3Cross(&temp, &forward, &cBRight);

			float sign = temp.y >= 0.0f ? 1.0f : -1.0f;
			float theta = sign * acosf(dot);

			rotation.y += theta * rotRatio;
		}
		else
		{
			position += cBackward * speed * Time::Delta();

			float dot = D3DXVec3Dot(&cBackward, &forward);
			Vector3 temp;
			D3DXVec3Cross(&temp, &forward, &cBackward);

			float sign = temp.y >= 0.0f ? 1.0f : -1.0f;
			float theta = sign * acosf(dot);

			rotation.y += theta * rotRatio;
		}
	}
	else if (Keyboard::Get()->Press('A'))
	{
		position += cLeft * speed * Time::Delta();

		float dot = D3DXVec3Dot(&cLeft, &forward);
		Vector3 temp;
		D3DXVec3Cross(&temp, &forward, &cLeft);

		float sign = temp.y >= 0.0f ? 1.0f : -1.0f;
		float theta = sign * acosf(dot);

		rotation.y += theta * rotRatio;
	}
	else if (Keyboard::Get()->Press('D'))
	{
		position += cRight * speed * Time::Delta();

		float dot = D3DXVec3Dot(&cRight, &forward);
		Vector3 temp;
		D3DXVec3Cross(&temp, &forward, &cRight);

		float sign = temp.y >= 0.0f ? 1.0f : -1.0f;
		float theta = sign * acosf(dot);

		rotation.y += theta * rotRatio;
	}

	if (trigger == false)
	{
		player->Run();

		trigger = true;
	}
}

void Player::Attack()
{
	static float time = 0.0f;
	static bool countTime = false;
	static UINT combo = 0;

	if (countTime == true)
		time += Time::Delta();

	if (time >= 1.0f)
	{
		CanReset();

		countTime = false;

		time = 0.0f;
		combo = 0;

		player->slash.Use = false;
	}

	if (canAttack[0] == false) return;

	if (Mouse::Get()->Down(0)) behavior = bAttack;
	if (behavior != bAttack) return;
	
	if (Mouse::Get()->Down(0))
	{
		canMove = false;
		canJump = false;
		canSkill = false;

		if (canAttack[1] == true)
		{
			canAttack[1] = false;
			
			countTime = false;

			combo = 1;

			mp += 300.0f;
			
			player->SwordTrail()->Reset();
			player->Attack1();

			if(buff == true)
				player->slash.Use = true;
		}
		else if (canAttack[2] == true)
		{
			canAttack[2] = false;
			countTime = false;
			combo = 2;

			mp += 300.0f;

			player->SwordTrail()->Reset();
			player->Attack2();

			if (buff == true)
				player->slash.Use = true;
		}
		else if (canAttack[3] == true)
		{
			canAttack[3] = false;
			countTime = false;
			combo = 3;

			mp += 300.0f;

			player->SwordTrail()->Reset();
			player->Attack3();

			if (buff == true)
				player->slash.Use = true;
		}
	}
	
	if (combo == 1)
	{
		int frameCount = player->GetPlayer()->GetModel()->ClipByIndex(4)->FrameCount();
		int currFrame = player->GetPlayer()->GetCurrentFrameTweenMode();

		if (currFrame == 12)
		{
			player->SwordTrail()->Delete(false);
			player->PlaySwordTrail(true);

			player->SetActiveAttBox(true);
		}

		if (currFrame == 17)
		{
			player->SwordTrail()->Delete(true);
			player->PlaySwordTrail(false);

			player->slash.Use = false;
			player->SetActiveAttBox(false);

		}

		if (currFrame > frameCount - comboInterval)
		{
			behavior = bIdle;

			canMove = true;
			canAttack[2] = true;
			canJump = true;
			canSkill = true;

			countTime = true;
		}
	}
	else if (combo == 2)
	{
		int frameCount = player->GetPlayer()->GetModel()->ClipByIndex(5)->FrameCount();
		int currFrame = player->GetPlayer()->GetCurrentFrameTweenMode();

		if (currFrame == 13)
		{
			player->SwordTrail()->Delete(false);
			player->PlaySwordTrail(true);

			player->SetActiveAttBox(true);
		}

		if (currFrame == 20)
		{
			player->SwordTrail()->Delete(true);
			player->PlaySwordTrail(false);

			player->slash.Use = false;

			player->SetActiveAttBox(false);
		}

		if (currFrame > frameCount - comboInterval)
		{
			behavior = bIdle;
			
			canMove = true;
			canAttack[3] = true;
			canJump = true;
			canSkill = true;

			countTime = true;
		}
	}
	else if (combo == 3)
	{
		int frameCount = player->GetPlayer()->GetModel()->ClipByIndex(6)->FrameCount();
		int currFrame = player->GetPlayer()->GetCurrentFrameTweenMode();

		if (currFrame == 12)
		{
			player->SwordTrail()->Delete(false);
			player->PlaySwordTrail(true);

			player->SetActiveAttBox(true);
		}

		if (currFrame == 18)
		{
			player->SwordTrail()->Delete(true);
			player->PlaySwordTrail(false);

			player->SetActiveAttBox(false);
		}

		if (currFrame > frameCount - comboInterval)
		{
			behavior = bIdle;

			canMove = true;
			canJump = true;
			canSkill = true;

			time = 1.0f;
		}
	}
}

void Player::Jump()
{
	if (canJump == false) return;

	if (Keyboard::Get()->Down(VK_SPACE) && behavior != bJump)
	{
		behavior = bJump;

		canMove = false;
		canAttack[0] = false;
		canEvade = false;
		canSkill = false;

		player->Jump();
	}

	static bool up = false;
	static bool down = false;
	
	if (behavior == bJump)
	{
		int frameCount = player->GetPlayer()->GetModel()->ClipByIndex(2)->FrameCount();
		int currFrame = player->GetPlayer()->GetCurrentFrameTweenMode();

		if (currFrame == 8)
			up = true;

		if (currFrame == 14)
			down = true;

		if (currFrame > frameCount - comboInterval)
		{
			CanReset();
			behavior = bIdle;

 			up = down = false;
		}
	}

	if (behavior != bJump) return;

	const float speed = 2.0f;

	if (down == true)
		position.y -= speed * Time::Delta();
	else if (up == true)
		position.y += speed * Time::Delta();
}

void Player::Evade()
{
	if (canEvade == false) return;
	if (rCoolTime[0] > 0.0f) return;

	if (Keyboard::Get()->Down('C') && behavior != bEvade && mp > consumeMana[0])
	{
		canMove = false;
		canAttack[0] = false;
		canJump = false;
		canSkill = false;

		behavior = bEvade;
		
		player->Evade();
		player->SetActiveHitBox(false);
		player->SetActiveAttBox(false);
		player->PlaySwordTrail(false);
		player->SwordTrail()->Reset();

		mp -= consumeMana[5];
	}

	if(behavior == bEvade)
	{
		int frameCount = player->GetPlayer()->GetModel()->ClipByIndex(3)->FrameCount();
		int currFrame = player->GetPlayer()->GetCurrentFrameTweenMode();

		if (currFrame > frameCount - 3)
		{
			CanReset();

			behavior = bIdle;
			rCoolTime[0] = coolTime[0];

			player->SetActiveHitBox(true);
		}
	}

	if (behavior != bEvade) return;

	Vector3 backward = player->Backward();
	float speed = 400.0f;

	position += backward * speed * Time::Delta();
}

void Player::Skill()
{
	if (canSkill == false) return;
	
	Skill1();
	Skill2();
	Skill3();
	Skill4();
	Skill5();
}

void Player::Skill1()
{
	if (rCoolTime[1] > 0.0f) return;

	if (Keyboard::Get()->Down('1') && behavior != bSkill && mp > consumeMana[1])
	{
		canMove = false;
		canAttack[0] = false;
		canJump = false;
		canEvade = false;

		behavior = bSkill;

		player->SwordTrail()->Reset();
		player->Skill1();

		currSkill = 1;
		mp -= consumeMana[1];
	}

	if (currSkill == 1 && behavior == bSkill)
	{
		int frameCount = player->GetPlayer()->GetModel()->ClipByIndex(7)->FrameCount();
		int currFrame = player->GetPlayer()->GetCurrentFrameTweenMode();
		
		if (currFrame == 13)
		{
			player->SwordTrail()->Delete(false);
			player->PlaySwordTrail(true);

			player->SetActiveAttBox(true);

			if(buff == true)
				player->slash.Use = true;
		}

		if (currFrame == 24)
		{
			player->SwordTrail()->Delete(true);
			player->PlaySwordTrail(false);

			player->slash.Use = false;

			player->SetActiveAttBox(false);
		}

		if (currFrame > frameCount - comboInterval)
		{
			CanReset();

			behavior = bIdle;
			currSkill = 0;
			rCoolTime[1] = coolTime[1];
		}
	}
}

void Player::Skill2()
{
	if (rCoolTime[2] > 0.0f) return;

	if (Keyboard::Get()->Down('2') && behavior != bSkill && mp > consumeMana[2])
	{
		canMove = false;
		canAttack[0] = false;
		canJump = false;
		canEvade = false;

		behavior = bSkill;

		player->Skill2();

		currSkill = 2;
		mp -= consumeMana[2];
	}

	if (currSkill == 2 && behavior == bSkill)
	{
		static bool trigger = false;
		int frameCount = player->GetPlayer()->GetModel()->ClipByIndex(8)->FrameCount();
		int currFrame = player->GetPlayer()->GetCurrentFrameTweenMode();

		if (Keyboard::Get()->Up('2'))
		{
			player->fireball.Shoot = true;
			player->fireball.Direct = player->Forward();
		}

		if (currFrame == 10)
		{
			player->fireball.StartPosition = GetPosition();
			player->fireball.Use = true;
			trigger = true;
		}
		
		if (trigger == true)
		{
			Vector3 forward = player->Forward();

			if (Keyboard::Get()->Press('A'))
			{
				float dot = D3DXVec3Dot(&cLeft, &forward);
				Vector3 temp;
				D3DXVec3Cross(&temp, &forward, &cLeft);

				float sign = temp.y >= 0.0f ? 1.0f : -1.0f;
				float theta = sign * acosf(dot);

				rotation.y += theta * rotRatio;
			}

			if (Keyboard::Get()->Press('D'))
			{
				float dot = D3DXVec3Dot(&cRight, &forward);
				Vector3 temp;
				D3DXVec3Cross(&temp, &forward, &cRight);

				float sign = temp.y >= 0.0f ? 1.0f : -1.0f;
				float theta = sign * acosf(dot);

				rotation.y += theta * rotRatio;
			}

			if (currFrame == 20)
				player->GetPlayer()->SetCurrFrameTweenMode(0, 19);
		}

		if ((currFrame > frameCount - 3) || player->fireball.Shoot == true)
		{
			CanReset();

			behavior = bIdle;
			currSkill = 0;
			rCoolTime[2] = coolTime[2];

			trigger = false;
		}
	}
}

void Player::Skill3()
{
	if (rCoolTime[3] > 0.0f) return;

	if (Keyboard::Get()->Down('Q') && behavior != bSkill && mp > consumeMana[3])
	{
		canMove = false;
		canAttack[0] = false;
		canJump = false;
		canEvade = false;

		behavior = bSkill;

		player->Skill3();

		currSkill = 3;
		mp -= consumeMana[3];

		player->footprint.Use = true;
		player->SetActiveAttBox(true);
	}

	if (currSkill == 3 && behavior == bSkill)
	{
		const float speed = 1000.0f;
		Vector3 forward = player->Forward();

		if (Keyboard::Get()->Press('A'))
		{
			float dot = D3DXVec3Dot(&cLeft, &forward);
			Vector3 temp;
			D3DXVec3Cross(&temp, &forward, &cLeft);

			float sign = temp.y >= 0.0f ? 1.0f : -1.0f;
			float theta = sign * acosf(dot);

			rotation.y += theta * rotRatio;
		}

		if (Keyboard::Get()->Press('D'))
		{
			float dot = D3DXVec3Dot(&cRight, &forward);
			Vector3 temp;
			D3DXVec3Cross(&temp, &forward, &cRight);

			float sign = temp.y >= 0.0f ? 1.0f : -1.0f;
			float theta = sign * acosf(dot);

			rotation.y += theta * rotRatio;
		}

		position += forward * speed * Time::Delta();

		static float time = 0.0f;
		time += Time::Delta();

		if (time >= 5.0f)
		{
			CanReset();

			behavior = bIdle;
			currSkill = 0;
			rCoolTime[3] = coolTime[3];

			time = 0.0f;

			player->footprint.Use = false;
			player->SetActiveAttBox(false);
		}
	}
}

void Player::Skill4()
{
	if (rCoolTime[4] > 0.0f) return;

	if (Keyboard::Get()->Down('E') && behavior != bSkill && mp > consumeMana[4])
	{
		canMove = false;
		canAttack[0] = false;
		canJump = false;
		canEvade = false;

		behavior = bSkill;

		player->Skill4();

		currSkill = 4;
		mp -= consumeMana[4];

		rBuffTime = 30.0f;
		player->buff.Use = true;
		buff = true;
	}

	if (currSkill == 4 && behavior == bSkill)
	{
		int frameCount = player->GetPlayer()->GetModel()->ClipByIndex(10)->FrameCount();
		int currFrame = player->GetPlayer()->GetCurrentFrameTweenMode();

		if (currFrame > frameCount - 3)
		{
			CanReset();

			behavior = bIdle;
			currSkill = 0;
			rCoolTime[4] = coolTime[4];
		}
	}
}

void Player::Skill5()
{
	if (rCoolTime[5] > 0.0f) return;

	static float time = 0.0f;
	static Vector3 finalPos;

	if (Keyboard::Get()->Down('X') && behavior != bSkill && mp > consumeMana[5])
	{
		canMove = false;
		canAttack[0] = false;
		canJump = false;
		canEvade = false;

		behavior = bSkill;

		player->Skill5();

		currSkill = 5;
		mp -= consumeMana[5];

		{
			time = 0.0f;

			player->SetSkillRange(&finalPos);
		}
	}
	
	if (currSkill == 5 && behavior == bSkill)
	{
		static float rate = 0.0f;
		static bool active = false;

		time += Time::Get()->Delta();
		rate = time;
		shader->AsScalar("projectorRate")->SetFloat(rate);

		if (time > 1.0f)
		{
			if (active == false)
			{
				player->SetSkillRange(&finalPos);

				player->SetActiveAttBox(true);

				active = true;
			}
			else if(active == true)
			{
				CanReset();

				behavior = bIdle;
				currSkill = 0;
				rCoolTime[5] = coolTime[5];

				time = 0.0f;

				position = finalPos;

				player->SetActiveAttBox(false);

				active = false;
			}
		}
	}
}

void Player::CountTime()
{
	static bool buffTrigger = false;
	static bool invinTrigger = false;

	for (int i = 0; i < 6; i++)
	{
		if (rCoolTime[i] == 0.0f) continue;

		rCoolTime[i] -= Time::Delta();
		rCoolTime[i] = Math::Clamp(rCoolTime[i], 0.0f, coolTime[i]);
	}

	if (rBuffTime > 0.0f)
	{
		rBuffTime -= Time::Delta();
		rBuffTime = Math::Clamp(rBuffTime, 0.0f, buffTime);

		if (player->SwordTrail()->TrailMapName() != L"/Trails/Unleashed_Purple.png")
			player->SwordTrail()->TrailMap(L"/Trails/Unleashed_Purple.png");

		atk = defaultAtk * 2.0f;

		buffTrigger = true;
	}
	else if(buffTrigger == true)
	{
		if (player->SwordTrail()->TrailMapName() != L"/Trails/Unleashed_Orange.png")
			player->SwordTrail()->TrailMap(L"/Trails/Unleashed_Orange.png");

		player->buff.Use = false;
		buff = false;

		atk = defaultAtk;
		buffTrigger = false;
	}

	if (rInvincibleTime > 0.0f)
	{
		rInvincibleTime -= Time::Delta();
		rInvincibleTime = Math::Clamp(rInvincibleTime, 0.0f, invincibleTime);

		invinTrigger = true;
	}
	else if(invinTrigger == true)
	{
		player->SetActiveHitBox(true);

		Color color = player->GetPlayer()->GetColor(0);
		color.r = 0.0f;
		player->GetPlayer()->SetColor(0, color);

		invinTrigger = false;
	}
}

void Player::SetDirection()
{
	player->Position(&position);
	player->Rotation(&rotation);
	
	/*static float aaa = 0.0f;
	ImGui::SliderFloat("height", &aaa, -10.0f, 50.0f);
	position.y = height + aaa;*/
	position.y = height;

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
}

void Player::CanReset()
{
	canMove = true;
	canAttack[0] = canAttack[1] = true;
	canAttack[2] = canAttack[3] = false;
	canJump = true;
	canEvade = true;
	canSkill = true;
}

void Player::CreateUI()
{
	texture[0] = new Texture(L"Red.png");
	texture[1] = new Texture(L"Blue.png");
	texture[2] = new Texture(L"sapling/Icon/Acrobat_Tex.png");
	texture[3] = new Texture(L"sapling/Icon/ShockBlow_Tex.png");
	texture[4] = new Texture(L"sapling/Icon/fireBall_Tex.png");
	texture[5] = new Texture(L"sapling/Icon/dash_Tex.png");
	texture[6] = new Texture(L"sapling/Icon/Roar_Tex.png");
	texture[7] = new Texture(L"sapling/Icon/JusticeLeap_Tex.png");
	texture[8] = new Texture(L"sapling/Icon/Roar_Tex.png");

	for (UINT i = 0; i < 9; i++)
		render2D[i] = new Render2D();

	float width = D3D::Width();
	float height = D3D::Height();

	render2D[0]->GetTransform()->Position(width * 0.5f - 205.0f, height * 0.5f - 215.0f, 0);
	render2D[1]->GetTransform()->Position(width * 0.5f + 205.0f, height * 0.5f - 215.0f, 0);

	for (UINT i = 0; i < 2; i++)
	{
		render2D[i]->GetTransform()->Scale(250.0f, 20.0f, 1);
		render2D[i]->SRV(texture[i]->SRV());
		render2D[i]->Pass(1);
	}

	float interval = 0.0f;
	for (UINT i = 2; i < 8; i++)
	{
		render2D[i]->GetTransform()->Position(width * 0.5f - 305.0f + interval, height * 0.5f - 300.0f, 0);
		render2D[i]->GetTransform()->Scale(50.0f, 50.0f, 1);
		render2D[i]->SRV(texture[i]->SRV());
		render2D[i]->Pass(2);

		if (i == 4) interval += 110.0f;

		interval += 100.0f;
	}

	render2D[8]->GetTransform()->Position(width * 0.5f - 319.5f, height * 0.5f - 190.0f, 0);
	render2D[8]->GetTransform()->Scale(20.0f, 20.0f, 1);
	render2D[8]->SRV(texture[8]->SRV());
	render2D[8]->Pass(2);
}

void Player::UpdateUI()
{
	hp = Math::Clamp(hp, 0.0f, maxHp);
	mp = Math::Clamp(mp, 0.0f, maxMp);

	float ratio = hp * 0.0001f;
	render2D[0]->ChangeColorPoint(ratio);

	ratio = mp * 0.0001f;
	render2D[1]->ChangeColorPoint(ratio);

	float width = D3D::Width();
	float height = D3D::Height();

	for (UINT i = 2; i < 8; i++)
		render2D[i]->GetShader()->AsScalar("CoolTime")->SetFloat(rCoolTime[i - 2] / coolTime[i - 2]);

	render2D[8]->GetShader()->AsScalar("CoolTime")->SetFloat(rBuffTime / buffTime);

	for (UINT i = 0; i < 9; i++)
		render2D[i]->Update();
}

void Player::RenderUI()
{
	string str = to_string((UINT)hp) + " / " + to_string((UINT)maxHp);
	Gui::Get()->RenderText(390.0f, 568.0f, str);

	str = to_string((UINT)mp) + " / " + to_string((UINT)maxMp);
	Gui::Get()->RenderText(390.0f + 415.0f, 568.0f, str);

	for (UINT i = 0; i < 8; i++)
		render2D[i]->Render();

	if(buff == true)
		render2D[8]->Render();
}

void Player::ColliderUpdate()
{
	if (behavior == bAttack || currSkill == 1)
	{
		player->AttBox()->Init->Position(0, 80, -80);
		player->AttBox()->Init->Scale(200.0f, 100.0f, 150.0f);
	}
	else if (currSkill == 3)
	{
		player->AttBox()->Init->Position(0, 80, -80);
		player->AttBox()->Init->Scale(150, 150, 150);
	}
	else if (currSkill == 5)
	{
		player->AttBox()->Init->Position(0, 80, -1000);
		player->AttBox()->Init->Scale(200, 150, 2000);
	}
	else
	{
		player->AttBox()->Init->Position(0, 80, -80);
		player->AttBox()->Init->Scale(0.0f, 0.0f, 0.0f);
	}


	if(player->GetActiveAttBox() == true)
		player->AttColor(Color(1, 1, 0, 1));
	else
		player->AttColor(Color(0, 1, 0, 1));

	if (player->GetActiveHitBox() == true)
		player->HitColor(Color(1, 1, 0, 1));
	else
		player->HitColor(Color(0, 1, 0, 1));
}

ColliderObject * Player::HitBox()
{
	return player->HitBox();
}

ColliderObject * Player::AttBox()
{
	return player->AttBox();
}

ColliderObject * Player::FireBox()
{
	return player->FireBox();
}

bool Player::GetActiveAttBox()
{
	return player->GetActiveAttBox();
}

bool Player::GetActiveHitBox()
{
	return player->GetActiveHitBox();
}

bool Player::GetActiveFireBox()
{
	return player->GetActiveFireBox();
}
