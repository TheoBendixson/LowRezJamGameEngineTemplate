#include "base_types.h"
#include "game_math.h"
#include "game_memory.h"
#include "memory_arena.h"
#include "game_sound.h"
#include "file_io.h"
#include "platform_logging.h"
#include "game_texture_map.h"
#include "spritesheet.h"
#include "game_renderer.h"

#define PLATFORM_FREE_MEMORY(name) void name(void *Memory) 
typedef PLATFORM_FREE_MEMORY(platform_free_memory);

struct game_state
{
    memory_arena ScratchArena;
    memory_arena SoundsArena;
};
