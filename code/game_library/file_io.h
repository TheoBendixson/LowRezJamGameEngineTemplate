
struct read_file_result 
{
    void *Contents;
    u64 ContentsSize;
    char *Filename;
};

#define PLATFORM_READ_ENTIRE_FILE(name) read_file_result name(char *Filename)
typedef PLATFORM_READ_ENTIRE_FILE(platform_read_entire_file);

#define PLATFORM_READ_PNG_FILE(name) read_file_result name(char *Filename)
typedef PLATFORM_READ_PNG_FILE(platform_read_png_file);

#define PLATFORM_FREE_FILE_MEMORY(name) void name(void *Memory)
typedef PLATFORM_FREE_FILE_MEMORY(platform_free_file_memory);

#define PLATFORM_LOG_MESSAGE(name) void name(char *Message, u32 Count) 
typedef PLATFORM_LOG_MESSAGE(platform_log_message);
