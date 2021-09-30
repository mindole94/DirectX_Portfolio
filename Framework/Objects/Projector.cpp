#include "Framework.h"
#include "Projector.h"

Projector::Projector(Shader * shader, wstring mapFile, UINT width, UINT height)
	: shader(shader), width(width), height(height)
{
	camera = new Fixity();
	camera->Position(0, 0, -20);

	camera->Position(15, 30, 0);
	camera->RotationDegree(90, 0, 0);
	projection = new Orthographic((float)width, (float)height);
	((Orthographic *)projection)->Set((float)width, (float)height, 1.0f, 100.0f);

	map = new Texture(mapFile);
	buffer = new ConstantBuffer(&desc, sizeof(Desc));

	sMap = shader->AsSRV("ProjectorMap");
	sMap->SetResource(map->SRV());

	sBuffer = shader->AsConstantBuffer("CB_Projector");
}

Projector::~Projector()
{
	SafeDelete(camera);
	SafeDelete(projection);

	SafeDelete(map);
	SafeDelete(buffer);
}

void Projector::Update()
{
	camera->GetMatrix(&desc.View);
	projection->GetMatrix(&desc.Projection);
}

void Projector::Render()
{
	buffer->Render();
	sBuffer->SetConstantBuffer(buffer->Buffer());
}

Vector3 Projector::GetPosition()
{
	Vector3 pos;
	camera->Position(&pos);
	return pos;
}

void Projector::SetPosition(Vector3 vec)
{
	camera->Position(vec);
}

Vector3 Projector::GetRotation()
{
	Vector3 rot;
	camera->Rotation(&rot);
	return rot;
}

void Projector::SetRotation(Vector3 vec)
{
	camera->Rotation(vec);
}

