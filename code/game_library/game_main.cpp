#include "game_renderer.h"
#include "game_input.h"
#include "game_texture.h"

#include "game_texture_loading.cpp"

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

struct y_component
{
    r32 Min;
    r32 Max;
};

inline
y_component InvertYAxis(int ViewportHeight, 
                        r32 YMin, r32 YMax)
{
    y_component Result = {};
    Result.Min = (r32)(ViewportHeight - YMin);
    Result.Max = (r32)(ViewportHeight - YMax);
    return (Result);
}

void DrawTexturedRectangle(game_render_commands *RenderCommands, 
                           v2 vMin, v2 vMax, u32 TextureID)
{
    y_component YComponent = InvertYAxis(RenderCommands->ViewportHeight, 
                                         vMin.Y, vMax.Y);

    GLfloat QuadVertices[] = { vMax.X, YComponent.Max, 
                               vMin.X, YComponent.Max, 
                               vMin.X, YComponent.Min, 
                               vMin.X, YComponent.Min, 
                               vMax.X, YComponent.Min, 
                               vMax.X, YComponent.Max };

    u32 TextureAtlasUnitWidth = 0;
    u32 TextureAtlasUnitHeight = 0;

    render_layer *TileLayer = &RenderCommands->TileLayer;

    TextureAtlasUnitWidth = TileLayer->TextureAtlasUnitWidth;
    TextureAtlasUnitHeight = TileLayer->TextureAtlasUnitHeight;
    
    // 1. Calculate X/Y Offset Increments.
    r32 XOffsetIncrement = (r32)(1.0f/TextureAtlasUnitWidth);
    r32 YOffsetIncrement = (r32)(1.0f/TextureAtlasUnitHeight);

    // 2. Calculate Y Position
    u32 YPosition = TextureID/TextureAtlasUnitWidth;
    u32 XPosition = TextureID%TextureAtlasUnitWidth;

    // 3. Calculate Offsets
    r32 TextureXMin = XPosition*XOffsetIncrement;
    r32 TextureXMax = TextureXMin+XOffsetIncrement;

    r32 TextureYMin = YPosition*YOffsetIncrement;
    r32 TextureYMax = TextureYMin+YOffsetIncrement;

    GLfloat QuadTextureCoordinates[] = { TextureXMax, TextureYMin,
                                         TextureXMin, TextureYMin,
                                         TextureXMin, TextureYMax,
                                         TextureXMin, TextureYMax,
                                         TextureXMax, TextureYMax,
                                         TextureXMax, TextureYMin };

    u32 QuadVertexCount = 12;


    for (u32 Index = 0;
         Index < QuadVertexCount;
         Index++)
    {
        TileLayer->TextureCoordinates[TileLayer->VertexCount] = QuadTextureCoordinates[Index];
        TileLayer->Vertices[TileLayer->VertexCount] = QuadVertices[Index];
        TileLayer->VertexCount++;
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
