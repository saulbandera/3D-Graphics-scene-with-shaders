#pragma once
#include "DXF.h"
#include "BaseShader.h"

class WaveShader : public BaseShader
{
private:
	struct LightBufferType {
		XMFLOAT4 ambient[2];
		XMFLOAT4 diffuse[2];
		XMFLOAT4 position[2];
		XMFLOAT4 specularPower[2];
		XMFLOAT4 specular[2];
		XMFLOAT4 direction[2];
	};

	struct TimeBufferType {
		float time;
		float speed;
		float amplitude;
		float steepness;
		XMFLOAT2A PI_and_Wavelength;
	};

	struct CameraBufferType
	{
		XMFLOAT3 position;
		float padding;
	};



public:
	WaveShader(ID3D11Device* device, HWND hwnd);
	~WaveShader();
	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, Light* light[2], float time, XMFLOAT3 camPos, float speed, float amplitude, float wavelength, float steepness, float ptBrightness, float dirBrightness, bool normals);



private: 
	void initShader(const wchar_t* cs, const wchar_t* ps);



	ID3D11SamplerState* sampleState;
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* timeBuffer;
	ID3D11Buffer* cameraBuffer;
};

