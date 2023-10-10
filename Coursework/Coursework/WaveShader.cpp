#include "WaveShader.h"

WaveShader::WaveShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"wave_vs.cso", L"wave_ps.cso");
}

WaveShader::~WaveShader()
{
	// Release the sampler state.
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	// Release the light constant buffer.
	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}

	// Release the time constant buffer.
	if (timeBuffer)
	{
		timeBuffer->Release();
		timeBuffer = 0;
	}

	// Release the camera constant buffer.
	if (cameraBuffer)
	{
		cameraBuffer->Release();
		cameraBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();

}


void WaveShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC timeBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;


	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	//setup camera buffer
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&cameraBufferDesc, NULL, &cameraBuffer);


	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	//Setup time Buffer 
	timeBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	timeBufferDesc.ByteWidth = sizeof(TimeBufferType);
	timeBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	timeBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	timeBufferDesc.MiscFlags = 0;
	timeBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&timeBufferDesc, NULL, &timeBuffer);


	// Setup light buffer
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);
}


void WaveShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, Light* light[2], float time, XMFLOAT3 camPos, float speed, float amplitude, float wavelength, float steepness, float ptBrightness, float dirBrightness, bool normals)
{
	HRESULT result; 
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	XMMATRIX tworld, tview, tproj;

	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(world);
	tview = XMMatrixTranspose(view);
	tproj = XMMatrixTranspose(projection);




	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	// send time and wave data to Vertex Shader 
	TimeBufferType* timePtr;
	result = deviceContext->Map(timeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	timePtr = (TimeBufferType*)mappedResource.pData;
	timePtr->time = time;
	timePtr->amplitude = amplitude;      
	timePtr->PI_and_Wavelength.x = atan(1.f)*4.f; // value of pi 
	timePtr->PI_and_Wavelength.y = wavelength;  // wavelength
	timePtr->steepness = steepness;
	timePtr->speed = speed;
	deviceContext->Unmap(timeBuffer, 0);
	deviceContext->VSSetConstantBuffers(2, 1, &timeBuffer);

	//send camera position
	CameraBufferType* camDataPtr;
	result = deviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	camDataPtr = (CameraBufferType*)mappedResource.pData;
	camDataPtr->position = camPos;
	camDataPtr->padding = 0.0f;
	deviceContext->Unmap(cameraBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &cameraBuffer);

	// send light data to pixel shader
	LightBufferType* lightPtr;
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;
	for (int i = 0; i < 2; i++)
	{		
		lightPtr->position[i] = XMFLOAT4(light[i]->getPosition().x, light[i]->getPosition().y, light[i]->getPosition().z, 0.0f);
		lightPtr->specularPower[i].x = light[i]->getSpecularPower();
		if (normals) {
			lightPtr->specularPower[i].w = 1;
		}
		else {
			lightPtr->specularPower[i].w = 0;
		}
		lightPtr->specular[i] = light[i]->getSpecularColour();
	}

	// adjust brightness, so that it can be changed through the user interface
	lightPtr->ambient[0] = XMFLOAT4(light[0]->getAmbientColour().x * dirBrightness, light[0]->getAmbientColour().y * dirBrightness, light[0]->getAmbientColour().z * dirBrightness, light[0]->getAmbientColour().w * dirBrightness);
	lightPtr->diffuse[0] = XMFLOAT4(light[0]->getDiffuseColour().x * dirBrightness, light[0]->getDiffuseColour().y * dirBrightness, light[0]->getDiffuseColour().z * dirBrightness, light[0]->getDiffuseColour().w * dirBrightness);
	lightPtr->ambient[1] = XMFLOAT4(light[1]->getAmbientColour().x * ptBrightness, light[1]->getAmbientColour().y * ptBrightness, light[1]->getAmbientColour().z * ptBrightness, light[1]->getAmbientColour().w * ptBrightness);
	lightPtr->diffuse[1] = XMFLOAT4(light[1]->getDiffuseColour().x * ptBrightness, light[1]->getDiffuseColour().y * ptBrightness, light[1]->getDiffuseColour().z * ptBrightness, light[1]->getDiffuseColour().w * ptBrightness);

	//setting the direction of the moonlight to follow the camera, for specular lighting
	lightPtr->direction[0] = XMFLOAT4(camPos.x - light[0]->getPosition().x, -70, camPos.z - light[0]->getPosition().z, 0.0f);
	lightPtr->direction[1] = XMFLOAT4(light[1]->getDirection().x, light[1]->getDirection().y, light[1]->getDirection().z, 0.0f);	
	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);




	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &sampleState);

}

