#pragma once
#include <iostream>
#include "PS4Texture.h"

using namespace PS4;

PS4Texture* PS4Texture::LoadTextureFromFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::binary);

	if (!file) {
		return NULL;
	}

	Gnf::Header header;
	file.read((char*)&header, sizeof(header));

	if (header.m_magicNumber != sce::Gnf::kMagic) {
		return NULL;
	}

	char* rawContents = new char[header.m_contentsSize];
	file.read((char*)rawContents, header.m_contentsSize);

	Gnf::contents* contentsDesc = (Gnf::Contents*)rawContents;

	Gnm::SizeAlign dataParams = getTexturePixelsSize(contentsDesc, 0);

	void* pixelsAddr = GarlicAllocator.allocate(dataParams);
	Gnm::registerResource(nullptr, *ownerHandle, pixelsAddr, dataParams.m_size, filename.c_str(), Gnm::kResourceTypeTextureBaseAddress, 0);

	file.seekg(getTexturePixelsByteOffset(contentsDesc, 0), ios::cur);
	file.read((char*)pixelsAddr, dataParams.m_size);

	PS4Texture* tex = new PS4Texture();
	tex->apiTexture = *patchTextures(contentsDesc, 0, 1, &pixelsAddr);

	tex->width = tex->apiTexture.getWidth();
	tex->height = tex->apiTexture.getHeight();
	tex->bpp = tex->apiTexture.getDepth();

	tex->apiTexture.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);

	file.close();
	delete rawContents;
	return tex;
}