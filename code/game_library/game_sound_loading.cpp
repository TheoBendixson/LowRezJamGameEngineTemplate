
inline
u32 GetSampleCount(u32 SoundDataSize)
{
    return SoundDataSize/sizeof(s16);
}

internal void
InitializeAndFillSoundInputBuffer(memory_arena *SoundsArena, game_sound_input_buffer *SoundInputBuffer,
                                  s16 *SourceData, u32 SampleCount)
{
    SoundInputBuffer->IsPlaying = false;
    SoundInputBuffer->CurrentIndex = 0;
    SoundInputBuffer->SampleCount = SampleCount;
    SoundInputBuffer->Samples = PushArray(SoundsArena, SoundInputBuffer->SampleCount, s16);

    s16 *Dest = SoundInputBuffer->Samples;

    for (u32 SampleIndex = 0; SampleIndex < SoundInputBuffer->SampleCount; SampleIndex++)
    {
        *Dest++ = *SourceData++;
    }

}

internal void
LoadSounds(game_memory *Memory, game_sound_mix_panel *GameSoundMixPanel)
{
    game_state *GameState = (game_state *)Memory->PermanentStorage;
    memory_arena *SoundsArena = &GameState->SoundsArena;
    InitializeArena(SoundsArena, Memory->SoundPartitionSize, (u8*)Memory->SoundsPartition);

    read_file_result BackgroundMusicFile = PlatformReadEntireFile("background_music.wav");

    if (BackgroundMusicFile.ContentsSize > 0)
    {
        PlatformLogMessage("Loaded Background Music \n");

        wav_file_header *WaveHeader = (wav_file_header *)BackgroundMusicFile.Contents;
        s16 *SoundSource = (s16*)((u8*)BackgroundMusicFile.Contents + sizeof(wav_file_header));
        u32 SampleCount = GetSampleCount(WaveHeader->DataSize);
        InitializeAndFillSoundInputBuffer(SoundsArena, &GameSoundMixPanel->BackgroundMusic,
                                          SoundSource, SampleCount);

        PlatformLogMessage("Setup Sound Input Buffer \n");
    }

    PlatformFreeMemory(BackgroundMusicFile.Contents);
   
    read_file_result SoundEffectFile = PlatformReadEntireFile("jump.wav");

    if (SoundEffectFile.ContentsSize > 0)
    {
        wav_file_header *WaveHeader = (wav_file_header *)SoundEffectFile.Contents;
        s16 *SoundSource = (s16*)((u8*)SoundEffectFile.Contents + sizeof(wav_file_header));
        u32 SampleCount = GetSampleCount(WaveHeader->DataSize);
        InitializeAndFillSoundInputBuffer(SoundsArena, &GameSoundMixPanel->Jump,
                                          SoundSource, SampleCount);

    }

    PlatformFreeMemory(SoundEffectFile.Contents);
}
