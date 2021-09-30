#include "stdafx.h"
#include "ComputeDemo.h"

void ComputeDemo::Initialize()
{
	//Raw();
	Texture2D();
}

void ComputeDemo::Update()
{
	render2D->Update();
}

void ComputeDemo::PostRender()
{
	render2D->Render();
}

void ComputeDemo::Raw()
{
	Shader* shader = new Shader(L"24_ByteAddress.fx");

	
	UINT count = 2 * 10 * 8 * 3;

	struct Output
	{
		UINT GroupId[3];
		UINT GroupThreadId[3];
		UINT DispatchThreadId[3];
		UINT GroupIndex;
		float RetValue;
	};

	struct Input
	{
		float Value = 0.0f;
	};
	Input* input = new Input[count];

	for (UINT i = 0; i < count; i++)
		input[i].Value = Math::Random(0.0f, 10000.0f);

	
	//RawBuffer* rawBuffer = new RawBuffer(NULL, 0, sizeof(Output) * count);
	RawBuffer* rawBuffer = new RawBuffer(input, sizeof(Input) * count, sizeof(Output) * count);

	shader->AsUAV("Output")->SetUnorderedAccessView(rawBuffer->UAV());
	shader->AsSRV("Input")->SetResource(rawBuffer->SRV());

	shader->Dispatch(0, 0, 2, 1, 1);


	Output* output = new Output[count];
	rawBuffer->CopyFromOutput(output);

	FILE* file;
	fopen_s(&file, "../Raw.csv", "w");

	for (UINT i = 0; i < count; i++)
	{
		Output temp = output[i];

		//fprintf
		//(
		//	file,
		//	"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		//	i,
		//	temp.GroupId[0], temp.GroupId[1], temp.GroupId[2],
		//	temp.GroupThreadId[0], temp.GroupThreadId[1], temp.GroupThreadId[2],
		//	temp.DispatchThreadId[0], temp.DispatchThreadId[1], temp.DispatchThreadId[2],
		//	temp.GroupIndex
		//);

		fprintf
		(
			file,
			"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f\n",
			i,
			temp.GroupId[0], temp.GroupId[1], temp.GroupId[2],
			temp.GroupThreadId[0], temp.GroupThreadId[1], temp.GroupThreadId[2],
			temp.DispatchThreadId[0], temp.DispatchThreadId[1], temp.DispatchThreadId[2],
			temp.GroupIndex, temp.RetValue
		);
	}
	fclose(file);
}

void ComputeDemo::Texture2D()
{
	texture = new Texture(L"Environment/Compute.png");

	render2D = new Render2D();
	render2D->GetTransform()->Scale(D3D::Width(), D3D::Height(), 1);
	render2D->GetTransform()->Position(D3D::Width() * 0.5f, D3D::Height() * 0.5f, 0);
	//render2D->SRV(texture->SRV());

	Shader* shader = new Shader(L"24_Texture.fxo");
	textureBuffer = new TextureBuffer(texture->GetTexture());

	shader->AsSRV("Input")->SetResource(textureBuffer->SRV());
	shader->AsUAV("Output")->SetUnorderedAccessView(textureBuffer->UAV());


	UINT width = textureBuffer->Width();
	UINT height = textureBuffer->Height();
	UINT arraySize = textureBuffer->ArraySize();

	float x = ((float)width / 32.0f) < 1.0f ? 1.0f : ((float)width / 32.0f);
	float y = ((float)height / 32.0f) < 1.0f ? 1.0f : ((float)height / 32.0f);

	shader->Dispatch(0, 0, (UINT)ceilf(x), (UINT)ceilf(y), arraySize);

	render2D->SRV(textureBuffer->OutputSRV());
}
