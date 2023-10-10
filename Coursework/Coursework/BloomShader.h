#pragma once
#include "DXF.h"

using namespace std;
using namespace DirectX;

class BloomShader : public BaseShader
{
public:

	BloomShader(ID3D11Device* device, HWND hwnd);
	~BloomShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* scene, ID3D11ShaderResourceView* bloomTexture);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:

	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
};

