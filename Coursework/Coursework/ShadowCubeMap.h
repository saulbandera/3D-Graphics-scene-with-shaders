#pragma once
#include "d3d.h"
using namespace DirectX;

class ShadowCubeMap
{
public:
	ShadowCubeMap(ID3D11Device* device, int mWidth, int mHeight);
	~ShadowCubeMap();

	void BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* dc);
	ID3D11ShaderResourceView* getDepthMapSRV() { return cubemap; };
	int getWidth();
	int getHeight();
private:
	int width;
	int height;


	ID3D11DepthStencilView* cubeDSV = nullptr;
	ID3D11ShaderResourceView* cubemap = nullptr;
	D3D11_VIEWPORT viewport;
	ID3D11RenderTargetView* renderTarget = nullptr;
	ID3D11Texture2D* depthTex = nullptr;
	ID3D11Texture2D* cubeTexture = nullptr;
};