#include "PS4MemoryAware.h"

using namespace PS4;

sce::Gnmx::Toolkit::IAllocator PS4MemoryAware::OnionAllocator;
sce::Gnmx::Toolkit::IAllocator PS4MemoryAware::GarlicAllocator;
sce::Gnm::OwnerHandle PS4MemoryAware::ownerHandle;