
struct game_texture_map
{
    u32 FirstTile;
};

struct game_texture
{
    u32 *Data;
    u32 Type;
    u32 Width;
    u32 Height;
};

struct game_texture_buffer
{
    game_texture *Textures;
    u32 MaxTextures;
    u32 TexturesLoaded;
};
