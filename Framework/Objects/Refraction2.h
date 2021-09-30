#pragma once

class Refraction2
{
public:
	Refraction2(Shader* shader, wstring normalFile, UINT width = 0, UINT height = 0);
	~Refraction2();

	void Update();

	void PreRender();
	void Render();

private:
	Shader* shader;

	UINT width, height;
	RenderTarget* renderTarget;
	DepthStencil* depthStencil;
	Viewport* viewport;

	ID3DX11EffectShaderResourceVariable* sRefraction2SRV;

	Texture* normalMap;
	ID3DX11EffectShaderResourceVariable* sNormalMap;
};