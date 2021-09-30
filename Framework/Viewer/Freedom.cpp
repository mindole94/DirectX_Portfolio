#include "Framework.h"
#include "Freedom.h"

Freedom::Freedom()
	: Camera()
{

}

Freedom::~Freedom()
{

}

void Freedom::Update()
{
	if (bFreeMode == false)
	{
		Vector3 r = Right();
		Vector3 f;
		D3DXVec3Cross(&f, &r, &Vector3(0, 1, 0));

		Vector3 val = Vector3(0, 0, 0);

		if (Mouse::Get()->Press(1) == true)
			val = Mouse::Get()->GetMoveValue();//이전 프레임부터 현재 프레임까지 마우스가 이동한 만큼의 차를 알려줌

		float delta = Mouse::Get()->GetMoveValue().z;
		if (fabs(delta) >= WHEEL_DELTA)
		{
			float delta_zoom = 1.0f - fabs(delta) / WHEEL_DELTA / 50.0f;

			if (delta < 0)
				zoom /= delta_zoom;
			else
				zoom *= delta_zoom;
		}

		zoom = Math::Clamp(zoom, 10.0f, 50.0f);
		//height = (zoom - 5.0f) * 0.5f;
		height = zoom * 0.5f;

		Vector3 R;
		Rotation(&R);

		R.x = R.x + val.y * rotation * Time::Delta();
		//x축 회전 : 위아래로 움직임
		R.y = R.y + val.x * rotation * Time::Delta();
		//y축 회전 : 양옆으로 움직임
		R.z = 0.0f;

		Rotation(R);

		theta = R.y;
		phi = R.x;

		Vector3 P;
		Position(&P);
		CameraShake();

		P.x = at.x - zoom * sinf(theta) * cosf(phi) * 2.0f;
		P.y = at.y + zoom * sinf(phi) + height * 2.0f;
		P.z = at.z - zoom * cosf(theta) * cosf(phi) * 2.0f;

		//P.y = Math::Clamp(P.y, at.y + 5.0f, 1000.0f);
		P.y = Math::Clamp(P.y, at.y + 5.0f, 300.0f);//카메라 최대 최소 높이
		position = P;

		Position(position);
	}
	else
	{
		if (Mouse::Get()->Press(1) == false) return;

		Vector3 f = Forward();
		Vector3 u = Up();
		Vector3 r = Right();

		//Move
		{
			Vector3 P;
			float speed = 1.0f;
			Position(&P);

			if (Keyboard::Get()->Press(VK_SHIFT))
				speed *= 10.0f;

			if (Keyboard::Get()->Press('W'))
				P = P + f * move * Time::Delta() * speed;
			if (Keyboard::Get()->Press('S'))
				P = P - f * move * Time::Delta() * speed;

			if (Keyboard::Get()->Press('D'))
				P = P + r * move * Time::Delta() * speed;
			if (Keyboard::Get()->Press('A'))
				P = P - r * move * Time::Delta() * speed;

			if (Keyboard::Get()->Press('E'))
				P = P + u * move * Time::Delta() * speed;
			if (Keyboard::Get()->Press('Q'))
				P = P - u * move * Time::Delta() * speed;

			Position(P);
		}

		//Rotation
		{
			Vector3 R;
			Rotation(&R);

			Vector3 val = Mouse::Get()->GetMoveValue();
			R.x = R.x + val.y * rotation * Time::Delta();
			R.y = R.y + val.x * rotation * Time::Delta();
			R.z = 0.0f;

			Rotation(R);
		}
	}
}

void Freedom::Speed(float move, float rotation)
{
	this->move = move;
	this->rotation = rotation;
}

void Freedom::SetShake(float shakeTime, float shakeScale)
{
	this->shakeTime = shakeTime;
	this->shakeScale += shakeScale;
	shakeForce = shakeScale;

	cameraShake = true;
}

void Freedom::CameraShake()
{
	if (cameraShake == false) return;
	
	shakeTime -= Time::Delta();
	if (shakeTime < 0.0f)
	{
		cameraShake = false;
		shakeTime = 0.0f;
		shakeScale = 0.0f;
		shakeForce = 0.0f;
	}

	shakeForce -= Time::Delta() * shakeScale;// *3.0f;
	shakeForce = Math::Clamp(shakeForce, 0.0f, shakeScale);

	Vector3 posi = at;
	posi.x = Math::Random(posi.x - shakeForce, posi.x + shakeForce);
	posi.y = Math::Random(posi.y - shakeForce, posi.y + shakeForce);
	posi.z = Math::Random(posi.z - shakeForce, posi.z + shakeForce);

	At(Vector3(posi.x, posi.y, posi.z));
}
