#ifndef HANDMADE_H
#define HANDMADE_H

#define Kilobytes(value) ((value) * 1024)
#define Megabytes(value) (Kilobytes(value) * 1024)
#define Gigabytes(value) (Megabytes(value) * 1024)

#define ArrayCount(array) (sizeof(array) / sizeof(array[0]))

typedef struct game_back_buffer
{
    u16 width;
    u16 height;
    void* bitmapMemory;
    u32 bytesPerPixel;
    u32 pitch;
} game_back_buffer;

typedef struct game_button_state
{
    i32 halfTransition;
    b32 endedDown;
} game_button_state;

typedef struct game_keyboard_input
{
    union
    {
	game_button_state buttons[6];
	struct
	{
	    game_button_state up;
	    game_button_state down;
	    game_button_state left;
	    game_button_state right;
	};
    };
} game_keyboard_input;

typedef struct game_input
{
    game_keyboard_input keyboard;
} game_input;

typedef struct game_memory
{
    b32 isInitialized;

    u64 permanentStorageSize;
    void* permanentStorage;
} game_memory;

typedef struct game_state
{
    u16 xOffset;
    u16 yOffset;
} game_state;

internal void 
GameUpdateAndRender(game_back_buffer* backBuffer);

#endif
