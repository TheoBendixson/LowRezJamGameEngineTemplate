
PLATFORM_READ_PNG_FILE(PlatformReadPNGFile)
{
    char LocalFilename[200];

    for (u32 Index = 0;
         Index < 200;
         Index++)
    {
        LocalFilename[Index] = '\0';
    }

    LocalFilename[0] = 'r';
    LocalFilename[1] = 'e';
    LocalFilename[2] = 's';
    LocalFilename[3] = 'o';
    LocalFilename[4] = 'u';
    LocalFilename[5] = 'r';
    LocalFilename[6] = 'c';
    LocalFilename[7] = 'e';
    LocalFilename[8] = 's';
    LocalFilename[9] = '/';

    char *scan = Filename;

    u32 CharacterIndex = 10;

    while(*scan != '\0')
    {
        LocalFilename[CharacterIndex] = *scan++;
        CharacterIndex++;
    }

    read_file_result Result = {};

    int X,Y,N;
    u32 *ImageData = (u32 *)stbi_load(LocalFilename, &X, &Y, &N, 0);

    if (X > 0 && Y > 0 && ImageData != NULL)
    {
        Result.Contents = ImageData;
        Result.ContentsSize = X*Y*sizeof(u32); 
    }

    return Result;
}

PLATFORM_READ_ENTIRE_FILE(PlatformReadEntireFile) 
{
    read_file_result Result = {};

    char LocalFilename[200];

    for (u32 Index = 0;
         Index < 200;
         Index++)
    {
        LocalFilename[Index] = '\0';
    }

    LocalFilename[0] = 'r';
    LocalFilename[1] = 'e';
    LocalFilename[2] = 's';
    LocalFilename[3] = 'o';
    LocalFilename[4] = 'u';
    LocalFilename[5] = 'r';
    LocalFilename[6] = 'c';
    LocalFilename[7] = 'e';
    LocalFilename[8] = 's';
    LocalFilename[9] = '/';

    char *scan = Filename;

    u32 CharacterIndex = 10;

    while(*scan != '\0')
    {
        LocalFilename[CharacterIndex] = *scan++;
        CharacterIndex++;
    }

    printf("Reading File At: %s\n", LocalFilename);

    FILE *FileHandle = fopen(LocalFilename, "r+");
    u8 *FileData;

    if (FileHandle != nullptr)
    {
		fseek(FileHandle, 0, SEEK_END);
		u64 FileSize = ftell(FileHandle);

        if(FileSize)
        {
        	rewind(FileHandle);
            FileData = (u8 *)malloc(FileSize);

            if (FileData)
            {
                u64 BytesRead = fread(FileData, 1, FileSize, FileHandle);

                if (FileSize == BytesRead)
                {
                    printf("File Read Successfully \n");
                    Result.Contents = (void *)FileData;
                    Result.ContentsSize = FileSize;

                    Result.Filename = (char *)malloc(200*sizeof(char));

                    char *Dest = Result.Filename;
                    char *Scan = Filename;

                    while (*Scan != '\0')
                    {
                        *Dest++ = *Scan++;
                    }

                    *Dest++ = '\0';

                } else
                {
                    printf("File Not Read \n");
                    Result.Contents = nullptr;
                    Result.ContentsSize = 0;
                }
            }
        } else
        {
            printf("Can't open file \n");
            Result.Contents = nullptr;
            Result.ContentsSize = 0;
        }

    } else
    {
        printf("Can't open file \n");
        Result.Contents = nullptr;
        Result.ContentsSize = 0;
    }

    fclose(FileHandle);
    return Result;
}

PLATFORM_FREE_FILE_MEMORY(PlatformFreeFileMemory) 
{
    if (Memory) {
        free(Memory);
    }
}
