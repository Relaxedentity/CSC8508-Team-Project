#pragma once
#include "PS4Mesh.h"
#include "Vector3.h"

using namespace PS4;
using namespace NCL::Maths;

PS4Mesh::PS4Mesh() : NCL::MeshGeometry() {
	attributeBuffers = NULL;
	indexBuffer = 0;
	vertexBuffer = 0;
	attributeCount = 0;
}

PS4Mesh::~PS4Mesh() {
	delete[] attributeBuffers;
}

PS4Mesh* PS4Mesh::GenerateQuad() {
	PS4Mesh* mesh = new PS4Mesh();

	mesh->indexType = sce::Gnm::IndexSize::kIndexSize32;
	mesh->primitiveType = sce::Gnm::PrimitiveType::kPrimitiveTypeTriStrip;

	mesh->SetVertexPositions({ Vector3(-1.0f, -1.0f, 0.0f), Vector3(-1.0f, 1.0f, 0.0f), Vector3(1.0f, -1.0f, 0.0f), Vector3(1.0f, 1.0f, 0.0f) });

	for (int i = 0; i < mesh->GetVertexCount(); ++i) {
		mesh->normals.emplace_back(Vector3(0, 0, 1));
		mesh->tangents.emplace_back(Vector3(1, 0, 0));
		mesh->indices.emplace_back(i);
	}

	mesh->BufferData();
	return mesh;
}

void PS4Mesh::BufferData() {
	vertexDataSize = GetVertexCount() * sizeof(MeshVertex);
	indexDataSize = GetVertexCount() * sizeof(int);

	indexBuffer = static_cast<int*>(GarlicAllocator.allocate(indexDataSize, Gnm::kAlignmentOfBufferInBytes));
	vertexBuffer = static_cast<MeshVertex*>(GarlicAllocator.allocate(vertexDataSize, Gnm::kAlignmentOfBufferInBytes));

	Gnm::registerResource(nullptr, *ownerHandle, indexBuffer, indexDataSize, "IndexData", Gnm::kResourceTypeIndexBufferBaseAddress, 0);
	Gnm::registerResource(nullptr, *ownerHandle, vertexBuffer, vertexDataSize, "VertexData", Gnm::kResourceTypeIndexBufferBaseAddress, 0);

	int flt = sizeof(float);
	for (int i = 0; i < GetVertexCount(); ++i) {
		memcpy(&vertexBuffer[i].position, &positions[i], flt * 3);
		memcpy(&vertexBuffer[i].textureCoord, &texCoords[i], flt * 2);
		memcpy(&vertexBuffer[i].normal, &normals[i], flt * 3);
		memcpy(&vertexBuffer[i].tangent, &tangents[i], flt * 3);
	}

	for (int i = 0; i < GetIndexCount(); ++i) {
		indexBuffer[i] = indices[i];
	}

	attributeCount = 4;
	attributeBuffers = new sce::Gnm::Buffer[4];

	InitAttributeBuffer(attributeBuffers[0], Gnm::kDataFormatR32G32B32Float, &(vertexBuffer[0].position));
	InitAttributeBuffer(attributeBuffers[1], Gnm::kDataFormatR32G32Float, &(vertexBuffer[0].textureCoord));
	InitAttributeBuffer(attributeBuffers[2], Gnm::kDataFormatR32G32B32Float, &(vertexBuffer[0].normal));
	InitAttributeBuffer(attributeBuffers[3], Gnm::kDataFormatR32G32B32Float, &(vertexBuffer[0].tangent));
}

void PS4Mesh::InitAttributeBuffer(sce::Gnm::Buffer& buffer, Gnm::DataFormat format, void* offset) {
	buffer.initAsVertexBuffer(offset, format, sizeof(MeshVertex), GetVertexCount());
	buffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);
}

void PS4Mesh::SubmitPreDraw(Gnmx::GnmxGfxContext& cmdList, Gnm::ShaderStage stage) {
	cmdList.setVertexBuffers(stage, 0, attributeCount, attributeBuffers);
}

void PS4Mesh::SubmitDraw(Gnmx::GnmxGfxContext& cmdList, Gnm::ShaderStage stage) {
	cmdList.setPrimitiveType(primitiveType);
	cmdList.setIndexSize(indexType);
	cmdList.drawIndex(GetIndexCount(), indexBuffer);
}