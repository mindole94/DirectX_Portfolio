#pragma once

class Sky
{
public:
	Sky(Shader* shader);
	~Sky();

	void Pass(UINT pass);
	void Pass(UINT scatteringPass, UINT domePass, UINT moonPass, UINT cloudPass);

	void Update();
	void PreRender();
	void Render();
	void PostRender();

	void RealTime(bool val, float theta, float timeFactor = 1.0f);

	void GetPlayerPosition(Vector3 vec) { playerPosition = vec; }

private:
	struct ScatterDesc
	{
		Vector3 WaveLength = Vector3(0.65f, 0.57f, 0.475f);
		float Padding;

		Vector3 InvWaveLength;
		int SampleCount = 8;//HDR(High Dynamic Range)을 몇번할것이냐

		Vector3 WaveLengthMie;
		float Padding2;
	} scatterDesc;

	struct CloudDesc
	{
		float Tiles = 1.5f;
		float Cover = 0.005f;
		float Sharpness = 0.405f;
		float Speed = 0.05f;
	} cloudDesc;

private:
	Shader* shader;

	bool bRealTime = false;

	float timeFactor = 1.0f;
	float theta = Math::PI;

	class Scattering* scattering;
	ConstantBuffer* scatterBuffer;
	ID3DX11EffectConstantBuffer* sScatterBuffer;

	ID3DX11EffectShaderResourceVariable* sRayleighMap;//레일리산란 srv
	ID3DX11EffectShaderResourceVariable* sMieMap;//미에산란 srv


	class Dome* dome;
	class Moon* moon;

	class Cloud* cloud;
	ConstantBuffer* cloudBuffer;
	ID3DX11EffectConstantBuffer* sCloudBuffer;

	Vector3 playerPosition;
};