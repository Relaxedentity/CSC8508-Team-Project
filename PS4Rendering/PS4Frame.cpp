#pragma once
#include "PS4Frame.h"
#include <gnmx\basegfxcontext.h>

using namespace PS4;
using namespace sce;

enum FrameState {
	FRAME_READY,
	FRAME_WAITING,
	FRAME_DONE
};

PS4Frame::PS4Frame() {
	frameTag = FRAME_READY;

	const int bufferBytes = (1 * 1024 * 1024);

	const uint32_t kNumRingEntries = 64;
	const uint32_t cueHeapSize = Gnmx::ConstantUpdateEngine::computeHeapSize(kNumRingEntries);

	void* constantUpdateEngine = GarlicAllocator.allocate(cueHeapSize, Gnm::kAlignmentOfBufferInBytes);
	void* drawCommandBuffer = OnionAllocator.allocate(bufferBytes, Gnm::kAlignmentOfBufferInBytes);
	void* constantCommandBuffer = OnionAllocator.allocate(bufferBytes, Gnm::kAlignmentOfBufferInBytes);

	commandBuffer.init(constantUpdateEngine, kNumRingEntries, drawCommandBuffer, bufferBytes, constantCommandBuffer, bufferBytes);

	Gnm::registerResource(nullptr, ownerHandle, drawCommandBuffer, bufferBytes, "FrameDrawCommandBuffer", Gnm::kResourceTypeDrawCommandBufferBaseAddress);
	Gnm::registerResource(nullptr, ownerHandle, constantUpdateEngine, bufferBytes, "FrameConstantUpdateEngine", Gnm::kResourceTypeDrawCommandBufferBaseAddress);
	Gnm::registerResource(nullptr, ownerHandle, constantCommandBuffer, bufferBytes, "FrameConstantCommandBuffer", Gnm::kResourceTypeDrawCommandBufferBaseAddress);
}

void PS4Frame::StartFrame() {
	BlockUntilReady();
	frameTag = FRAME_WAITING;

	commandBuffer.reset();
	commandBuffer.initializeDefaultHardwareState();
}

void PS4Frame::BlockUntilReady() {
	switch (frameTag)
	{
		case FRAME_READY: return;
		case FRAME_DONE: return;
		case FRAME_WAITING: {
			int spinCount = 0;
			while (frameTag != FRAME_DONE) {
				++spinCount;
			}
			frameTag = FRAME_READY;
		} break;
	}
}

void PS4Frame::EndFrame() {
	commandBuffer.writeImmediateAtEndOfPipeWithInterrupt(Gnm::kEopFlushCbDbCaches, &frameTag, FRAME_DONE, Gnm::kCacheActionNone);
}