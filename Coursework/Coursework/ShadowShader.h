
// Light shader.h
// Basic single light shader setup


#include "DXF.h"

using namespace std;
using namespace DirectX;


class ShadowShader : public BaseShader
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;

	};

	struct LightBufferType
	{
		XMFLOAT4 ambient[2];
		XMFLOAT4 diffuse[2];
		XMFLOAT4 position[2];
		XMFLOAT4 specularPower[2];
		XMFLOAT4 specular[2];
		XMFLOAT4 direction[2];
	};

	struct CameraBufferType
	{
		XMFLOAT3 position; // for point light
		float padding;
	};


public:

	ShadowShader(ID3D11Device* device, HWND hwnd);
	~ShadowShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap,ID3D11ShaderResourceView* depthCubeMap, XMFLOAT3 camPos, Light* light[2], float, float, bool);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);


	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* cameraBuffer;

};


