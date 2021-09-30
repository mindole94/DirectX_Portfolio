#pragma once

class Projector
{
public:
	Projector(Shader* shader, wstring mapFile, UINT width, UINT height);
	~Projector();

	void Update();
	void Render();

	Camera* GetCamera() { return (Camera *)camera; }

	Vector3 GetPosition();
	void SetPosition(Vector3 vec);

	Vector3 GetRotation();
	void SetRotation(Vector3 vec);

private:
	struct Desc
	{
		Color Color = D3DXCOLOR(1, 1, 1, 1);

		Matrix View;
		Matrix Projection;
	} desc;

private:
	Shader* shader;

	UINT width, height;

	Fixity* camera;
	Projection* projection;

	Texture* map;
	ID3DX11EffectShaderResourceVariable* sMap;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;
};