#pragma once

class Render2D : public Renderer
{
public:
	Render2D();
	~Render2D();

	void Update();
	void Render();

	void SRV(ID3D11ShaderResourceView* srv);
	void ChangeColorPoint(float point) { changeColorPoint = point; }

private:
	struct Desc
	{
		Matrix View;
		Matrix Projection;
	} desc;

private:
	ConstantBuffer* buffer;
	ID3DX11EffectShaderResourceVariable* sDiffuseMap;

	float changeColorPoint = 1.0f;
};