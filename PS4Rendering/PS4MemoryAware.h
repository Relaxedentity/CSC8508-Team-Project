#pragma once

#include <gnm.h>
#include <gnmx\context.h>
#include <.\graphics\api_gnm\toolkit\allocators.h>
#include <.\graphics\api_gnm\toolkit\stack_allocator.h>

class PS4MemoryAware {
protected:
	PS4MemoryAware() {}
	~PS4MemoryAware() {}

	static sce::Gnmx::Toolkit::IAllocator OnionAllocator;
	static sce::Gnmx::Toolkit::IAllocator GarlicAllocator;
	static sce::Gnm::OwnerHandle ownerHandle;
};