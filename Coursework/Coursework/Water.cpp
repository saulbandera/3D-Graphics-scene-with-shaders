#include "Water.h"

Water::Water(HWND hwnd, D3D* rendr, TextureManager* texMgr)
{
	renderer = rendr;
	textureMgr = texMgr;

	textureMgr->loadTexture(L"waves", L"res/waves.png");
	waveMesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext(), 100);
	wavesShader = new WaveShader(renderer->getDevice(), hwnd);


	speed = 3.f;
	amplitude = 0.3f;
	steepness = 0.9f;
	wavelength = 20.0f;
}

Water::~Water()
{
	if (wavesShader) {
		delete wavesShader;
	}
	if (waveMesh) {
		delete waveMesh;
	}
}

void Water::render(XMMATRIX& camview, Light* light[2], float time, XMFLOAT3 camPos, float ptBrightness, float dirBrightness, bool normals)
{
	
	// render the wave mesh plane using the waves shader that is responsible for manipulating the vertices of the plane 
	waveMesh->sendData(renderer->getDeviceContext());
	wavesShader->setShaderParameters(renderer->getDeviceContext(), renderer->getWorldMatrix(), camview, renderer->getProjectionMatrix(), textureMgr->getTexture(L"waves"), light, time, camPos, speed, amplitude, wavelength, steepness, ptBrightness, dirBrightness, normals);
	wavesShader->render(renderer->getDeviceContext(), waveMesh->getIndexCount());
}


void Water::GUI()
{
	// User interface settings for the wave 
	if (ImGui::CollapsingHeader("Water Settings")) {
		ImGui::SliderFloat(" Speed", &speed, 0.0f, 100.0f);
		ImGui::SliderFloat(" Amplitude", &amplitude, 0.01f, 3.0f);
		ImGui::SliderFloat(" Steepness", &steepness, 0.0f, 1.0f);
		ImGui::SliderFloat(" Wavelength", &wavelength, 6.0f, 100.f);


		if (ImGui::Button(" Water Default Values", ImVec2(170, 20))) {
			speed = 3.f;
			amplitude = 0.3f;
			steepness = 0.9f;
			wavelength = 20.0f;
		}
	}		

}
