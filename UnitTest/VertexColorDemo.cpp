#include "stdafx.h"
#include "VertexColorDemo.h"

void VertexColorDemo::Initialize()
{
	shader = new Shader(L"02_VertexColor.fx");

	vertices[0].Position = Vector3(0.0f, 0.0f, 0.0f);
	vertices[1].Position = Vector3(1.0f, 0.0f, 0.0f);

	vertices[0].Color = Color(1, 0, 0, 1);
	vertices[1].Color = Color(0, 0, 1, 1);

	
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = sizeof(VertexColor) * 2;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = vertices;

	Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
}

void VertexColorDemo::Destroy()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);
}

void VertexColorDemo::Update()
{
	if (Keyboard::Get()->Press(VK_RIGHT))
		position.x += 0.1f * Time::Delta();
	else if (Keyboard::Get()->Press(VK_LEFT))
		position.x -= 0.1f * Time::Delta();

	if (Keyboard::Get()->Press(VK_UP))
		position.y += 0.1f * Time::Delta();
	else if (Keyboard::Get()->Press(VK_DOWN))
		position.y -= 0.1f * Time::Delta();

	vertices[0].Position = position;

	D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, vertices, sizeof(VertexColor) * 2, 0);
}

void VertexColorDemo::Render()
{
	UINT stride = sizeof(VertexColor);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	shader->Draw(0, 0, 2);
}
