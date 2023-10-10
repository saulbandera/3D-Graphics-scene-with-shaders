#include "ShadowCubeMap.h"

ShadowCubeMap::ShadowCubeMap(ID3D11Device* device, int mWidth, int mHeight)
{
	width = mWidth;
	height = mHeight;

	// create description of a 2D texture cube (array of 6 2D textures)
	D3D11_TEXTURE2D_DESC texDesc{};
	texDesc.Width = mWidth;
	texDesc.Height = mHeight;
	texDesc.ArraySize = 6;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.MipLevels = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;

	device->CreateTexture2D(&texDesc, 0, &cubeTexture);
	assert(cubeTexture);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MipLevels = texDesc.MipLevels;
	device->CreateShaderResourceView(cubeTexture, &srvDesc, &cubemap);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	dsvDesc.Texture2DArray.ArraySize = 6;
	dsvDesc.Texture2DArray.FirstArraySlice = 0;
	device->CreateDepthStencilView(cubeTexture, &dsvDesc, &cubeDSV);



	viewport.Width = (float)mWidth;
	viewport.Height = (float)mHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

}

ShadowCubeMap::~ShadowCubeMap()
{
	delete cubeDSV;
	delete cubemap;
}

void ShadowCubeMap::BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* dc)
{
	dc->RSSetViewports(1, &viewport);	
	dc->OMSetRenderTargets(1, &renderTarget, cubeDSV);
	dc->ClearDepthStencilView(cubeDSV, D3D11_CLEAR_DEPTH, 1.f, 0);
}

int ShadowCubeMap::getWidth()
{
	return width;
}

int ShadowCubeMap::getHeight()
{
	return height;
}


