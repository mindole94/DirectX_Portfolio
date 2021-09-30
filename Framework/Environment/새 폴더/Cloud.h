#pragma once

class Cloud : public Renderer
{
public:
	Cloud(Shader* shader);
	Cloud(Shader* shader, Vector3 position, Vector3 scale, UINT drawCount = 32);
	~Cloud();

	void Update();
	void Render();
	void PostRender();

private:
	void CreateTexture();

private:
	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* srv;
	ID3DX11EffectShaderResourceVariable* sSRV;

	Render2D* render2D;

	UINT drawCount;
};