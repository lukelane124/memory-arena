#include <stdint.h>
#include <stddef.h>

#include "arena.h"
// typedef struct arena_s ARENA;

typedef struct arena_region_s
{
    void* regionStart;
    size_t promisedSize;
    size_t allocatedSize;
    uint64_t sum;
} arena_region_t;

struct arena_s
{
    void* memoryStart;
    size_t memoryFootprint;
    size_t currentIndex;
    arena_region_t* firstFreeRegion;
};


void* arena_internalGetMemory(ARENA* arena, size_t requestedSize)
{
    void* ret = NULL;
    uint8_t* p8;
    size_t availableSpace = arena->memoryFootprint - arena->currentIndex;
    if (availableSpace < arena->memoryFootprint)
    {
        // Underflow. Exit.
    }
    else
    {
        // No maths errors.
        if (availableSpace >= requestedSize)
        {
            // Requested size will fit.
            p8 = arena->memoryStart;

            ret = &p8[arena->currentIndex];
            // Set the return value to the next available byte.

            arena->currentIndex += requestedSize;
            // Increment the current offset into the arena.
        }
    }

    return ret;
}

ARENA* arena_createArena(void* memory, size_t sizeOfMemory)
{
    ARENA* RET = NULL;
    // Set default return value to indicate failure.

    ARENA ar = {0};
    // Create a local (on stack) ARENA object to use during initialization.
    if ( 
        (memory != NULL) &&
        // Was the region we were passed valid?
        (sizeOfMemory > (sizeof(ARENA) + sizeof(arena_region_t)))
        // will their be enough memory to allocate at least the internal structures?
    )
    {
        // Preconditions met. Begin internal configuration.

        ar.memoryStart = memory;
        // Set the start of the arena area to the passed memory region.

        ar.memoryFootprint = sizeOfMemory;
        // Set the size of the memory region.

        ar.currentIndex = 0;
        // Update the index into the arena for the next available byte.

        ar.firstFreeRegion = NULL;
        // No allocations have been made yet.

        RET = arena_internalGetMemory(&ar, sizeof(arena_region_t));
        // Set the return value to a new region of memory reserved to hold the arena metadata.

    }

    if (RET != NULL)
    {
        // Arena pointer is a valid memory location.

        memcpy(RET, &ar, sizeof(ARENA));
    }
}

void* arena_getMemory(ARENA* arena, size_t requestedSize)
{
    void* ret = NULL;
    // Default return indicates failure.
    size_t arenaInitialIndex = arena->currentIndex;
    arena_region_t region = {0};
    // Create a local (on stack) region object to temporarly hold the metadata for the requested region.

    arena_region_t* pRegion = arena_internalGetMemory(arena, sizeof(arena_region_t));
    // Get a reservation to hold the metadata for this memory allocation.
    if (pRegion != NULL)
    {
        // pRegion is valid.

        region.regionStart = arena_internalGetMemory(arena, requestedSize);
        // Get an additional reservation to account for the requested memory size.
    }

    if (pRegion == NULL || region.regionStart == NULL)
    {
        // region could not be allocated.

        arena->currentIndex = arenaInitialIndex;
        // Rollback the index to the state it was in when we entered here.
    }
    else
    {
        // Allocations went through successfully.

        region.allocatedSize = requestedSize;
        // Set the size of the actual allocation alloted for this object.
        
        region.promisedSize = requestedSize;
        // Set the size of the memory requested.

        region.sum = (region.allocatedSize + region.promisedSize + (size_t) region.regionStart);
        // Calculate the sum of the header information. This is used to detect failures.
    }
}

int arena_internalAddToFreePool(ARENA* arena, arena_region_t* region)
{
    int ret = -1;

    arena_region_t** currentFreeRegion = &(arena->firstFreeRegion);
    while(*currentFreeRegion != NULL)
    {
        
    }

    return ret;
}

int arena_giveMemory(ARENA* arena, void* ptr)
{
    int ret = -1;
    // Indicate failure by default.
    arena_region_t region;
    
    if (arena != NULL)
    {
        if (ptr == NULL)
        {
            ret = 1;
            // Indicate success. null Ptr parameter is a valid call.
        }
        else
        {
            // Arena is valid and ptr is now null;

            if ((ptr >= arena->memoryStart) && (ptr <= (&((uint8_t*)arena->memoryStart)[arena->memoryFootprint])) )
            {
                // ptr is from the correct region of memory.

                memcpy(&region, ((uint8_t*) ptr)[-(sizeof(arena_region_t))], sizeof(arena_region_t));
                if ( (region.allocatedSize + region.promisedSize + (size_t) region.regionStart) == region.sum)
                {
                    if (arena->firstFreeRegion == NULL)
                    {
                        arena->firstFreeRegion = ((uint8_t*) ptr)[-(sizeof(arena_region_t))];
                    }
                    else
                    {
                        ret = arena_internalAddToFreePool(arena, ((uint8_t*) ptr)[-(sizeof(arena_region_t))]);
                    }
                }
            }
        }
    }

    return ret;
}