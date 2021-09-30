#pragma once

#define MAX_TRAIL_NUM 800

class TrailSystem : public Renderer
{
public:
	TrailSystem(wstring file, bool bColor = false);
	~TrailSystem();

	void Reset();

	void Add(Vector3 start, Vector3 end);
	void Delete(bool bDelete) { this->bDelete = bDelete; }

	void Update();
	void Render();

	void SetColor(Color color);

private:
	void Delete();
	void ReadFile(wstring file);

private:
	struct VertexTrail
	{
		Vector3 Position;
		Vector2 Uv;

		float padding[3];
	};

private:
	Texture* map = NULL;
	ID3DX11EffectShaderResourceVariable* sMap;

	VertexTrail* vertices = NULL;
	UINT* indices = NULL;

	bool bDelete = false;
	
	float deleteTime = 0.0f;

	UINT deleteCount = 0;
	UINT currentCount = 0;
	UINT activeVertex = 0;

	Vector3 firstStart;
	Vector3 firstEnd;

	Vector3 prevStart;
	Vector3 prevEnd;

	Color color;
};
