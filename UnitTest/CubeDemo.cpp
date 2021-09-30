#include "stdafx.h"
#include "CubeDemo.h"

void CubeDemo::Initialize()
{
	shader = new Shader(L"07_Cube.fx");

	
	vertexCount = 24;
	vertices = new Vertex[vertexCount];


	float w = 0.5f, h = 0.5f, d = 0.5;

	//Front
	vertices[0].Position = Vector3(-w, -h, -d);
	vertices[1].Position = Vector3(-w, +h, -d);
	vertices[2].Position = Vector3(+w, -h, -d);
	vertices[3].Position = Vector3(+w, +h, -d);

	//Back
	vertices[4].Position = Vector3(-w, -h, +d);
	vertices[5].Position = Vector3(+w, -h, +d);
	vertices[6].Position = Vector3(-w, +h, +d);
	vertices[7].Position = Vector3(+w, +h, +d);

	//Top
	vertices[8].Position = Vector3(-w, +h, -d);
	vertices[9].Position = Vector3(-w, +h, +d);
	vertices[10].Position = Vector3(+w, +h, -d);
	vertices[11].Position = Vector3(+w, +h, +d);

	//Bottom
	vertices[12].Position = Vector3(-w, -h, -d);
	vertices[13].Position = Vector3(+w, -h, -d);
	vertices[14].Position = Vector3(-w, -h, +d);
	vertices[15].Position = Vector3(+w, -h, +d);

	//Left
	vertices[16].Position = Vector3(-w, -h, +d);
	vertices[17].Position = Vector3(-w, +h, +d);
	vertices[18].Position = Vector3(-w, -h, -d);
	vertices[19].Position = Vector3(-w, +h, -d);

	//Right
	vertices[20].Position = Vector3(+w, -h, -d);
	vertices[21].Position = Vector3(+w, +h, -d);
	vertices[22].Position = Vector3(+w, -h, +d);
	vertices[23].Position = Vector3(+w, +h, +d);
	

	//Create VertexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(Vertex) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
	}
	

	indexCount = 36; 
	indices = new UINT[indexCount]
	{
		0, 1, 2, 2, 1, 3,
		4, 5, 6, 6, 5, 7,
		8, 9, 10, 10, 9, 11,
		12, 13, 14, 14, 13, 15,
		16, 17, 18, 18, 17, 19,
		20, 21, 22, 22, 21, 23,
	};
	
	//Create IndexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(UINT) * indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = indices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &indexBuffer));
	}


	D3DXMatrixIdentity(&world);
}

void CubeDemo::Destroy()
{
	SafeDelete(shader);

	SafeDeleteArray(vertices);
	SafeRelease(vertexBuffer);

	SafeDeleteArray(indices);
	SafeRelease(indexBuffer);
}

void CubeDemo::Update()
{
	ImGui::SliderFloat("Speed", &speed, 5, 20);

	if (Keyboard::Get()->Press(VK_SHIFT))
	{
		if (Keyboard::Get()->Press(VK_RIGHT))
			rotation.y += speed * Time::Delta();
		else if (Keyboard::Get()->Press(VK_LEFT))
			rotation.y -= speed * Time::Delta();

		if (Keyboard::Get()->Press(VK_UP))
			rotation.x += speed * Time::Delta();
		else if (Keyboard::Get()->Press(VK_DOWN))
			rotation.x -= speed * Time::Delta();
	}
	else
	{
		if (Keyboard::Get()->Press(VK_RIGHT))
			position.x += speed * Time::Delta();
		else if (Keyboard::Get()->Press(VK_LEFT))
			position.x -= speed * Time::Delta();

		if (Keyboard::Get()->Press(VK_UP))
			position.z += speed * Time::Delta();
		else if (Keyboard::Get()->Press(VK_DOWN))
			position.z -= speed * Time::Delta();
	}

	Matrix R, T;
	D3DXMatrixRotationYawPitchRoll(&R, rotation.y, rotation.x, rotation.z);
	D3DXMatrixTranslation(&T, position.x, position.y + 0.5f, position.z);

	world = R * T;


	shader->AsVector("Color")->SetFloatVector(color);
	shader->AsMatrix("World")->SetMatrix(world);
	shader->AsMatrix("View")->SetMatrix(Context::Get()->View());
	shader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());
}

void CubeDemo::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	static bool bWireframe = true;
	ImGui::Checkbox("CubeWireframe", &bWireframe);

	shader->DrawIndexed(0, bWireframe == true ? 1 : 0, indexCount);
}
