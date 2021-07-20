
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
