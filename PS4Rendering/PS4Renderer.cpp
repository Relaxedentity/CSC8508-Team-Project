#pragma once
#include "PS4Renderer.h"
#include "PS4Shader.h"
#include "PS4Mesh.h"
#include "PS4Texture.h"
#include "PS4Frame.h"
#include "Matrix4.h"
#include "Vector4.h"
#include <video_out.h>
#include <C:\Program Files (x86)\SCE\ORBIS SDKs\10.000\target\samples\sample_code\graphics\api_gnm\toolkit\toolkit.h>

using namespace PS4;
using namespace sce;

PS4Renderer::PS4Renderer() : _MaxCMDBufferCount(3), _bufferCount(3), _GarlicMemory(1024 * 1024 * 512), _OnionMemory(1024 * 1024 * 256) {
	framesSubmitted = 0;
	currentGPUBuffer = 0;
	currentScreenBuffer = 0;
	prevScreenBuffer = 0;

	currentScreenBuffer = NULL;
	currentGFXContext = NULL;

	InitialiseMemoryAllocators();
	InitialiseGCMRendering();
	IniialiseVideoSystem();

	defaultShader = PS4Shader::GenerateShader("/app0/VertexShader.sb", "/app0/PixelShader.sb");

	defaultMesh = PS4Mesh::GenerateQuad();
	defaultTexture = PS4Texture::LoadTextureFromFile("/app0/doge.gnf");

	SwapBuffers();
}

PS4Renderer::~PS4Renderer() {
	DestroyGCMRendering();
	DestroyVideoSystem();
	DestroyMemoryAllocators();
}

void PS4Renderer::IniialiseVideoSystem() {
	screenBuffers = new PS4ScreenBuffer * [_bufferCount];

	for (int i = 0; i < _bufferCount; ++i) {
		screenBuffers[i] = GenerateScreenBuffer(1920, 1080);
	}

	videoHandle = sceVideoOutOpen(0, SCE_VIDEO_OUT_BUS_TYPE_MAIN, 0, NULL);

	SceVideoOutBufferAttribute attribute;
	sceVideoOutSetBufferAttribute(&attribute, SCE_VIDEO_OUT_PIXEL_FORMAT_B8_G8_R8_A8_SRGB, SCE_VIDEO_OUT_TILING_MODE_TILE, SCE_VIDEO_OUT_ASPECT_RATIO_16_9,
		screenBuffers[0]->colourTarget.getWidth(), screenBuffers[0]->colourTarget.getHeight(), screenBuffers[0]->colourTarget.getPitch());

	void** bufferAddresses = new void*[_bufferCount];

	for (int i = 0; i < _bufferCount; ++i) {
		bufferAddresses[i] = screenBuffers[i]->colourTarget.getBaseAddress();
	}

	sceVideoOutRegisterBuffers(videoHandle, 0, bufferAddresses, _bufferCount, &attribute);
}

void PS4Renderer::InitialiseMemoryAllocators() {
	stackAllocators[GARLIC].init(SCE_KERNEL_WC_GARLIC, _GarlicMemory);
	stackAllocators[ONION].init(SCE_KERNEL_WB_ONION, _OnionMemory);

	this->GarlicAllocator = Gnmx::Toolkit::GetInterface(&stackAllocators[GARLIC]);
	this->OnionAllocator = Gnmx::Toolkit::GetInterface(&stackAllocators[ONION]);

	Gnm::registerOwner(&ownerHandle, "PS4Renderer");
}

void PS4Renderer::InitialiseGCMRendering() {
	frames = (PS4Frame*)OnionAllocator.allocate(sizeof(PS4Frame) * _MaxCMDBufferCount, alignof(PS4Frame));

	for (int i = 0; i < _MaxCMDBufferCount; ++i) {
		new (&frames[i])PS4Frame();
	}

	Gnmx::Toolkit::Allocators allocators = sce::Gnmx::Toolkit::Allocators(OnionAllocator, GarlicAllocator, ownerHandle);

	Gnmx::Toolkit::initializeWithAllocators(&allocators);
}

PS4ScreenBuffer* PS4Renderer::GenerateScreenBuffer(uint width, uint height, bool colour, bool depth, bool stencil) {
	PS4ScreenBuffer* buffer = new PS4ScreenBuffer();

	if (colour) {
		Gnm::DataFormat format = Gnm::kDataFormatB8G8R8A8UnormSrgb;
		Gnm::TileMode tileMode;
		GpuAddress::computeSurfaceTileMode(&tileMode, GpuAddress::kSurfaceTypeColorTargetDisplayable, format, 1);

		const Gnm::SizeAlign colourAlign = buffer->colourTarget.init(width, height, 1, format, tileMode, Gnm::kNumSamples1, Gnm::kNumFragments1, NULL, NULL);

		void* colourMemory = stackAllocators[GARLIC].allocate(colourAlign);

		Gnm::registerResource(nullptr, ownerHandle, colourMemory, colourAlign.m_size, "Colour", Gnm::kResourceTypeDepthRenderTargetBaseAddress, 0);

		buffer->colourTarget.setAddresses(colourMemory, NULL, NULL);
	}

	if (depth) {
		Gnm::DataFormat depthFormat = Gnm::DataFormat::build(Gnm::kZFormat32Float);
		Gnm::TileMode depthTileMode;

		GpuAddress::computeSurfaceTileMode(&depthTileMode, GpuAddress::kSurfaceTypeDepthOnlyTarget, depthFormat, 1);

		Gnm::StencilFormat stencilFormat = (stencil ? Gnm::kStencil8 : Gnm::kStencilInvalid);

		void* stencilMemory = 0;
		Gnm::SizeAlign stencilAlign;

		const Gnm::SizeAlign depthAlign = buffer->depthTarget.init(width, height, depthFormat.getZFormat(), stencilFormat, depthTileMode, Gnm::kNumFragments1, stencil ? &stencilAlign : 0, 0);

		void* depthMemory = stackAllocators[GARLIC].allocate(depthAlign);

		Gnm::registerResource(nullptr, ownerHandle, depthMemory, depthAlign.m_size, "Depth", Gnm::kResourceTypeDepthRenderTargetBaseAddress, 0);

		if (stencil) {
			stencilMemory = stackAllocators[GARLIC].allocate(stencilAlign);
			Gnm::registerResource(nullptr, ownerHandle, stencilMemory, stencilAlign.m_size, "Stencil", Gnm::kResourceTypeDepthRenderTargetBaseAddress, 0);
		}
		buffer->depthTarget.setAddresses(depthMemory, stencilMemory);
	}
	return buffer;
}

void PS4Renderer::DestroyGCMRendering() {

}

void PS4Renderer::DestroyMemoryAllocators() {

}

void PS4Renderer::DestroyVideoSystem() {
	for (int i = 0; i < _bufferCount; ++i) {
		delete screenBuffers[i];
	}

	delete[] screenBuffers;
	sceVideoOutClose(videoHandle);
}

float rad = 0.0f;

void PS4Renderer::Update(float dt) {
	rad += 0.01f * dt;
}

void PS4Renderer::RenderFrame() {
	currentFrame->StartFrame();

	currentGFXContext->waitUntilSafeForRendering(videoHandle, currentGPUBuffer);

	SetRenderBuffer(currentPS4Buffer, true, true, true);

	currentGFXContext->setupScreenViewport(0, 0, currentPS4Buffer->colourTarget.getWidth(), currentPS4Buffer->colourTarget.getHeight(), 0.5f, 0.5f);

	defaultShader->SubmitShaderSwitch(*currentGFXContext);

	Gnm::DepthStencilControl dsc;
	dsc.init();
	dsc.setDepthControl(Gnm::kDepthControlZWriteDisable, Gnm::kCompareFuncAlways);
	dsc.setDepthEnable(false);
	currentGFXContext->setDepthStencilControl(dsc);

	Gnm::Sampler trilinearSampler;
	trilinearSampler.init();
	trilinearSampler.setMipFilterMode(Gnm::kMipFilterModeLinear);
	trilinearSampler.setXyFilterMode(
	Gnm::kFilterModeBilinear, Gnm::kFilterModeBilinear);
	
	currentGFXContext->setTextures(Gnm::kShaderStagePs, 0, 1, &defaultTexture->GetAPITexture());
	currentGFXContext->setSamplers(Gnm::kShaderStagePs, 0, 1, &trilinearSampler);

	Gnm::PrimitiveSetup primitiveSetup;
	primitiveSetup.init();
	primitiveSetup.setCullFace(Gnm::kPrimitiveSetupCullFaceNone);
	primitiveSetup.setFrontFace(Gnm::kPrimitiveSetupFrontFaceCcw);
	currentGFXContext->setPrimitiveSetup(primitiveSetup);

	DrawMesh(*defaultMesh);
	currentFrame->EndFrame();
	framesSubmitted++;
}

void PS4Renderer::DrawMesh(PS4Mesh& mesh) {
	NCL::Maths::Matrix4* modelViewProj = (NCL::Maths::Matrix4*)currentGFXContext->allocateFromCommandBuffer(sizeof(NCL::Maths::Matrix4), Gnm::kEmbeddedDataAlignment4);
	*modelViewProj = NCL::Maths::Matrix4::Rotation(rad, NCL::Maths::Vector3(0, 0, 1));
	
	Gnm::Buffer constantBuffer;
	constantBuffer.initAsConstantBuffer(modelViewProj, sizeof(NCL::Maths::Matrix4));
	constantBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);
	
	int indexA = defaultShader->GetConstantBuffer("ShaderConstants");
	
	currentGFXContext->setConstantBuffers(Gnm::kShaderStageVs, 0, 1, &constantBuffer);
	
	defaultMesh->SubmitDraw(*currentGFXContext, Gnm::ShaderStage::kShaderStageVs);
}

void PS4Renderer::SwapBuffers() {
	SwapScreenBuffer();
	SwapCommandBuffer();
}

void PS4Renderer::SwapScreenBuffer() {
	prevScreenBuffer = currentScreenBuffer;
	currentScreenBuffer = (currentScreenBuffer + 1) % _bufferCount;
	sceVideoOutSubmitFlip(videoHandle, prevScreenBuffer, SCE_VIDEO_OUT_FLIP_MODE_VSYNC, 0);
	currentPS4Buffer = screenBuffers[currentScreenBuffer];
}

void PS4Renderer::SwapCommandBuffer() {
	if (currentGFXContext) {
		currentGFXContext->submit();
		Gnm::submitDone();
	}
	
	prevGPUBuffer = currentGPUBuffer;
	currentGPUBuffer = (currentGPUBuffer + 1) % _MaxCMDBufferCount;
	
	currentFrame = &frames[currentGPUBuffer];
	currentGFXContext = &currentFrame->GetCommandBuffer();
}

void PS4Renderer::SetRenderBuffer(PS4ScreenBuffer* buffer,
	bool clearColour, bool clearDepth, bool clearStencil) {
	currentPS4Buffer = buffer;
	
	ClearBuffer(clearColour, clearDepth, clearStencil);
	
	currentGFXContext->setRenderTargetMask(0xF);
	currentGFXContext->setRenderTarget(0, &currentPS4Buffer->colourTarget);
	currentGFXContext->setDepthRenderTarget(& currentPS4Buffer->depthTarget);
}

void PS4Renderer::ClearBuffer(bool colour,
	bool depth, bool stencil) {
	if (colour) {
		Gnmx::Toolkit::SurfaceUtil::clearRenderTarget(*currentGFXContext, &currentPS4Buffer->colourTarget, Vector4(0.1f, 0.1f, 0.1f, 1.0f));
	}
	
	if (depth) {
		Gnmx::Toolkit::SurfaceUtil::clearDepthTarget(*currentGFXContext, &currentPS4Buffer->depthTarget, 1.0f);
	}
	if (stencil &&
		currentPS4Buffer->depthTarget.getStencilReadAddress()) {
		
		Gnmx::Toolkit::SurfaceUtil::clearStencilTarget(*currentGFXContext, &currentPS4Buffer->depthTarget, 0);
	}
}