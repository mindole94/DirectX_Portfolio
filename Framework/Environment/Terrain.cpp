#include "Framework.h"
#include "Terrain.h"
#include "Utilities/BinaryFile.h"
#include "Utilities/Xml.h"

const wstring Terrain::DataFolder = L"../../_Textures/TerrainData/";

Terrain::Terrain(Shader * shader, wstring file)
	: Renderer(shader)
{
	if (Path::GetExtension(file) == L"terrain")
	{
		LoadXmlFile(file);
	}
	else
	{
		Texture* heightMap = new Texture(file);

		vector<Color> heightPixels;
		heightMap->ReadPixel(DXGI_FORMAT_R8G8B8A8_UNORM, &heightPixels);

		width = heightMap->GetWidth();
		height = heightMap->GetHeight();

		heights = new float[width * height];
		for (UINT i = 0; i < heightPixels.size(); i++)
			heights[i] = heightPixels[i].r;
	}

	CreateVertexData();
	CreateIndexData();
	CreateNormalData();

	vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(VertexTerrain), 0, true);
	indexBuffer = new IndexBuffer(indices, indexCount);

	material = new Material(shader);
	material->Diffuse(1, 1, 1, 1);
	material->Specular(0.25f, 0.25f, 0.25f, 20);

	brushBuffer = new ConstantBuffer(&brushDesc, sizeof(BrushDesc));
	sBrushBuffer = shader->AsConstantBuffer("CB_Brush");

	lineBuffer = new ConstantBuffer(&lineDesc, sizeof(LineDesc));
	sLineBuffer = shader->AsConstantBuffer("CB_TerrainLine");
}

Terrain::~Terrain()
{
	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);

	SafeDeleteArray(heights);
	SafeDelete(baseMap);

	SafeDelete(material);

	for (UINT i = 0; i < layers.size(); i++)
	{
		SafeDelete(layers[i].LayerMap);
		SafeDeleteArray(layers[i].Alphas);

		SafeRelease(layers[i].Texture);
		SafeRelease(layers[i].SRV);
	}

	SafeDelete(brushBuffer);
	SafeDelete(lineBuffer);
}

void Terrain::Update()
{
	Super::Update();
}

void Terrain::Render()
{
	Super::Render();

	if (baseMap != NULL)
		shader->AsSRV("BaseMap")->SetResource(baseMap->SRV());


	if (layers.size() > 0)
	{
		ID3D11ShaderResourceView* layerSRVs[4];
		ID3D11ShaderResourceView* alphaSRVs[4];

		for (UINT i = 0; i < layers.size(); i++)
		{
			layerSRVs[i] = layers[i].LayerMap->SRV();
			alphaSRVs[i] = layers[i].SRV;
		}

		sLayerSRV->SetResourceArray(layerSRVs, 0, layers.size());
		sAlphaSRV->SetResourceArray(alphaSRVs, 0, layers.size());
	}

	brushBuffer->Render();
	sBrushBuffer->SetConstantBuffer(brushBuffer->Buffer());

	lineBuffer->Render();
	sLineBuffer->SetConstantBuffer(lineBuffer->Buffer());

	material->Render();

	shader->DrawIndexed(0, Pass(), indexCount);
}

void Terrain::BaseMap(wstring file)
{
	SafeDelete(baseMap);

	baseMap = new Texture(file);
	//material->DiffuseMap(file);
}

float Terrain::GetHeight(Vector3 & position)
{
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (x < 0 || x > width) return FLT_MIN;
	if (z < 0 || z > height) return FLT_MIN;


	UINT index[4];
	index[0] = width * z + x;
	index[1] = width * (z + 1) + x;
	index[2] = width * z + x + 1;
	index[3] = width * (z + 1) + x + 1;


	Vector3 v[4];
	for (int i = 0; i < 4; i++)
		v[i] = vertices[index[i]].Position;


	float ddx = (position.x - v[0].x) / 1.0f;
	float ddz = (position.z - v[0].z) / 1.0f;


	Vector3 result;

	if (ddx + ddz <= 1.0f)
		result = v[0] + (v[2] - v[0]) * ddx + (v[1] - v[0]) * ddz;
	else
	{
		ddx = 1.0f - ddx;
		ddz = 1.0f - ddz;

		result = v[3] + (v[1] - v[3]) * ddx + (v[2] - v[3]) * ddz;
	}

	//ImGui::LabelText("Delta", "%f, %f", ddx, ddz);


	return result.y;
}

float Terrain::GetVerticalRaycast(Vector3 & position)
{
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (x < 0 || x > width) return FLT_MIN;
	if (z < 0 || z > height) return FLT_MIN;


	UINT index[4];
	index[0] = width * z + x;
	index[1] = width * (z + 1) + x;
	index[2] = width * z + x + 1;
	index[3] = width * (z + 1) + x + 1;


	Vector3 p[4];
	for (int i = 0; i < 4; i++)
		p[i] = vertices[index[i]].Position;


	Vector3 start(position.x, 50, position.z);
	Vector3 direction(0, -1, 0);

	float u, v, distance;
	Vector3 result(-1, FLT_MIN, -1);

	if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
		result = p[0] + (p[1] - p[0]) * u + (p[2] - p[0]) * v;

	if (D3DXIntersectTri(&p[3], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
		result = p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;

	return result.y;
}

Vector3 Terrain::GetRaycastPosition()
{
	Matrix V = Context::Get()->View();
	Matrix P = Context::Get()->Projection();
	Viewport* Vp = Context::Get()->GetViewport();

	Vector3 mouse = Mouse::Get()->GetPosition();


	Vector3 n, f;
	mouse.z = 0.0f;
	Vp->Unproject(&n, mouse, transform->World(), V, P);

	mouse.z = 1.0f;
	Vp->Unproject(&f, mouse, transform->World(), V, P);

	Vector3 start = n;
	Vector3 direction = f - n;


	for (UINT z = 0; z < height - 1; z++)
	{
		for (UINT x = 0; x < width - 1; x++)
		{
			UINT index[4];
			index[0] = width * z + x;
			index[1] = width * (z + 1) + x;
			index[2] = width * z + x + 1;
			index[3] = width * (z + 1) + x + 1;

			Vector3 p[4];
			for (int i = 0; i < 4; i++)
				p[i] = vertices[index[i]].Position;


			float u, v, distance;
			if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
				return p[0] + (p[1] - p[0]) * u + (p[2] - p[0]) * v;

			if (D3DXIntersectTri(&p[3], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
				return p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;
		}
	}

	return Vector3(-1, FLT_MIN, -1);
}

void Terrain::RaiseHeight(Vector3 & center, float intensity)
{
	UINT left = (UINT)center.x - brushDesc.Range;
	UINT right = (UINT)center.x + brushDesc.Range;
	UINT bottom = (UINT)center.z - brushDesc.Range;
	UINT top = (UINT)center.z + brushDesc.Range;


	if (left < 0) left = 0;
	if (right >= width) right = width;
	if (bottom < 0) bottom = 0;
	if (top >= height) top = height;

	for (UINT z = bottom; z <= top; z++)
	{
		for (UINT x = left; x <= right; x++)
		{
			UINT index = width * z + x;

			if (brushDesc.Type == 2)
			{
				float dx = x - center.x;
				float dz = z - center.z;
				float dist = dx * dx + dz * dz;

				if (dist > brushDesc.Range * brushDesc.Range)
					continue;
			}

			vertices[index].Position.y += intensity * Time::Delta();
		}
	}
	CreateNormalData();


	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(vertexBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, vertices, sizeof(VertexTerrain) * vertexCount);
	}
	D3D::GetDC()->Unmap(vertexBuffer->Buffer(), 0);
}

void Terrain::PaintHeight(UINT layer, Vector3 & center, float intensity)
{
	assert(layer < layers.size());


	UINT left = (UINT)center.x - brushDesc.Range;
	UINT right = (UINT)center.x + brushDesc.Range;
	UINT bottom = (UINT)center.z - brushDesc.Range;
	UINT top = (UINT)center.z + brushDesc.Range;


	if (left < 0) left = 0;
	if (right >= width) right = width;
	if (bottom < 0) bottom = 0;
	if (top >= height) top = height;

	for (UINT z = bottom; z <= top; z++)
	{
		for (UINT x = left; x <= right; x++)
		{
			UINT index = width * (height - 1 - z) + x;

			if (brushDesc.Type == 2)
			{
				float dx = x - center.x;
				float dz = z - center.z;
				float dist = dx * dx + dz * dz;

				if (dist > brushDesc.Range * brushDesc.Range)
					continue;
			}

			layers[layer].Alphas[index] += (UINT)(intensity * Time::Delta());
			layers[layer].Alphas[index] = Math::Clamp<UINT>(layers[layer].Alphas[index], 0, 255);
		}
	}//for(z)

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(layers[layer].Texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, layers[layer].Alphas, sizeof(UINT) * width * height);
	}
	D3D::GetDC()->Unmap(layers[layer].Texture, 0);
}

void Terrain::LoadXmlFile(wstring file)
{
	file = DataFolder + file;
	xmlFile = file;

	Xml::XMLDocument* document = new Xml::XMLDocument();
	Xml::XMLError error = document->LoadFile(String::ToString(file).c_str());
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement* root = document->FirstChildElement();
	Xml::XMLElement* materialNode = root->FirstChildElement();

	Xml::XMLElement* node = root->FirstChildElement();
	width = node->IntText();

	node = node->NextSiblingElement();
	height = node->IntText();

	node = node->NextSiblingElement();
	heightFile = DataFolder + String::ToWString(node->GetText());

	BinaryReader* r = new BinaryReader();
	r->Open(heightFile);

	UINT* heights = new UINT[width * height];
	r->Byte((void **)&heights, sizeof(UINT) * width * height);

	this->heights = new float[width * height];
	for (UINT i = 0; i < width * height; i++)
		this->heights[i] = (float)heights[i] / 255.0f;

	r->Close();
	SafeDelete(r);
	SafeDeleteArray(heights);


	node = node->NextSiblingElement();
	wstring baseFile = String::ToWString(node->GetText());
	BaseMap(L"Terrain/" + baseFile);


	while ((node = node->NextSiblingElement()) != NULL)
	{
		Layer temp;
		temp.LayerMapFile = String::ToWString(node->Attribute("LayerMapFile"));
		temp.LayerMap = new Texture(L"Terrain/" + temp.LayerMapFile);


		temp.AlphaMapFile = String::ToWString(node->Attribute("AlphaMapFile"));
		temp.AlphaMapFile = DataFolder + temp.AlphaMapFile;

		r = new BinaryReader();
		r->Open(temp.AlphaMapFile);

		temp.Alphas = new UINT[width * height];
		r->Byte((void **)&temp.Alphas, sizeof(UINT) * width * height);


		//Create Texture2D
		{
			D3D11_TEXTURE2D_DESC desc;
			ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
			desc.Width = width;
			desc.Height = height;
			desc.ArraySize = 1;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.SampleDesc.Count = 1;
			desc.MipLevels = 1;

			D3D11_SUBRESOURCE_DATA subResource = { 0 };
			subResource.pSysMem = temp.Alphas;
			subResource.SysMemPitch = sizeof(UINT) * width;
			subResource.SysMemSlicePitch = sizeof(UINT) * width * height;

			Check(D3D::GetDevice()->CreateTexture2D(&desc, &subResource, &temp.Texture));
		}

		//CreateSRV
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipLevels = 1;

			Check(D3D::GetDevice()->CreateShaderResourceView(temp.Texture, &desc, &temp.SRV));
		}

		layers.push_back(temp);
	}

	r->Close();
	SafeDelete(r);
	SafeDelete(document);


	sAlphaSRV = shader->AsSRV("AlphaMap");
	sLayerSRV = shader->AsSRV("LayerMap");
}

void Terrain::SaveXmlFile()
{
	//높이 정보
	{
		BinaryWriter w;
		w.Open(heightFile);

		for (UINT i = 0; i < width * height; i++)
			w.UInt((UINT)(heights[i] * 255.0f));

		w.Close();
	}

	//레이어 정보
	{
		for (UINT layer = 0; layer < layers.size(); layer++)
		{
			BinaryWriter w;
			w.Open(layers[layer].AlphaMapFile);
			w.Byte(layers[layer].Alphas, sizeof(UINT) * width * height);
			w.Close();
		}
	}


	Xml::XMLDocument* document = new Xml::XMLDocument();

	Xml::XMLDeclaration* decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	Xml::XMLElement* root = document->NewElement("Map");
	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
	document->LinkEndChild(root);


	Xml::XMLElement* node = NULL;

	node = document->NewElement("Width");
	node->SetText(width);
	root->LinkEndChild(node);

	node = document->NewElement("Height");
	node->SetText(height);
	root->LinkEndChild(node);

	node = document->NewElement("HeightMapFile");
	node->SetText(String::ToString(heightFile).c_str());
	root->LinkEndChild(node);

	node = document->NewElement("BaseMapFile");

	wstring baseFile = Path::GetFileName(baseMap->GetFile());
	node->SetText(String::ToString(baseFile).c_str());
	root->LinkEndChild(node);

	for (UINT i = 0; i < layers.size(); i++)
	{
		node = document->NewElement("Layer");
		node->SetAttribute("Index", i);

		wstring layerMapFile = Path::GetFileName(layers[i].LayerMapFile);
		node->SetAttribute("LayerMapFile", String::ToString(layerMapFile).c_str());

		wstring alphaMapFile = Path::GetFileName(layers[i].AlphaMapFile);
		node->SetAttribute("AlphaMapFile", String::ToString(alphaMapFile).c_str());
		root->LinkEndChild(node);
	}

	wstring xmlFile = Path::GetFileName(this->xmlFile);
	document->SaveFile(String::ToString(Terrain::DataFolder + xmlFile).c_str());
	SafeDelete(document);
}

void Terrain::CreateVertexData()
{
	//FILE* file;
	//fopen_s(&file, "../Test.csv", "w");

	vertexCount = width * height;
	vertices = new VertexTerrain[vertexCount];
	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			UINT index = width * z + x;
			UINT pixel = width * (height - 1 - z) + x;

			vertices[index].Position.x = (float)x;
			vertices[index].Position.y = heights[pixel] * 255 / heightScale;
			vertices[index].Position.z = (float)z;

			vertices[index].Uv.x = (float)x / width;
			vertices[index].Uv.y = (float)(height - 1 - z) / height;

			//vertices[index].Uv.x = ((float)x / width) * 10.0f;
			//vertices[index].Uv.y = ((float)(height - 1 - z) / height) * 10.0f;


			//fprintf(file, "%d,%f,%f,%f\n", index, vertices[index].Position.x, vertices[index].Position.y, vertices[index].Position.z);

			vertices[index].Color = Color(1, 1, 1, 1);
		}
	}

	//fclose(file);
}

void Terrain::CreateIndexData()
{
	indexCount = (width - 1) * (height - 1) * 6;
	indices = new UINT[indexCount];

	UINT index = 0;
	for (UINT y = 0; y < height - 1; y++)
	{
		for (UINT x = 0; x < width - 1; x++)
		{
			indices[index + 0] = width * y + x;
			indices[index + 1] = width * (y + 1) + x;
			indices[index + 2] = width * y + x + 1;
			indices[index + 3] = width * y + x + 1;
			indices[index + 4] = width * (y + 1) + x;
			indices[index + 5] = width * (y + 1) + x + 1;

			index += 6;
		}
	}
}

void Terrain::CreateNormalData()
{
	for (UINT i = 0; i < indexCount / 3; i++)
	{
		UINT index0 = indices[i * 3 + 0];
		UINT index1 = indices[i * 3 + 1];
		UINT index2 = indices[i * 3 + 2];

		VertexTerrain v0 = vertices[index0];
		VertexTerrain v1 = vertices[index1];
		VertexTerrain v2 = vertices[index2];


		//Normal Vector
		{
			Vector3 e1 = v1.Position - v0.Position;
			Vector3 e2 = v2.Position - v0.Position;

			Vector3 normal;
			D3DXVec3Cross(&normal, &e1, &e2);

			vertices[index0].Normal += normal;
			vertices[index1].Normal += normal;
			vertices[index2].Normal += normal;
		}

		//Tangent Vector
		{
			Vector3 d1 = v1.Position - v0.Position;
			Vector3 d2 = v2.Position - v0.Position;

			Vector2 u = v1.Uv - v0.Uv;
			Vector2 v = v2.Uv - v0.Uv;

			float d = 1.0f / (u.x * v.y - u.y * v.x);

			Vector3 tangent;
			tangent.x = (v.y * d1.x - v.x * d2.x) * d;
			tangent.y = (v.y * d1.y - v.x * d2.y) * d;
			tangent.z = (v.y * d1.z - v.x * d2.z) * d;

			vertices[index0].Tangent += tangent;
			vertices[index1].Tangent += tangent;
			vertices[index2].Tangent += tangent;
		}
	}

	for (UINT i = 0; i < vertexCount; i++)
	{
		D3DXVec3Normalize(&vertices[i].Normal, &vertices[i].Normal);
		D3DXVec3Normalize(&vertices[i].Tangent, &vertices[i].Tangent);
	}
}