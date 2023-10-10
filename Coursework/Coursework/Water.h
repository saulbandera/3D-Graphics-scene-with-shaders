#pragma once
#include "DXF.h"
#include "WaveShader.h"
#include <iostream>
#include <Timer.h>
class Water
{

public:
	Water(HWND hwnd, D3D* renderer, TextureManager* textureMgr);
	~Water();

	void render(XMMATRIX &camview, Light* light[2], float time, XMFLOAT3 camPos, float ptBrightness, float dirBrightness, bool normals);
	void GUI();
	

private:
	WaveShader* wavesShader;
	PlaneMesh* waveMesh;
	D3D* renderer;
	TextureManager* textureMgr;

	// variables to change water appearance 
	float wavelength;
	float speed;
	float amplitude; 
	float steepness;	
};

