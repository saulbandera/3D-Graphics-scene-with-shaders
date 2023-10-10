#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class VerticalBlurShader : public BaseShader
{
private:
	struct HeightBufferType
	{
		float height;
		XMFLOAT3 padding;
	};

public:


	VerticalBlurShader(ID3D11Device* device, HWND hwnd);
	~VerticalBlurShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, float height);

private:

	void initShader(const wchar_t* vs, const wchar_t* ps);


	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* heightBuffer;
	ID3D11SamplerState* sampleState;

};
