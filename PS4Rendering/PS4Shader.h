#pragma once
#include "ShaderBase.h"
#include "PS4MemoryAware.h"

using namespace sce;

namespace PS4 {
	class PS4Shader : public NCL::Rendering::ShaderBase, public PS4MemoryAware {
		friend class PS4RendererBase;
	public:
		~PS4Shader();

		static PS4Shader* GenerateShader(const string& vertex, const string& pixel);

	protected:
		PS4Shader();
		void GenerateVertexShader(const string& name, bool makeFetch);
		void GeneratePixelShader(const string& name);
		void GenerateFetchShader(char* binData);

		bool LoadShaderBinary(const string& name, char*& into, int& dataSize);
		bool ShaderIsBinary(const string& name);

		void SubmitShaderSwitch(Gnmx::GnmxGfxContext& cmdList);
		int GetConstantBuffer(const string& name);

		void* fetchShader;

		sce::Shader::Binary::Program vertexBinary;
		sce::Shader::Binary::Program pixelBinary;

		Gnmx::VsShader* vertexShader;
		Gnmx::InputOffsetsCache vertexCache;

		Gnmx::PsShader* pixelShader;
		Gnmx::InputOffsetsCache pixelCache;
	};
}