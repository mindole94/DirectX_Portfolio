#include "stdafx.h"
#include "World2Demo.h"

void World2Demo::Initialize()
{
	shader = new Shader(L"05_World.fx");

	vertices[0].Position = Vector3(-0.5f, -0.5f, 0.0f);
	vertices[1].Position = Vector3(-0.5f, +0.5f, 0.0f);
	vertices[2].Position = Vector3(+0.5f, -0.5f, 0.0f);

	vertices[3].Position = Vector3(+0.5f, -0.5f, 0.0f);
	vertices[4].Position = Vector3(-0.5f, +0.5f, 0.0f);
	vertices[5].Position = Vector3(+0.5f, +0.5f, 0.0f);

	
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = sizeof(Vertex) * 6;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = vertices;

	Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
}

void World2Demo::Destroy()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);
}

void World2Demo::Update()
{
	if (Keyboard::Get()->Press(VK_SHIFT))
	{
		if (Keyboard::Get()->Press(VK_RIGHT))
			position[0].x += 2.0f * Time::Delta();
		else if (Keyboard::Get()->Press(VK_LEFT))
			position[0].x -= 2.0f * Time::Delta();

		if (Keyboard::Get()->Press(VK_UP))
			position[0].y += 2.0f * Time::Delta();
		else if (Keyboard::Get()->Press(VK_DOWN))
			position[0].y -= 2.0f * Time::Delta();
	}
	else
	{
		if (Keyboard::Get()->Press(VK_RIGHT))
			scale[0].x += 2.0f * Time::Delta();
		else if (Keyboard::Get()->Press(VK_LEFT))
			scale[0].x -= 2.0f * Time::Delta();

		if (Keyboard::Get()->Press(VK_UP))
			scale[0].y += 2.0f * Time::Delta();
		else if (Keyboard::Get()->Press(VK_DOWN))
			scale[0].y -= 2.0f * Time::Delta();
	}
}

void World2Demo::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	static bool bWireframe = false;
	ImGui::Checkbox("Wireframe", &bWireframe);

	shader->AsMatrix("View")->SetMatrix(Context::Get()->View());
	shader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());

	//World
	{
		Matrix S, T;
		D3DXMatrixScaling(&S, scale[0].x, scale[0].y, scale[0].z);
		D3DXMatrixTranslation(&T, position[0].x, position[0].y, position[0].z);
		Matrix world = S * T;

		shader->AsMatrix("World")->SetMatrix(world);
		shader->AsVector("Color")->SetFloatVector(Color(1, 0, 0, 1));

		shader->Draw(0, bWireframe == true ? 1 : 0, 6);
	}

	//World2
	{
		Matrix world;
		D3DXMatrixIdentity(&world);

		shader->AsMatrix("World")->SetMatrix(world);
		shader->AsVector("Color")->SetFloatVector(Color(0, 0, 1, 1));

		shader->Draw(0, bWireframe == true ? 1 : 0, 6);
	}
}
