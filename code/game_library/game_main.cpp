#include "game_input.h"
#include "game_texture.h"

#include "game_texture_loading.cpp"
#include "game_renderer.cpp"

internal void
LoadTextures(game_memory *Memory, game_texture_buffer *GameTextureBuffer, 
             game_texture_map *TextureMap)
{
    game_state *GameState = (game_state *)Memory->PermanentStorage;
    memory_arena *ScratchArena = &GameState->ScratchArena;
    InitializeArena(ScratchArena, Memory->TransientStorageSize, (u8 *)Memory->TransientStorage);
    InitializeTextureBuffer(ScratchArena, GameTextureBuffer, 30);

    read_file_result AssetPackFile = PlatformReadPNGFile("LowRez16.png");

    if (AssetPackFile.ContentsSize > 0)
    {
        PlatformLogMessage("Loaded Spritesheet \n");

        spritesheet TileTextureSpriteSheet = {};
        TileTextureSpriteSheet.Data = (u32 *)AssetPackFile.Contents;
        TileTextureSpriteSheet.TextureWidth = 8;
        TileTextureSpriteSheet.TextureHeight = 8;
        TileTextureSpriteSheet.TexturesPerRow = 2;
        TileTextureSpriteSheet.TexturesPerColumn = 2;

        spritesheet_section TileSection = {};
        TileSection.XOffset = 0;
        TileSection.YOffset = 0;
        TileSection.SamplingWidth = 8;
        TileSection.SamplingHeight = 8;

        spritesheet_position Position = {};
        Position.Row = 0;
        Position.Column = 0;

        TextureMap->FirstTile = GameTextureBuffer->TexturesLoaded;
        LoadTileTextureFromSpritesheet(ScratchArena, &TileTextureSpriteSheet, GameTextureBuffer, 
                                       TileSection, Position);
    }
}

internal void
GameUpdateAndRender(game_memory *GameMemory, game_texture_map *TextureMap, 
                    game_input *GameInput, game_render_commands *RenderCommands)
{
    v2 Min = { 0.0f, 0.0f };
    v2 Max = { 64.0f, 64.0f };
    DrawTexturedRectangle(RenderCommands, Min, Max, TextureMap->FirstTile);
}
