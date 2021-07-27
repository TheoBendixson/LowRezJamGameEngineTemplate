
struct wav_file_header
{
    char Riff[4];
    u32 FileSize;
    char Wave[4];
    char FMTChunkMarker[4];
    u32 FMTLength;
    u16 FormatType;
    u16 Channels;
    u32 SampleRate;
    u32 ByteRate;
    u16 BlockAlign;
    u16 BitsPerSample;
    char DataChunkHeader[4];
    u32 DataSize;
};

struct game_sound_input_buffer
{
    u32 SampleCount;
    u32 CurrentIndex;
    s16 *Samples;
    b8 IsPlaying;
};

struct game_sound_mix_panel
{
    game_sound_input_buffer BackgroundMusic;
};

struct game_sound_output_buffer 
{
    u32 SamplesPerSecond;
    u32 SamplesToWriteThisFrame;
    u32 SamplesWrittenThisFrame;
    s16 *Samples;
};
