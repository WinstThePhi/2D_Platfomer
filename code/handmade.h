#ifndef HANDMADE_H
#define HANDMADE_H

typedef struct game_back_buffer
{
    u16 width;
    u16 height;
    void* bitmapMemory;
    u32 bytesPerPixel;
    u32 pitch;
} game_back_buffer;

internal void 
GameUpdateAndRender(game_back_buffer* backBuffer);

#endif
