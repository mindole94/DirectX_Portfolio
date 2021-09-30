#include "Framework.h"
#include "Sky.h"
#include "Scattering.h"
#include "Dome.h"
#include "Moon.h"
#include "Cloud.h"

Sky::Sky(Shader * shader)
	: shader(shader)
{
	scatterDesc.InvWaveLength.x = 1.0f / powf(scatterDesc.WaveLength.x, 4.0f);
	scatterDesc.InvWaveLength.y = 1.0f / powf(scatterDesc.WaveLength.y, 4.0f);
	scatterDesc.InvWaveLength.z = 1.0f / powf(scatterDesc.WaveLength.z, 4.0f);

	scatterDesc.WaveLengthMie.x = powf(scatterDesc.WaveLength.x, -0.84f);
	scatterDesc.WaveLengthMie.y = powf(scatterDesc.WaveLength.y, -0.84f);
	scatterDesc.WaveLengthMie.z = powf(scatterDesc.WaveLength.z, -0.84f);

	scattering = new Scattering(shader);
	scatterBuffer = new ConstantBuffer(&scatterDesc, sizeof(ScatterDesc));
	sScatterBuffer = shader->AsConstantBuffer("CB_Scattering");


	dome = new Dome(shader, Vector3(0, 32, 0), Vector3(500, 500, 500));
	moon = new Moon(shader);

	cloud = new Cloud(shader, Vector3(0, 32, 0), Vector3(450, 450, 450));
	//cloud = new Cloud(shader);
	//cloud->GetTransform()->Scale(50, 50, 50);
	//cloud->GetTransform()->Scale(50, 50, 50);
	//cloud->GetTransform()->Rotation(-Math::PI * 0.5f, 0, 0);
	cloudBuffer = new ConstantBuffer(&cloudDesc, sizeof(CloudDesc));
	sCloudBuffer = shader->AsConstantBuffer("CB_Cloud");


	sRayleighMap = shader->AsSRV("RayleighMap");
	sMieMap = shader->AsSRV("MieMap");
}

Sky::~Sky()
{
	SafeDelete(scattering);
	SafeDelete(scatterBuffer);

	SafeDelete(dome);
	SafeDelete(moon);

	SafeDelete(cloud);
	SafeDelete(cloudBuffer);
}

void Sky::Pass(UINT pass)
{
	Pass(pass + 0, pass + 1, pass + 2, pass + 3);
}

void Sky::Pass(UINT scatteringPass, UINT domePass, UINT moonPass, UINT cloudPass)
{
	scattering->Pass(scatteringPass);
	dome->Pass(domePass);
	moon->Pass(moonPass);
	cloud->Pass(cloudPass);
}

void Sky::Update()
{
	if (bRealTime == true)
	{
		theta += Time::Delta() * timeFactor;

		if (theta > Math::PI)
			theta -= Math::PI * 2.0f;


		float x = sinf(theta);
		float y = cosf(theta);

		Context::Get()->Direction() = Vector3(x, y, 0.0f);
	}
	else
	{
		ImGui::SliderFloat("Theta", &theta, -Math::PI, Math::PI);

		float x = sinf(theta);
		float y = cosf(theta);

		Context::Get()->Direction() = Vector3(x, y, 0.0f);
	}


	//Vector3 domePosition;
	//Context::Get()->GetCamera()->Position(&domePosition);
	//dome->GetTransform()->Position(domePosition);
	//domePosition.y += 100.0f;
	//cloud->GetTransform()->Position(domePosition);

	dome->GetTransform()->Position(playerPosition);
	moon->GetPlayerPosition(playerPosition);

	scattering->Update();
	dome->Update();
	moon->Update();
	cloud->Update();
}

void Sky::PreRender()
{
	scatterBuffer->Render();
	sScatterBuffer->SetConstantBuffer(scatterBuffer->Buffer());

	scattering->PreRender();
}

void Sky::Render()
{
	//Dome
	{
		sRayleighMap->SetResource(scattering->RayleighRTV()->SRV());
		sMieMap->SetResource(scattering->MieRTV()->SRV());

		dome->Render();
	}

	//Moon
	{
		moon->Render(theta);
	}

	//Cloud
	{
		cloudBuffer->Render();
		sCloudBuffer->SetConstantBuffer(cloudBuffer->Buffer());

		cloud->Render();
	}
}

void Sky::PostRender()
{
	scattering->PostRender();
	cloud->PostRender();
}

void Sky::RealTime(bool val, float theta, float timeFactor)
{
	bRealTime = val;

	this->theta = theta;
	this->timeFactor = timeFactor;
}
