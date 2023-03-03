#include "GameTechRenderer.h"
#include <reactphysics3d/reactphysics3d.h>
#include "GameObject.h"
#include "RenderObject.h"
#include "Camera.h"
#include "TextureLoader.h"
#include "Debug.h"
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

	delete aimShader;
	delete aimQuad;
	delete aimTex;

	delete miniMapPlayer;
	delete miniMapEnemy;
	delete miniMapWall;

	
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
}

void NCL::CSC8503::GameTechRenderer::RenderTriangle(Vector2& v1, Vector2& v2, Vector2& v3, Vector4& color, unsigned int type)
{
	
	BindShader(miniMapPlayer);
	const auto& u_color = glGetUniformLocation(miniMapPlayer->GetProgramID(), "u_Color");
	float scaledWidth = (windowWidth / 2);

	if (!type) {
		v1.x = (v1.x / windowWidth) * 2 - 1.0f;
		v1.y = (v1.y / windowHeight) * 2 - 1.0f;
		v2.x = (v2.x / windowWidth) * 2 - 1.0f;
		v2.y = (v2.y / windowHeight) * 2 - 1.0f;
		v3.x = (v3.x / windowWidth) * 2 - 1.0f;
		v3.y = (v3.y / windowHeight) * 2 - 1.0f;
	}
	else if (type == 1) {

		// first screen
		v1.x = (v1.x / scaledWidth) * 2 - 1.0f;
		v1.y = (v1.y / windowHeight) * 2 - 1.0f;
		v2.x = (v2.x / scaledWidth) * 2 - 1.0f;
		v2.y = (v2.y / windowHeight) * 2 - 1.0f;
		v3.x = (v3.x / scaledWidth) * 2 - 1.0f;
		v3.y = (v3.y / windowHeight) * 2 - 1.0f;
	}
	else if (type == 2) {
		// second screen
		v1.x = (v1.x / scaledWidth) * 2 - 3.0f;
		v1.y = (v1.y / windowHeight) * 2 - 1.0f;
		v2.x = (v2.x / scaledWidth) * 2 - 3.0f;
		v2.y = (v2.y / windowHeight) * 2 - 1.0f;
		v3.x = (v3.x / scaledWidth) * 2 - 3.0f;
		v3.y = (v3.y / windowHeight) * 2 - 1.0f;
	}


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
}

void NCL::CSC8503::GameTechRenderer::RenderMap(float map_x, float map_y, GameObject& player, Camera& camera, float offset, unsigned int screenNum)
{
	float map_size = 80.0f;

	float draw_distance = 25.0f;
	Vector4 map_color(0.8f, 0.8f, 0.8f, 0.7f);


	RenderCircle(map_x, map_y, map_size, map_color);

	// player triangle(center in map)
	// in counter clock order

	//float p_size = 10.0f;
	Vector2 p_left(map_x - offset, map_y - offset); // x  = 1140
	Vector2 p_right(map_x + offset, map_y - offset); // x = 1160
	Vector2 p_up(map_x, map_y + offset);			// x = 1150
	Vector4 p_color(0.0f, 0.8f, 0.0f, 0.7f);


	glClear(GL_DEPTH_BUFFER_BIT);

	RenderTriangle(p_left, p_right, p_up, p_color, screenNum);

	//const auto& player = gameWorld.GetPlayer();
	//const auto& camera = gameWorld.GetMainCamera();

	// objects that is near the player

	vector<GameObject*> near_objects;

	gameWorld.OperateOnContents(
		[&](GameObject* o)
		{
			if (o->GetName() != "cube" && o->GetName() != "goose")
				return;
			Vector2 p_pos(player.GetPhysicsObject()->getTransform().getPosition().x, player.GetPhysicsObject()->getTransform().getPosition().z);
			Vector2 o_pos(o->GetPhysicsObject()->getTransform().getPosition().x, o->GetPhysicsObject()->getTransform().getPosition().z);
			float dis = (p_pos - o_pos).Length();
			if (dis < draw_distance)
			{
				near_objects.push_back(o);
			}
		});

	// rotation matrix of camera
	// the built in yaw is a little bit strange , which is not continous
	// make it continous here
	float camera_yaw = camera.GetYaw() < 0 ? -camera.GetYaw() : 360.0f - camera.GetYaw();
	// cause the initial pos of the player is facing down , offset the yaw by 180 degress
	// but at the same time , rotated map is x mirrored , will inverse that in the code below
	Matrix2 rotation = Matrix2::Matrix2().Rotation(camera_yaw - 180.0f);
	const auto& rot_array = rotation.array;

	// get inverse of the rotation
	float coe = 1.0f / (rot_array[1][1] * rot_array[0][0] - rot_array[1][0] * rot_array[0][1]);
	Matrix2 inverse = rotation
		.SetColumn(0, Vector2(rot_array[0][0], -rot_array[0][1]) * coe)
		.SetColumn(1, Vector2(-rot_array[1][0], rot_array[1][1]) * coe);

	Vector4 cubeColor(0.3f, 0.3f, 0.3f, 0.7f);
	Vector4 gooseColor(0.0f, 0.0f, 0.3f, 0.7f);

	for (const auto& object : near_objects)
	{
		Vector2 p_pos(player.GetPhysicsObject()->getTransform().getPosition().x, player.GetPhysicsObject()->getTransform().getPosition().z);
		Vector2 o_pos(object->GetPhysicsObject()->getTransform().getPosition().x, object->GetPhysicsObject()->getTransform().getPosition().z);

		// the relative vector without any rotation for the object
		Vector2 object_vec = o_pos - p_pos;

		const float dis = object_vec.Length();

		// inverse the yaw of the camera
		Vector2 object_vec_inverse = inverse * object_vec.Normalised();

		// determin how far we should draw from the player ?
		float draw_ratio = dis / draw_distance;
		object_vec_inverse.x *= -1;

		// the final position in the map
		Vector2 object_pos = Vector2(map_x, map_y) + ((object_vec_inverse * map_size) * draw_ratio);

		glClear(GL_DEPTH_BUFFER_BIT);

		RenderRectangle(object_pos.x, object_pos.y, 10, 10, cubeColor, screenNum);
		//RenderCircle(object_pos.x, object_pos.y, object->GetName() == "cube" ? 15.0f : 10.0f, object->GetName() == "cube" ? cubeColor : gooseColor);
	}

	//std::cout << "near item count: " << near_objects.size() << std::endl;

	//std::cout << "player"<< ": " << player->GetPhysicsObject()->getTransform().getPosition() << std::endl;

	//std::cout << "camera"
	///	<< ": "
	//	<< " pos:" << camera->GetPosition()
	//	<< " pitch:" << camera->GetPitch()
	//	<< " yaw:" << camera_yaw
	//	<< std::endl;

	return;
}

void NCL::CSC8503::GameTechRenderer::RenderRectangle(float px, float py, float width, float height, Vector4& color, unsigned int filler)
{
	Vector2 p_triangle1, p_triangle2, p_triangle3;
	p_triangle1 = Vector2(px, py);
	p_triangle2 = Vector2(px, py - height);
	p_triangle3 = Vector2(px + width, py);
	RenderTriangle(p_triangle1, p_triangle2, p_triangle3, color, filler);

	p_triangle1 = Vector2(px, py - height);
	p_triangle2 = Vector2(px + width, py - height);
	p_triangle3 = Vector2(px + width, py);
	RenderTriangle(p_triangle1, p_triangle2, p_triangle3, color, filler);
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
	glClearColor(1, 1, 1, 1);
	BuildObjectList();
	SortObjectList();
	RenderShadowMap(0, 0, windowWidth, windowHeight);

	RenderSkybox(*gameWorld.GetMainCamera());
	RenderCamera(*gameWorld.GetMainCamera(), screenAspect);

	glDisable(GL_CULL_FACE); //Todo - text indices are going the wrong way...
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
	RenderCrossHair();

	RenderMap(510, 100, *gameWorld.GetPlayer(), *gameWorld.GetMainCamera(), 10.0f, 1);

	RenderHealthBar(gameWorld.GetPlayerHealth());

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
	RenderCrossHair();

	RenderMap(1150, 100, *gameWorld.GetPlayerCoop(), *gameWorld.GetSecCamera(), 10.0f,2);

	RenderHealthBar(gameWorld.GetPlayerCoopHealth());
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void NCL::CSC8503::GameTechRenderer::RenderHUD()
{
	NewRenderLines();
	RenderTimerQuad();
	RenderCrossHair();
	NewRenderText();
	RenderMap(1100,100, *gameWorld.GetPlayer(),*gameWorld.GetMainCamera(), 10,0);

	RenderHealthBar(gameWorld.GetPlayerHealth());
	RenderProgressBar(gameWorld.getColourOneScore() );
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

	RenderTimerQuad();
	NewRenderText();

	RenderProgressBar((0.5f + gameWorld.getColourOneScore()) - gameWorld.getColourTwoScore());
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

	int cameraLocation = 0;

	//TODO - PUT IN FUNCTION
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	for (const auto&i : activeObjects) {
		OGLShader* shader = (OGLShader*)(*i).GetShader();
		BindShader(shader);

		BindTextureToShader((OGLTexture*)(*i).GetDefaultTexture(), "mainTex", 0);

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

			Vector3 camPos = camera.GetPosition();
			
			
			for (int i = 0; i < gameWorld.painted.size();i++) {
				Vector4 paintedPos = gameWorld.painted[i];
				char buffer[64];
				sprintf_s(buffer, "paintedPos[%i]", i);
				paintedLocation = glGetUniformLocation(shader->GetProgramID(), buffer);
				glUniform4fv(paintedLocation, 1, paintedPos.array);
			}
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
