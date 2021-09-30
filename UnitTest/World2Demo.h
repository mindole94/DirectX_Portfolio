#pragma once
#include "Systems/IExecute.h"

class World2Demo : public IExecute
{
public:
	virtual void Initialize() override;
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override {}
	virtual void Render() override;
	virtual void PostRender() override {}
	virtual void ResizeScreen() override {}

private:
	Shader* shader;

	Vertex vertices[6];
	ID3D11Buffer* vertexBuffer;

	
	Vector3 position[2] = { Vector3(0, 0, 0), Vector3(0, 0, 0) };
	Vector3 scale[2] = { Vector3(1, 1, 1), Vector3(1, 1, 1) };
};