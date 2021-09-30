#include "Framework.h"
#include "Reflection.h"

Reflection::Reflection(Shader * shader, Transform * transform, UINT width, UINT height)
	: shader(shader), transform(transform)
{
	this->width = width > 0 ? width : (UINT)D3D::Width();
	this->height = height > 0 ? height : (UINT)D3D::Height();

	camera = new Fixity();
	renderTarget = new RenderTarget(this->width, this->height);
	depthStencil = new DepthStencil(this->width, this->height);
	viewport = new Viewport((float)this->width, (float)this->height);

	sReflectionSRV = shader->AsSRV("ReflectionMap");
	sReflectionView = shader->AsMatrix("ReflectionView");
}

Reflection::~Reflection()
{
	SafeDelete(camera);
	SafeDelete(renderTarget);
	SafeDelete(depthStencil);
	SafeDelete(viewport);
}

void Reflection::Update()
{
	Vector3 R, T;
	Context::Get()->GetCamera()->Rotation(&R);
	Context::Get()->GetCamera()->Position(&T);

	R.x *= -1.0f;
	camera->Rotation(R);

	Vector3 position;
	transform->Position(&position);

	T.y = (position.y * 2.0f) - T.y;
	camera->Position(T);
}

void Reflection::PreRender()
{
	renderTarget->PreRender(depthStencil);
	viewport->RSSetViewport();

	Matrix view;
	camera->GetMatrix(&view);
	sReflectionView->SetMatrix(view);
}

void Reflection::Render()
{
	sReflectionSRV->SetResource(renderTarget->SRV());
}
