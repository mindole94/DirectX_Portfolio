#pragma once
#include "Systems/IExecute.h"

class ComputeDemo : public IExecute
{
public:
	virtual void Initialize() override;
	virtual void Destroy() override {}
	virtual void Update() override;
	virtual void PreRender() override {}
	virtual void Render() override {}
	virtual void PostRender() override;
	virtual void ResizeScreen() override {}

private:
	void Raw();
	void Texture2D();

private:
	Texture* texture;

	TextureBuffer* textureBuffer;

	Render2D* render2D;
};