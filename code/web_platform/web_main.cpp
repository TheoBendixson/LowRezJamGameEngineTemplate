#include <emscripten.h>
#include <SDL.h>
#include <stdio.h>
#include <SDL_opengles2.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_JPEG
#define STBI_NO_BMP
#define STBI_NO_TGA
#define STBI_NO_PSD
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_GIF
#define STBI_NO_PNM
#include "stb_image.h"

#include "web_platform_shaders.h"

#include "web_main.h"
#include "web_file.cpp"

#define MAX_TEXTURES    200

PLATFORM_LOG_MESSAGE(PlatformLogMessage)
{
    printf("Log: %s", Message); 
}

#include "../game_library/game_main.cpp"

struct render_loop_arguments
{
    SDL_Window *Window;
};

static game_memory GameMemory = {};
static game_render_commands RenderCommands = {};
static game_texture_map TextureMap = {};
static game_input GameInput = {};

void RenderLoop(void *Arg)
{
    render_loop_arguments *Args = (render_loop_arguments *)Arg;
    SDL_Window *Window = Args->Window;

    game_controller_input *Controller = &GameInput.Controller;

    SDL_Event Event = {};

    while(SDL_PollEvent(&Event))
    {
        SDL_Keycode Key = Event.key.keysym.sym;

        switch(Event.type)
        {
            case SDL_KEYDOWN:
                if(Key == SDLK_RIGHT)
                {
                    Controller->Right.EndedDown = true;
                } 
                else if(Key == SDLK_LEFT)
                {
                    Controller->Left.EndedDown = true;
                } 
                else if(Key == SDLK_UP)
                {
                    Controller->Up.EndedDown = true;
                }
                else if(Key == SDLK_SPACE)
                {
                    Controller->A.EndedDown = true;
                } 
                else if(Key == SDLK_d)
                {
                    Controller->X.EndedDown = true;
                }
                else if(Key == SDLK_s)
                {
                    Controller->B.EndedDown = true;
                }
                else if(Key == SDLK_z)
                {
                    Controller->LeftShoulder1.EndedDown = true;
                }
                else if (Key == SDLK_y)
                {
                    Controller->RightShoulder2.EndedDown = true;
                }

                break;

            case SDL_KEYUP:
                if (Key == SDLK_RIGHT)
                {
                    Controller->Right.EndedDown = false;
                } 
                else if (Key == SDLK_LEFT)
                {
                    Controller->Left.EndedDown = false;
                } 
                else if (Key == SDLK_UP)
                {
                    Controller->Up.EndedDown = false;
                }
                else if (Key == SDLK_SPACE)
                {
                    Controller->A.EndedDown = false;
                } 
                else if(Key == SDLK_d)
                {
                    Controller->X.EndedDown = false;
                }
                else if(Key == SDLK_s)
                {
                    Controller->B.EndedDown = false;
                }
                else if(Key == SDLK_z)
                {
                    Controller->LeftShoulder1.EndedDown = false;
                }
                else if (Key == SDLK_y)
                {
                    Controller->RightShoulder2.EndedDown = false;
                }

                break;
        }
    }

    render_layer *TileLayer = &RenderCommands.TileLayer;
    TileLayer->VertexCount = 0;

    for(u32 Vertex = 0;
        Vertex < TileLayer->MaxVertices;
        Vertex++)
    {
        TileLayer->Vertices[Vertex] = { { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } };
    }

    GameUpdateAndRender(&GameMemory, &TextureMap, &GameInput, &RenderCommands);

    glClearColor(0.667f, 0.667f, 0.667f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, TileLayer->AtlasTextureID);
    glBindBuffer(GL_ARRAY_BUFFER, TileLayer->VertexBufferObject);

    printf("glBufferData \n");
    glBufferData(GL_ARRAY_BUFFER, (sizeof(game_2d_vertex)*TileLayer->MaxVertices), TileLayer->Vertices, GL_STATIC_DRAW);

    GLenum error = glGetError();

    switch (error)
    {
        case GL_NO_ERROR:
            printf("No GL Error \n");
            break;
        case GL_INVALID_OPERATION:
            printf("GL Error: Invalid Operation \n");
            break;
        case GL_INVALID_ENUM:
            printf("GL Error: Invalid Enum \n (glBufferData)");
            break;
        case GL_OUT_OF_MEMORY:
            printf("GL Error: Out of Memory \n (glBufferData)");
            break;
    }

    GLint PositionAttribLocation = glGetAttribLocation(RenderCommands.ShaderProgram, "Position");
    glVertexAttribPointer(PositionAttribLocation, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*6, (void *)0);
    glEnableVertexAttribArray(PositionAttribLocation);

    GLint TextureCoordinateAttribute = glGetAttribLocation(RenderCommands.ShaderProgram, "InTextureCoordinate");
    glVertexAttribPointer(TextureCoordinateAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*6, (void *)(4 * sizeof(GLfloat)));
    glEnableVertexAttribArray(TextureCoordinateAttribute);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    printf("glDrawElements \n");
    glDrawArrays(GL_TRIANGLES, 0, TileLayer->VertexCount);

    error = glGetError();

    switch (error)
    {
        case GL_NO_ERROR:
            break;
        case GL_INVALID_ENUM:
            printf("GL Error: Invalid Enum (glDrawElements) \n");
            break;
        case GL_INVALID_VALUE:
            printf("GL Error: Invalid Value (glDrawElements) \n");
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            printf("GL Error: Invalid Frame Buffer Operation (glDrawElements) \n");
            break;
        case GL_OUT_OF_MEMORY:
            printf("GL Error: Out of Memory (glDrawElements) \n");
            break;
    }


    // TODO: (Ted)  Bring back the player layer once we have established the ability to load
    //              and draw the tiles in a single pass.
    
    /*
    glBindTexture(GL_TEXTURE_2D, RenderCommands.PlayerAtlasTextureID);
    glBindBuffer(GL_ARRAY_BUFFER, RenderCommands.PlayerVertexBufferObject);
    GLint PlayerPositionAttribute = glGetAttribLocation(RenderCommands.ShaderProgram, "Position");
    glVertexAttribPointer(PlayerPositionAttribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(PlayerPositionAttribute);
    glBufferData(GL_ARRAY_BUFFER, sizeof(RenderCommands.PlayerTextureVertices), RenderCommands.PlayerTextureVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, RenderCommands.PlayerTextureCoordinateBufferObject);
    GLint PlayerTextureCoordinateAttribute = glGetAttribLocation(RenderCommands.ShaderProgram, "InTextureCoordinate");
    glVertexAttribPointer(PlayerTextureCoordinateAttribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(PlayerTextureCoordinateAttribute);
    glBufferData(GL_ARRAY_BUFFER, sizeof(RenderCommands.PlayerTextureCoordinates), 
                 RenderCommands.PlayerTextureCoordinates, GL_STATIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, RenderCommands.PlayerVertexCount);*/

    SDL_GL_SwapWindow(Window);
}

int main(int argc, const char * argv[]) 
{
    printf("Starting Game\n");

    RenderCommands.ViewportWidth = 1024;
    RenderCommands.ViewportHeight = 1024;
    RenderCommands.ScreenScaleFactor = 2.0f;

    SDL_Window *Window;

    SDL_CreateWindowAndRenderer((r32)RenderCommands.ViewportWidth, 
                                (r32)RenderCommands.ViewportHeight, 
                                0, &Window, nullptr);

    GameMemory.PermanentStorageSize = Megabytes(64);
    GameMemory.TransientStorageSize = Megabytes(64);

    GameMemory.PermanentStorage = malloc(GameMemory.PermanentStorageSize);
    GameMemory.TransientStorage = malloc(GameMemory.TransientStorageSize);

    u64 TransientStoragePartitionSize = Megabytes(32);

    render_layer *TileLayer = &RenderCommands.TileLayer;
    TileLayer->VertexCount = 0;
    TileLayer->MaxVertices = 1000;
    TileLayer->Vertices = (game_2d_vertex *)malloc(sizeof(game_2d_vertex)*TileLayer->MaxVertices);

    game_texture_buffer GameTextureBuffer = {};
    LoadTextures(&GameMemory, &GameTextureBuffer, &TextureMap);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    u32 TileAtlasTextureID;
    glGenTextures(1, &TileAtlasTextureID);
    glBindTexture(GL_TEXTURE_2D, TileAtlasTextureID);
    RenderCommands.TileLayer.AtlasTextureID = TileAtlasTextureID;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    u32 AtlasWidthHeightInUnits = 4;
    RenderCommands.TileLayer.TextureAtlasUnitWidth = AtlasWidthHeightInUnits;
    RenderCommands.TileLayer.TextureAtlasUnitHeight = AtlasWidthHeightInUnits;

    u32 AtlastUnitWidthHeightInPixels = 8;
    u32 AtlasTextureWidthHeightInPixels = AtlasWidthHeightInUnits*AtlastUnitWidthHeightInPixels;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, AtlasTextureWidthHeightInPixels, AtlasTextureWidthHeightInPixels, 
                 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    for (u32 TextureIndex = 0;
         TextureIndex < GameTextureBuffer.TexturesLoaded;
         TextureIndex++)
    {
        u32 YPosition = TextureIndex/AtlasWidthHeightInUnits;
        u32 XPosition = TextureIndex%AtlasWidthHeightInUnits;

        u32 XOffset = XPosition*AtlastUnitWidthHeightInPixels;
        u32 YOffset = YPosition*AtlastUnitWidthHeightInPixels;

        glTexSubImage2D(GL_TEXTURE_2D, 0, XOffset, YOffset, AtlastUnitWidthHeightInPixels, AtlastUnitWidthHeightInPixels, 
                        GL_RGBA, GL_UNSIGNED_BYTE, GameTextureBuffer.Textures[TextureIndex].Data);
    }

    // TODO: (Ted)  Bring back the player layer later.

    /*
    u32 PlayerAtlasTextureID;
    glGenTextures(1, &PlayerAtlasTextureID);
    glBindTexture(GL_TEXTURE_2D, PlayerAtlasTextureID);
    RenderCommands.PlayerAtlasTextureID = PlayerAtlasTextureID;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    RenderCommands.PlayerTextureAtlasUnitWidth = 2;
    RenderCommands.PlayerTextureAtlasUnitHeight = 1;

    u32 PlayerAtlasUnitWidthInPixels = 20;
    u32 PlayerAtlasUnitHeightInPixels = 28;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, PlayerAtlasUnitWidthInPixels*RenderCommands.PlayerTextureAtlasUnitWidth, 
                 PlayerAtlasUnitHeightInPixels, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, PlayerAtlasUnitWidthInPixels, PlayerAtlasUnitHeightInPixels, 
                    GL_RGBA, GL_UNSIGNED_BYTE, GameTextureBuffer.Textures[0].Data);

    glTexSubImage2D(GL_TEXTURE_2D, 0, PlayerAtlasUnitWidthInPixels, 0, PlayerAtlasUnitWidthInPixels, 
                    PlayerAtlasUnitHeightInPixels, GL_RGBA, GL_UNSIGNED_BYTE, GameTextureBuffer.Textures[1].Data);

    for (u32 TextureIndex = 0; 
         TextureIndex < GameTextureBuffer.MaxTextures; 
         TextureIndex++)
    {
        free(GameTextureBuffer.Textures[TextureIndex].Data);
    }*/

    // TODO: (Ted)  Clear the transient storage arena

    GLuint VertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(VertexShader, 1, &VertexSource, nullptr);
    glCompileShader(VertexShader);

    GLuint FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(FragmentShader, 1, &FragmentSource, nullptr);
    glCompileShader(FragmentShader);

    GLuint ShaderProgram = glCreateProgram();
    glAttachShader(ShaderProgram, VertexShader);
    glAttachShader(ShaderProgram, FragmentShader);
    glLinkProgram(ShaderProgram);

    glUseProgram(ShaderProgram);

    RenderCommands.ShaderProgram = ShaderProgram;

    GLchar *Log = (GLchar *)malloc(400*sizeof(GLchar));
    GLsizei LogLength;

    glGetProgramInfoLog(ShaderProgram, 400, &LogLength, Log);

    if (LogLength > 0)
    {
        printf("A Log was generated. \n");
        printf("%s \n", Log);
    } else
    {
        printf("No log returned from get program info log \n");
    }

    free(Log);

    RenderCommands.TileLayer.VertexCount = 0;

    GameInput.dtForFrame = 1.0f/60.0f;

    GLuint VertexBufferObject;
    glGenBuffers(1, &VertexBufferObject);
    RenderCommands.TileLayer.VertexBufferObject = VertexBufferObject;

    /*
    GLuint TextureCoordinateBufferObject;
    glGenBuffers(1, &TextureCoordinateBufferObject);
    RenderCommands.TileLayer.TextureCoordinateBufferObject = TextureCoordinateBufferObject;
    
    GLuint PlayerVertexBufferObject;
    glGenBuffers(1, &PlayerVertexBufferObject);
    RenderCommands.PlayerLayer.VertexBufferObject = PlayerVertexBufferObject;

    GLuint PlayerTextureCoordinateBufferObject;
    glGenBuffers(1, &PlayerTextureCoordinateBufferObject);
    RenderCommands.PlayerLayer.TextureCoordinateBufferObject = PlayerTextureCoordinateBufferObject;*/

    render_loop_arguments Args = {};
    Args.Window = Window;

    emscripten_set_main_loop_arg(RenderLoop, (void *)&Args, 60, 1);

    return 0;
}
