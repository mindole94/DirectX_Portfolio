#pragma once

class Moon : public Renderer
{
public:
	Moon(Shader* shader);
	~Moon();

	void Update();
	void Render(float theta);

	void GetPlayerPosition(Vector3 vec) { playerPosition = vec; }

private:
	float GetAlpha(float theta);

	Matrix GetTransform(float theta);
	Matrix GetGlowTransform(float theta);

private:
	float distance, glowDistance;

	ID3DX11EffectScalarVariable* sAlpha;
	float al = 0.5f;

	Texture* moon;
	Texture* moonGlow;
	ID3DX11EffectShaderResourceVariable* sMoon;

	Vector3 playerPosition;
};