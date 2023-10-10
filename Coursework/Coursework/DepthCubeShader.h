
#pragma once
#include "DXF.h"

using namespace std;
using namespace DirectX;


class DepthCubeShader : public BaseShader
{

private:

	struct vsMatrixBufferType
	{
		XMMATRIX world;
	};
	struct gsMatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view[6];
		XMMATRIX projection;
	};



public:

	DepthCubeShader(ID3D11Device* device, HWND hwnd);
	~DepthCubeShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& projection, XMMATRIX shadowTransforms[6], XMFLOAT3 lightpos);


private:
	void initShader(const wchar_t* vs, const wchar_t* ps);


	ID3D11Buffer* vsmatrixBuffer;
	ID3D11Buffer* gsmatrixBuffer;
};
