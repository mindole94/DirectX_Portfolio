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

	float size = 500.0f;

	dome = new Dome(shader, Vector3(0, 32, 0), Vector3(size, size, size));
	moon = new Moon(shader);

	size -= 5.0f;
	cloud = new Cloud(shader, Vector3(0, 32, 0), Vector3(size, size, size));
	
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
		float x = sinf(theta);
		float y = cosf(theta);
		
		Context::Get()->Direction() = Vector3(x, y, 0.0f);
	}
	
	float area = 450.0f;
	playerPosition.x = Math::Clamp(playerPosition.x, -area, area);
	playerPosition.z = Math::Clamp(playerPosition.z, -area, area);
	static float a = 50.0f;
	playerPosition.y = a;
	//ImGui::SliderFloat("AA", &a, 100.0f, 250.0f);
	
	Vector3 pos = Vector3(0, 0, 0);
	pos.y = 200.0f;
	/*dome->GetTransform()->Position(pos);
	cloud->GetTransform()->Position(pos);
	moon->GetPlayerPosition(pos);*/
	dome->GetTransform()->Position(playerPosition);
	cloud->GetTransform()->Position(playerPosition);
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
