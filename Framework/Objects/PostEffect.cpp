#include "Framework.h"
#include "PostEffect.h"

PostEffect::PostEffect(wstring shaderFile)
	: Renderer(shaderFile)
{
	VertexTexture vertices[6];
	vertices[0].Position = Vector3(-1.0f, -1.0f, 0.0f);
	vertices[1].Position = Vector3(-1.0f, +1.0f, 0.0f);
	vertices[2].Position = Vector3(+1.0f, -1.0f, 0.0f);
	vertices[3].Position = Vector3(+1.0f, -1.0f, 0.0f);
	vertices[4].Position = Vector3(-1.0f, +1.0f, 0.0f);
	vertices[5].Position = Vector3(+1.0f, +1.0f, 0.0f);

	vertexBuffer = new VertexBuffer(vertices, 6, sizeof(VertexTexture));
	sDiffuseMap = shader->AsSRV("DiffuseMap");
	sBloomMap = shader->AsSRV("BloomMap");
	sAfterImageMap = shader->AsSRV("AfterImageMap");

	float width = D3D::Width(), height = D3D::Height();
	pixelSize = Vector2(1.0f / D3D::Width(), 1.0f / D3D::Height());

	for (UINT i = 0; i < 10; i++)
		afterImageTarget[i] = new RenderTarget((UINT)width, (UINT)height);
	
	for (UINT i = 0; i < 4; i++)
		bloomTarget[i] = new RenderTarget((UINT)width, (UINT)height);

	resultBloom = new RenderTarget((UINT)width, (UINT)height);
	resultAfterImage = new RenderTarget((UINT)width, (UINT)height);
}

PostEffect::~PostEffect()
{
	SafeDelete(resultAfterImage);
	for (UINT i = 0; i < 11; i++)
		SafeDelete(afterImageTarget[i]);

	for (UINT i = 0; i < 4; i++)
		SafeDelete(bloomTarget[i]);
}

void PostEffect::Update()
{
	Super::Update();

}

void PostEffect::PreRender_AfterImage(RenderTarget * afterImageTarget, DepthStencil * depthStencil, Viewport * viewport)
{
	{
		shader->AsVector("pixelSize")->SetFloatVector(pixelSize);
		shader->AsScalar("threshold_Bloom")->SetFloat(threshold);

		static float currentTime = 0.0f;
		static float nextFrame = 1.0f * 0.03f;
		static UINT index = 0;

		if (Time::Get()->Running() - currentTime >= nextFrame)
		{
			currentTime = Time::Get()->Running();

			index %= 10;
			
			this->afterImageTarget[index]->PreRender(depthStencil);
			index++;
			
			Pass(0);
			SRV(afterImageTarget->SRV());
			PreRender();
		}
	}

	//Total Render
	{
		resultAfterImage->PreRender(depthStencil);

		ID3D11ShaderResourceView* srvs[10];

		for (UINT i = 0; i < 10; i++)
		{
			srvs[i] = this->afterImageTarget[i]->SRV();
		}

		Pass(11);
		GetShader()->AsSRV("AfterImageMaps")->SetResourceArray(srvs, 0, 10);
		PreRender();
	}
}

void PostEffect::PreRender_Bloom(RenderTarget * afterImageTarget, DepthStencil * depthStencil, Viewport * viewport)
{
	//Luminosity
	{
		shader->AsVector("pixelSize")->SetFloatVector(pixelSize);

		this->bloomTarget[0]->PreRender(depthStencil);

		Pass(7);
		SRV(afterImageTarget->SRV());
		PreRender();
	}

	SetBlur();

	//BlurX
	{
		shader->AsScalar("weights_Bloom")->SetFloatArray(&weightX[0], 0, weightX.size());
		shader->AsVector("offsets_Bloom")->SetRawValue(&offsetX[0], 0, sizeof(Vector2) * offsetX.size());


		this->bloomTarget[1]->PreRender(depthStencil);
		viewport->RSSetViewport();

		SRV(this->bloomTarget[0]->SRV());
		Pass(8);
		PreRender();
	}

	//BlurY
	{
		shader->AsScalar("weights_Bloom")->SetFloatArray(&weightY[0], 0, weightY.size());
		shader->AsVector("offsets_Bloom")->SetRawValue(&offsetY[0], 0, sizeof(Vector2) * offsetY.size());


		this->bloomTarget[2]->PreRender(depthStencil);
		viewport->RSSetViewport();

		SRV(this->bloomTarget[1]->SRV());
		Pass(8);
		PreRender();
	}

	//Comsite
	{
		this->bloomTarget[3]->PreRender(depthStencil);
		viewport->RSSetViewport();

		shader->AsSRV("LuminosityMap")->SetResource(this->bloomTarget[0]->SRV());
		shader->AsSRV("BlurMap")->SetResource(this->bloomTarget[2]->SRV());

		Pass(9);
		PreRender();
	}

	//Color Grading
	{
		resultBloom->PreRender(depthStencil);
		viewport->RSSetViewport();

		Pass(10); 
		SRV(this->bloomTarget[3]->SRV());
		PreRender();
	}
}

void PostEffect::PreRender()
{
	Super::Render();

	shader->Draw(0, Pass(), 6);
}

void PostEffect::Render()
{
	ImGuiSetting();

	if (bAfterImage == true)
	{
		Pass(12);
		sBloomMap->SetResource(resultBloom->SRV());
		sAfterImageMap->SetResource(resultAfterImage->SRV());
	}
	else
	{
		Pass(0);
		if (renderIndex == 4)
			SRV(resultBloom->SRV());
		else
			SRV(this->bloomTarget[renderIndex]->SRV());
	}

	Super::Render();

	shader->Draw(0, Pass(), 6);
}

void PostEffect::SRV(ID3D11ShaderResourceView * srv)
{
	sDiffuseMap->SetResource(srv);
}

void PostEffect::SetBlur()
{
	float x = 1.0f / D3D::Width();
	float y = 1.0f / D3D::Height();

	GetBlurParameter(weightX, offsetX, x, 0);
	GetBlurParameter(weightY, offsetY, 0, y);
}

void PostEffect::GetBlurParameter(vector<float>& weights, vector<Vector2>& offsets, float x, float y)
{
	weights.clear();
	weights.assign(blurCount, float());

	offsets.clear();
	offsets.assign(blurCount, Vector2());

	weights[0] = GetGaussFunction(0); //1
	offsets[0] = Vector2(0, 0);


	float sum = weights[0];
	for (UINT i = 0; i < blurCount / 2; i++)
	{
		float temp = GetGaussFunction((float)(i + 1));

		weights[i * 2 + 1] = temp;
		weights[i * 2 + 2] = temp;
		sum += temp * 2;

		Vector2 temp2 = Vector2(x, y) * (i * 2 + 1.5f);
		offsets[i * 2 + 1] = temp2;
		offsets[i * 2 + 2] = -temp2;
	}

	for (UINT i = 0; i < blurCount; i++)
		weights[i] /= sum;
}

float PostEffect::GetGaussFunction(float val)
{
	return (float)((1.0 / sqrt(2 * Math::PI * blurCount)) * exp(-(val * val) / (2 * blurCount * blurCount)));
}

void PostEffect::ImGuiSetting()
{
	ImGui::Begin("PostEffect");
	{
		ImGui::SliderInt("Index", &renderIndex, 0, 4);

		ImGui::InputFloat("Threshold", &threshold, 0.01f);
		shader->AsScalar("threshold_Bloom")->SetFloat(threshold);

		ImGui::InputInt("BlurCount", (int *)&blurCount, 2);
		blurCount = Math::Clamp<UINT>(blurCount, 1, 33);

		ImGui::End();
	}
}