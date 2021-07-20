
struct game_memory 
{
    b32 IsInitialized;

    u64 PermanentStorageSize;
    void *PermanentStorage;

    void *SoundsPartition;
    u64 SoundPartitionSize;

    u64 TransientStorageSize;
    void *TransientStorage;
};
