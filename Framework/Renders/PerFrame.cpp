#include "Framework.h"
#include "PerFrame.h"

PerFrame::PerFrame(Shader * shader)
	: shader(shader)
{
	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_PerFrame");

	lightBuffer = new ConstantBuffer(&lightDesc, sizeof(LightDesc));
	sLightBuffer = shader->AsConstantBuffer("CB_Light");

	pointLightBuffer = new ConstantBuffer(&pointLightDesc, sizeof(PointLightDesc));
	sPointLightBuffer = shader->AsConstantBuffer("CB_PointLights");

	spotLightBuffer = new ConstantBuffer(&spotLightDesc, sizeof(SpotLightDesc));
	sSpotLightBuffer = shader->AsConstantBuffer("CB_SpotLights");

	ZeroMemory(desc.Culling, sizeof(Plane) * 4);
	ZeroMemory(desc.Clipping, sizeof(Plane));

	perspective = new Perspective(D3D::Width(), D3D::Height(), 0.1f, zFar, Math::PI * fov);
	frustum = new Frustum(Context::Get()->GetCamera(), perspective);
}

PerFrame::~PerFrame()
{
	SafeDelete(buffer);
	SafeDelete(lightBuffer);
	SafeDelete(pointLightBuffer);
	SafeDelete(spotLightBuffer);

	SafeDelete(perspective);
	SafeDelete(frustum);
}

void PerFrame::Update()
{
	desc.Time = Time::Get()->Running();

	lightDesc.Ambient = Context::Get()->Ambient();
	lightDesc.Specular = Context::Get()->Specular();
	lightDesc.Direction = Context::Get()->Direction();
	lightDesc.Position = Context::Get()->Position();

	pointLightDesc.Count = Lighting::Get()->PointLights(pointLightDesc.Lights);
	spotLightDesc.Count = Lighting::Get()->SpotLights(spotLightDesc.Lights);
	
	static float fov = 0.31f, zFar = 3000.0f;
	static int a = 0;
	const int c = 95;

	if (Context::Get()->GetOnOff() == true)
	{
		if (a == c)
		{
			ImGui::Begin("Setting");
			{
				ImGui::SliderFloat("fov", &fov, 0.001f, 0.31f);
				ImGui::SliderFloat("zFar", &zFar, 0.001f, 3000.0f);
			}
			ImGui::End();
		}
	}
	a++;

	if (a > c)
	{
		//std::cout << b << " " << a << std::endl;
		a -= c + 1;
	}

	perspective->Set(D3D::Width(), D3D::Height(), 0.1f, zFar, Math::PI * fov);
	Context::Get()->GetPerspective()->Set(D3D::Width(), D3D::Height(), 0.1f, zFar, Math::PI * 0.31f);
	frustum->Update();
}

void PerFrame::Render()
{
	desc.View = Context::Get()->View();
	D3DXMatrixInverse(&desc.ViewInverse, NULL, &desc.View);

	desc.Projection = Context::Get()->Projection();
	desc.VP = desc.View * desc.Projection;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	desc.Clipping = Context::Get()->Clipping();

	Plane planes[6];
	frustum->Planes(planes);

	desc.Culling[0] = planes[0];
	desc.Culling[1] = planes[1];
	desc.Culling[2] = planes[4];
	desc.Culling[3] = planes[5];
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	buffer->Render();
	sBuffer->SetConstantBuffer(buffer->Buffer());

	lightBuffer->Render();
	sLightBuffer->SetConstantBuffer(lightBuffer->Buffer());

	pointLightBuffer->Render();
	sPointLightBuffer->SetConstantBuffer(pointLightBuffer->Buffer());

	spotLightBuffer->Render();
	sSpotLightBuffer->SetConstantBuffer(spotLightBuffer->Buffer());
}

void PerFrame::Culling(Plane * planes)
{
	memcpy(desc.Culling, planes, sizeof(Plane) * 4);
}

void PerFrame::Clipping(Plane & plane)
{
	desc.Clipping = plane;
}

void PerFrame::GetFrustumPlanes(Plane * planes)
{
	frustum->Planes(planes);
}
