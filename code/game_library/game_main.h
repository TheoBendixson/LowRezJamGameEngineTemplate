#include "base_types.h"
#include "game_math.h"
#include "game_memory.h"
#include "memory_arena.h"
#include "file_io.h"
#include "platform_logging.h"
#include "game_texture_map.h"
#include "spritesheet.h"
#include "game_renderer.h"

struct game_state
{
    memory_arena ScratchArena;
};
