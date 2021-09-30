#include "Framework.h"
#include "SeaLOD.h"

SeaLOD::SeaLOD(Shader* shader,  UINT width, UINT height, float cellSpace)
	: Renderer(shader)
{
	Topology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	
	//this->width = width > 0 ? width : this->width;
	//this->height = height > 0 ? height : this->height;
	
	this->width		= width / cellSpace;
	this->height	= height / cellSpace;

	if (this->width < cellsPerPatch || this->height < cellsPerPatch)
		cellsPerPatch = min(this->width, this->height);

	seaMap = new Texture(L"Environment/Water.png");
	sSeaMap = shader->AsSRV("SeaMap");

	patchWidth = ((this->width - 1) / cellsPerPatch) + 1;
	patchHeight = ((this->height - 1) / cellsPerPatch) + 1;
	
	vertexCount = (patchWidth + 1) * (patchHeight + 1);
	indexCount = patchWidth * patchHeight;

	CalcVertexBounds();
	CreatePatchVertex();
	CreatePatchIndex();

	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_SeaLOD");

	desc.CellspacingU = 1.0f / (this->width + 1);
	desc.CellspacingV = 1.0f / (this->height + 1);

	material = new Material(shader);

	reflection = new Reflection2(shader, transform, D3D::Width(), D3D::Height());

	//normalMap = new Texture(L"Environment/Wave.dds");
	//sNormalMap = shader->AsSRV("NormalMap2");
	refraction = new Refraction2(shader, L"Environment/Wave.dds", width, height);
}

SeaLOD::~SeaLOD()
{
	SafeDelete(refraction);
	SafeDelete(reflection);
	SafeDelete(seaMap);
	SafeDeleteArray(bounds);
	SafeDelete(buffer);
	SafeDelete(material);
}

void SeaLOD::Update()
{
	Super::Update();

	desc.Padding += 0.01f * Time::Delta();
	if (desc.Padding > 1.0f)
		desc.Padding -= 1.0f;

	reflection->Update();
	refraction->Update();

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

void SeaLOD::Render()
{
	Super::Render();
	
	reflection->Render();
	refraction->Render();
	//sNormalMap->SetResource(normalMap->SRV());

	material->Render();

	sSeaMap->SetResource(seaMap->SRV());

	buffer->Render();
	sBuffer->SetConstantBuffer(buffer->Buffer());

	sSeaMap->SetResource(seaMap->SRV());

	shader->DrawIndexed(0, Pass(), indexCount * 4);
}

float SeaLOD::GetWidth()
{
	return width * desc.Cellspacing;
}

float SeaLOD::GetHeight()
{
	return height * desc.Cellspacing;
}

void SeaLOD::SeaMap(wstring file)
{
	SafeDelete(seaMap);

	seaMap = new Texture(file);
}

void SeaLOD::Position(float x, float y, float z)
{
	GetTransform()->Position(x, y, z);
}

void SeaLOD::Position(Vector3 & vec)
{
	GetTransform()->Position(vec);
}

void SeaLOD::Position(Vector3 * vec)
{
	GetTransform()->Position(vec);
}

void SeaLOD::CalcVertexBounds()
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
					
					minY = min(minY, -100.0f);
					maxY = max(maxY, 100.0f);
				}
			}

			UINT patchID = z * patchWidth + x;
			bounds[patchID] = Vector2(minY, maxY);
		}//for(x)
	}//for(z)
}

void SeaLOD::CreatePatchVertex()
{
	vector<VertexSeaLOD> vertices(vertexCount);

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

	vertexBuffer = new VertexBuffer(&vertices[0], vertices.size(), sizeof(VertexSeaLOD));
}

void SeaLOD::CreatePatchIndex()
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

void SeaLOD::GetCullingPlanes(Plane * planes)
{
	memcpy(this->planes, planes, sizeof(Plane) * 6);
}

void SeaLOD::PreRender_Reflection()
{
	Vector3 position;
	transform->Position(&position);

	Plane plane = Plane(0, 1, 0, -position.y);
	Context::Get()->Clipping() = plane;

	reflection->PreRender();
}

void SeaLOD::PreRender_Refraction()
{
	Vector3 position;
	transform->Position(&position);

	Plane plane = Plane(0, -1, 0, position.y + 0.1f);
	Context::Get()->Clipping() = plane;

	refraction->PreRender();
}

void SeaLOD::PreRender_End()
{
	Plane plane = Plane(0, 0, 0, 0);
	Context::Get()->Clipping() = plane;
}