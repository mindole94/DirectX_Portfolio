#pragma once
#include "Systems/IExecute.h"

class ShadowFrameworkDemo : public IExecute
{
public:
	virtual void Initialize() override;
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual void ResizeScreen() override {}

private:
	void Mesh();
	void Airplane();

	void Sapling();
	void Weapon();

	void PointLights();
	void SpotLights();

	void HP();

	void Pass(UINT mesh, UINT model, UINT anim);
	void SetTransform(Transform* transform);

private:
	Shader* shader;
	Shadow* shadow;

	CubeSky* sky;
	Terrain* terrain;

	Material* stone;
	Material* brick;
	Material* wall;

	MeshRender* cube;
	MeshRender* cylinder;
	MeshRender* sphere;

	ModelRender* airplane = NULL;

	ModelAnimator* sapling = NULL;
	ModelRender* weapon = NULL;
	Transform* weaponTransform;

	vector<MeshRender *> meshes;
	vector<ModelRender *> models;
	vector<ModelAnimator *> animators;


	Shader* textureShader;

	Texture* hpTexture;
	Render2D * hpBar;
	TextureBuffer* hpBuffer;

};