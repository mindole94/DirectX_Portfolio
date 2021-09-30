#pragma once

#include "Systems/IExecute.h"

class Test : public IExecute
{
public:
	virtual void Initialize() override;
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override {}
	virtual void ResizeScreen() override {}

private:
	Shader* shader;

	class Player* player = NULL;
	class Monster1* monster1 = NULL;
	class Monster2* monster2 = NULL;

	RenderTarget* renderTarget;
	DepthStencil* depthStencil;
	Viewport* viewport;

	ID3DX11EffectShaderResourceVariable* sDiffuseMap;


	Sky* sky;

};
