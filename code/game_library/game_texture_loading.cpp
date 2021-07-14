
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
