#pragma once
#include "OGLRenderer.h"
#include "OGLShader.h"
#include "OGLTexture.h"
#include "OGLMesh.h"

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


			
			void RenderHealthBar(float health);
			void RenderProgressBar(float score);
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
			void RenderTriangle(Vector2& v1, Vector2& v2, Vector2& v3, Vector4& color);
			void RenderMap();
			void RenderRectangle(float px, float py, float width, float height , Vector4& color);
			void RenderGameSelection();


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

			//timer background
			OGLShader* simpleShader;
			OGLMesh* quad;

			//crosshair background
			OGLShader*aimShader;
			OGLMesh* aimQuad;
			OGLTexture* aimTex;

			OGLShader* uiShader;//////////ÐÞ¸Ä
			OGLTexture* uiTex;////////////ÐÞ¸Ä
			OGLMesh* menuMesh;////////////ÐÞ¸Ä

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
		};
	}
}

