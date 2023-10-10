#include "DepthCubeShader.h"

DepthCubeShader::DepthCubeShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"depthCube_vs.cso", L"depthCube_ps.cso");
	loadGeometryShader(L"depthCube_gs.cso");

	
}

DepthCubeShader::~DepthCubeShader()
{
	// Release the matrix constant buffer.
	if (gsmatrixBuffer)
	{
		gsmatrixBuffer->Release();
		gsmatrixBuffer = 0;
	}

	if (vsmatrixBuffer)
	{
		vsmatrixBuffer->Release();
		vsmatrixBuffer = 0;
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void DepthCubeShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC gsmatrixBufferDesc;
	D3D11_BUFFER_DESC vsmatrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	vsmatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vsmatrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	vsmatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vsmatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vsmatrixBufferDesc.MiscFlags = 0;
	vsmatrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&vsmatrixBufferDesc, NULL, &vsmatrixBuffer);




	// Setup the description of the dynamic matrix constant buffer that is in the geometry shader.
	gsmatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	gsmatrixBufferDesc.ByteWidth = sizeof(gsMatrixBufferType);
	gsmatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	gsmatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	gsmatrixBufferDesc.MiscFlags = 0;
	gsmatrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&gsmatrixBufferDesc, NULL, &gsmatrixBuffer);




}

void DepthCubeShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& projectionMatrix, XMMATRIX shadowTransfroms[6], XMFLOAT3 lightpos)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tviews[6];
	for (int i = 0; i < 6; i++) {
		tviews[i] = XMMatrixTranspose(shadowTransfroms[i]);
	}
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);


	// set up the vertex shader constant matrix buffer
	vsMatrixBufferType* vsDataPtr;
	deviceContext->Map(vsmatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	vsDataPtr = (vsMatrixBufferType*)mappedResource.pData;
	vsDataPtr->world = tworld;
	deviceContext->Unmap(vsmatrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &vsmatrixBuffer);



	// set up the geometry shader constant matrix buffer
	gsMatrixBufferType* dataPtr;
	deviceContext->Map(gsmatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (gsMatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;

	for (int i = 0; i < 6; i++) {
		dataPtr->view[i] = tviews[i];
	}

	dataPtr->projection = tproj;
	deviceContext->Unmap(gsmatrixBuffer, 0);
	deviceContext->GSSetConstantBuffers(0, 1, &gsmatrixBuffer);

}
