#ifndef WIN32_MAIN_H
#define WIN32_MAIN_H

typedef struct client_dimension
{
    u16 width;
    u16 height;
} client_dimension;

typedef struct back_buffer
{
    u16 width;
    u16 height;
    void* bitmapMemory;
    BITMAPINFO bitmapInfo;
    u32 bytesPerPixel;
    u32 pitch;
} back_buffer;

typedef struct game_code
{
    HMODULE gameCode_DLL;
    game_update_and_render* UpdateAndRender;
    b32 isValid;
} game_code;

#endif
