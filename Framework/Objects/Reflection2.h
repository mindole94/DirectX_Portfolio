#pragma once

class Reflection2
{
public:
	Reflection2(Shader* shader, Transform* transform, UINT width = 0, UINT height = 0);
	~Reflection2();

	void Update();

	void PreRender();
	void Render();

private:
	Shader* shader;

	Transform* transform;
	Fixity* camera;

	UINT width, height;
	RenderTarget* renderTarget;
	DepthStencil* depthStencil;
	Viewport* viewport;

	ID3DX11EffectShaderResourceVariable* sReflection2SRV;
	ID3DX11EffectMatrixVariable* sReflection2View;
};