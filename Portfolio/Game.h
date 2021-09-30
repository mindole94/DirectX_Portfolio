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
	void CheckHit();
	void CheckFireball();
	void CheckDamage();

private:
	void CameraSetting();

private:
	Shader* shader;
	Shadow* shadow;
	
	CubeSky* sky;
	Terrain* terrain;

	class Player* player = NULL;
	class Monster1* monster1 = NULL;
	class Monster2* monster2 = NULL;

	bool showCollider = true;
};