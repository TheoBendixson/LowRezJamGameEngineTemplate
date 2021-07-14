#include "base_types.h"
#include "file_io.h"
#include "game_memory.h"
#include "memory_arena.h"
#include "game_renderer.h"
#include "game_input.h"
#include "game_texture.h"

struct game_state
{
    memory_arena ScratchArena;
};

internal void
LoadTextures(game_memory *GameMemory, game_texture_buffer *GameTextureBuffer, 
             game_texture_map *TextureMap)
{
    game_state *GameState = (game_state *)GameMemory->PermanentStorage;

    // TODO: (Ted)  Setup the scratch memory arena, and load textures here.
}

internal void
GameUpdateAndRender(game_memory *GameMemory, game_texture_map *TextureMap, 
                    game_input *GameInput, game_render_commands *RenderCommands)
{

}
