typedef struct arena_s ARENA;

ARENA* arena_createArena(void* memory, size_t sizeOfMemory);

void* arena_getMemory(ARENA* arena, size_t requestedSize);

int arena_giveMemory(ARENA* arena, void* ptr);