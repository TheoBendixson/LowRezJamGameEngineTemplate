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

#define SHOW_LOG                0
#define SHOW_RENDER_LOOP_LOG    0

PLATFORM_FREE_MEMORY(PlatformFreeMemory)
{	
   free(Memory); 
}

PLATFORM_LOG_MESSAGE(PlatformLogMessage)
{
#if SHOW_LOG
    printf("Log: %s", Message); 
#endif
}

PLATFORM_LOG_MESSAGE_U32(PlatformLogMessageU32)
{
#if SHOW_LOG
    printf("Log: %s, Param: %u \n", Message, Param); 
#endif
}

#include "../game_library/game_main.cpp"

struct sdl_audio_ring_buffer
{
    s32 Size;
    s32 WriteCursor;
    s32 PlayCursor;
    void *Data;
};

struct render_loop_arguments
{
    SDL_Window *Window;
    sdl_audio_ring_buffer *AudioRingBuffer;
    game_sound_mix_panel *GameSoundMixPanel;
    game_sound_output_buffer *GameSoundOutputBuffer;
    s32 SamplesPerSecond;
    s32 BytesPerSample;
};

internal game_memory GameMemory = {};
internal game_render_commands RenderCommands = {};
internal game_texture_map *TextureMap = (game_texture_map *)malloc(sizeof(game_texture_map));
internal game_input GameInput = {};
internal u32 RunningSampleIndex = 0;
internal s32 SDLAudioSampleCount = 512; 

void RenderLoop(void *Arg)
{
    render_loop_arguments *Args = (render_loop_arguments *)Arg;
    SDL_Window *Window = Args->Window;
    sdl_audio_ring_buffer *AudioRingBuffer = Args->AudioRingBuffer;
    game_sound_output_buffer *GameSoundOutputBuffer = Args->GameSoundOutputBuffer;
    game_sound_mix_panel *GameSoundMixPanel = Args->GameSoundMixPanel;

    s32 BytesPerSample = Args->BytesPerSample;
    s32 SamplesPerSecond = Args->SamplesPerSecond;

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
        TileLayer->Vertices[Vertex] = { { 0.0f, 0.0f }, { 0.0f, 0.0f } };
    }

    GameUpdateAndRender(&GameMemory, TextureMap, &GameInput, &RenderCommands);

    s32 SecondaryBufferSize = AudioRingBuffer->Size;
    u32 SamplesPerFrameUpdate = SamplesPerSecond/60; 
    u32 FramesAhead = 2;
    u32 DesiredFrameBytesToWrite = SamplesPerFrameUpdate*FramesAhead*BytesPerSample;

    SDL_LockAudio();

    u32 TargetCursor = (AudioRingBuffer->PlayCursor + DesiredFrameBytesToWrite)%SecondaryBufferSize;

    u32 ByteToLock = (RunningSampleIndex*BytesPerSample)%SecondaryBufferSize; 
    u32 BytesToWrite;

     if (ByteToLock > TargetCursor) {
        // NOTE: (ted)  Play Cursor wrapped.

        // Bytes to the end of the circular buffer.
        BytesToWrite = (SecondaryBufferSize - ByteToLock);

        // Bytes up to the target cursor.
        BytesToWrite += TargetCursor;
    } else {
        BytesToWrite = TargetCursor - ByteToLock;
    }

    void *Region1 = (u8*)AudioRingBuffer->Data + ByteToLock;
    s32 Region1Size = BytesToWrite;

    if (Region1Size + ByteToLock > SecondaryBufferSize)
    {
        Region1Size = SecondaryBufferSize - ByteToLock;
    }

    void *Region2 = AudioRingBuffer->Data;
    s32 Region2Size = BytesToWrite - Region1Size;

    SDL_UnlockAudio();

    GameSoundOutputBuffer->SamplesToWriteThisFrame = (BytesToWrite/BytesPerSample);
    GameSoundOutputBuffer->SamplesWrittenThisFrame = 0;

    GameGetSoundSamples(&GameMemory, GameSoundOutputBuffer, GameSoundMixPanel);
    s16 *SoundSrc = GameSoundOutputBuffer->Samples;

    s32 Region1SampleCount = Region1Size/BytesPerSample;
    s16 *SampleOut = (s16 *)Region1;

    for(s32 SampleIndex = 0;
        SampleIndex < Region1SampleCount;
        ++SampleIndex)
    {
        *SampleOut++ = *SoundSrc++;
        *SampleOut++ = *SoundSrc++;
        RunningSampleIndex++;
    }

    s32 Region2SampleCount = Region2Size/BytesPerSample;
    SampleOut = (s16 *)Region2;

    for(s32 SampleIndex = 0;
        SampleIndex < Region2SampleCount;
        ++SampleIndex)
    {
        *SampleOut++ = *SoundSrc++;
        *SampleOut++ = *SoundSrc++;
        RunningSampleIndex++;
    }

    glClearColor(0.667f, 0.667f, 0.667f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, TileLayer->AtlasTextureID);
    glBindBuffer(GL_ARRAY_BUFFER, TileLayer->VertexBufferObject);

#if SHOW_RENDER_LOG
    printf("glBufferData \n");
#endif
    glBufferData(GL_ARRAY_BUFFER, (sizeof(game_2d_vertex)*TileLayer->MaxVertices), TileLayer->Vertices, GL_STATIC_DRAW);

#if SHOW_RENDER_LOG
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
#endif

    GLint PositionAttribLocation = glGetAttribLocation(RenderCommands.ShaderProgram, "Position");
    glVertexAttribPointer(PositionAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*4, (void *)0);
    glEnableVertexAttribArray(PositionAttribLocation);

    GLint TextureCoordinateAttribute = glGetAttribLocation(RenderCommands.ShaderProgram, "InTextureCoordinate");
    glVertexAttribPointer(TextureCoordinateAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*4, (void *)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(TextureCoordinateAttribute);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

#if SHOW_RENDER_LOG
    printf("glDrawElements \n");
#endif

    glDrawArrays(GL_TRIANGLES, 0, TileLayer->VertexCount);

#if SHOW_RENDER_LOG
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
#endif

    SDL_GL_SwapWindow(Window);
}


internal void
SDLAudioCallback(void *UserData, u8 *AudioData, s32 Length)
{
    sdl_audio_ring_buffer *RingBuffer = (sdl_audio_ring_buffer *)UserData;

    s32 Region1Size = Length;
    s32 Region2Size = 0;

    if (RingBuffer->PlayCursor + Length > RingBuffer->Size)
    {
        Region1Size = RingBuffer->Size - RingBuffer->PlayCursor;
        Region2Size = Length - Region1Size;
    }

    memcpy(AudioData, (u8*)(RingBuffer->Data) + RingBuffer->PlayCursor, Region1Size);
    memcpy(&AudioData[Region1Size], RingBuffer->Data, Region2Size);
    RingBuffer->PlayCursor = (RingBuffer->PlayCursor + Length) % RingBuffer->Size;
    RingBuffer->WriteCursor = (RingBuffer->PlayCursor + (SDLAudioSampleCount*2)) % RingBuffer->Size;
}

int main(int argc, const char * argv[]) 
{

#if SHOW_LOG
    printf("Starting Game\n");
#endif

    RenderCommands.ViewportWidth = 1024;
    RenderCommands.ViewportHeight = 1024;
    RenderCommands.ScreenScaleFactor = 2.0f;

    SDL_Window *Window;

    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
#if SHOW_LOG
        printf ("Couldn't initialize SDL Audio: %s \n", SDL_GetError());
#endif
    }

#if SHOW_LOG
    printf("Start of SDL Audio Setup \n");
#endif

    s32 SamplesPerSecond = 44100;

    s32 BytesPerSample = sizeof(s16)*2;  
    s32 SecondaryBufferSize = SamplesPerSecond*BytesPerSample;

    sdl_audio_ring_buffer AudioRingBuffer = {};
    AudioRingBuffer.Size = SecondaryBufferSize;
    AudioRingBuffer.Data = malloc(SecondaryBufferSize);
    AudioRingBuffer.PlayCursor = AudioRingBuffer.WriteCursor = 0;

    game_sound_output_buffer SoundOutputBuffer = {};
    SoundOutputBuffer.Samples = (s16*)malloc(BytesPerSample*SamplesPerSecond);
    SoundOutputBuffer.SamplesPerSecond = SamplesPerSecond;
    SoundOutputBuffer.SamplesToWriteThisFrame = 0;
    SoundOutputBuffer.SamplesWrittenThisFrame = 0;

    SDL_AudioSpec AudioSettings = {0};
    AudioSettings.freq = SamplesPerSecond;
    AudioSettings.format = AUDIO_S16LSB;
    AudioSettings.channels = 2;
    AudioSettings.samples = SDLAudioSampleCount;
    AudioSettings.callback = &SDLAudioCallback;
    AudioSettings.userdata = &AudioRingBuffer;

#if SHOW_LOG
    printf("Before Call to SDL_OpenAudio \n");
#endif
    SDL_OpenAudio(&AudioSettings, 0);

    if (AudioSettings.format != AUDIO_S16LSB)
    {
        printf("Warning: Audio format doesn't match S16LSB \n");
    }

#if SHOW_LOG
    printf("Start of SDL Audio Buffer Setup \n");
#endif

    b32 SoundIsPlaying = false;

    if (!SoundIsPlaying)
    {
        SDL_PauseAudio(0);
        SoundIsPlaying = true;
    }

    SDL_CreateWindowAndRenderer((r32)RenderCommands.ViewportWidth, 
                                (r32)RenderCommands.ViewportHeight, 
                                0, &Window, nullptr);

    GameMemory.PermanentStorageSize = Megabytes(256);
    GameMemory.TransientStorageSize = Megabytes(64);

    GameMemory.PermanentStorage = malloc(GameMemory.PermanentStorageSize);
    GameMemory.SoundsPartition = (u8*)GameMemory.PermanentStorage + Megabytes(64);
    GameMemory.SoundPartitionSize = Megabytes(128);
    GameMemory.TransientStorage = malloc(GameMemory.TransientStorageSize);

    GameMemory.IsInitialized = false;

    game_sound_mix_panel GameSoundMixPanel = {};
    LoadSounds(&GameMemory, &GameSoundMixPanel);

    render_layer *TileLayer = &RenderCommands.TileLayer;
    TileLayer->VertexCount = 0;
    TileLayer->MaxVertices = 1000;
    TileLayer->Vertices = (game_2d_vertex *)malloc(sizeof(game_2d_vertex)*TileLayer->MaxVertices);

    game_texture_buffer GameTextureBuffer = {};
    LoadTextures(&GameMemory, &GameTextureBuffer, TextureMap);

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

    u32 AtlasWidthHeightInUnits = 8;
    RenderCommands.TileLayer.TextureAtlasUnitWidth = AtlasWidthHeightInUnits;
    RenderCommands.TileLayer.TextureAtlasUnitHeight = AtlasWidthHeightInUnits;

    u32 AtlastUnitWidthHeightInPixels = 8;
    u32 AtlasTextureWidthHeightInPixels = AtlasWidthHeightInUnits*AtlastUnitWidthHeightInPixels;

#if SHOW_LOG
    printf("Setup 2D Textures \n");
#endif

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, AtlasTextureWidthHeightInPixels, AtlasTextureWidthHeightInPixels, 
                 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

#if SHOW_LOG
    printf("Fill in 2D Texture Atlas \n");
#endif

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

#if SHOW_LOG
    printf("Clear Scratch Memory Arena \n");
#endif

    u8* Byte = (u8 *)GameMemory.TransientStorage;

    for (u32 Index = 0; 
         Index < GameMemory.TransientStorageSize; 
         Index++)
    {
        *Byte++ = 0;
    }

#if SHOW_LOG
    printf("Setup Vertex Shader \n");
#endif

    GLuint VertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(VertexShader, 1, &VertexSource, nullptr);
    glCompileShader(VertexShader);

#if SHOW_LOG
    printf("Setup Fragment Shader \n");
#endif 

    GLuint FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(FragmentShader, 1, &FragmentSource, nullptr);
    glCompileShader(FragmentShader);

#if SHOW_LOG
    printf("Setup Shader Program \n");
#endif

    GLuint ShaderProgram = glCreateProgram();
    glAttachShader(ShaderProgram, VertexShader);
    glAttachShader(ShaderProgram, FragmentShader);
    glLinkProgram(ShaderProgram);

    glUseProgram(ShaderProgram);

    RenderCommands.ShaderProgram = ShaderProgram;

#if SHOW_LOG
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
#endif

    RenderCommands.TileLayer.VertexCount = 0;
    GameInput.dtForFrame = 1.0f/60.0f;

#if SHOW_LOG
    printf("Setup Vertex Buffer Object \n");
#endif

    GLuint VertexBufferObject;
    glGenBuffers(1, &VertexBufferObject);
    RenderCommands.TileLayer.VertexBufferObject = VertexBufferObject;

    render_loop_arguments Args = {};
    Args.Window = Window;
    Args.AudioRingBuffer = &AudioRingBuffer;
    Args.SamplesPerSecond = SamplesPerSecond;
    Args.BytesPerSample = BytesPerSample;
    Args.GameSoundMixPanel = &GameSoundMixPanel;
    Args.GameSoundOutputBuffer = &SoundOutputBuffer;

    emscripten_set_main_loop_arg(RenderLoop, (void *)&Args, 60, 1);

    return 0;
}
