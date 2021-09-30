#include "stdafx.h"
#include "TextureDemo.h"
int num = 4;
void TextureDemo::Initialize()
{
	Context::Get()->GetCamera()->Position(0, 0, -5);
	((Freedom *)Context::Get()->GetCamera())->Speed(5, 0);

	shader = new Shader(L"09_Texture.fx");


	vertices = new VertexTexture[num];

	/*vertices[0].Position = Vector3(-0.5f, -0.5f, 0.0f);
	vertices[1].Position = Vector3(-0.5f, +0.5f, 0.0f);

	vertices[2].Position = Vector3(+0.5f, -0.5f, 0.0f);
	vertices[3].Position = Vector3(+0.5f, +0.5f, 0.0f);

	vertices[4].Position = Vector3(+1.5f, -0.5f, 0.0f);
	vertices[5].Position = Vector3(+1.5f, +0.5f, 0.0f);

	vertices[6].Position = Vector3(+3.0f, -0.5f, 0.0f);
	vertices[7].Position = Vector3(+3.0f, +0.5f, 0.0f);*/

	for (UINT i = 0; i < num / 2; i++)
	{
		float next = i;
		vertices[(i * 2) + 0].Position = Vector3(0.0f + next, -0.5f, 0);
		vertices[(i * 2) + 1].Position = Vector3(0.0f + next, +0.5f, 0);
	}

	/*for (UINT i = 0; i < num / 2; i++)
	{
		vertices[(2 * i) + 0].Uv = Vector2((float)(i / ((num - 2) / 2)), 1);
		vertices[(2 * i) + 1].Uv = Vector2((float)(i / ((num - 2) / 2)), 0);
		std::cout << i << " " << ((float)i / ((num - 2) / 2)) << std::endl;
	}*/
	//vertices[0].Uv = Vector2(0, 1);
	//vertices[1].Uv = Vector2(0, 0);

	//vertices[2].Uv = Vector2((float)1/3, 1);
	//vertices[3].Uv = Vector2((float)1/3, 0);

	//vertices[4].Uv = Vector2((float)2/3, 1);
	//vertices[5].Uv = Vector2((float)2/3, 0);

	//vertices[6].Uv = Vector2(1, 1);
	//vertices[7].Uv = Vector2(1, 0);
	//std::cout << (float)1 / 3 << std::endl;
	for (int i = 0; i < num / 2; i++)
	{
		vertices[(i * 2) + 0].Uv = Vector2(((float)i / ((num - 2) / 2)), 1.0f);
		vertices[(i * 2) + 1].Uv = Vector2(((float)i / ((num - 2) / 2)), 0.0f);
		//std::cout << i << " " << ((float)i / ((num - 2) / 2)) << std::endl;
		std::cout << (i * 2) + 0 << " : " << vertices[(i * 2) + 0].Uv.x << " " << vertices[(i * 2) + 0].Uv.y << std::endl;
		std::cout << (i * 2) + 1 << " : " << vertices[(i * 2) + 1].Uv.x << " " << vertices[(i * 2) + 1].Uv.y << std::endl;
	}
	vertices[0].Uv = Vector2(0, 1);
	vertices[1].Uv = Vector2(0, 0);

	vertices[2].Uv = Vector2(1, 0);
	vertices[3].Uv = Vector2(1, 1);


	//for (int i = 0; i < num;)
	//{
	//	//vertices[(i * 2) + 0].Uv = Vector2(((float)i / ((num - 2) / 2)), 1.0f);
	//	//vertices[(i * 2) + 1].Uv = Vector2(((float)i / ((num - 2) / 2)), 0.0f);
	//	vertices[i].Uv = Vector2((float)i / ((num - 2)), 1.0f);
	//	i++;
	//	vertices[i].Uv = Vector2((float)i / ((num - 2)), 0.0f);
	//	i++;
	//}
	//vertices[0].Uv = Vector2(0, 1);
	//vertices[1].Uv = Vector2(0, 0);
	//vertices[2].Uv = Vector2(1, 1);
	//vertices[3].Uv = Vector2(1, 0);
	////
	//vertices[4].Uv = Vector2(0, 1);
	//vertices[5].Uv = Vector2(0, 0);


	//Create VertexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(VertexTexture) * num;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
	}
	

	indices = new UINT[num]
	{
		//0, 1, 2, 2, 1, 3,
		//0,1,2,3,4,5,
	};
	
	for (UINT i = 0; i < num; i++)
	{
		indices[i] = i;
	}

	//Create IndexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(UINT) * num;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = indices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &indexBuffer));
	}
}

void TextureDemo::Destroy()
{
	SafeDelete(shader);

	SafeDeleteArray(vertices);
	SafeRelease(vertexBuffer);

	SafeDeleteArray(indices);
	SafeRelease(indexBuffer);

	SafeDelete(texture);
}

void TextureDemo::Update()
{
	if (ImGui::Button("Open") == true)
	{
		function<void(wstring)> f = bind(&TextureDemo::LoadTexture, this, placeholders::_1);

		Path::OpenFileDialog(L"", Path::ImageFilter, L"../../_Textures/", f, D3D::GetDesc().Handle);
	}

	Context::Get()->GetCamera()->Position(0, 0, -5);
	((Freedom *)Context::Get()->GetCamera())->Speed(5, 0);

	Matrix world;
	D3DXMatrixIdentity(&world);
	
	if(texture != NULL)
		shader->AsSRV("Map")->SetResource(texture->SRV());

	shader->AsMatrix("World")->SetMatrix(world);
	shader->AsMatrix("View")->SetMatrix(Context::Get()->View());
	shader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());
}

void TextureDemo::Render()
{
	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	shader->DrawIndexed(0, 0, num);
}

void TextureDemo::LoadTexture(wstring file)
{
	SafeDelete(texture);

	MessageBox(D3D::GetDesc().Handle, file.c_str(), L"", MB_OK);
	texture = new Texture(file);
}
