#include "Island.h"

Island::Island(HWND hwnd, D3D* rendr, TextureManager* texMgr)
{

	textureMgr = texMgr;
	renderer = rendr;

	// Load textures for island
	textureMgr->loadTexture(L"campfire", L"res/models/campfire/Textures/HD/Campfire_MAT_BaseColor_00.jpg");
	textureMgr->loadTexture(L"sand", L"res/models/sandd-1.jpg"); 
	textureMgr->loadTexture(L"trunk", L"res/models/palmtree/_palma_4_apg_.png"); 
	textureMgr->loadTexture(L"leaves", L"res/models/palmtree/_palma_1_leaves_apg_.png"); 
	textureMgr->loadTexture(L"rocks", L"res/models/rocks.jpg"); 
	textureMgr->loadTexture(L"black", L"res/black.jpg");

	// Import models
	island = new AModel(renderer->getDevice(), "res/models/islandTerrain2.obj");
	campfire = new AModel(renderer->getDevice(), "res/models/campfire/OBJ/campfire2.obj");
	trunk = new AModel(renderer->getDevice(), "res/models/palmtree/trunkC.obj");
	leaves = new AModel(renderer->getDevice(), "res/models/palmtree/leaves.obj");
	rocks = new AModel(renderer->getDevice(), "res/models/rocks.obj");


	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	depthCubeShader = new DepthCubeShader(renderer->getDevice(), hwnd);
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd);



	// Variables for defining shadow map
	int shadowmapSize = 2048;

	// create depth cube map
	islandShadowCubeMap = new ShadowCubeMap(renderer->getDevice(), shadowmapSize, shadowmapSize);

	// create shadowmap
	islandShadowMap = new ShadowMap(renderer->getDevice(), shadowmapSize, shadowmapSize);
}

Island::~Island()
{
	if (island) {
		delete island;
	}

	if (campfire) {
		delete campfire;
	}

	if (trunk) {
		delete trunk;
	}

	if (leaves) {
		delete leaves;
	}

	if (depthShader) {
		delete depthShader;
	}

	if (islandShadowMap) {
		delete islandShadowMap;
	}

	if (depthCubeShader) {
		delete depthCubeShader;
	}

	if (shadowShader) {
		delete shadowShader;
	}

	if (islandShadowCubeMap) {
		delete islandShadowCubeMap;
	}

}


///////////////////////////           [ DEPTH PASS ]           ///////////////////////////                     
void Island::depthPass(Light* light[2])
{
	// set render to depth buffer
	islandShadowMap->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	light[0]->generateViewMatrix();
	XMMATRIX lightViewMatrix = light[0]->getViewMatrix();
	XMMATRIX lightProjectionMatrix = light[0]->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	// render models which cast shadow on island from directinal light 
	
	// render trunk 
	worldMatrix = XMMatrixTranslation(56.0f, 9.74f, 42.0f);
	trunk->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), trunk->getIndexCount());

	// render leaves 
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(56.0f, 14.74f, 42.0f);
	leaves->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), leaves->getIndexCount());


	// render trunk 2
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(40.0f, 8.3f, 40.0f);
	trunk->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), trunk->getIndexCount());

	// render leaves 2
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(40.0f, 13.3f, 40.0f);
	leaves->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), leaves->getIndexCount());


	// render rocks 
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(57.0f / 0.2f, 4.9f / 0.2f, 57.0f / 0.2f);
	XMMATRIX scaleMatrix = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	rocks->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), rocks->getIndexCount());


	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}


///////////////////////////           [ DEPTH CUBE PASS ]           ///////////////////////////                     
void Island::depthPassCUBE(Light* light[2])
{

	XMMATRIX worldMatrix = renderer->getWorldMatrix();


	XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, 1, 1.0f, 100.f);
	XMMATRIX shadowTransforms[6];
	// define the matrices used by the geometry shader when generating the cube depth map 
	XMFLOAT3 lightPos = light[1]->getPosition();
	shadowTransforms[0] = XMMatrixLookAtLH(XMLoadFloat3(&lightPos), XMLoadFloat3(&XMFLOAT3(lightPos.x + 1.0, lightPos.y + 0.0, lightPos.z + 0.0)), XMVectorSet(0.0, 1.0, 0.0, 0.0));
	shadowTransforms[1] = XMMatrixLookAtLH(XMLoadFloat3(&lightPos), XMLoadFloat3(&XMFLOAT3(lightPos.x - 1.0, lightPos.y + 0.0, lightPos.z + 0.0)), XMVectorSet(0.0, 1.0, 0.0, 0.0));
	shadowTransforms[2] = XMMatrixLookAtLH(XMLoadFloat3(&lightPos), XMLoadFloat3(&XMFLOAT3(lightPos.x + 0.0, lightPos.y + 1.0, lightPos.z + 0.0)), XMVectorSet(0.0, 0.0,-1.0, 0.0));
	shadowTransforms[3] = XMMatrixLookAtLH(XMLoadFloat3(&lightPos), XMLoadFloat3(&XMFLOAT3(lightPos.x + 0.0, lightPos.y - 1.0, lightPos.z + 0.0)), XMVectorSet(0.0, 0.0,1.0, 0.0));
	shadowTransforms[4] = XMMatrixLookAtLH(XMLoadFloat3(&lightPos), XMLoadFloat3(&XMFLOAT3(lightPos.x + 0.0, lightPos.y + 0.0, lightPos.z + 1.0)), XMVectorSet(0.0, 1.0, 0.0, 0.0));
	shadowTransforms[5] = XMMatrixLookAtLH(XMLoadFloat3(&lightPos), XMLoadFloat3(&XMFLOAT3(lightPos.x + 0.0, lightPos.y + 0.0, lightPos.z - 1.0)), XMVectorSet(0.0, 1.0, 0.0, 0.0));



	// render depth to texture cube 
	islandShadowCubeMap->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	// render models which cast a shadow from the point light 

	// render trunk 
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(56.0f, 9.74f, 42.0f);
	trunk->sendData(renderer->getDeviceContext());
	depthCubeShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, projection, shadowTransforms, light[1]->getPosition());
	depthCubeShader->render(renderer->getDeviceContext(), trunk->getIndexCount());

	// render leaves 
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(56.0f, 14.74f, 42.0f);
	leaves->sendData(renderer->getDeviceContext());
	depthCubeShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, projection, shadowTransforms, light[1]->getPosition());
	depthCubeShader->render(renderer->getDeviceContext(), leaves->getIndexCount());


	// render trunk 2
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(40.0f, 8.3f, 40.0f);
	trunk->sendData(renderer->getDeviceContext());
	depthCubeShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, projection, shadowTransforms, light[1]->getPosition());
	depthCubeShader->render(renderer->getDeviceContext(), trunk->getIndexCount());

	// render leaves 2
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(40.0f, 13.3f, 40.0f);
	leaves->sendData(renderer->getDeviceContext());
	depthCubeShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, projection, shadowTransforms, light[1]->getPosition());
	depthCubeShader->render(renderer->getDeviceContext(), leaves->getIndexCount());

	// render rocks 
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(57.0f / 0.2f, 4.8f / 0.2f, 57.0f / 0.2f);
	XMMATRIX scaleMatrix = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	rocks->sendData(renderer->getDeviceContext());
	depthCubeShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, projection, shadowTransforms, light[1]->getPosition());
	depthCubeShader->render(renderer->getDeviceContext(), rocks->getIndexCount());

	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}


///////////////////////////           [ RENDER ]           ///////////////////////////                     
void Island::render(XMMATRIX& camview, XMMATRIX projection, Light* light[2], XMFLOAT3 camPos, float ptBrightness, float dirBrightness, bool normals)
{
	// render island
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(50.f / 1.5, -2.6f / 1.5, 50.f / 1.5);
	XMMATRIX scaleMatrix = XMMatrixScaling(1.5f, 1.5f, 1.5f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	island->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, camview, projection, textureMgr->getTexture(L"sand"),islandShadowMap->getDepthMapSRV(), islandShadowCubeMap->getDepthMapSRV(), camPos, light, ptBrightness, dirBrightness, normals);
	shadowShader->render(renderer->getDeviceContext(), island->getIndexCount());

	// render campfire
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(light[1]->getPosition().x / 0.03f, (light[1]->getPosition().y - 3.8f) / 0.03f, light[1]->getPosition().z / 0.03);
	scaleMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	campfire->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, camview, projection, textureMgr->getTexture(L"campfire"), islandShadowMap->getDepthMapSRV(), islandShadowCubeMap->getDepthMapSRV(), camPos, light, ptBrightness, dirBrightness, normals);
	shadowShader->render(renderer->getDeviceContext(), campfire->getIndexCount());

	// render trunk 
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(56.0f, 9.74f, 42.0f);
	trunk->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, camview, projection, textureMgr->getTexture(L"trunk"), islandShadowMap->getDepthMapSRV(), islandShadowCubeMap->getDepthMapSRV(), camPos, light, ptBrightness, dirBrightness, normals);
	shadowShader->render(renderer->getDeviceContext(), trunk->getIndexCount());

	// render leaves 
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(56.0f, 14.74f, 42.0f);
	leaves->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, camview, projection, textureMgr->getTexture(L"leaves"), islandShadowMap->getDepthMapSRV(), islandShadowCubeMap->getDepthMapSRV(), camPos, light, ptBrightness, dirBrightness, normals);
	shadowShader->render(renderer->getDeviceContext(), leaves->getIndexCount());

	// render trunk 2
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(40.0f, 8.3f, 40.0f);
	trunk->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, camview, projection, textureMgr->getTexture(L"trunk"), islandShadowMap->getDepthMapSRV(), islandShadowCubeMap->getDepthMapSRV(), camPos, light, ptBrightness, dirBrightness, normals);
	shadowShader->render(renderer->getDeviceContext(), trunk->getIndexCount());

	// render leaves 2
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(40.0f, 13.3f, 40.0f);
	leaves->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, camview, projection, textureMgr->getTexture(L"leaves"), islandShadowMap->getDepthMapSRV(), islandShadowCubeMap->getDepthMapSRV(), camPos, light, ptBrightness, dirBrightness, normals);
	shadowShader->render(renderer->getDeviceContext(), leaves->getIndexCount());

	// render rocks 
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(57.0f / 0.2f, 4.9f / 0.2f, 57.0f / 0.2f);
	scaleMatrix = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	rocks->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, camview, projection, textureMgr->getTexture(L"rocks"), islandShadowMap->getDepthMapSRV(), islandShadowCubeMap->getDepthMapSRV(), camPos, light, ptBrightness, dirBrightness, normals);
	shadowShader->render(renderer->getDeviceContext(), rocks->getIndexCount());
}

void Island::renderinBlack(XMMATRIX& camview, XMMATRIX projection, Light* light[2], XMFLOAT3 camPos, float ptBrightness, float dirBrightness, bool normals)
{
	// render island
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(50.f / 1.5, -2.6f / 1.5, 50.f / 1.5);
	XMMATRIX scaleMatrix = XMMatrixScaling(1.5f, 1.5f, 1.5f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	island->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, camview, projection, textureMgr->getTexture(L"black"), islandShadowMap->getDepthMapSRV(), islandShadowCubeMap->getDepthMapSRV(), camPos, light, ptBrightness, dirBrightness, normals);
	shadowShader->render(renderer->getDeviceContext(), island->getIndexCount());

	// render campfire
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(light[1]->getPosition().x / 0.03f, (light[1]->getPosition().y - 3.8f) / 0.03f, light[1]->getPosition().z / 0.03);
	scaleMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	campfire->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, camview, projection, textureMgr->getTexture(L"black"), islandShadowMap->getDepthMapSRV(), islandShadowCubeMap->getDepthMapSRV(), camPos, light, ptBrightness, dirBrightness, normals);
	shadowShader->render(renderer->getDeviceContext(), campfire->getIndexCount());

	// render trunk 
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(56.0f, 9.74f, 42.0f);
	trunk->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, camview, projection, textureMgr->getTexture(L"black"), islandShadowMap->getDepthMapSRV(), islandShadowCubeMap->getDepthMapSRV(), camPos, light, ptBrightness, dirBrightness, normals);
	shadowShader->render(renderer->getDeviceContext(), trunk->getIndexCount());

	// render leaves 
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(56.0f, 14.74f, 42.0f);
	leaves->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, camview, projection, textureMgr->getTexture(L"black"), islandShadowMap->getDepthMapSRV(), islandShadowCubeMap->getDepthMapSRV(), camPos, light, ptBrightness, dirBrightness, normals);
	shadowShader->render(renderer->getDeviceContext(), leaves->getIndexCount());

	// render trunk 2
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(40.0f, 8.3f, 40.0f);
	trunk->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, camview, projection, textureMgr->getTexture(L"black"), islandShadowMap->getDepthMapSRV(), islandShadowCubeMap->getDepthMapSRV(), camPos, light, ptBrightness, dirBrightness, normals);
	shadowShader->render(renderer->getDeviceContext(), trunk->getIndexCount());

	// render leaves 2
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(40.0f, 13.3f, 40.0f);
	leaves->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, camview, projection, textureMgr->getTexture(L"black"), islandShadowMap->getDepthMapSRV(), islandShadowCubeMap->getDepthMapSRV(), camPos, light, ptBrightness, dirBrightness, normals);
	shadowShader->render(renderer->getDeviceContext(), leaves->getIndexCount());

	// render rocks 
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(57.0f / 0.2f, 4.9f / 0.2f, 57.0f / 0.2f);
	scaleMatrix = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	rocks->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, camview, projection, textureMgr->getTexture(L"black"), islandShadowMap->getDepthMapSRV(), islandShadowCubeMap->getDepthMapSRV(), camPos, light, ptBrightness, dirBrightness, normals);
	shadowShader->render(renderer->getDeviceContext(), rocks->getIndexCount());
}
