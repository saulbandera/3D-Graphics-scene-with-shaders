#pragma once
#include "DXF.h"
#include "WaveShader.h"
#include <iostream>
#include <Timer.h>
#include "DepthShader.h"
#include "ShadowCubeMap.h"
#include "DepthCubeShader.h"
#include "ShadowShader.h"


class Island
{

public:
	Island(HWND hwnd, D3D* renderer, TextureManager* textureMgr);
	~Island();


	void depthPass(Light* light[2]);

	void depthPassCUBE(Light* light[2]);
	void render(XMMATRIX& camview, XMMATRIX projection, Light* light[2], XMFLOAT3 camPos, float brightness, float dirbrightness, bool normals);

	void renderinBlack(XMMATRIX& camview, XMMATRIX projection, Light* light[2], XMFLOAT3 camPos, float brightness, float dirbrightness, bool normals);


	inline XMMATRIX lookAt(const XMFLOAT3& eyePos, const XMFLOAT3& focusPos, const XMFLOAT3& upDir) {
		return XMMatrixLookAtLH(
			XMLoadFloat3(&eyePos),
			XMLoadFloat3(&focusPos),
			XMLoadFloat3(&upDir)
		);
	}


private:

	D3D* renderer;
	TextureManager* textureMgr;

	ShadowMap* islandShadowMap;
	ShadowCubeMap* islandShadowCubeMap;


	DepthShader* depthShader;
	DepthCubeShader* depthCubeShader;
	
	
	
	ShadowShader* shadowShader;

	AModel* island;
	AModel* campfire;
	AModel* trunk;
	AModel* leaves;
	AModel* rocks;

};

