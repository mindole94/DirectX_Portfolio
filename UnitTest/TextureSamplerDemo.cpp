#include "stdafx.h"
#include "TextureSamplerDemo.h"

void TextureSamplerDemo::Initialize()
{
	Context::Get()->GetCamera()->Position(0, 0, -5);
	((Freedom *)Context::Get()->GetCamera())->Speed(5, 0);

	shader = new Shader(L"10_TextureSampler.fx");


	vertices = new VertexTexture[4];

	vertices[0].Position = Vector3(-0.5f, -0.5f, 0.0f);
	vertices[1].Position = Vector3(-0.5f, +0.5f, 0.0f);
	vertices[2].Position = Vector3(+0.5f, -0.5f, 0.0f);
	vertices[3].Position = Vector3(+0.5f, +0.5f, 0.0f);

	//vertices[0].Uv = Vector2(0, 1);
	//vertices[1].Uv = Vector2(0, 0);
	//vertices[2].Uv = Vector2(1, 1);
	//vertices[3].Uv = Vector2(1, 0);

	vertices[0].Uv = Vector2(0, 3);
	vertices[1].Uv = Vector2(0, 0);
	vertices[2].Uv = Vector2(3, 3);
	vertices[3].Uv = Vector2(3, 0);


	//Create VertexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(VertexTexture) * 4;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
	}
	

	indices = new UINT[6]
	{
		0, 1, 2, 2, 1, 3,
	};
	
	//Create IndexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(UINT) * 6;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = indices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &indexBuffer));
	}

	texture = new Texture(L"Box.png");


	//ID3D11SamplerState;
	//D3D11_SAMPLER_DESC;
}

void TextureSamplerDemo::Destroy()
{
	SafeDelete(shader);

	SafeDeleteArray(vertices);
	SafeRelease(vertexBuffer);

	SafeDeleteArray(indices);
	SafeRelease(indexBuffer);

	SafeDelete(texture);
}

void TextureSamplerDemo::Update()
{
	Matrix world;
	D3DXMatrixIdentity(&world);
	
	shader->AsSRV("Map")->SetResource(texture->SRV());
	shader->AsMatrix("World")->SetMatrix(world);
	shader->AsMatrix("View")->SetMatrix(Context::Get()->View());
	shader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());
}

void TextureSamplerDemo::Render()
{
	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	static UINT Filter = 0;
	ImGui::InputInt("Filter", (int *)&Filter);
	Filter %= 2;
	shader->AsScalar("Filter")->SetInt(Filter);

	//shader->DrawIndexed(0, 0, 6);


	static UINT Address = 0;
	ImGui::InputInt("Address", (int *)&Address);
	Address %= 4;
	shader->AsScalar("Address")->SetInt(Address);

	shader->DrawIndexed(0, 1, 6);
}
