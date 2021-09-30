#pragma once

#define MAX_TRAIL_NUM 1000

class TrailSystem : public Renderer
{
public:
	TrailSystem();
	~TrailSystem();

	void Update();
	void Render();

	void Reset();

	void Add(Vector3 start, Vector3 end);
	void Delete(bool bDelete) { this->bDelete = bDelete; }

	void TrailMap(string file);
	void TrailMap(wstring file);

	wstring TrailMapName() { return trailMap->GetFile(); }

	void UseColor(bool use) { bColor = use; }

	void SetCount(int num) { count = num; }

private:
	void Delete();

private:
	struct VertexTrail
	{
		Vector3 Position;
		Vector2 Uv;

		Color Color;
	};

	struct TrailPosition
	{
		Vector3 Bottom;
		Vector3 Top;
	};

private:
	Texture* trailMap = NULL;
	ID3DX11EffectShaderResourceVariable* sTrailMap;

	VertexTrail* vertices = NULL;
	UINT* indices = NULL;

	bool bDelete = false;

	float deleteTime = 0.0f;

	Color color = Color(1, 1, 0, 1);
	bool bColor = false;

	vector<TrailPosition> position; //벡터 한개당 vertices (count * 2)개씩
	int count = 5;
};
