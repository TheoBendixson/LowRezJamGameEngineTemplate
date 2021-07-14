#include "game_memory.h"
#include "memory_arena.h"
#include "game_renderer.h"
#include "game_input.h"
#include "game_texture.h"

struct game_state
{
    memory_arena ScratchArena;
};

enum axis_flip_mode
{
    AxisFlipModeNone,
    AxisFlipModeX
};

struct spritesheet
{
    u32 *Data;
    u32 TextureWidth;
    u32 TextureHeight;
    u32 TexturesPerRow;
    u32 TexturesPerColumn;
};

struct spritesheet_section
{
    s32 XOffset;
    s32 YOffset;
    u32 SamplingWidth;
    u32 SamplingHeight;
};

struct spritesheet_position
{
    s32 Row;
    s32 Column;
};

void
InitializeTextureBuffer(memory_arena *ScratchArena, game_texture_buffer *TextureBuffer, u32 MaxTextures)
{
    TextureBuffer->TexturesLoaded = 0;
    TextureBuffer->MaxTextures = MaxTextures;
    TextureBuffer->Textures = PushArray(ScratchArena, TextureBuffer->MaxTextures, game_texture);
}

void 
LoadTileTextureFromSpritesheet(memory_arena *ScratchArena, spritesheet *SpriteSheet, game_texture_buffer *TextureBuffer, 
                               spritesheet_section SpriteSheetSection, spritesheet_position Position)
{
    game_texture Texture = TextureBuffer->Textures[TextureBuffer->TexturesLoaded];
    Texture.Width = SpriteSheetSection.SamplingWidth;
    Texture.Height = SpriteSheetSection.SamplingHeight;
    u32 TotalPixels = Texture.Width*Texture.Height;
    Texture.Data = PushArray(ScratchArena, TotalPixels, u32);
    u32 *PixelDest = (u32 *)Texture.Data;
    u32 *PixelSource = SpriteSheet->Data;

    u32 TextureWidth = SpriteSheet->TextureWidth;
    u32 TextureHeight = SpriteSheet->TextureHeight;
    u32 RowSizeInPixels = SpriteSheet->TexturesPerRow*TextureWidth;

    s32 StartingXOffsetInPixels = Position.Column*TextureWidth;
    u32 DestRowCount = 0;

    for (s32 Row = (Position.Row*TextureHeight + SpriteSheetSection.SamplingHeight -1); 
         ((Row >= (Position.Row*TextureHeight)) && (Row >= 0)); 
         Row--)
    {
        u32 *DestRow = PixelDest + (DestRowCount*Texture.Width);

        for (s32 Column = SpriteSheetSection.XOffset; 
             Column <= (SpriteSheetSection.SamplingWidth); 
             Column++)
        {
            *DestRow++ = PixelSource[StartingXOffsetInPixels + (Row*RowSizeInPixels) + Column];
        }

        DestRowCount++;
    }

   TextureBuffer->Textures[TextureBuffer->TexturesLoaded] = Texture; 
   TextureBuffer->TexturesLoaded++;
}

internal void
LoadTextures(game_memory *Memory, game_texture_buffer *GameTextureBuffer, 
             game_texture_map *TextureMap)
{
    game_state *GameState = (game_state *)Memory->PermanentStorage;
    memory_arena *ScratchArena = &GameState->ScratchArena;
    InitializeArena(ScratchArena, Memory->TransientStorageSize, (u8 *)Memory->TransientStorage);
    InitializeTextureBuffer(ScratchArena, GameTextureBuffer, 30);

    read_file_result AssetPackFile = PlatformReadPNGFile("LowRez.png");

    if (AssetPackFile.ContentsSize > 0)
    {
        spritesheet TileTextureSpriteSheet = {};
        TileTextureSpriteSheet.Data = (u32 *)AssetPackFile.Contents;
        TileTextureSpriteSheet.TextureWidth = 8;
        TileTextureSpriteSheet.TextureHeight = 8;
        TileTextureSpriteSheet.TexturesPerRow = 31;
        TileTextureSpriteSheet.TexturesPerColumn = 45;

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

}
