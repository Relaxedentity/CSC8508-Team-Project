#pragma once
#include "RendererBase.h"
#include "PS4MemoryAware.h"

namespace PS4 {
	enum MemoryLocation {
		GARLIC,
		ONION,
		MEMORYMAX
	};

	struct PS4ScreenBuffer {
		sce::Gnm::RenderTarget colourTarget;
		sce::Gnm::DepthRenderTarget depthTarget;
	};

	class PS4Mesh;
	class PS4Shader;
	class PS4Texture;
	class PS4Frame;
	class PS4Renderer : public NCL::Rendering::RendererBase, public PS4MemoryAware {
	public:
		PS4Renderer();
		~PS4Renderer();

		void Update(float dt) override;
		void RenderFrame() override;
		void SwapBuffers() override;
	protected:
		void SwapScreenBuffer();
		void SwapCommandBuffer();

		const int _GarlicMemory;
		const int _OnionMemory;
		const int _MaxCMDBufferCount;
		const int _bufferCount;

		int videoHandle;
		int framesSubmitted;

		int currentGPUBuffer;
		int prevGPUBuffer;
		int currentScreenBuffer;
		int prevScreenBuffer;
		PS4ScreenBuffer** screenBuffers;

		sce::Gnmx::Toolkit::StackAllocator stackAllocators[MEMORYMAX];

		PS4Shader* defaultShader;
		PS4Mesh* defaultMesh;
		PS4Texture* defaultTexture;

		PS4Frame* frames;
		PS4ScreenBuffer* currentPS4Buffer;
		sce::Gnmx::GnmxGfxContext* currentGFXContext;
		PS4Frame* currentFrame;
	private:
		void InitialiseMemoryAllocators();
		void IniialiseVideoSystem();
		void InitialiseGCMRendering();
		void InitialiseExternalPointers();
		void DestroyMemoryAllocators();
		void DestroyVideoSystem();
		void DestroyGCMRendering();
		void SetRenderBuffer(PS4ScreenBuffer* buffer, bool clearColour, bool clearDepth, bool clearStencil);
		void ClearBuffer(bool colour, bool depth, bool stencil);
		PS4ScreenBuffer* GenerateScreenBuffer(uint width, uint height, bool colour = true, bool depth = true, bool stencil = false);
		void DrawMesh(PS4Mesh& mesh);
	};
}