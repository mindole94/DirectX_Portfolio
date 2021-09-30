#pragma once
#include "Systems/IExecute.h"

class TextureSamplerDemo : public IExecute
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

	VertexTexture* vertices;
	ID3D11Buffer* vertexBuffer;

	UINT* indices;
	ID3D11Buffer* indexBuffer;

	Texture* texture = NULL;
};