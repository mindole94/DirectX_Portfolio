#pragma once

class PostEffect : public Renderer
{
public:
	PostEffect(wstring shaderFile);
	~PostEffect();

	void Update();
	void PreRender_AfterImage(RenderTarget* renderTarget, DepthStencil* depthStencil, Viewport* viewport);
	void PreRender_Bloom(RenderTarget* renderTarget, DepthStencil* depthStencil, Viewport* viewport);
	void Render();

	void SRV(ID3D11ShaderResourceView* srv);

	void SetBlur();

	void GetBlurParameter(vector<float>& weights, vector<Vector2>& offsets, float x, float y);
	float GetGaussFunction(float val);

	UINT GetBlurCount() { return blurCount; }
	void SetBlurCount(UINT count) { blurCount = count; }

	void PlayAfterImage(bool bPlay) { bAfterImage = bPlay; }

private:
	void PreRender();
	void ImGuiSetting();

private:
	ID3DX11EffectShaderResourceVariable* sDiffuseMap;
	ID3DX11EffectShaderResourceVariable* sBloomMap;
	ID3DX11EffectShaderResourceVariable* sAfterImageMap;

	Vector2 pixelSize;

	//bloom(index) : 0 Luminosity, 1 BlurX, 2 BlurY, 3 Composite, 4 ColorGrading
	//AfterImage : 0 ~ 9 AfterImage, 10 : RenderTarget
	RenderTarget* afterImageTarget[10];
	RenderTarget* bloomTarget[4];
	RenderTarget* resultBloom;
	RenderTarget* resultAfterImage;

	vector<float> weightX;
	vector<Vector2> offsetX;

	vector<float> weightY;
	vector<Vector2> offsetY;

	float threshold = 0.05f;
	UINT blurCount = 31;

	int renderIndex = 4;

	bool bAfterImage = false;
};