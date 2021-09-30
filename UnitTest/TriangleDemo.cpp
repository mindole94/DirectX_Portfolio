#include "stdafx.h"
#include "TriangleDemo.h"

void TriangleDemo::Initialize()
{
	shader = new Shader(L"03_Triangle.fx");

	vertices[0].Position = Vector3(-0.5f, +0.0f, 0.0f);
	vertices[1].Position = Vector3(+0.0f, +0.5f, 0.0f);
	vertices[2].Position = Vector3(+0.5f, +0.0f, 0.0f);
	vertices[3].Position = Vector3(+1.0f, +0.5f, 0.0f);

	
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = sizeof(Vertex) * 4;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = vertices;

	Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
}

void TriangleDemo::Destroy()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);
}

void TriangleDemo::Update()
{
	
}

void TriangleDemo::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	shader->Draw(0, 0, 4);
}
