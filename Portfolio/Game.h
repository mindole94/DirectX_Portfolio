#pragma once
#include "Systems/IExecute.h"

class Game : public IExecute
{
public:
	virtual void Initialize() override;
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override {}
	virtual void ResizeScreen() override {}

private:
	void Interact();

	void InteractDefault();//��Ÿ
	void InteractSkill1();//��ų1
	void InteractSkill2();//���̾
	void InteractSkill3();//�˹�
	void InteractSkill5();//�ߵ�

	void PlayerDamaged();

private:
	void CameraSetting(Vector3 pos);
	void ImGuiSetting();

private:
	Shader* shader;
	Shadow* shadow;
	
	class Map* map;

	RenderTarget* renderTarget;
	DepthStencil* depthStencil;
	Viewport* viewport;

	PostEffect* postEffect;

	class Player* player = NULL;
	class Monster1* monster1 = NULL;
	class Monster2* monster2 = NULL;

	bool showCollider = true;
	bool cameraShake = false;

	Texture* dissolveMap;
	ID3DX11EffectShaderResourceVariable* sDissolveMap;

	bool onUI = true;

	ColliderObject* weaponBox;

};