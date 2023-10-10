#include "App1.h"

App1::App1()
{
	water = nullptr;
	island = nullptr;
	moon = nullptr;
	moonShader = nullptr;
	moonTarget = nullptr;
	aboveThresholdBrightness = nullptr;
	downsampledTexture = nullptr;
	mainScene = nullptr;
	bloomShader = nullptr;
	thresholdCheckShader = nullptr;	
	horizontalBlurShader = nullptr;
	verticalBlurShader = nullptr;
	orthoMesh = nullptr;
	renderNormals = 0;

}


///////////////////////////           [ INITIALISATION ]           ///////////////////////////                     
void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN)
{
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// init scene models 
	water =  new Water(hwnd, renderer, textureMgr);
	island = new Island(hwnd, renderer, textureMgr);
	moon =   new SphereMesh(renderer->getDevice(), renderer->getDeviceContext(), 100);
	moonShader = new MoonShader(renderer->getDevice(), hwnd);
	textureMgr->loadTexture(L"moon", L"res/moon.png");


	// bloom related shaders initialisation 
	thresholdCheckShader = new ThresholdCheckShader(renderer->getDevice(), hwnd);
	thresholdCheckShader->setBrightnessThreshold(0.01);
	bloomShader = new BloomShader(renderer->getDevice(), hwnd);
	horizontalBlurShader = new HorizontalBlurShader(renderer->getDevice(), hwnd);
	verticalBlurShader = new VerticalBlurShader(renderer->getDevice(), hwnd);


	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight, 0.0f, 0.0f);

	
	downsampleAmount = 7;
	int downsampledWidth = sWidth / downsampleAmount;
	int downsampledHeight = sHeight / downsampleAmount;
	bloomColour = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.f);
	// render textures used for post processing 
	mainScene =    new RenderTexture(renderer->getDevice(), sWidth, sHeight, 0.1f, 100.0f);
	moonTarget =   new RenderTexture(renderer->getDevice(), sWidth, sHeight, 0.1f, 100.0f);
	aboveThresholdBrightness =  new RenderTexture(renderer->getDevice(), sWidth, sHeight, 0.1f, 100.0f);
	downsampledTexture = new RenderTexture(renderer->getDevice(), downsampledWidth, downsampledHeight,0.1f, 100.0f);

	
	// Variables for defining shadow map
	int sceneWidth = 100;
	int sceneHeight = 100;

	// directional light definition 
	light[0] = new Light;
	light[0] = new Light();
	light[0]->setAmbientColour(0.01f, 0.01f, 0.01f, 1.0f);
	light[0]->setDiffuseColour(0.4f, 0.4f, 0.5f, 1.0f);
	light[0]->setPosition(50.0f, 70.0f, 80.0f);
	light[0]->setDirection(0.0f, -1.0f, -0.7f);
	light[0]->setSpecularColour(0.3f, 0.3f, 0.4f, 1.0f);
	light[0]->setSpecularPower(50.0f);
	light[0]->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);

	//point light definition
	light[1] = new Light();
	light[1]->setAmbientColour(0.1f, 0.1f, 0.1f, 1.0f);
	light[1]->setDiffuseColour(1.0f, 0.2f, 0.f, 1.0f);
	light[1]->setPosition(47.5f, 8.0f, 47.5f);
	light[1]->setDirection(0.0f, 1.0f, 0.0f);
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	if (water) {
		delete water;
	}

	if (island) {
		delete island;
	}

	if (light) {
		delete light;
	}

	if (mainScene) {
		delete mainScene;
	}

	if (moonTarget) {
		delete moonTarget;
	}

	if (aboveThresholdBrightness) {
		delete aboveThresholdBrightness;
	}

	if (downsampledTexture) {
		delete downsampledTexture;
	}


	if (moon) {
		delete moon;
	}

	if (moonShader) {
		delete moonShader;
	}

	if (thresholdCheckShader) {
		delete thresholdCheckShader;
	}

	if (bloomShader) {
		delete bloomShader;
	}

	if (horizontalBlurShader) {
		delete horizontalBlurShader;
	}

	if (verticalBlurShader) {
		delete verticalBlurShader;
	}


		
}

///////////////////////////           [ FRAME ]           /////////////////////////// 
bool App1::frame()
{

	bool result;
	time.frame();

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}

	// Render the graphics.
	result = render();

	if (!result)
	{
		return false;
	}
	return true;
}


///////////////////////////           [ RENDER PASS ]           ///////////////////////////                     
void App1::renderMainPass()
{
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();


	// Set the render target to be the main scene render texture
	mainScene->setRenderTarget(renderer->getDeviceContext());
	mainScene->clearRenderTarget(renderer->getDeviceContext(), 0.07f, 0.07f, 0.09f, 1.0f);

		// render the main scene objects to texture 
		water->render(viewMatrix, light, timeVar, camera->getPosition(), ptBrightness, dirBrightness, renderNormals);

		island->render(viewMatrix, projectionMatrix, light, camera->getPosition(), ptBrightness, dirBrightness, renderNormals);

		worldMatrix = renderer->getWorldMatrix();
		worldMatrix = renderer->getWorldMatrix();
		worldMatrix = XMMatrixTranslation(light[0]->getPosition().x / 10.f, light[0]->getPosition().y / 10.f, light[0]->getPosition().z / 10.f);
		XMMATRIX scaleMatrix = XMMatrixScaling(10.f, 10.f, 10.f);
		worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
		moon->sendData(renderer->getDeviceContext());
		moonShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, camera->getViewMatrix(), renderer->getProjectionMatrix(), textureMgr->getTexture(L"moon"), light, timeVar, camera->getPosition(), XMFLOAT4(0.2, 0, 0, 0), renderNormals);
		moonShader->render(renderer->getDeviceContext(), moon->getIndexCount());

	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}


///////////////////////////           [ MOON PASS ]           ///////////////////////////                     
void App1::renderMoonPass()
{
	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	// Set the render target to be the moon render texture
	moonTarget->setRenderTarget(renderer->getDeviceContext());
	moonTarget->clearRenderTarget(renderer->getDeviceContext(), 0.f, 0.f, 0.f, 1.0f);
	
	
		// render the moon to texture
		worldMatrix = renderer->getWorldMatrix();
		worldMatrix = renderer->getWorldMatrix();
		worldMatrix = XMMatrixTranslation(light[0]->getPosition().x / 10.f, light[0]->getPosition().y / 10.f, light[0]->getPosition().z / 10.f);
		XMMATRIX scaleMatrix = XMMatrixScaling(10.f, 10.f, 10.f);
		worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);

		island->renderinBlack(viewMatrix, projectionMatrix, light, camera->getPosition(), 1, 1, false);
		moon->sendData(renderer->getDeviceContext());
		moonShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, camera->getViewMatrix(), renderer->getProjectionMatrix(), textureMgr->getTexture(L"moon"), light, timeVar, camera->getPosition(), XMFLOAT4(0.5, 0, 0, 0), renderNormals);
		moonShader->render(renderer->getDeviceContext(), moon->getIndexCount());

	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}


///////////////////////////           [ BLOOM PASS ]           ///////////////////////////                     
void App1::bloomPass()
{

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();


	// Set the render target to be the threshold texture 
	aboveThresholdBrightness->setRenderTarget(renderer->getDeviceContext());
	aboveThresholdBrightness->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 1.0f, 1.0f);

		// Send the moon render texture and check that each pixel is above a certain level of brightness, if it is then it gets rendered to new render texture
		orthoMesh->sendData(renderer->getDeviceContext());
		thresholdCheckShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, moonTarget->getShaderResourceView(), bloomColour);
		thresholdCheckShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();




	// Set the render target to be the downsampled render texture
	downsampledTexture->setRenderTarget(renderer->getDeviceContext());
	downsampledTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 0.0f, 1.0f, 1.0f);

		// the size of the downsampled render texture 
		XMFLOAT2 size = XMFLOAT2(downsampledTexture->getTextureWidth(), downsampledTexture->getTextureHeight());

		// ortho mesh is renderered with the horizontally blurred texture of bright areas 
		orthoMesh->sendData(renderer->getDeviceContext());
		horizontalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, aboveThresholdBrightness->getShaderResourceView(), size.x); 
		horizontalBlurShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
 



	// Set the render target back to the moon render texture
	moonTarget->setRenderTarget(renderer->getDeviceContext());
	moonTarget->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 0.0f, 1.0f); 

		// ortho mesh is rendered with the vertically blurred texture of the horizontally-blurred-downsampled texture  
		orthoMesh->sendData(renderer->getDeviceContext());
		verticalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, downsampledTexture->getShaderResourceView(), size.y); 
		verticalBlurShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}


///////////////////////////           [ FINAL PASS ]           ///////////////////////////                     
void App1::finalPass()
{
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();

	
	if (!renderer->getWireframeState()) {

		// render the ortho mesh textured with the combined full scene and bloomed moon texture 

		orthoMesh->sendData(renderer->getDeviceContext());
		bloomShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, mainScene->getShaderResourceView(), moonTarget->getShaderResourceView()); 
		bloomShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());




	}
	else {
		// if wireframe is on dont render ortho mesh as it covers the screen 

		water->render(viewMatrix, light, timeVar, camera->getPosition(), ptBrightness, dirBrightness, renderNormals);
		island->render(viewMatrix, projectionMatrix, light, camera->getPosition(), ptBrightness, dirBrightness, renderNormals);
		worldMatrix = renderer->getWorldMatrix();
		worldMatrix = XMMatrixTranslation(light[0]->getPosition().x / 10.f, light[0]->getPosition().y / 10.f, light[0]->getPosition().z / 10.f);
		XMMATRIX scaleMatrix = XMMatrixScaling(10.f, 10.f, 10.f);
		worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
		moon->sendData(renderer->getDeviceContext());
		moonShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, camera->getViewMatrix(), renderer->getProjectionMatrix(), textureMgr->getTexture(L"moon"), light, timeVar, camera->getPosition(), XMFLOAT4(0.2, 0, 0, 0), renderNormals);
		moonShader->render(renderer->getDeviceContext(), moon->getIndexCount());
	}

}


///////////////////////////           [ RENDER ]           ///////////////////////////                     
bool App1::render()
{
	// Clear the scene. 
	renderer->beginScene(0.07f, 0.07f, 0.09f, 1.0f);
	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();

	timeVar += time.getTime(); // increase time counter for gerstner wave

	island->depthPass(light); // depth pass for directional light
	island->depthPassCUBE(light); // depth pass for point light

	renderMainPass(); // render main scene to texture
	renderMoonPass(); // render moon to texture
	bloomPass(); // apply bloom to moon render texture 
	finalPass(); // render the final scene with bloom

	// Render GUI
	gui();

	// Present the rendered scene to the screen.
	renderer->endScene();
	return true;
}



///////////////////////////           [ USER INTERFACE ]           ///////////////////////////                     
void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text(" FPS: %.2f", timer->getFPS());
	ImGui::Checkbox(" Wireframe mode", &wireframeToggle);
	ImGui::Checkbox(" Render normals", &renderNormals);
	// water settings
	water->GUI();

	// directional light settings
	float dirDiffuse[4] = { light[0]->getDiffuseColour().x, light[0]->getDiffuseColour().y, light[0]->getDiffuseColour().z ,light[0]->getDiffuseColour().w };
	float dirAmbient[4] = { light[0]->getAmbientColour().x, light[0]->getAmbientColour().y, light[0]->getAmbientColour().z ,light[0]->getAmbientColour().w };
	float dirSpecular[4] = { light[0]->getSpecularColour().x, light[0]->getSpecularColour().y, light[0]->getSpecularColour().z ,light[0]->getSpecularColour().w };
	float specularValue = light[0]->getSpecularPower();
	float bloomCol[4] = { bloomColour.x, bloomColour.y, bloomColour.z, 0};
	 
	if (ImGui::CollapsingHeader("Moon Settings")) {

		if (ImGui::ColorEdit3(" Diffuse colour ", (float*)&dirDiffuse)) {
			light[0]->setDiffuseColour(dirDiffuse[0], dirDiffuse[1], dirDiffuse[2], dirDiffuse[3]);

		}
		if (ImGui::ColorEdit3(" Ambient colour ", (float*)&dirAmbient)) {
			light[0]->setAmbientColour(dirAmbient[0], dirAmbient[1], dirAmbient[2], dirAmbient[3]);

		}
		if (ImGui::ColorEdit3(" Specular colour ", (float*)&dirSpecular)) {
			light[0]->setSpecularColour(dirSpecular[0], dirSpecular[1], dirSpecular[2], dirSpecular[3]);

		}
		if (ImGui::SliderFloat(" Specular power", &specularValue, 0, 200)) {
			light[0]->setSpecularPower(specularValue);
		}


		ImGui::SliderFloat(" Brightness Value ", (float*)&dirBrightness, 0, 10);



		if (ImGui::ColorEdit3(" Bloom colour ", (float*)&bloomCol)) {
			bloomColour.x = bloomCol[0];
			bloomColour.y = bloomCol[1];
			bloomColour.z = bloomCol[2];

		}

		bool changed = ImGui::SliderInt(" Bloom size", &downsampleAmount, 1, 20);

		if (changed) {
			delete downsampledTexture;

			int dsWidth = sWidth / downsampleAmount;
			int dsHeight = sHeight / downsampleAmount;

			downsampledTexture = new RenderTexture(renderer->getDevice(), dsWidth, dsHeight, 0.1f, 30.0f);
		}



		if (ImGui::Button(" Moon Default Values", ImVec2(170, 20))) {
			light[0]->setAmbientColour(0.01f, 0.01f, 0.01f, 1.0f);
			light[0]->setDiffuseColour(0.4f, 0.4f, 0.5f, 1.0f);
			light[0]->setSpecularColour(0.3f, 0.3f, 0.4f, 1.0f);
			light[0]->setSpecularPower(50.0f);
			dirBrightness = 1;
			downsampleAmount = 7;
			bloomColour = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.f);

			delete downsampledTexture;

			int dsWidth = sWidth / downsampleAmount;
			int dsHeight = sHeight / downsampleAmount;

			downsampledTexture = new RenderTexture(renderer->getDevice(), dsWidth, dsHeight, 0.1f, 30.0f);
		}
	}


	// point light settings
	float ptDiffuse[4] = { light[1]->getDiffuseColour().x, light[1]->getDiffuseColour().y, light[1]->getDiffuseColour().z ,light[1]->getDiffuseColour().w };
	float ptAmbient[4] = { light[1]->getAmbientColour().x, light[1]->getAmbientColour().y, light[1]->getAmbientColour().z ,light[1]->getAmbientColour().w };
	float ptSpecular[4] = { light[1]->getSpecularColour().x, light[1]->getSpecularColour().y, light[1]->getSpecularColour().z ,light[1]->getSpecularColour().w };
	
	ImVec2 pos = { light[1]->getPosition().x, light[1]->getPosition().z };
	float xPosition = light[1]->getPosition().x;
	float yPosition = light[1]->getPosition().y;
	float zPosition = light[1]->getPosition().z;
	

	if (ImGui::CollapsingHeader("Campfire Settings")) {
		
		if (ImGui::ColorEdit3(" Diffuse ", (float*)&ptDiffuse)) {
			light[1]->setDiffuseColour(ptDiffuse[0], ptDiffuse[1], ptDiffuse[2], ptDiffuse[3]);

		}

		if (ImGui::ColorEdit3(" Ambient ", (float*)&ptAmbient)) {
			light[1]->setAmbientColour(ptAmbient[0], ptAmbient[1], ptAmbient[2], ptAmbient[3]);

		}

		ImGui::SliderFloat(" Brightness ", (float*)&ptBrightness, 0, 10);

		


		if (ImGui::SliderFloat(" X Position", &xPosition, 0, 100)) {
			light[1]->setPosition(xPosition, light[1]->getPosition().y, light[1]->getPosition().z);
		}
		if (ImGui::SliderFloat(" Y Position", &yPosition, 0, 50)) {
			light[1]->setPosition(light[1]->getPosition().x, yPosition, light[1]->getPosition().z);
		}
		if (ImGui::SliderFloat(" Z Position", &zPosition, 0, 100)) {
			light[1]->setPosition(light[1]->getPosition().x, light[1]->getPosition().y, zPosition);
		}



		if (ImGui::Button(" Campfire Default Values", ImVec2(170, 20))) {
			light[1]->setAmbientColour(0.1f, 0.1f, 0.1f, 1.0f);
			light[1]->setDiffuseColour(1.0f, 0.2f, 0.f, 1.0f);
			light[1]->setPosition(47.5f, 8.0f, 47.5f);
			ptBrightness = 1;
		}

	}

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
