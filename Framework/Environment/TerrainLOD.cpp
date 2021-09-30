#include "Framework.h"
#include "TerrainLOD.h"

TerrainLOD::TerrainLOD(Shader * shader, wstring heightFile)
	: Renderer(shader)
{
	Topology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	heightMap = new Texture(heightFile);

	vector<Color> pixels;
	heightMap->ReadPixel(DXGI_FORMAT_R8G8B8A8_UNORM, &pixels);

	width = heightMap->GetWidth();
	height = heightMap->GetHeight();

	//heights = new float[width * height * 3];
	heights = new float[width * height];

	for (UINT i = 0; i < pixels.size(); i++)
	{
		heights[i] = (pixels[i].r + pixels[i].g + pixels[i].b) * 255.0f / desc.HeightScale;
	}

	saveVertices = new Vector3[width * height];
	
	for (UINT z = 0; z < width; z++)
	{
		for (UINT x = 0; x < height; x++)
		{
			UINT index = width * z + x;
			float tempX = -(GetWidth() * 0.5f) + x * desc.Cellspacing;
			float tempZ = (GetHeight() * 0.5f) - z * desc.Cellspacing;

			saveVertices[index] = Vector3(tempX, heights[index], tempZ);
		}
	}

	patchWidth = ((width - 1) / cellsPerPatch) + 1;
	patchHeight = ((height - 1) / cellsPerPatch) + 1;

	vertexCount = (patchWidth + 1) * (patchHeight + 1);
	indexCount = patchWidth * patchHeight;

	CalcVertexBounds();
	CreatePatchVertex();
	CreatePatchIndex();

	sHeightMap = shader->AsSRV("HeightMap");
	sBaseMap = shader->AsSRV("BaseMap");

	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_TerrainLOD");

	desc.CellspacingU = 1.0f / (width + 1);
	desc.CellspacingV = 1.0f / (height + 1);

	material = new Material(shader);
}

TerrainLOD::TerrainLOD(Shader * shader, wstring heightFile, float scale)
	: Renderer(shader)
{
	Topology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	desc.Cellspacing *= scale;
	heightMap = new Texture(heightFile);

	vector<Color> pixels;
	heightMap->ReadPixel(DXGI_FORMAT_R8G8B8A8_UNORM, &pixels);

	width = heightMap->GetWidth();
	height = heightMap->GetHeight();

	//heights = new float[width * height * 3];
	heights = new float[width * height];

	for (UINT i = 0; i < pixels.size(); i++)
	{
		heights[i] = (pixels[i].r + pixels[i].g + pixels[i].b) * 255.0f / desc.HeightScale;
	}

	saveVertices = new Vector3[width * height];

	for (UINT z = 0; z < width; z++)
	{
		for (UINT x = 0; x < height; x++)
		{
			UINT index = width * z + x;
			float tempX = -(GetWidth() * 0.5f) + x * desc.Cellspacing;
			float tempZ = (GetHeight() * 0.5f) - z * desc.Cellspacing;

			saveVertices[index] = Vector3(tempX, heights[index], tempZ);
		}
	}

	patchWidth = ((width - 1) / cellsPerPatch) + 1;
	patchHeight = ((height - 1) / cellsPerPatch) + 1;

	vertexCount = (patchWidth + 1) * (patchHeight + 1);
	indexCount = patchWidth * patchHeight;

	CalcVertexBounds();
	CreatePatchVertex();
	CreatePatchIndex();

	sHeightMap = shader->AsSRV("HeightMap");
	sBaseMap = shader->AsSRV("BaseMap");

	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_TerrainLOD");

	desc.CellspacingU = 1.0f / (width + 1);
	desc.CellspacingV = 1.0f / (height + 1);

	material = new Material(shader);
}

TerrainLOD::~TerrainLOD()
{
	SafeDelete(saveVertices);
	SafeDelete(material);
}

void TerrainLOD::Update()
{
	Super::Update();

	Matrix mat;
	Vector3 backward = Context::Get()->GetCamera()->Backward();
	backward *= backwardRate;
	D3DXMatrixIdentity(&mat);
	D3DXMatrixTranslation(&mat, backward.x, backward.y, backward.z);
	D3DXMatrixInverse(&mat, NULL, &mat);
	D3DXMatrixTranspose(&mat, &mat);
	D3DXPlaneTransform(&planes[2], &planes[2], &mat);
	
	desc.CullPlane[0] = planes[0];
	desc.CullPlane[1] = planes[1];
	desc.CullPlane[2] = planes[2];
	desc.CullPlane[3] = planes[3];
	desc.CullPlane[4] = planes[4];
	desc.CullPlane[5] = planes[5];
}

void TerrainLOD::Render()
{
	Super::Render();
	material->Render();

	sHeightMap->SetResource(heightMap->SRV());
	sBaseMap->SetResource(baseMap->SRV());

	buffer->Render();
	sBuffer->SetConstantBuffer(buffer->Buffer());

	shader->DrawIndexed(0, Pass(), indexCount * 4);
}

float TerrainLOD::GetWidth()
{
	return width * desc.Cellspacing;
}

float TerrainLOD::GetHeight()
{
	return height * desc.Cellspacing;
}

void TerrainLOD::BaseMap(wstring file)
{
	SafeDelete(baseMap);

	baseMap = new Texture(file);
}

float TerrainLOD::GetHeight(Vector3 position)
{
	float x = position.x;
	float z = position.z;

	float halfWidth = GetWidth() * 0.5f; //실제 절반 넓이 
	float halfHeight = GetHeight() * 0.5f; //실제 절반 높이

	if (x < -halfWidth || x > halfWidth) return 0.0f;
	if (z < -halfHeight || z >= halfHeight) return 0.0f;

	x += halfWidth; 
	z += halfHeight;

	x /= desc.Cellspacing;
	z /= desc.Cellspacing;
	
	UINT tempX = (UINT)x;
	UINT tempZ = (UINT)z;
	tempZ = (height - 1) - tempZ;

	UINT index[4];

	index[1] = width * tempZ + tempX;
	index[0] = width * (tempZ + 1) + tempX;
	index[3] = width * tempZ + tempX + 1;
	index[2] = width * (tempZ + 1) + tempX + 1;

	Vector3 v[4];
	for (int i = 0; i < 4; i++)
		v[i] = saveVertices[index[i]];

	float ddx = (position.x - v[0].x) / desc.Cellspacing;
	float ddz = (position.z - v[0].z) / desc.Cellspacing;

	Vector3 result;

	if (ddx + ddz <= 1.0f)
		result = v[0] + (v[2] - v[0]) * ddx + (v[1] - v[0]) * ddz;
	else
	{
		ddx = 1.0f - ddx;
		ddz = 1.0f - ddz;

		result = v[3] + (v[1] - v[3]) * ddx + (v[2] - v[3]) * ddz;
	}

	return result.y;

	//Vector3 start(position.x, position.y, position.z);
	//Vector3 direction(0, -1, 0);

	//float u, a, distance;
	////Vector3 result(-1, FLT_MIN, -1);

	//if (D3DXIntersectTri(&v[0], &v[1], &v[2], &start, &direction, &u, &a, &distance) == TRUE)
	//	result = v[0] + (v[1] - v[0]) * u + (v[2] - v[0]) * a;

	//if (D3DXIntersectTri(&v[3], &v[1], &v[2], &start, &direction, &u, &a, &distance) == TRUE)
	//	result = v[3] + (v[1] - v[3]) * u + (v[2] - v[3]) * a;

	//return result.y;
}

void TerrainLOD::GetCullingPlanes(Plane * planes)
{
	memcpy(this->planes, planes, sizeof(Plane) * 6);
}

void TerrainLOD::CalcVertexBounds()
{
	bounds = new Vector2[vertexCount];

	for (UINT z = 0; z < patchHeight + 1; z++)
	{
		for (UINT x = 0; x < patchWidth + 1; x++)
		{
			UINT x0 = x * cellsPerPatch;
			UINT x1 = (x + 1) * cellsPerPatch;

			UINT z0 = z * cellsPerPatch;
			UINT z1 = (z + 1) * cellsPerPatch;


			float minY = +FLT_MAX;
			float maxY = -FLT_MAX;

			for (UINT tempZ = z0; tempZ <= z1; tempZ++)
			{
				for (UINT tempX = x0; tempX <= x1; tempX++)
				{
					UINT k = z * width + x;

					minY = min(minY, heights[k]);
					maxY = max(maxY, heights[k]);
				}
			}

			UINT patchID = z * patchWidth + x;
			bounds[patchID] = Vector2(minY, maxY);
		}//for(x)
	}//for(z)
}

void TerrainLOD::CreatePatchVertex()
{
	vector<VertexTerrainLOD> vertices(vertexCount);

	float halfWidth = GetWidth() * 0.5f; //실제 절반 넓이 
	float halfHeight = GetHeight() * 0.5f; //실제 절반 높이

	float tempWidth = GetWidth() / patchWidth; //Patch당 넓이
	float tempHeight = GetHeight() / patchHeight; //Patch당 높이
	float du = 1.0f / patchWidth;
	float dv = 1.0f / patchHeight;

	for (UINT z = 0; z < patchHeight + 1; z++)
	{
		float tempZ = halfHeight - z * tempHeight; //halfHeight가 실제 높이의 절반이니 중점에서 맨위로 이동 한후 z * tempHeight

		for (UINT x = 0; x < patchWidth + 1; x++)
		{
			float tempX = -halfWidth + x * tempWidth;

			vertices[z * (patchWidth + 1) + x].Position = Vector3(tempX, 0.0f, tempZ);
			vertices[z * (patchWidth + 1) + x].Uv.x = x * du;
			vertices[z * (patchWidth + 1) + x].Uv.y = z * dv;
		}
	}

	for (UINT z = 0; z < patchHeight + 1; z++)
	{
		for (UINT x = 0; x < patchWidth + 1; x++)
		{
			UINT patchID = z * patchWidth + x;

			vertices[z * (patchWidth + 1) + x].Bound = bounds[patchID];
		}
	}

	vertexBuffer = new VertexBuffer(&vertices[0], vertices.size(), sizeof(VertexTerrainLOD));
}

void TerrainLOD::CreatePatchIndex()
{
	vector<UINT> indices(indexCount * 4);

	UINT index = 0;
	for (UINT z = 0; z < patchHeight; z++)
	{
		for (UINT x = 0; x < patchWidth; x++)
		{
			indices[index + 0] = z * (patchWidth + 1) + x;
			indices[index + 1] = z * (patchWidth + 1) + x + 1;
			indices[index + 2] = (z + 1) * (patchWidth + 1) + x;
			indices[index + 3] = (z + 1) * (patchWidth + 1) + x + 1;

			index += 4;
		}
	}

	indexBuffer = new IndexBuffer(&indices[0], indices.size());
}
