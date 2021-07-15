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
