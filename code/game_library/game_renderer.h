struct vector_2d_float
{
    GLfloat X, Y;
};

struct game_2d_vertex
{
    vector_2d_float Vertex;
    vector_2d_float TextureCoordinate;
};

struct render_layer
{
    game_2d_vertex *Vertices;
    u32 VertexCount;
    u32 MaxVertices;

    u32 TextureAtlasUnitWidth; 
    u32 TextureAtlasUnitHeight;

    u32 AtlasTextureID;
    GLuint VertexBufferObject;
    GLuint TextureCoordinateBufferObject;
};

struct game_render_commands
{
    s32 ViewportWidth;
    s32 ViewportHeight;
    r32 ScreenScaleFactor;

    GLuint ShaderProgram;

    render_layer TileLayer;
    render_layer PlayerLayer;
};
