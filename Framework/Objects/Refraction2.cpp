#include "Framework.h"
#include "Refraction2.h"

Refraction2::Refraction2(Shader * shader, wstring normalFile, UINT width, UINT height)
	: shader(shader)
{
	this->width = width > 0 ? width : (UINT)D3D::Width();
	this->height = height > 0 ? height : (UINT)D3D::Height();

	renderTarget = new RenderTarget(this->width, this->height);
	depthStencil = new DepthStencil(this->width, this->height);
	viewport = new Viewport((float)this->width, (float)this->height);

	sRefraction2SRV = shader->AsSRV("Refraction2Map");

	normalMap = new Texture(normalFile);
	sNormalMap = shader->AsSRV("Normal2Map");
}

Refraction2::~Refraction2()
{
	SafeDelete(renderTarget);
	SafeDelete(depthStencil);
	SafeDelete(viewport);

	SafeDelete(normalMap);
}

void Refraction2::Update()
{
	
}

void Refraction2::PreRender()
{
	renderTarget->PreRender(depthStencil);
	viewport->RSSetViewport();
}

void Refraction2::Render()
{
	sRefraction2SRV->SetResource(renderTarget->SRV());
	sNormalMap->SetResource(normalMap->SRV());
}
