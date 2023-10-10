#pragma once
#include "DXF.h"
#include "BaseShader.h"

class MoonShader : public BaseShader
{


private:
	struct LightBufferType {
		XMFLOAT4 brightness;
		XMFLOAT4 normals;
	};




public:
	MoonShader(ID3D11Device* device, HWND hwnd);
	~MoonShader();
	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, Light* light[2], float time, XMFLOAT3 camPos, XMFLOAT4 brightness, bool normals);



private:
	void initShader(const wchar_t* cs, const wchar_t* ps);
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* lightBuffer;

};
