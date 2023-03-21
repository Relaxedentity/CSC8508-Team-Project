#include "GameTechRenderer.h"
#include <reactphysics3d/reactphysics3d.h>
#include "GameObject.h"
#include "RenderObject.h"
#include "Camera.h"
#include "TextureLoader.h"
#include "Debug.h"
#include "Gamelock.h"
#include "MapNode.h"
#include <string>
using namespace NCL;
using namespace Rendering;
using namespace CSC8503;

#define SHADOWSIZE 4096

Matrix4 biasMatrix = Matrix4::Translation(Vector3(0.5f, 0.5f, 0.5f)) * Matrix4::Scale(Vector3(0.5f, 0.5f, 0.5f));

GameTechRenderer::GameTechRenderer(GameWorld& world) : OGLRenderer(*Window::GetWindow()), gameWorld(world)	{
	glEnable(GL_DEPTH_TEST);

	debugShader  = new OGLShader("debug.vert", "debug.frag");
	shadowShader = new OGLShader("shadow.vert", "shadow.frag");

	screenAspectSplit = ((float)windowWidth * 0.5) / (float)windowHeight;
	screenAspect = (float)windowWidth / (float)windowHeight;
	
	// build texture that serves as the shadow attachment of the first framebuffer
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);


	// build texture that will serve as colour attachment of the second framebuffer

	// shadow buffer
	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	
	glClearColor(1, 1, 1, 1);

	//Animated Objects
	animatedShader = new OGLShader("skinningVertex.glsl", "charFrag.frag"); 

	//Set up the light properties
	lightColour = Vector4(0.8f, 0.8f, 0.5f, 1.0f);
	lightRadius = 1000.0f;
	lightPosition = Vector3(-100.0f, 80.0f, -100.0f);

	//Skybox!
	skyboxShader = new OGLShader("skybox.vert", "skybox.frag");
	skyboxMesh = new OGLMesh();
	skyboxMesh->SetVertexPositions({Vector3(-1, 1,-1), Vector3(-1,-1,-1) , Vector3(1,-1,-1) , Vector3(1,1,-1) });
	skyboxMesh->SetVertexIndices({ 0,1,2,2,3,0 });
	skyboxMesh->UploadToGPU();
	LoadSkybox();

	// healthbar
	healthShader = new OGLShader("basicVertex.glsl", "colourFragment.glsl");
	healthQuad = new OGLMesh();
	healthQuad->SetVertexPositions({ Vector3(-0.95, -0.80,-1), Vector3(-0.95,-0.90,-1) , Vector3(-0.6,-0.90,-1) , Vector3(-0.6, -0.80,-1) });
	healthQuad->SetVertexColours({ Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f) });
	healthQuad->SetVertexTextureCoords({ Vector2(0.0f, 0.0f), Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f), Vector2(1.0f, 0.0f) });
	healthQuad->SetVertexIndices({ 0,1,2,2,3,0 });
	healthQuad->UploadToGPU();

	// progressbar
	progressShader = new OGLShader("progressVert.glsl", "progressFrag.glsl");
	progressBar = new OGLMesh();
	progressBar->SetVertexPositions({ Vector3(-0.4, 0.85,-1), Vector3(-0.4,0.9,-1) , Vector3(0.4, 0.9,-1) , Vector3(0.4, 0.85,-1) });
	progressBar->SetVertexColours({ Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f) });
	progressBar->SetVertexTextureCoords({ Vector2(0.0f, 0.0f), Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f), Vector2(1.0f, 0.0f)});
	progressBar->SetVertexIndices({ 0,1,2,2,3,0 });
	progressBar->UploadToGPU();

	// loadingBar  //////////////////////////////////////////
	loadingBarShader = new OGLShader("LoadVert.glsl", "LoadFrag.glsl");
	loadingBar = new OGLMesh();
	loadingBar->SetVertexPositions({ Vector3(-0.5, 0,-1), Vector3(-0.5,0.1,-1) , Vector3(0.5, 0.1,-1) , Vector3(0.5, 0,-1) });
	loadingBar->SetVertexColours({ Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f) });
	loadingBar->SetVertexTextureCoords({ Vector2(0.0f, 0.0f), Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f), Vector2(1.0f, 0.0f) });
	loadingBar->SetVertexIndices({ 0,1,2,2,3,0 });
	loadingBar->UploadToGPU();

	//timer quad
	simpleShader = new OGLShader("simpleVert.glsl", "simpleFrag.glsl");
	quad = new OGLMesh();
	quad->GenerateQuad(quad);
	quad->UploadToGPU();

	// crosshair quad 
	aimShader = new OGLShader("aimVert.glsl", "aimFrag.glsl");
	aimQuad = new OGLMesh();

	aimTex = new OGLTexture();
	aimTex = (OGLTexture*)aimTex->RGBATextureFromFilename("crosshair018.png");
	
	aimQuad->SetVertexPositions({ Vector3(-0.05, -0.1,-1), Vector3(-0.05, 0.1,-1) , Vector3(0.05, 0.1,-1) , Vector3(0.05, -0.1,-1) });
	aimQuad->SetVertexColours({ Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f) });
	aimQuad->SetVertexTextureCoords({ Vector2(0.0f, 0.0f), Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f), Vector2(1.0f, 0.0f) });
	aimQuad->SetVertexIndices({ 0,1,2,2,3,0 });
	aimQuad->UploadToGPU();

	// mini map
	miniMapWall = new OGLShader("circle.vert", "circle.frag");
	//miniMapEnemy = new OGLShader()
	miniMapPlayer = new OGLShader("triangle.vert", "triangle.frag");

	//Scoure
	Score1Tex = new OGLTexture();
	Score2Tex = new OGLTexture();
	Score3Tex = new OGLTexture();
	Score4Tex = new OGLTexture();
	Score1Mesh = new OGLMesh();
	Score2Mesh = new OGLMesh();
	Score3Mesh = new OGLMesh();
	Score4Mesh = new OGLMesh();

	CoopScore1Tex = new OGLTexture();
	CoopScore2Tex = new OGLTexture();
	CoopScore3Tex = new OGLTexture();
	CoopScore4Tex = new OGLTexture();
	CoopScore1Mesh = new OGLMesh();
	CoopScore2Mesh = new OGLMesh();
	CoopScore3Mesh = new OGLMesh();
	CoopScore4Mesh = new OGLMesh();


	Score1Mesh->SetVertexPositions({ Vector3(0.15, 0.55,0), Vector3(0.15, 0.4,0) , Vector3(0.1, 0.4, 0) , Vector3(0.1, 0.55, 0) });
	Score1Mesh->SetVertexColours({ Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f) });
	Score1Mesh->SetVertexTextureCoords({ Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f),Vector2(0.0f, 1.0f), Vector2(0.0f, 0.0f), });
	Score1Mesh->SetVertexIndices({ 0,1,2,2,3,0 });
	Score1Mesh-> UploadToGPU();
	Score2Mesh->SetVertexPositions({ Vector3(0.2, 0.55,0), Vector3(0.2, 0.4,0) , Vector3(0.15, 0.4, 0) , Vector3(0.15, 0.55, 0) });
	Score2Mesh->SetVertexColours({ Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f) });
	Score2Mesh->SetVertexTextureCoords({ Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f),Vector2(0.0f, 1.0f), Vector2(0.0f, 0.0f), });
	Score2Mesh->SetVertexIndices({ 0,1,2,2,3,0 });
	Score2Mesh->UploadToGPU();
	Score3Mesh->SetVertexPositions({ Vector3(0.25, 0.55,0), Vector3(0.25, 0.4,0) , Vector3(0.2, 0.4, 0) , Vector3(0.2, 0.55, 0) });
	Score3Mesh->SetVertexColours({ Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f) });
	Score3Mesh->SetVertexTextureCoords({ Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f),Vector2(0.0f, 1.0f), Vector2(0.0f, 0.0f), });
	Score3Mesh->SetVertexIndices({ 0,1,2,2,3,0 });
	Score3Mesh->UploadToGPU();
	Score4Mesh->SetVertexPositions({ Vector3(0.3, 0.55,0), Vector3(0.3, 0.4,0) , Vector3(0.25, 0.4, 0) , Vector3(0.25, 0.55, 0) });
	Score4Mesh->SetVertexColours({ Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f) });
	Score4Mesh->SetVertexTextureCoords({ Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f),Vector2(0.0f, 1.0f), Vector2(0.0f, 0.0f), });
	Score4Mesh->SetVertexIndices({ 0,1,2,2,3,0 });
	Score4Mesh->UploadToGPU();

	CoopScore1Mesh->SetVertexPositions({ Vector3(0.15, 0.35,0), Vector3(0.15, 0.2,0) , Vector3(0.1, 0.2, 0) , Vector3(0.1, 0.35, 0) });
	CoopScore1Mesh->SetVertexColours({ Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f) });
	CoopScore1Mesh->SetVertexTextureCoords({ Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f),Vector2(0.0f, 1.0f), Vector2(0.0f, 0.0f), });
	CoopScore1Mesh->SetVertexIndices({ 0,1,2,2,3,0 });
	CoopScore1Mesh->UploadToGPU();
	CoopScore2Mesh->SetVertexPositions({ Vector3(0.2, 0.35,0), Vector3(0.2, 0.2,0) , Vector3(0.15, 0.2, 0) , Vector3(0.15, 0.35, 0) });
	CoopScore2Mesh->SetVertexColours({ Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f) });
	CoopScore2Mesh->SetVertexTextureCoords({ Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f),Vector2(0.0f, 1.0f), Vector2(0.0f, 0.0f), });
	CoopScore2Mesh->SetVertexIndices({ 0,1,2,2,3,0 });
	CoopScore2Mesh->UploadToGPU();
	CoopScore3Mesh->SetVertexPositions({ Vector3(0.25, 0.35,0), Vector3(0.25, 0.2,0) , Vector3(0.2, 0.2, 0) , Vector3(0.2, 0.35, 0) });
	CoopScore3Mesh->SetVertexColours({ Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f) });
	CoopScore3Mesh->SetVertexTextureCoords({ Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f),Vector2(0.0f, 1.0f), Vector2(0.0f, 0.0f), });
	CoopScore3Mesh->SetVertexIndices({ 0,1,2,2,3,0 });
	CoopScore3Mesh->UploadToGPU();
	CoopScore4Mesh->SetVertexPositions({ Vector3(0.3, 0.35,0), Vector3(0.3, 0.2,0) , Vector3(0.25, 0.2, 0) , Vector3(0.25, 0.35, 0) });
	CoopScore4Mesh->SetVertexColours({ Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f) });
	CoopScore4Mesh->SetVertexTextureCoords({ Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f),Vector2(0.0f, 1.0f), Vector2(0.0f, 0.0f), });
	CoopScore4Mesh->SetVertexIndices({ 0,1,2,2,3,0 });
	CoopScore4Mesh->UploadToGPU();


	//
	uiShader = new OGLShader("ui.vert", "ui.frag");
	menuTex = new OGLTexture();
	menuTex = (OGLTexture*)menuTex->SRGBTextureFromFilename("MenuBG.png");
	menuMesh = new OGLMesh();
	menuMesh->SetVertexPositions({ Vector3(1, 1.1,0), Vector3(1, -1.1,0) , Vector3(-1, -1.1,0) , Vector3(-1, 1.1,0) });
	menuMesh->SetVertexColours({ Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f) });
	menuMesh->SetVertexTextureCoords({  Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f),Vector2(0.0f, 1.0f), Vector2(0.0f, 0.0f), });
	menuMesh->SetVertexIndices({ 0,1,2,2,3,0 });
	menuMesh->UploadToGPU();

	normalBtnTex = new OGLTexture();
	normalBtnTex = (OGLTexture*)normalBtnTex->SRGBTextureFromFilename("mode1.png");
	normalBtnMesh = new OGLMesh();
	normalBtnMesh->SetVertexPositions({ Vector3(0.2, 0.3,0), Vector3(0.2, 0.1,0) , Vector3(-0.2, 0.1, 0) , Vector3(-0.2, 0.3, 0) });
	normalBtnMesh->SetVertexColours({ Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f) });
	normalBtnMesh->SetVertexTextureCoords({ Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f),Vector2(0.0f, 1.0f), Vector2(0.0f, 0.0f), });
	normalBtnMesh->SetVertexIndices({ 0,1,2,2,3,0 });
	normalBtnMesh->UploadToGPU();

	coopBtnTex = new OGLTexture();
	coopBtnTex = (OGLTexture*)coopBtnTex->SRGBTextureFromFilename("mode2.png");
	coopBtnMesh = new OGLMesh();
	coopBtnMesh->SetVertexPositions({ Vector3(0.2, 0,0), Vector3(0.2, -0.2,0) , Vector3(-0.2, -0.2, 0) , Vector3(-0.2, 0, 0) });
	coopBtnMesh->SetVertexColours({ Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f) });
	coopBtnMesh->SetVertexTextureCoords({ Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f),Vector2(0.0f, 1.0f), Vector2(0.0f, 0.0f), });
	coopBtnMesh->SetVertexIndices({ 0,1,2,2,3,0 });
	coopBtnMesh->UploadToGPU();

	introBtnTex = new OGLTexture();
	introBtnTex = (OGLTexture*)introBtnTex->SRGBTextureFromFilename("intro.png");
	introBtnMesh = new OGLMesh();
	introBtnMesh->SetVertexPositions({ Vector3(0.2, -0.3,0), Vector3(0.2, -0.5,0) , Vector3(-0.2, -0.5, 0) , Vector3(-0.2, -0.3, 0) });
	introBtnMesh->SetVertexColours({ Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f) });
	introBtnMesh->SetVertexTextureCoords({ Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f),Vector2(0.0f, 1.0f), Vector2(0.0f, 0.0f), });
	introBtnMesh->SetVertexIndices({ 0,1,2,2,3,0 });
	introBtnMesh->UploadToGPU();

	exitBtnTex = new OGLTexture();
	exitBtnTex = (OGLTexture*)exitBtnTex->SRGBTextureFromFilename("exit.png");
	exitBtnMesh = new OGLMesh();
	exitBtnMesh->SetVertexPositions({ Vector3(0.1, -0.6,0), Vector3(0.1, -0.77,0) , Vector3(-0.1, -0.77, 0) , Vector3(-0.1, -0.6, 0) });
	exitBtnMesh->SetVertexColours({ Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f) });
	exitBtnMesh->SetVertexTextureCoords({ Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f),Vector2(0.0f, 1.0f), Vector2(0.0f, 0.0f), });
	exitBtnMesh->SetVertexIndices({ 0,1,2,2,3,0 });
	exitBtnMesh->UploadToGPU();

	endTex = new OGLTexture();
	endTex = (OGLTexture*)endTex->SRGBTextureFromFilename("EndBG.png");
	endMesh = new OGLMesh();
	endMesh->SetVertexPositions({ Vector3(1, 1.1,0), Vector3(1, -1.1,0) , Vector3(-1, -1.1,0) , Vector3(-1, 1.1,0) });
	endMesh->SetVertexColours({ Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f) });
	endMesh->SetVertexTextureCoords({ Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f),Vector2(0.0f, 1.0f), Vector2(0.0f, 0.0f), });
	endMesh->SetVertexIndices({ 0,1,2,2,3,0 });
	endMesh->UploadToGPU();

	sinscoreTex = new OGLTexture();
	sinscoreTex = (OGLTexture*)sinscoreTex->SRGBTextureFromFilename("Score_3.png");
	sinscoreMesh = new OGLMesh();
	sinscoreMesh->SetVertexPositions({ Vector3(0.15, 0.6,0), Vector3(0.15, 0.4,0) , Vector3(-0.4, 0.4, 0) , Vector3(-0.4, 0.6, 0) });
	sinscoreMesh->SetVertexColours({ Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f) });
	sinscoreMesh->SetVertexTextureCoords({ Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f),Vector2(0.0f, 1.0f), Vector2(0.0f, 0.0f), });
	sinscoreMesh->SetVertexIndices({ 0,1,2,2,3,0 });
	sinscoreMesh->UploadToGPU();

	cooscore1Tex = new OGLTexture();
	cooscore1Tex = (OGLTexture*)cooscore1Tex->SRGBTextureFromFilename("Score_5.png");
	cooscore1Mesh = new OGLMesh();
	cooscore1Mesh->SetVertexPositions({ Vector3(0.09, 0.55,0), Vector3(0.09, 0.4,0) , Vector3(-0.29, 0.4, 0) , Vector3(-0.29, 0.55, 0) });
	cooscore1Mesh->SetVertexColours({ Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f) });
	cooscore1Mesh->SetVertexTextureCoords({ Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f),Vector2(0.0f, 1.0f), Vector2(0.0f, 0.0f), });
	cooscore1Mesh->SetVertexIndices({ 0,1,2,2,3,0 });
	cooscore1Mesh->UploadToGPU();

	cooscore2Tex = new OGLTexture();
	cooscore2Tex = (OGLTexture*)cooscore2Tex->SRGBTextureFromFilename("Score_6.png");
	cooscore2Mesh = new OGLMesh();
	cooscore2Mesh->SetVertexPositions({ Vector3(0.09, 0.35,0), Vector3(0.09, 0.2,0) , Vector3(-0.29, 0.2, 0) , Vector3(-0.29, 0.35, 0) });
	cooscore2Mesh->SetVertexColours({ Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f) });
	cooscore2Mesh->SetVertexTextureCoords({ Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f),Vector2(0.0f, 1.0f), Vector2(0.0f, 0.0f), });
	cooscore2Mesh->SetVertexIndices({ 0,1,2,2,3,0 });
	cooscore2Mesh->UploadToGPU();

	endexitBtnTex = new OGLTexture();
	endexitBtnTex = (OGLTexture*)endexitBtnTex->SRGBTextureFromFilename("endexit.png");
	endexitBtnMesh = new OGLMesh();
	endexitBtnMesh->SetVertexPositions({ Vector3(0.11, -0.26, 0), Vector3(0.11, -0.45, 0), Vector3(-0.11, -0.45, 0), Vector3(-0.11, -0.26, 0)});
	endexitBtnMesh->SetVertexColours({ Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f) });
	endexitBtnMesh->SetVertexTextureCoords({ Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f),Vector2(0.0f, 1.0f), Vector2(0.0f, 0.0f), });
	endexitBtnMesh->SetVertexIndices({ 0,1,2,2,3,0 });
	endexitBtnMesh->UploadToGPU();

	
	vector<Vector2> normalBtn = { {  Vector2(-0.2, 0.3) , Vector2(0.2, 0.1) } };
	vector<Vector2> coopBtn = { {  Vector2(-0.2,0),   Vector2(0.2,-0.2)  } };
	vector<Vector2> introBtn = { {  Vector2(-0.2,-0.3),  Vector2(0.2,-0.5)} };
	vector<Vector2> exitBtn = { {  Vector2(-0.1,-0.6),  Vector2(0.1,-0.75)} };
	vector<Vector2> endexitBtn = { {  Vector2(-0.11, -0.26) , Vector2(0.11, -0.45)} };
	vector<Vector2> coopendexitBtn = { {  Vector2(-0.11, -0.26) , Vector2(0.11, -0.45)} };
	vector<Vector2> backBtn = { {  Vector2(-0.1, -0.6),   Vector2(0.1, -0.75)  } };
	

	loadingTex = new OGLTexture();
	loadingTex = (OGLTexture*)loadingTex->SRGBTextureFromFilename("loadingBG.png");
	loadingMesh = new OGLMesh();
	loadingMesh->SetVertexPositions({ Vector3(1, 1.1,0), Vector3(1, -1.1,0) , Vector3(-1, -1.1,0) , Vector3(-1, 1.1,0) });
	loadingMesh->SetVertexColours({ Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f) });
	loadingMesh->SetVertexTextureCoords({ Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f),Vector2(0.0f, 1.0f), Vector2(0.0f, 0.0f), });
	loadingMesh->SetVertexIndices({ 0,1,2,2,3,0 });
	loadingMesh->UploadToGPU();

	intropageTex = new OGLTexture();
	intropageTex = (OGLTexture*)intropageTex->SRGBTextureFromFilename("intropage.png");
	intropageMesh = new OGLMesh();
	intropageMesh->SetVertexPositions({ Vector3(1, 1.1,0), Vector3(1, -1.1,0) , Vector3(-1, -1.1,0) , Vector3(-1, 1.1,0) });
	intropageMesh->SetVertexColours({ Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f) });
	intropageMesh->SetVertexTextureCoords({ Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f),Vector2(0.0f, 1.0f), Vector2(0.0f, 0.0f), });
	intropageMesh->SetVertexIndices({ 0,1,2,2,3,0 });
	intropageMesh->UploadToGPU();

	backBtnTex = new OGLTexture();
	backBtnTex = (OGLTexture*)backBtnTex->SRGBTextureFromFilename("back.png");
	backBtnMesh = new OGLMesh();
	backBtnMesh->SetVertexPositions({ Vector3(0.1, -0.59,0), Vector3(0.1, -0.77,0) , Vector3(-0.11, -0.77, 0) , Vector3(-0.11, -0.59, 0) });
	backBtnMesh->SetVertexColours({ Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f) });
	backBtnMesh->SetVertexTextureCoords({ Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f),Vector2(0.0f, 1.0f), Vector2(0.0f, 0.0f), });
	backBtnMesh->SetVertexIndices({ 0,1,2,2,3,0 });
	backBtnMesh->UploadToGPU();

	GameLock::IntroButtonPos.push_back(normalBtn);
	GameLock::IntroButtonPos.push_back(coopBtn);
	GameLock::IntroButtonPos.push_back(introBtn);
	GameLock::IntroButtonPos.push_back(exitBtn);
	GameLock::EndButtonPos.push_back(endexitBtn);
	GameLock::CoopButtonPos.push_back(coopendexitBtn);
	GameLock::BackButtonPos.push_back(backBtn);
	
	glGenVertexArrays(1, &lineVAO);
	glGenVertexArrays(1, &textVAO);

	glGenBuffers(1, &lineVertVBO);
	glGenBuffers(1, &textVertVBO);
	glGenBuffers(1, &textColourVBO);
	glGenBuffers(1, &textTexVBO);

	SetDebugStringBufferSizes(10000);
	SetDebugLineBufferSizes(1000);
}

GameTechRenderer::~GameTechRenderer()	{
	delete debugShader;
	delete shadowShader;
	delete skyboxShader;
	delete progressShader;
	delete simpleShader;

	delete healthShader;
	delete skyboxMesh;	
	delete healthQuad;
	delete progressBar;
	delete quad;
	delete loadingBarShader;

	delete aimShader;
	delete aimQuad;
	delete aimTex;

	delete loadingBar;
	delete miniMapPlayer;
	delete miniMapEnemy;
	delete miniMapWall;

	delete endTex;
	delete cooscore1Tex;
	delete cooscore2Tex;

	delete intropageTex; 
	delete menuTex;

	delete Score1Tex;
	delete Score2Tex;
	delete Score3Tex;
	delete Score4Tex;

	delete CoopScore1Tex;
	delete CoopScore2Tex;
	delete CoopScore3Tex;
	delete CoopScore4Tex;
	
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);

	//glDeleteTextures(1, &secScreenTex);
	//glDeleteFramebuffers(1, &secScreenFBO);
}

void GameTechRenderer::LoadSkybox() {
	string filenames[6] = {
		"/Cubemap/skyrender0004.png",
		"/Cubemap/skyrender0001.png",
		"/Cubemap/skyrender0003.png",
		"/Cubemap/skyrender0006.png",
		"/Cubemap/skyrender0002.png",
		"/Cubemap/skyrender0005.png"
	};

	int width[6]	= { 0 };
	int height[6]	= { 0 };
	int channels[6] = { 0 };
	int flags[6]	= { 0 };

	vector<char*> texData(6, nullptr);

	for (int i = 0; i < 6; ++i) {
		TextureLoader::LoadTexture(filenames[i], texData[i], width[i], height[i], channels[i], flags[i]);
		if (i > 0 && (width[i] != width[0] || height[0] != height[0])) {
			std::cout << __FUNCTION__ << " cubemap input textures don't match in size?\n";
			return;
		}
	}
	glGenTextures(1, &skyboxTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

	GLenum type = channels[0] == 4 ? GL_RGBA : GL_RGB;

	for (int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width[i], height[i], 0, type, GL_UNSIGNED_BYTE, texData[i]);
	}

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void NCL::CSC8503::GameTechRenderer::RenderCircle(float cx, float cy, float r, const Vector4& color)
{
	BindShader(miniMapWall);
	const auto& u_center = glGetUniformLocation(miniMapWall->GetProgramID(), "u_Center");
	const auto& u_color = glGetUniformLocation(miniMapWall->GetProgramID(), "u_Color");
	const auto& u_radius = glGetUniformLocation(miniMapWall->GetProgramID(), "u_Radius");

	// rectangle that covers the whole screen
	// cause we might draw circles anywhere in the screen
	float map_area[] = {
		-1.0f,
		-1.0f,
		1.0f,
		-1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		-1.0f,
		1.0f,
		-1.0f,
		-1.0f,
	};

	// bind a new vertex array
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// bind vertex buffers
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), (const void*)map_area, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

	// set shader uniforms
	glUniform2f(u_center, cx, cy);
	glUniform4f(u_color, color.x, color.y, color.z, color.w);
	glUniform1f(u_radius, r);

	// perform draw call
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void NCL::CSC8503::GameTechRenderer::RenderTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Vector4& color, Vector2 windowSize)
{
	BindShader(miniMapPlayer);
	const auto& u_color = glGetUniformLocation(miniMapPlayer->GetProgramID(), "u_Color");

	v1.x = (v1.x / windowSize.x) * 2 - 1.0f;
	v1.y = (v1.y / windowSize.y) * 2 - 1.0f;

	v2.x = (v2.x / windowSize.x) * 2 - 1.0f;
	v2.y = (v2.y / windowSize.y) * 2 - 1.0f;

	v3.x = (v3.x / windowSize.x) * 2 - 1.0f;
	v3.y = (v3.y / windowSize.y) * 2 - 1.0f;

	// form the triangle in constrained format
	float map_area[] = {
		v1.x,
		v1.y,
		v2.x,
		v2.y,
		v3.x,
		v3.y,
	};

	// bind a new vertex array
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// bind vertex buffers
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), (const void*)map_area, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

	// set shader uniforms
	glUniform4f(u_color, color.x, color.y, color.z, color.w);

	// perform draw call
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void NCL::CSC8503::GameTechRenderer::RenderMap(Vector2 window_pos, Vector2 windowSize, GameObject* player)
{
	float map_x = windowSize.x - 100.0f;
	float map_y = 100;
	float map_size = 80.0f;
	float draw_distance = 50.0f;
	Vector4 map_color(0.8f, 0.8f, 0.8f, 0.7f);
	RenderCircle(window_pos.x + map_x, window_pos.y + map_y, map_size, map_color);

	// player triangle(center in map)
	// in counter clock order
	float p_size = 10.0f;
	Vector2 p_left(map_x - p_size, map_y - p_size);
	Vector2 p_right(map_x + p_size, map_y - p_size);
	Vector2 p_up(map_x, map_y + p_size);
	Vector4 p_color(0.0f, 0.8f, 0.0f, 0.7f);
	glClear(GL_DEPTH_BUFFER_BIT);
	RenderTriangle(p_left, p_right, p_up, p_color, windowSize);

	/*const auto &player = gameWorld.GetPlayer();*/
	const auto& camera = gameWorld.GetMainCamera();

	// objects that is near the player
	vector<MapNode*> near_objects;

	gameWorld.OperateOnMapNodes(
		[&](MapNode* o)
		{
	        Vector2 p_pos(player->GetPhysicsObject()->getTransform().getPosition().x, player->GetPhysicsObject()->getTransform().getPosition().z);
        	Vector2 o_pos(o->location.x, o->location.z);
	        float dis = (p_pos - o_pos).Length();
	        if (dis < draw_distance)
	        {
		       near_objects.push_back(o);
	        }
		});

	// rotation matrix of camera
	// the built in yaw is a little bit strange , which is not continous
	// make it continous here
	float camera_yaw = camera->GetYaw() < 0 ? -camera->GetYaw() : 360.0f - camera->GetYaw();
	// cause the initial pos of the player is facing down , offset the yaw by 180 degress
	// but at the same time , rotated map is x mirrored , will inverse that in the code below
	Matrix2 rotation = Matrix2::Matrix2().Rotation(camera_yaw - 180.0f);
	const auto& rot_array = rotation.array;

	// get inverse of the rotation
	float coe = 1.0f / (rot_array[1][1] * rot_array[0][0] - rot_array[1][0] * rot_array[0][1]);
	Matrix2 inverse = rotation
		.SetColumn(0, Vector2(rot_array[0][0], -rot_array[0][1]) * coe)
		.SetColumn(1, Vector2(-rot_array[1][0], rot_array[1][1]) * coe);

	Vector4 gooseColor(0.0f, 0.0f, 0.3f, 0.7f);

	for (const auto& object : near_objects)
	{
		Vector2 p_pos(player->GetPhysicsObject()->getTransform().getPosition().x, player->GetPhysicsObject()->getTransform().getPosition().z);
		Vector2 o_pos(object->location.x, object->location.z);

		int rect_width = 10;
		int rect_height = 10;

		Vector2 tl = { o_pos.x - rect_width / 2, o_pos.y + rect_height / 2 };
		Vector2 br = { o_pos.x + rect_width / 2, o_pos.y - rect_height / 2 };

		// the relative vector without any rotation for the object
		Vector2 tl_vec = tl - p_pos;
		Vector2 br_vec = br - p_pos;

		const float tl_dis = tl_vec.Length();
		const float br_dis = br_vec.Length();

		// inverse the yaw of the cameraf
		Vector2 tl_vec_inverse = inverse * tl_vec.Normalised();
		Vector2 br_vec_inverse = inverse * br_vec.Normalised();

		// determin how far we should draw from the player ?
		float tl_draw_ratio = tl_dis / draw_distance;
		float br_draw_ratio = br_dis / draw_distance;

		tl_vec_inverse.x *= -1;
		br_vec_inverse.x *= -1;
		// the final position in the map
		Vector2 tl_pos = Vector2(map_x, map_y) + ((tl_vec_inverse * map_size) * tl_draw_ratio);
		Vector2 br_pos = Vector2(map_x, map_y) + ((br_vec_inverse * map_size) * br_draw_ratio);

		glClear(GL_DEPTH_BUFFER_BIT);

		// I'm working out a colour to depict each rectangle as here, based upon its constituent objects and their paint nodes.

		Vector4 finalColour = object->getMapColour();

		// the reason why the rectangle is rotating , is because we ignored that the cube also has a
		// facing direction , and rotate just the center of it ,
		// but the decisive points are all required lp;p;p;p;p;p;to be rotated (top left , bottom right)
		RenderRectangle(tl_pos, br_pos, finalColour, windowSize);
	}

	// std::cout << "near item count: " << near_objects.size() << std::endl;

	// std::cout << "player"<< ": " << player->GetPhysicsObject()->getTransform().getPosition() << std::endl;

	// std::cout << "camera"
	///	<< ": "
	//	<< " pos:" << camera->GetPosition()
	//	<< " pitch:" << camera->GetPitch()
	//	<< " yaw:" << camera_yaw
	//	<< std::endl;

	return;

}
// render a rectangle with 2 points given , top left point  and  the bottom right point
void NCL::CSC8503::GameTechRenderer::RenderRectangle(Vector2 tl, Vector2 br, Vector4 color, Vector2 windowSize)
{

	Vector2 point_1 = tl;
	Vector2 point_2 = br;
	Vector2 mid = { (tl.x + br.x) / 2, (tl.y + br.y) / 2 };
	Vector2 diagonal = tl - mid;

	Matrix2 rot_90 = Matrix2::Matrix2().Rotation(90.0);
	// the other diagonal is the diagonal above rotated by 90 degress
	auto diag_other = rot_90 * diagonal;
	// one of the point is diag_other - the mid
	auto point_3 = diag_other + mid;

	// the other is asymmetric
	auto point_4 = mid * 2 - point_3;

	RenderTriangle(point_1, point_4, point_2, color, windowSize);
	RenderTriangle(point_2, point_3, point_1, color, windowSize);
}
//void NCL::CSC8503::GameTechRenderer::RenderRectangle(float px, float py, float width, float height, Vector4& color)
//{
//	
//}

void NCL::CSC8503::GameTechRenderer::RenderMainMenu() {
	BindShader(uiShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, menuTex->GetObjectID());
	glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

	BindMesh(menuMesh);
	DrawBoundMesh();
}

void NCL::CSC8503::GameTechRenderer::RenderNormalButton() {
	BindShader(uiShader);

	if (GameLock::normalBtnChange) normalBtnTex = (OGLTexture*)normalBtnTex->SRGBTextureFromFilename("mode1_selected.png");
	else normalBtnTex = (OGLTexture*)normalBtnTex->SRGBTextureFromFilename("mode1.png");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, normalBtnTex->GetObjectID());
	glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

	BindMesh(normalBtnMesh);
	DrawBoundMesh();
	delete normalBtnTex;
}

void NCL::CSC8503::GameTechRenderer::RenderCoopButton() {
	BindShader(uiShader);
	
	if (GameLock::coopBtnChange) coopBtnTex = (OGLTexture*)coopBtnTex->SRGBTextureFromFilename("mode2_selected.png");
	else coopBtnTex = (OGLTexture*)coopBtnTex->SRGBTextureFromFilename("mode2.png");
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, coopBtnTex->GetObjectID());
	glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

	BindMesh(coopBtnMesh);
	DrawBoundMesh();
	delete coopBtnTex;
}


void NCL::CSC8503::GameTechRenderer::RenderIntroButton() {
	BindShader(uiShader);

	if (GameLock::introBtnChange) introBtnTex = (OGLTexture*)introBtnTex->SRGBTextureFromFilename("intro_selected.png");
	else introBtnTex = (OGLTexture*)introBtnTex->SRGBTextureFromFilename("intro.png");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, introBtnTex->GetObjectID());
	glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

	BindMesh(introBtnMesh);
	DrawBoundMesh();
	delete introBtnTex;
}

void NCL::CSC8503::GameTechRenderer::RenderExitButton(){
	BindShader(uiShader);

	if (GameLock::exitBtnChange) exitBtnTex = (OGLTexture*)exitBtnTex->SRGBTextureFromFilename("exit_selected.png");
	else exitBtnTex = (OGLTexture*)exitBtnTex->SRGBTextureFromFilename("exit.png");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, exitBtnTex->GetObjectID());
	glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

	BindMesh(exitBtnMesh);
	DrawBoundMesh();
	delete exitBtnTex;
}

void NCL::CSC8503::GameTechRenderer::RenderIntroPage() { 
	BindShader(uiShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, intropageTex->GetObjectID());
	glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

	BindMesh(intropageMesh);
	DrawBoundMesh();
}

void NCL::CSC8503::GameTechRenderer::RenderBackButton() { 
	BindShader(uiShader);

	if (GameLock::introBtnChange) backBtnTex = (OGLTexture*)backBtnTex->SRGBTextureFromFilename("back_selected.png");
	else backBtnTex = (OGLTexture*)backBtnTex->SRGBTextureFromFilename("back.png");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, backBtnTex->GetObjectID());
	glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

	BindMesh(backBtnMesh);
	DrawBoundMesh();
	delete backBtnTex;
}

void NCL::CSC8503::GameTechRenderer::RenderEndScreen() {
	BindShader(uiShader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, endTex->GetObjectID());
	glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

	BindMesh(endMesh);
	DrawBoundMesh();
}

void NCL::CSC8503::GameTechRenderer::RendererSinScore() {
	BindShader(uiShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sinscoreTex->GetObjectID());
	glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

	BindMesh(sinscoreMesh);
	DrawBoundMesh();
	//delete sinscoreTex;
}

void NCL::CSC8503::GameTechRenderer::RendererCooScore1() {
	BindShader(uiShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cooscore1Tex->GetObjectID());
	glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

	BindMesh(cooscore1Mesh);
	DrawBoundMesh();

	BindShader(uiShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Score1Tex->GetObjectID());
	glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

	BindMesh(Score1Mesh);
	DrawBoundMesh();
	int n = int(GameLock::RedS);
	if (!GameLock::istoString) {
		if (n > 999 && n <= 9999) {
			int s1 = n / 1000 % 10; a = std::to_string(s1);
			int s2 = n / 100 % 10;  b = std::to_string(s2);
			int s3 = n / 10 % 10;   c = std::to_string(s3);
			int s4 = n % 10;        d = std::to_string(s4);
			Score1Tex = (OGLTexture*)menuTex->SRGBTextureFromFilename(a + ".png");
			Score2Tex = (OGLTexture*)menuTex->SRGBTextureFromFilename(b + ".png");
			Score3Tex = (OGLTexture*)menuTex->SRGBTextureFromFilename(c + ".png");
			Score4Tex = (OGLTexture*)menuTex->SRGBTextureFromFilename(d + ".png");
		}
		else if (n > 99 && n <= 999) {
			int s1 = n / 100 % 10; a = std::to_string(s1);
			int s2 = n / 10 % 10;  b = std::to_string(s2);
			int s3 = n % 10;       c = std::to_string(s3);
			Score1Tex = (OGLTexture*)menuTex->SRGBTextureFromFilename(a + ".png");
			Score2Tex = (OGLTexture*)menuTex->SRGBTextureFromFilename(b + ".png");
			Score3Tex = (OGLTexture*)menuTex->SRGBTextureFromFilename(c + ".png");
		}
		else if (n > 9 && n <= 99) {
			int s1 = n / 10 % 10; a = std::to_string(s1);
			int s2 = n % 10;      b = std::to_string(s2);
			Score1Tex = (OGLTexture*)menuTex->SRGBTextureFromFilename(a + ".png");
			Score2Tex = (OGLTexture*)menuTex->SRGBTextureFromFilename(b+ ".png");
		}
		else {
			int s1 = n % 10;      a = std::to_string(s1);
			Score1Tex = (OGLTexture*)menuTex->SRGBTextureFromFilename(a + ".png");
		}
		GameLock::istoString = true;
	}


	if (n > 999 && n <= 9999) {
		BindShader(uiShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Score1Tex->GetObjectID());
		glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

		BindMesh(Score1Mesh);
		DrawBoundMesh();

		BindShader(uiShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Score2Tex->GetObjectID());
		glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

		BindMesh(Score2Mesh);
		DrawBoundMesh();

		BindShader(uiShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Score3Tex->GetObjectID());
		glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

		BindMesh(Score3Mesh);
		DrawBoundMesh();

		BindShader(uiShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Score4Tex->GetObjectID());
		glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

		BindMesh(Score4Mesh);
		DrawBoundMesh();

	}
	else if (n > 99 && n <= 999) {
		BindShader(uiShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Score1Tex->GetObjectID());
		glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

		BindMesh(Score1Mesh);
		DrawBoundMesh();

		BindShader(uiShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Score2Tex->GetObjectID());
		glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

		BindMesh(Score2Mesh);
		DrawBoundMesh();

		BindShader(uiShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Score3Tex->GetObjectID());
		glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

		BindMesh(Score3Mesh);
		DrawBoundMesh();

	}
	else if (n > 9 && n <= 99) {
		BindShader(uiShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Score1Tex->GetObjectID());
		glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

		BindMesh(Score1Mesh);
		DrawBoundMesh();

		BindShader(uiShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Score2Tex->GetObjectID());
		glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

		BindMesh(Score2Mesh);
		DrawBoundMesh();
	}
	else {
		BindShader(uiShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Score1Tex->GetObjectID());
		glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

		BindMesh(Score1Mesh);
		DrawBoundMesh();
	}

}

void NCL::CSC8503::GameTechRenderer::RendererCooScore2() {
	BindShader(uiShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cooscore2Tex->GetObjectID());
	glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

	BindMesh(cooscore2Mesh);
	DrawBoundMesh();

	int n = int(GameLock::BlueS);
	if (!GameLock::isto2String) {
		if (n > 999 && n <= 9999) {
			int s1 = n / 1000 % 10; a2 = std::to_string(s1);
			int s2 = n / 100 % 10;  b2 = std::to_string(s2);
			int s3 = n / 10 % 10;   c2 = std::to_string(s3);
			int s4 = n % 10;        d2 = std::to_string(s4);
			CoopScore1Tex = (OGLTexture*)menuTex->SRGBTextureFromFilename(a2 + ".png");
			CoopScore2Tex = (OGLTexture*)menuTex->SRGBTextureFromFilename(b2 + ".png");
			CoopScore3Tex = (OGLTexture*)menuTex->SRGBTextureFromFilename(c2 + ".png");
			CoopScore4Tex = (OGLTexture*)menuTex->SRGBTextureFromFilename(d2 + ".png");
		}
		else if (n > 99 && n <= 999) {
			int s1 = n / 100 % 10; a2 = std::to_string(s1);
			int s2 = n / 10 % 10;  b2 = std::to_string(s2);
			int s3 = n % 10;       c2 = std::to_string(s3);
			CoopScore1Tex = (OGLTexture*)menuTex->SRGBTextureFromFilename(a2 + ".png");
			CoopScore2Tex = (OGLTexture*)menuTex->SRGBTextureFromFilename(b2 + ".png");
			CoopScore3Tex = (OGLTexture*)menuTex->SRGBTextureFromFilename(c2 + ".png");
		}
		else if (n > 9 && n <= 99) {
			int s1 = n / 10 % 10; a2 = std::to_string(s1);
			int s2 = n % 10;      b2 = std::to_string(s2);
			CoopScore1Tex = (OGLTexture*)menuTex->SRGBTextureFromFilename(a2 + ".png");
			CoopScore2Tex = (OGLTexture*)menuTex->SRGBTextureFromFilename(b2 + ".png");
		}
		else {
			int s1 = n % 10;      a2 = std::to_string(s1);
			CoopScore1Tex = (OGLTexture*)menuTex->SRGBTextureFromFilename(a2 + ".png");
		}
		GameLock::isto2String = true;
	}


	if (n > 999 && n <= 9999) {
		BindShader(uiShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, CoopScore1Tex->GetObjectID());
		glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

		BindMesh(CoopScore1Mesh);
		DrawBoundMesh();

		BindShader(uiShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, CoopScore2Tex->GetObjectID());
		glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

		BindMesh(CoopScore2Mesh);
		DrawBoundMesh();

		BindShader(uiShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, CoopScore3Tex->GetObjectID());
		glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

		BindMesh(CoopScore3Mesh);
		DrawBoundMesh();

		BindShader(uiShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, CoopScore4Tex->GetObjectID());
		glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

		BindMesh(CoopScore4Mesh);
		DrawBoundMesh();

	}
	else if (n > 99 && n <= 999) {
		BindShader(uiShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, CoopScore1Tex->GetObjectID());
		glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

		BindMesh(CoopScore1Mesh);
		DrawBoundMesh();

		BindShader(uiShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, CoopScore2Tex->GetObjectID());
		glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

		BindMesh(CoopScore2Mesh);
		DrawBoundMesh();

		BindShader(uiShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, CoopScore3Tex->GetObjectID());
		glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

		BindMesh(CoopScore3Mesh);
		DrawBoundMesh();

	}
	else if (n > 9 && n <= 99) {
		BindShader(uiShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, CoopScore1Tex->GetObjectID());
		glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

		BindMesh(CoopScore1Mesh);
		DrawBoundMesh();

		BindShader(uiShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Score2Tex->GetObjectID());
		glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

		BindMesh(Score2Mesh);
		DrawBoundMesh();
	}
	else {
		BindShader(uiShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, CoopScore1Tex->GetObjectID());
		glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

		BindMesh(CoopScore1Mesh);
		DrawBoundMesh();
	}

}

void NCL::CSC8503::GameTechRenderer::RenderEndExitScreen() {
	BindShader(uiShader);
	if (GameLock::SingleExitBtnChange) endexitBtnTex = (OGLTexture*)endexitBtnTex->SRGBTextureFromFilename("endexit_selected.png");
	else endexitBtnTex = (OGLTexture*)endexitBtnTex->SRGBTextureFromFilename("endexit.png");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, endexitBtnTex->GetObjectID());
	glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

	BindMesh(endexitBtnMesh);
	DrawBoundMesh();
	delete endexitBtnTex;
}

void NCL::CSC8503::GameTechRenderer::RenderCoopEndExitScreen() {
	BindShader(uiShader);
	if (GameLock::CoopExitBtnChange) endexitBtnTex = (OGLTexture*)endexitBtnTex->SRGBTextureFromFilename("endexit_selected.png");
	else endexitBtnTex = (OGLTexture*)endexitBtnTex->SRGBTextureFromFilename("endexit.png");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, endexitBtnTex->GetObjectID());
	glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);
	BindMesh(endexitBtnMesh);
	DrawBoundMesh();
	//delete endexitBtnTex;
	//RenderTriangle(point_1, point_4, point_2, color, windowSize);
	//RenderTriangle(point_2, point_3, point_1, color, windowSize);
	//BindMesh(endexitBtnMesh);
	//DrawBoundMesh();
	delete endexitBtnTex;
}

void NCL::CSC8503::GameTechRenderer::RenderHealthBar(float health)
{
	glEnable(GL_BLEND);
	BindShader(healthShader);
	glUniform1f(glGetUniformLocation(healthShader->GetProgramID(), "health"), health);
	glUniform4f(glGetUniformLocation(healthShader->GetProgramID(), "firstColour"), 1,0,0,1);
	glUniform4f(glGetUniformLocation(healthShader->GetProgramID(), "secColour"), 0,1,0,1);
	BindMesh(healthQuad);
	DrawBoundMesh();
}

void NCL::CSC8503::GameTechRenderer::RenderProgressBar(float score)
{
	BindShader(progressShader);
	glUniform1f(glGetUniformLocation(progressShader->GetProgramID(), "score"), score);
	glUniform4f(glGetUniformLocation(progressShader->GetProgramID(), "playerColour"), 0.752f, 0.027f, 0.286f, 1);

	BindMesh(progressBar);
	DrawBoundMesh();
}



void NCL::CSC8503::GameTechRenderer::RenderLoadingScreen() {
	BindShader(uiShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, loadingTex->GetObjectID());
	glUniform1i(glGetUniformLocation(uiShader->GetProgramID(), "texture1"), 0);

	BindMesh(loadingMesh);
	DrawBoundMesh();

}

void NCL::CSC8503::GameTechRenderer::RenderloadingBar(float progress)
{
	BindShader(loadingBarShader);
	glUniform1f(glGetUniformLocation(loadingBarShader->GetProgramID(), "progress"), progress);
	glUniform4f(glGetUniformLocation(loadingBarShader->GetProgramID(), "loadedColour"), 0.408, 0.584, 0.506, 1);

	BindMesh(loadingBar);
	DrawBoundMesh();
}

void NCL::CSC8503::GameTechRenderer::RenderTimerQuad()
{
	glEnable(GL_BLEND);
	BindShader(simpleShader);
	glUniform4f(glGetUniformLocation(simpleShader->GetProgramID(), "bgColour"), 0.2f, 0.2f, 0.2f, 1);

	BindMesh(quad);
	DrawBoundMesh();
}

void NCL::CSC8503::GameTechRenderer::RenderCrossHair()
{

	BindShader(aimShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, aimTex->GetObjectID());
	glUniform1i(glGetUniformLocation(aimShader->GetProgramID(), "mainTex"), 0);

	BindMesh(aimQuad);
	DrawBoundMesh();
}

void GameTechRenderer::RenderFrame( ) 
{
	glEnable(GL_CULL_FACE);
	BuildObjectList();
	SortObjectList();
	RenderShadowMap(0, 0, windowWidth, windowHeight);

	RenderSkybox(*gameWorld.GetMainCamera());
	RenderCamera(*gameWorld.GetMainCamera(), screenAspect);


	glDisable(GL_CULL_FACE); //Todo - text indices are going the wrong way...
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if (progress < 1) {
		progress += 0.0025f;
	}
	if (progress >= 1) {
		if(GameLock::loadingdone == 1)
			GameLock::isloading1 = false;
		if (GameLock::loadingdone == 2)
			GameLock::isloading2 = false;
		progress = 0;
	}

	if (GameLock::Mainmenuawake) {
		RenderMainMenu();
		RenderNormalButton();
		RenderCoopButton();
		RenderIntroButton();
		RenderExitButton();
	}

	if (GameLock::SingleEndMenuawake) {
		RenderEndScreen();
		RendererSinScore();
		RenderEndExitScreen();
	}
	if (GameLock::CoopEndMenuawake) {
		RenderEndScreen();
		RendererCooScore1();
		RendererCooScore2();
		RenderCoopEndExitScreen();
	}


	if (GameLock::isloading1){
		RenderLoadingScreen();
		GameLock::loadingdone = 1;
		RenderloadingBar(progress);
	}
	else if (GameLock::isloading2) {
		RenderLoadingScreen();
		GameLock::loadingdone = 2;
		RenderloadingBar(progress);
	}

	if (GameLock::IntroMenuawake) {
		RenderIntroPage();
		RenderBackButton();
	}


	//RenderHUD();

}
void NCL::CSC8503::GameTechRenderer::AnimUpdate(MeshAnimation* playerAnim, float dt) {
	frameTime -= dt;
	while (frameTime < 0.0f) {
		currentFrame = (currentFrame + 1) % playerAnim->GetFrameCount();
		frameTime += 1.0f / playerAnim->GetFrameRate();
	}
}
void NCL::CSC8503::GameTechRenderer::RenderFirstFrame()
{
	glEnable(GL_CULL_FACE);
	glClearColor(1, 1, 1, 1);
	BuildObjectList();
	SortObjectList();
	RenderShadowMap(0, 0, windowWidth * 0.5, windowHeight);

	RenderSkybox(*gameWorld.GetMainCamera());
	RenderCamera(*gameWorld.GetMainCamera(), screenAspectSplit);
	glDisable(GL_CULL_FACE); //Todo - text indices are going the wrong way...
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_BLEND);
	if (GameLock::gamestart) {
		RenderCrossHair();
		RenderMap({ 0, 0 }, { windowWidth * 0.5f, (float)windowHeight }, gameWorld.GetPlayer());
		RenderHealthBar(gameWorld.GetPlayerHealth());
	}

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void NCL::CSC8503::GameTechRenderer::RenderSecFrame()
{
	glEnable(GL_CULL_FACE);
	glClearColor(1, 1, 1, 1);
	BuildObjectList();
	SortObjectList();
	RenderShadowMap(windowWidth * 0.5, 0, windowWidth * 0.5, windowHeight);

	RenderSkybox(*gameWorld.GetSecCamera());
	RenderCamera(*gameWorld.GetSecCamera(), screenAspectSplit);

	glDisable(GL_CULL_FACE); //Todo - text indices are going the wrong way...
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_BLEND);
	if (GameLock::gamestart) {
		//RenderCrossHair();
		//RenderMap();
		//RenderHealthBar(gameWorld.GetPlayerCoopHealth());

		RenderCrossHair();
		RenderMap({ windowWidth * 0.5f, 0 }, { windowWidth * 0.5f, (float)windowHeight }, gameWorld.GetPlayerCoop());
		RenderHealthBar(gameWorld.GetPlayerCoopHealth());
	}


	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void NCL::CSC8503::GameTechRenderer::RenderHUD()
{

	if (GameLock::gamestart) {
		RenderTimerQuad();
		RenderCrossHair();
		NewRenderLines();
		NewRenderText();
		RenderMap({ 0, 0 }, { (float)windowWidth, (float)windowHeight }, gameWorld.GetPlayer());
		RenderHealthBar(gameWorld.GetPlayerHealth());
		RenderProgressBar(gameWorld.getColourOneScore());
	}
	

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void NCL::CSC8503::GameTechRenderer::RenderCoopHUD()
{

	glViewport(0, 0, windowWidth, windowHeight);
	
	glDisable(GL_CULL_FACE); //Todo - text indices are going the wrong way...
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (GameLock::gamestart) {
		RenderTimerQuad();
		NewRenderText();
		RenderProgressBar(0.5f + gameWorld.getColourOneScore() - gameWorld.getColourTwoScore());
	}

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void GameTechRenderer::BuildObjectList() {
	activeObjects.clear();

	gameWorld.OperateOnContents(
		[&](GameObject* o) {
			if (o->IsActive()) {
				const RenderObject* g = o->GetRenderObject();
				if (g) {
					activeObjects.emplace_back(g);
				}
			}
		}
	);
}
void GameTechRenderer::SortObjectList() 
{

}

void GameTechRenderer::RenderShadowMap(int start, int end, int width, int height) {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glCullFace(GL_FRONT);

	BindShader(shadowShader);
	int mvpLocation = glGetUniformLocation(shadowShader->GetProgramID(), "mvpMatrix");

	Matrix4 shadowViewMatrix = Matrix4::BuildViewMatrix(lightPosition, Vector3(0, 0, 0), Vector3(0,1,0));
	Matrix4 shadowProjMatrix = Matrix4::Perspective(100.0f, 500.0f, 1, 45.0f);

	Matrix4 mvMatrix = shadowProjMatrix * shadowViewMatrix;

	shadowMatrix = biasMatrix * mvMatrix; //we'll use this one later on

	for (const auto&i : activeObjects) {
		reactphysics3d::Transform iTransform = i->GetPhysicsObject()->getTransform();
		Matrix4 modelMatrix = Matrix4() * Matrix4::Translation(Vector3(iTransform.getPosition())) * Matrix4(Quaternion(iTransform.getOrientation())) * Matrix4::Scale((*i).GetScale());
		Matrix4 mvpMatrix	= mvMatrix * modelMatrix;
		glUniformMatrix4fv(mvpLocation, 1, false, (float*)&mvpMatrix);
		BindMesh((*i).GetMesh());
		int layerCount = (*i).GetMesh()->GetSubMeshCount();
		for (int i = 0; i < layerCount; ++i) {
			DrawBoundMesh(i);
		}
	}

	glViewport(start, end, width, height);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glCullFace(GL_BACK);
}


void GameTechRenderer::RenderSkybox(Camera& camera) {
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	float screenAspect = (float)windowWidth / (float)windowHeight;
	Matrix4 viewMatrix = camera.BuildViewMatrix();
	Matrix4 projMatrix = camera.BuildProjectionMatrix(screenAspect);

	BindShader(skyboxShader);

	int projLocation = glGetUniformLocation(skyboxShader->GetProgramID(), "projMatrix");
	int viewLocation = glGetUniformLocation(skyboxShader->GetProgramID(), "viewMatrix");
	int texLocation  = glGetUniformLocation(skyboxShader->GetProgramID(), "cubeTex");

	glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
	glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

	glUniform1i(texLocation, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

	BindMesh(skyboxMesh);
	DrawBoundMesh();

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

void GameTechRenderer::RenderCamera(Camera & camera, float& aspectRatio) {
	//float screenAspect = ((float)windowWidth/2) / (float)windowHeight;
	Matrix4 viewMatrix = camera.BuildViewMatrix();
		
	//	gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = camera.BuildProjectionMatrix(aspectRatio);
		
		//gameWorld.GetMainCamera()->BuildProjectionMatrix(aspectRatio);

	OGLShader* activeShader = nullptr;
	int projLocation	= 0;
	int viewLocation	= 0;
	int modelLocation	= 0;
	int colourLocation  = 0;
	int hasVColLocation = 0;
	int hasTexLocation  = 0;
	int shadowLocation  = 0;
	int paintedLocation = 0;
	int lightPosLocation	= 0;
	int lightColourLocation = 0;
	int lightRadiusLocation = 0;
	int paintCount = 0;
	int restartLocation = 0;

	int cameraLocation = 0;

	//TODO - PUT IN FUNCTION
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	for (const auto&i : activeObjects) {
		OGLShader* shader = (OGLShader*)(*i).GetShader();
		BindShader(shader);

		BindTextureToShader((OGLTexture*)(*i).GetDefaultTexture(), "mainTex", 0);

	

		if (i->GetPaintedPos().size()) {
			for (int j = 0; j < i->GetPaintedPos().size(); j++)
			{
				vector<Vector4> allPos = i->GetPaintedPos();
				Vector4 paintedPos = allPos[j];
				restartLocation = glGetUniformLocation(shader->GetProgramID(),"gamestart");
				glUniform1i(restartLocation,GameLock::gamestart);
				char buffer[64];
				sprintf_s(buffer, "paintedPos[%i]", j);
				paintedLocation = glGetUniformLocation(shader->GetProgramID(), buffer);
				glUniform4fv(paintedLocation, 1, paintedPos.array);
			}
		}

		/*if (GameLock::gamemod == 0) {
			gameWorld.OperateOnContents(
				[&](GameObject* o)
				{
					if (o->GetRenderObject()->GetPaintedPos().size() > 0) {
						o->GetRenderObject()->Clear();
					}
				});
		}*/


		/*else if (i->GetPaintedPos().size() > 0) {//////PaintSystem
			for (int j = 0; j < i->GetPaintedPos().size(); j++)
			{
				std::cout << i->GetPaintedPos().size() << std::endl;
				vector<Vector4> allPos = i->GetPaintedPos();
				Vector4 paintedPos = allPos[j];
				//std::cout <<i<< "//////paintedPos::" << paintedPos << std::endl;
				char buffer[64];
				sprintf_s(buffer, "paintedPos[%i]", j);
				paintedLocation = glGetUniformLocation(shader->GetProgramID(), buffer);
				glUniform4fv(paintedLocation, 1, paintedPos.array);
			 }
		}*/



		if (activeShader != shader) {
			projLocation = glGetUniformLocation(shader->GetProgramID(), "projMatrix");
			viewLocation = glGetUniformLocation(shader->GetProgramID(), "viewMatrix");
			modelLocation = glGetUniformLocation(shader->GetProgramID(), "modelMatrix");
			shadowLocation = glGetUniformLocation(shader->GetProgramID(), "shadowMatrix");
			colourLocation = glGetUniformLocation(shader->GetProgramID(), "objectColour");
			hasVColLocation = glGetUniformLocation(shader->GetProgramID(), "hasVertexColours");
			hasTexLocation = glGetUniformLocation(shader->GetProgramID(), "hasTexture");

			lightPosLocation = glGetUniformLocation(shader->GetProgramID(), "lightPos");
			lightColourLocation = glGetUniformLocation(shader->GetProgramID(), "lightColour");
			lightRadiusLocation = glGetUniformLocation(shader->GetProgramID(), "lightRadius");
			cameraLocation = glGetUniformLocation(shader->GetProgramID(), "cameraPos");
			//paintCount = glGetUniformLocation(shader->GetProgramID(), "paintCount");
			int j = glGetUniformLocation(shader->GetProgramID(), "joints");
			glUniformMatrix4fv(j, i->GetFrameMatrices().size(), false, (float*)i->GetFrameMatrices().data());
			Vector3 camPos = camera.GetPosition();
	
			
			/*for (int i = 0; i < gameWorld.painted.size();i++) {
				Vector4 paintedPos = gameWorld.painted[i];
				char buffer[64];
				sprintf_s(buffer, "paintedPos[%i]", i);
				paintedLocation = glGetUniformLocation(shader->GetProgramID(), buffer);
				glUniform4fv(paintedLocation, 1, paintedPos.array);
			}*/

			glUniform3fv(cameraLocation, 1, camPos.array);
			glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
			glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);
			//int size = gameWorld.painted.size();
			//glUniform1f(paintCount,size);
			glUniform3fv(lightPosLocation	, 1, (float*)&lightPosition);
			glUniform4fv(lightColourLocation, 1, (float*)&lightColour);
			glUniform1f(lightRadiusLocation , lightRadius);
			int shadowTexLocation = glGetUniformLocation(shader->GetProgramID(), "shadowTex");
			glUniform1i(shadowTexLocation, 1);

			activeShader = shader;
		}

		reactphysics3d::Transform iTransform = i->GetPhysicsObject()->getTransform();
		Matrix4 modelMatrix = Matrix4() * Matrix4::Translation(Vector3(iTransform.getPosition())) * Matrix4(Quaternion(iTransform.getOrientation())) * Matrix4::Scale((*i).GetScale());
		glUniformMatrix4fv(modelLocation, 1, false, (float*)&modelMatrix);			
		
		Matrix4 fullShadowMat = shadowMatrix * modelMatrix;
		glUniformMatrix4fv(shadowLocation, 1, false, (float*)&fullShadowMat);

		Vector4 colour = i->GetColour();
		glUniform4fv(colourLocation, 1, colour.array);

		glUniform1i(hasVColLocation, !(*i).GetMesh()->GetColourData().empty());

		glUniform1i(hasTexLocation, (OGLTexture*)(*i).GetDefaultTexture() ? 1:0);

		BindMesh((*i).GetMesh());
		int layerCount = (*i).GetMesh()->GetSubMeshCount();
		for (int i = 0; i < layerCount; ++i) {
			DrawBoundMesh(i);
		}
	}
}

MeshGeometry* GameTechRenderer::LoadMesh(const string& name) {
	OGLMesh* mesh = new OGLMesh(name);
	mesh->SetPrimitiveType(GeometryPrimitive::Triangles);
	mesh->UploadToGPU();
	return mesh;
}

void GameTechRenderer::NewRenderLines() {
	const std::vector<Debug::DebugLineEntry>& lines = Debug::GetDebugLines();
	if (lines.empty()) {
		return;
	}
	float screenAspect = (float)windowWidth / (float)windowHeight;
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);
	
	Matrix4 viewProj  = projMatrix * viewMatrix;

	BindShader(debugShader);
	int matSlot = glGetUniformLocation(debugShader->GetProgramID(), "viewProjMatrix");
	GLuint texSlot = glGetUniformLocation(debugShader->GetProgramID(), "useTexture");
	glUniform1i(texSlot, 0);

	glUniformMatrix4fv(matSlot, 1, false, (float*)viewProj.array);

	debugLineData.clear();

	int frameLineCount = lines.size() * 2;

	SetDebugLineBufferSizes(frameLineCount);

	glBindBuffer(GL_ARRAY_BUFFER, lineVertVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, lines.size() * sizeof(Debug::DebugLineEntry), lines.data());
	

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, frameLineCount);
	glBindVertexArray(0);
}

void GameTechRenderer::NewRenderText() {
	const std::vector<Debug::DebugStringEntry>& strings = Debug::GetDebugStrings();
	if (strings.empty()) {
		return;
	}

	BindShader(debugShader);

	OGLTexture* t = (OGLTexture*)Debug::GetDebugFont()->GetTexture();

	if (t) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, t->GetObjectID());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);	
		BindTextureToShader(t, "mainTex", 0);
	}
	Matrix4 proj = Matrix4::Orthographic(0.0, 100.0f, 100, 0, -1.0f, 1.0f);

	int matSlot = glGetUniformLocation(debugShader->GetProgramID(), "viewProjMatrix");
	glUniformMatrix4fv(matSlot, 1, false, (float*)proj.array);

	GLuint texSlot = glGetUniformLocation(debugShader->GetProgramID(), "useTexture");
	glUniform1i(texSlot, 1);

	debugTextPos.clear();
	debugTextColours.clear();
	debugTextUVs.clear();

	int frameVertCount = 0;
	for (const auto& s : strings) {
		frameVertCount += Debug::GetDebugFont()->GetVertexCountForString(s.data);
	}
	SetDebugStringBufferSizes(frameVertCount);

	for (const auto& s : strings) {
		float size = 20.0f;
		Debug::GetDebugFont()->BuildVerticesForString(s.data, s.position, s.colour, size, debugTextPos, debugTextUVs, debugTextColours);
	}

	glBindBuffer(GL_ARRAY_BUFFER, textVertVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, frameVertCount * sizeof(Vector3), debugTextPos.data());
	glBindBuffer(GL_ARRAY_BUFFER, textColourVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, frameVertCount * sizeof(Vector4), debugTextColours.data());
	glBindBuffer(GL_ARRAY_BUFFER, textTexVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, frameVertCount * sizeof(Vector2), debugTextUVs.data());

	glBindVertexArray(textVAO);
	glDrawArrays(GL_TRIANGLES, 0, frameVertCount);
	glBindVertexArray(0);
}

TextureBase* GameTechRenderer::LoadTexture(const string& name) {
	return TextureLoader::LoadAPITexture(name);
}

ShaderBase* GameTechRenderer::LoadShader(const string& vertex, const string& fragment) {
	return new OGLShader(vertex, fragment);
}

void GameTechRenderer::SetDebugStringBufferSizes(size_t newVertCount) {
	if (newVertCount > textCount) {
		textCount = newVertCount;

		glBindBuffer(GL_ARRAY_BUFFER, textVertVBO);
		glBufferData(GL_ARRAY_BUFFER, textCount * sizeof(Vector3), nullptr, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, textColourVBO);
		glBufferData(GL_ARRAY_BUFFER, textCount * sizeof(Vector4), nullptr, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, textTexVBO);
		glBufferData(GL_ARRAY_BUFFER, textCount * sizeof(Vector2), nullptr, GL_DYNAMIC_DRAW);

		debugTextPos.reserve(textCount);
		debugTextColours.reserve(textCount);
		debugTextUVs.reserve(textCount);

		glBindVertexArray(textVAO);

		glVertexAttribFormat(0, 3, GL_FLOAT, false, 0);
		glVertexAttribBinding(0, 0);
		glBindVertexBuffer(0, textVertVBO, 0, sizeof(Vector3));

		glVertexAttribFormat(1, 4, GL_FLOAT, false, 0);
		glVertexAttribBinding(1, 1);
		glBindVertexBuffer(1, textColourVBO, 0, sizeof(Vector4));

		glVertexAttribFormat(2, 2, GL_FLOAT, false, 0);
		glVertexAttribBinding(2, 2);
		glBindVertexBuffer(2, textTexVBO, 0, sizeof(Vector2));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
	}
}

void GameTechRenderer::SetDebugLineBufferSizes(size_t newVertCount) {
	if (newVertCount > lineCount) {
		lineCount = newVertCount;

		glBindBuffer(GL_ARRAY_BUFFER, lineVertVBO);
		glBufferData(GL_ARRAY_BUFFER, lineCount * sizeof(Debug::DebugLineEntry), nullptr, GL_DYNAMIC_DRAW);

		debugLineData.reserve(lineCount);

		glBindVertexArray(lineVAO);

		int realStride = sizeof(Debug::DebugLineEntry) / 2;

		glVertexAttribFormat(0, 3, GL_FLOAT, false, offsetof(Debug::DebugLineEntry, start));
		glVertexAttribBinding(0, 0);
		glBindVertexBuffer(0, lineVertVBO, 0, realStride);

		glVertexAttribFormat(1, 4, GL_FLOAT, false, offsetof(Debug::DebugLineEntry, colourA));
		glVertexAttribBinding(1, 0);
		glBindVertexBuffer(1, lineVertVBO, sizeof(Vector4), realStride);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}
}
