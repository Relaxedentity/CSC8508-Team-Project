#pragma once
#include "OGLRenderer.h"
#include "OGLShader.h"
#include "OGLTexture.h"
#include "OGLMesh.h"
#include "MeshAnimation.h"
#include "MeshMaterial.h"
#include "GameWorld.h"

namespace NCL {
	class Maths::Vector3;
	class Maths::Vector4;
	namespace CSC8503 {
		class GameObject;
		class RenderObject;

		class GameTechRenderer : public OGLRenderer	{
		public:
			GameTechRenderer(GameWorld& world);
			~GameTechRenderer();

			MeshGeometry*	LoadMesh(const string& name);
			TextureBase*	LoadTexture(const string& name);
			ShaderBase*		LoadShader(const string& vertex, const string& fragment);


			void AnimUpdate(MeshAnimation* playerAnim, float dt);
			void RenderHealthBar(float health);
			void RenderProgressBar(float score);
			void RenderloadingBar(float progress);
			void RenderTimerQuad();
			void RenderCrossHair();
			

		protected:
			void NewRenderLines();
			void NewRenderText();

			void RenderFrame()		override;
			void RenderFirstFrame() override;
			void RenderSecFrame()	override;
			void RenderHUD()		override;
			void RenderCoopHUD()	override;
	

			OGLShader*		defaultShader;
			GameWorld&	gameWorld;

			float screenAspectSplit;
			float screenAspect;

			void BuildObjectList();
			void SortObjectList();

			void RenderShadowMap(int start, int end, int width, int height);

			void RenderCamera(Camera& camera, float& aspectRatio); 

			void RenderSkybox(Camera& camera);
			void Particles();
			void LoadSkybox();
			
			void RenderCircle(float cx, float cy, float r, const Vector4& color);
			void RenderTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Vector4& color, Vector2 windowSize);
			void RenderMap(Vector2 window_pos, Vector2 window_size, GameObject* player);
			void RenderRectangle(float px, float py, float width, float height , Vector4& color);
			void RenderMainMenu();
			void RenderNormalButton();
			void RenderCoopButton();
			void RenderIntroButton();
			void RenderExitButton();

			void RenderIntroPage();
			void RenderBackButton();

			void RenderEndScreen();
			void RendererSinScore();
			void RendererCooScore1();
			void RendererCooScore2();
			void RenderEndExitScreen();
			void Restart();
			void RenderCoopEndExitScreen();

			void RenderLoadingScreen();
	

			void RenderRectangle(Vector2 tl, Vector2 br, Vector4 color, Vector2 windowSize);
			
			void SetDebugStringBufferSizes(size_t newVertCount);
			void SetDebugLineBufferSizes(size_t newVertCount);

			vector<const RenderObject*> activeObjects;

			OGLShader*  debugShader;
			OGLShader*  skyboxShader;
			OGLMesh*	skyboxMesh;
			GLuint		skyboxTex;

			//map shading
			OGLShader* miniMapWall;
			OGLShader* miniMapEnemy;
			OGLShader* miniMapPlayer;

			// health bar shading
			OGLShader* healthShader;
			OGLMesh* healthQuad;

			// progress shader
			OGLShader* progressShader;
			OGLMesh* progressBar;

			OGLTexture* loadingTex;////////////修改
			OGLMesh* loadingMesh;////////////修改
			OGLShader* loadingBarShader;
			OGLMesh* loadingBar;

			//timer background
			OGLShader* simpleShader;
			OGLMesh* quad;

			//crosshair background
			OGLShader*aimShader;
			OGLMesh* aimQuad;
			OGLTexture* aimTex;

			/// <summary>
			/// MenuBG
			/// </summary>
			OGLShader* uiShader;//////////修改
			OGLTexture* menuTex;////////////修改
			OGLMesh* menuMesh;////////////修改

			//score
			OGLTexture* Score1Tex;
			OGLTexture* Score2Tex;
			OGLTexture* Score3Tex;
			OGLTexture* Score4Tex;

			OGLMesh* Score1Mesh;
			OGLMesh* Score2Mesh;
			OGLMesh* Score3Mesh;
			OGLMesh* Score4Mesh;

			OGLTexture* CoopScore1Tex;
			OGLTexture* CoopScore2Tex;
			OGLTexture* CoopScore3Tex;
			OGLTexture* CoopScore4Tex;

			OGLMesh* CoopScore1Mesh;
			OGLMesh* CoopScore2Mesh;
			OGLMesh* CoopScore3Mesh;
			OGLMesh* CoopScore4Mesh;

			/// <summary>
			/// normalButton
			/// </summary>
			OGLTexture* normalBtnTex;///////修改
			OGLMesh* normalBtnMesh;///////修改
			/// <summary>
			/// coopButton
			/// </summary>
			OGLTexture* coopBtnTex;
			OGLMesh* coopBtnMesh;
			/// <summary>
			/// IntroductionButton
			/// </summary>
			OGLTexture* introBtnTex;
			OGLMesh* introBtnMesh;
			/// <summary>
			/// exitButton
			/// </summary>
			OGLTexture* exitBtnTex;
			OGLMesh* exitBtnMesh;

			OGLTexture* RestartBtnTex;
			OGLMesh* RestartBtnMesh;

			/// introscreen 
            /// </summary>
			OGLTexture* intropageTex;////////////修改
			OGLMesh* intropageMesh;////////////修改
			/// normalButton
			/// </summary>
			OGLTexture* backBtnTex;///////修改
			OGLMesh* backBtnMesh;///////修改 


			/// EndBG//------------
			OGLTexture* endTex;
			OGLMesh* endMesh;

			OGLTexture* sinscoreTex;
			OGLMesh* sinscoreMesh;

			OGLTexture* cooscore1Tex;
			OGLMesh* cooscore1Mesh;

			OGLTexture* cooscore2Tex;
			OGLMesh* cooscore2Mesh;

			OGLTexture* endexitBtnTex;
			OGLMesh* endexitBtnMesh;



			//shadow mapping things
			OGLShader*	shadowShader;
			GLuint		shadowTex;
			GLuint		shadowFBO;
			GLuint		particleTex;
			Matrix4     shadowMatrix;

			Vector4		lightColour;
			float		lightRadius;
			Vector3		lightPosition;

			//Debug data storage things
			float progress = 0;
			vector<Vector3> debugLineData;

			vector<Vector3> debugTextPos;
			vector<Vector4> debugTextColours;
			vector<Vector2> debugTextUVs;

			GLuint lineVAO;
			GLuint lineVertVBO;
			size_t lineCount;

			GLuint textVAO;
			GLuint textVertVBO;
			GLuint textColourVBO;
			GLuint textTexVBO;
			size_t textCount;

			//animated things
			OGLShader* animatedShader;
			TextureBase* playerTex;
			MeshMaterial* playerMat;
			MeshAnimation* playerAnim;
			MeshGeometry* playerMesh;

			int currentFrame;
			float frameTime;
			string a;
			string b;
			string c;
			string d;

			string a2;
			string b2;
			string c2;
			string d2;
		};
	}
}

