#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class ThresholdCheckShader : public BaseShader
{
private:

	struct thresholdBufferType
	{
		XMFLOAT4 brightnessThreshhold;
		XMFLOAT4 colour;
	};

public:


	ThresholdCheckShader(ID3D11Device* device, HWND hwnd);
	~ThresholdCheckShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, XMFLOAT4 colour);
	void setBrightnessThreshold(float level);

private:

	void initShader(const wchar_t* vs, const wchar_t* ps);


	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* configBuffer;
	ID3D11SamplerState* sampleState;
	XMFLOAT4 thresholds;
};

