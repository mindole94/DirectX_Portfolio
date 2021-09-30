#include "stdafx.h"
#include "VertexLineDemo.h"

void VertexLineDemo::Initialize()
{
	shader = new Shader(L"02_Vertex.fx");

	vertices[0].Position = Vector3(0, 0, 0);
	vertices[1].Position = Vector3(1, 0, 0);

	
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = sizeof(Vertex) * 2;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = vertices;

	Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
}

void VertexLineDemo::Destroy()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);
}

void VertexLineDemo::Update()
{

}

void VertexLineDemo::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);


	static UINT pass = 0;
	ImGui::InputInt("Pass", (int *)&pass);
	pass %= 4;

	static Color color = Color(0, 0, 0, 1);
	ImGui::ColorEdit3("Color", color);

	shader->AsVector("Color")->SetFloatVector(color);

	shader->Draw(0, pass, 2);
}
