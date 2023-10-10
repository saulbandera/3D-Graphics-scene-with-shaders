// Application.h
#ifndef _APP1_H
#define _APP1_H
#include "DXF.h"
#include <iostream>
#include <Timer.h>
#include "Water.h"
#include "Island.h"
#include "BloomShader.h"
#include "HorizontalBlurShader.h"
#include "VerticalBlurShader.h"
#include "ThresholdCheckShader.h"
#include "MoonShader.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);
	bool frame();


protected:
	void renderMainPass();
	void renderMoonPass();
	void bloomPass();
	void finalPass();

	bool render();

	void gui();


private:

	float timeVar;
	Timer time;

	Light* light[2];  // scene lights

	// models 
	Water* water;
	Island* island;
	SphereMesh* moon;
	MoonShader* moonShader;


	// bloom related render textures 
	RenderTexture* moonTarget;
	RenderTexture* aboveThresholdBrightness;
	RenderTexture* downsampledTexture;
	RenderTexture* mainScene;


	// bloom related shaders
	BloomShader* bloomShader;
	ThresholdCheckShader* thresholdCheckShader;
	HorizontalBlurShader* horizontalBlurShader;
	VerticalBlurShader* verticalBlurShader;
	int downsampleAmount;
	XMFLOAT4 bloomColour;
	// 2D orthographic mesh used for post processing
	OrthoMesh* orthoMesh;
	bool renderNormals;

	float ptBrightness = 1;
	float dirBrightness = 1;
};

#endif