#ifndef HANDMADE_H
#define HANDMADE_H

//~
/* NOTE(winston): 
HANDMADE_INTERNAL = 1 for personal builds
HANDMADE_INTERNAL = 0 for public builds
  
HANDMADE_SLOW = 1: slow code
HANDMADE_SLOW = 0: faster code

*/

//~

#include <stdio.h>
#include <assert.h>

#include "layer.h"

#if HANDMADE_INTERNAL

typedef struct DEBUG_file_read_result
{
    u32 contentSize;
    void* contents;
} DEBUG_file_read_result;

#define FILE_EXISTS(name) \
b32 name(char* filename)
typedef FILE_EXISTS(file_exists);

#define DEBUG_PLATFORM_READ_ENTIRE_FILE(name) DEBUG_file_read_result \
name(char* filename)
typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(debug_platform_read_entire_file);

#define DEBUG_PLATFORM_FREE_FILE_MEMORY(name) \
void name(void* fileMemory)
typedef DEBUG_PLATFORM_FREE_FILE_MEMORY(debug_platform_free_file_memory);

#define DEBUG_PLATFORM_WRITE_ENTIRE_FILE(name) \
b32 name(char* filename, u32 size, void* memory)
typedef DEBUG_PLATFORM_WRITE_ENTIRE_FILE(debug_platform_write_entire_file);

#endif

#if HANDMADE_SLOW 
#define Assert(expression) assert(expression) 
#else
#define Assert(expression)
#endif

#define Kilobytes(value) ((value) * 1024)
#define Megabytes(value) (Kilobytes(value) * 1024)
#define Gigabytes(value) (Megabytes(value) * 1024)

#define ArrayCount(array) (sizeof(array) / sizeof(array[0]))

inline u32
SafeTruncate_u64(u64 value)
{
    Assert(value <= 0xFFFFFF);
    u32 result = (u32)value;
    return result;
}

typedef struct game_back_buffer
{
    u16 width;
    u16 height;
    void* memory;
    u32 bytesPerPixel;
    u32 pitch;
} game_back_buffer;

typedef struct game_button_state
{
    i32 halfTransitionCount;
    b32 endedDown;
} game_button_state;

typedef struct game_keyboard_input
{
    union
    {
        game_button_state buttons[11];
        struct
        {
            game_button_state moveUp;
            game_button_state moveDown;
            game_button_state moveLeft;
            game_button_state moveRight;
            
            game_button_state actionUp;
            game_button_state actionDown;
            game_button_state actionLeft;
            game_button_state actionRight;
            
            game_button_state leftShoulder;
            game_button_state rightShoulder;
            
            game_button_state jump;
        };
    };
} game_keyboard_input;

typedef struct game_input
{
    // TODO(winston): sum timing info passed?
    game_keyboard_input keyboard;
} game_input;

typedef struct game_memory
{
    b32 isInitialized;
    
    u64 permanentStorageSize;
    void* permanentStorage;
    
    u64 transientStorageSize;
    void* transientStorage;
    
    debug_platform_read_entire_file* DEBUG_PlatformReadEntireFile;
    debug_platform_write_entire_file* DEBUG_PlatformWriteEntireFile;
    debug_platform_free_file_memory* DEBUG_PlatformFreeFileMemory;
} game_memory;

typedef struct game_state
{
    u16 xOffset;
    u16 yOffset;
    
    u16 playerX;
    u16 playerY;
    
    f32 yVel;
    
    u16 level;
} game_state;

typedef enum keyboard_keys
{
    KEY_W,
    KEY_A,
    KEY_S,
    KEY_D,
    KEY_Q,
    KEY_E,
    KEY_UP,
    KEY_LEFT,
    KEY_DOWN,
    KEY_RIGHT,
    KEY_ESC,
    KEY_SPACE
} keyboard_keys;

typedef struct key_state
{
    b32 isDown;
    u32 pressCount;
} key_state;

typedef struct keyboard_input
{
    key_state key[12];
} keyboard_input;

inline game_keyboard_input* 
GetController(game_input* input, u8 controllerIndex)
{
    Assert(controllerIndex < 1);
    game_keyboard_input* result = &(input->keyboard);
    return result;
}

typedef struct client_dimension
{
    u16 width;
    u16 height;
} client_dimension;

#define GAME_UPDATE_AND_RENDER(name) \
void name(game_memory* memory, keyboard_input* input, game_back_buffer* backBuffer, client_dimension dimension)

typedef GAME_UPDATE_AND_RENDER(game_update_and_render);

GAME_UPDATE_AND_RENDER(GameUpdateAndRenderStub)
{
}

extern "C" 
GAME_UPDATE_AND_RENDER(GameUpdateAndRender);

typedef union v3
{
    struct
    {
        u8 r;
        u8 g;
        u8 b;
    };
} v3;

#endif
