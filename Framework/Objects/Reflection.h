#pragma once

class Reflection
{
public:
	Reflection(Shader* shader, Transform* transform, UINT width = 0, UINT height = 0);
	~Reflection();

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

	ID3DX11EffectShaderResourceVariable* sReflectionSRV;
	ID3DX11EffectMatrixVariable* sReflectionView;
};