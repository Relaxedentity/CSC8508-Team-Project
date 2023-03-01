#pragma once
#include "MeshGeometry.h"
#include "PS4MemoryAware.h"

#include <gnm.h>

#include <gnmx\context.h>
#include <C:\Program Files (x86)\SCE\ORBIS SDKs\10.000\target\samples\sample_code\graphics\api_gnm\toolkit\allocators.h>
#include <C:\Program Files (x86)\SCE\ORBIS SDKs\10.000\target\samples\sample_code\graphics\api_gnm\toolkit\stack_allocator.h>
#include <gnm\dataformats.h>

using namespace sce;

namespace PS4 {
	class PS4Mesh : public NCL::MeshGeometry, public PS4MemoryAware {
		friend class PS4RendererBase;
	public:
		static PS4Mesh* GenerateQuad();
		~PS4Mesh();

	protected:
		void SubmitPreDraw(Gnmx::GnmxGfxContext& cmdList, Gnm::ShaderStage stage);
		void SubmitDraw(Gnmx::GnmxGfxContext& cmdList, Gnm::ShaderStage stage);
		void BufferData();
		void InitAttributeBuffer(sce::Gnm::Buffer& buffer, Gnm::DataFormat format, void* offset);
		PS4Mesh();

		void UploadToGPU(NCL::Rendering::RendererBase* renderer = nullptr) = 0;

		struct MeshVertex {
			float position[3];
			float textureCoord[2];
			float normal[3];
			float tangent[3];
		};

		Gnm::IndexSize indexType;
		Gnm::PrimitiveType primitiveType;

		int* indexBuffer;
		MeshVertex* vertexBuffer;

		int vertexDataSize;
		int indexDataSize;

		sce::Gnm::Buffer* attributeBuffers;
		int attributeCount;
	};
}