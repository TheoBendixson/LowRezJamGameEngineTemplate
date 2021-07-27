#include "game_input.h"
#include "game_texture.h"
#include "game_texture_loading.cpp"
#include "game_renderer.cpp"
#include "game_sound_loading.cpp"

internal void
LoadTextures(game_memory *Memory, game_texture_buffer *GameTextureBuffer, 
             game_texture_map *TextureMap)
{
    game_state *GameState = (game_state *)Memory->PermanentStorage;
    memory_arena *ScratchArena = &GameState->ScratchArena;
    InitializeArena(ScratchArena, Memory->TransientStorageSize, (u8 *)Memory->TransientStorage);
    InitializeTextureBuffer(ScratchArena, GameTextureBuffer, MAX_TILES);

    read_file_result AssetPackFile = PlatformReadPNGFile("LowRez64.png");

    u64 ExpectedContentSize = 64*64*sizeof(u32);

    if (AssetPackFile.ContentsSize == ExpectedContentSize)
    {
        PlatformLogMessage("Loaded Spritesheet \n");

        spritesheet TileTextureSpriteSheet = {};
        TileTextureSpriteSheet.Data = (u32 *)AssetPackFile.Contents;
        TileTextureSpriteSheet.TextureWidth = 8;
        TileTextureSpriteSheet.TextureHeight = 8;
        TileTextureSpriteSheet.TexturesPerRow = 8;
        TileTextureSpriteSheet.TexturesPerColumn = 8;

        spritesheet_section TileSection = {};
        TileSection.XOffset = 0;
        TileSection.YOffset = 0;
        TileSection.SamplingWidth = 8;
        TileSection.SamplingHeight = 8;

        spritesheet_position Position = {};
        Position.Row = 0;
        Position.Column = 0;

        u32 ScreenWidthInTiles = 8;
        u32 TileIndex = 0;

        for (u32 Row = 0;
             Row < ScreenWidthInTiles;
             Row++)
        {
            Position.Row = Row;

            for (u32 Column = 0;
                 Column < ScreenWidthInTiles;
                 Column++)
            {
                Position.Column = Column;
                TextureMap->Tiles[TileIndex] = GameTextureBuffer->TexturesLoaded;
                LoadTileTextureFromSpritesheet(ScratchArena, &TileTextureSpriteSheet, GameTextureBuffer, 
                                               TileSection, Position);
                PlatformLogMessageU32("Loaded Tile", TileIndex);
                TileIndex++;
            }
        }

    } else
    {
        PlatformLogMessage("Loaded PNG Was Not Expected Content Size \n");
    }

    PlatformFreeMemory(AssetPackFile.Contents);

    PlatformLogMessage("Texture Loading Before Assert \n");
    Assert(GameTextureBuffer->TexturesLoaded < GameTextureBuffer->MaxTextures);
    PlatformLogMessage("End of Texture Loading \n");
}

internal void
GameUpdateAndRender(game_memory *GameMemory, game_texture_map *TextureMap, 
                    game_input *GameInput, game_render_commands *RenderCommands)
{
    r32 TileWidthInPixels = 128.0f;
    v2 Min = { 0.0f, 0.0f };
    v2 Max = { TileWidthInPixels, TileWidthInPixels };

    u32 ScreenWidthHeightInTiles = 8; 
    u32 TileIndex = 0;

    for (u32 Row = 0;
         Row < ScreenWidthHeightInTiles;
         Row++)
    {
        for (u32 Column = 0;
             Column < ScreenWidthHeightInTiles;
             Column++)
        {
            DrawTexturedRectangle(RenderCommands, Min, Max, TextureMap->Tiles[TileIndex]);
            Min.X += TileWidthInPixels;
            Max.X += TileWidthInPixels;
            TileIndex++;
        }

        Min.X = 0.0f;
        Max.X = TileWidthInPixels;
        Min.Y += TileWidthInPixels;
        Max.Y += TileWidthInPixels;
    }

}

internal void
GameGetSoundSamples(game_memory *Memory, game_sound_output_buffer *SoundOutputBuffer, 
                    game_sound_mix_panel *GameSoundMixPanel)
{
    game_state *GameState = (game_state *)Memory->PermanentStorage;

    game_sound_input_buffer *BackgroundMusic = &GameSoundMixPanel->BackgroundMusic;

    u32 HalfAudioFramesToWriteThisFrame = SoundOutputBuffer->SamplesToWriteThisFrame*2;

    for (u32 SampleIndex = 0; 
         SampleIndex < HalfAudioFramesToWriteThisFrame; 
         SampleIndex++)
    {
        s16 BackgroundMusicSample = BackgroundMusic->Samples[BackgroundMusic->CurrentIndex];
        BackgroundMusic->CurrentIndex++;

        SoundOutputBuffer->SamplesWrittenThisFrame++;

        if (BackgroundMusic->CurrentIndex >= BackgroundMusic->SampleCount)
        {
            BackgroundMusic->CurrentIndex = 0;
        }

        r32 MasterVolume = 0.7f;
        r32 MixedSoundsInFloatSpace = (r32)BackgroundMusicSample*MasterVolume;
        s16 MixedSounds = (s16)(MixedSoundsInFloatSpace + 0.5);
        SoundOutputBuffer->Samples[SampleIndex] = MixedSounds;
    }
}
