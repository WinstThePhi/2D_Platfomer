// (255,165,0)

#include "handmade.h"
#include <math.h>

#if 0
internal void
RenderGradient(game_back_buffer* backBuffer,
               u16 xOffset, u16 yOffset)
{
    u8* row = (u8*)backBuffer->memory;
    
    for(u16 y = 0; y < backBuffer->height; ++y)
    {
        u32* pixel = (u32*)row;
        for(u16 x = 0; x < backBuffer->width; ++x)
        {
            u8 red = 0;
            u8 green = ((u8)x + (u8)xOffset);
            u8 blue = ((u8)y + (u8)yOffset);
            *pixel++ = red << 16 | green << 8 | blue;
        }
        row += backBuffer->pitch;
    }
}
#endif
#if 0
internal void
RenderBackground(game_back_buffer* backBuffer,
                 v3 color)
{
    u8* row = (u8*)backBuffer->memory;
    
    for(u16 y = 0; y < backBuffer->height; ++y)
    {
        u32* pixel = (u32*)row;
        for(u16 x = 0; x < backBuffer->width; ++x)
        {
            u8 red = color.r;
            u8 green = color.g;
            u8 blue = color.b;
            *pixel++ = red << 16 | green << 8 | blue;
        }
        row += backBuffer->pitch;
    }
}
#endif
#if 0
internal void
RenderPlayer(game_back_buffer* backBuffer,
             u16 playerX, u16 playerY)
{
    u32 color = 0xffffffff;
    i32 top = playerY;
    i32 bottom = playerY + 25;
    for(i32 x = playerX; x < playerX + 25; ++x)
    {
        u8* pixel = ((u8*)backBuffer->memory +
                     x*backBuffer->bytesPerPixel + top*backBuffer->pitch);
        for(i32 y = top; y < bottom; ++y)
        {
            *(u32*)pixel = color;
            pixel += backBuffer->pitch;
        }
    }
}
#endif

internal void 
RenderRectangle(game_back_buffer* backBuffer,
                u16 x, u16 y,
                u16 width, u16 height,
                u32 color)
{
    u8* row = ((u8*)backBuffer->memory +
               x*backBuffer->bytesPerPixel + y*backBuffer->pitch);
    
    u16 minX = x;
    u16 minY = y;
    u16 maxY = y + height;
    u16 maxX = x + width;
    
    if(minX < 0)
        minX = 0;
    if(minY < 0)
        minY = 0;
    if(maxX > backBuffer->width)
        maxX = backBuffer->width;
    if(maxY > backBuffer->height)
        maxY = backBuffer->height;
    
    for(i32 yIndex = minY; yIndex < maxY; ++yIndex)
    {
        u32* pixel = (u32*)row;
        for(i32 xIndex = minX; xIndex < maxX; ++xIndex)
        {
            *pixel++ = color;
        }
        row += backBuffer->pitch;
    }
}

struct tile_map
{
    u32 tileMapCountX;
    u32 tileMapCountY;
    
    f32 upperLeftX;
    f32 upperLeftY;
    f32 tileWidth;
    f32 tileHeight;
    
    u32* tiles;
};

inline i32
Truncate_F32ToI32(f32 number)
{
    return (i32)number;
}

internal b32 
IsTileEmpty(tile_map* tilemap, f32 x, f32 y)
{
    b32 isEmpty = false;
    
    u32 playerTileX = Truncate_F32ToI32((x - tilemap->upperLeftX) / tilemap->tileWidth);
    u32 playerTileY = Truncate_F32ToI32((y - tilemap->upperLeftY) / tilemap->tileHeight); 
    
    if(playerTileX >= 0 && playerTileX < tilemap->tileMapCountX &&
       playerTileY >= 0 && playerTileY < tilemap->tileMapCountY)
    {
        u32 tileValue = tilemap->tiles[playerTileY * tilemap->tileMapCountX + playerTileX];
        isEmpty = (tileValue == 0);
    }
    return isEmpty;
}

extern "C" 
GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    Assert(sizeof(game_state) <= memory->permanentStorageSize);
    
    game_state* gameState = (game_state*)memory->permanentStorage;
    
    if(!memory->isInitialized)
    {
        DEBUG_file_read_result fileInfo =
            memory->DEBUG_PlatformReadEntireFile(__FILE__);
        if(fileInfo.contents)
        {
            memory->DEBUG_PlatformWriteEntireFile("../data/output.txt",
                                                  fileInfo.contentSize,
                                                  fileInfo.contents);
            memory->DEBUG_PlatformFreeFileMemory(fileInfo.contents);
        }
        gameState->xOffset = 0;
        gameState->yOffset = 0;
        
        gameState->yVel = 0;
        
        gameState->playerX = 100;
        gameState->playerY = 100;
        
        memory->isInitialized = true;
    }
    
    tile_map tilemap;
    
#define TILEMAP_WIDTH 16
#define TILEMAP_HEIGHT 9
    
    u32 tilemapData[TILEMAP_HEIGHT][TILEMAP_WIDTH] = 
    {
        {1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1},
        {1, 0, 0, 0,  0, 1, 1, 0,  0, 1, 1, 0,  0, 0, 0, 1},
        {1, 0, 0, 0,  0, 1, 1, 0,  0, 0, 0, 0,  0, 0, 0, 0},
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0},
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 1, 1, 0,  0, 0, 0, 1},
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 1, 1, 0,  0, 0, 0, 1},
        {1, 0, 0, 0,  0, 1, 1, 0,  0, 1, 1, 0,  0, 1, 0, 1},
        {1, 0, 0, 0,  0, 1, 1, 0,  0, 1, 1, 0,  0, 1, 0, 1},
        {1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1},
    };
    
    tilemap.tileWidth = 75;
    tilemap.tileHeight = 75;
    tilemap.upperLeftX = 0;
    tilemap.upperLeftY = 0;
    tilemap.tileMapCountX = TILEMAP_WIDTH;
    tilemap.tileMapCountY = TILEMAP_HEIGHT;
    tilemap.tiles = (u32*)tilemapData;
    
#define PLAYER_WIDTH 50
#define PLAYER_HEIGHT 75
    
    local_persist u16 newPlayerX = gameState->playerX;
    local_persist u16 newPlayerY = gameState->playerY;
    local_persist f32 newPlayerYVel;
    local_persist b32 isGravityActive = false;
    b32 canJump = (!IsTileEmpty(&tilemap, newPlayerX, (f32)(newPlayerY + PLAYER_HEIGHT + 5)) ||
                   !IsTileEmpty(&tilemap, (f32)(newPlayerX + PLAYER_WIDTH), (f32)(newPlayerY + PLAYER_HEIGHT + 5)));
    
#define GRAVITY .163f
#define JUMP_POWER 8
#if 1
    if(input->key[KEY_W].isDown && canJump)
    {
        newPlayerYVel = -1 * JUMP_POWER;
    }
#endif
    if(input->key[KEY_A].isDown)
    {
        newPlayerX -= 4;
    }
    if(input->key[KEY_D].isDown)
    {
        newPlayerX += 4;
    }
#if 0
    if(input->key[KEY_S].isDown)
    {
        newPlayerY += 4;
    }
#endif
    if(input->key[KEY_SPACE].isDown && canJump)
    {
        newPlayerYVel = -1 * JUMP_POWER;
    }
    
    if(isGravityActive)
        newPlayerYVel += GRAVITY;
    else
        newPlayerYVel = 0;
    
    for(i32 row = 0; row < 9; ++row)
    {
        for(i32 column = 0; column < 16; ++column)
        {
            u32 tileID = tilemapData[row][column];
            u32 color = 0xff808080;
            
            if(tileID)
            {
                //color = 0xffffffff;
                color = 0xffa9ba9d;
            }
            
            f32 x = tilemap.upperLeftX + (column * tilemap.tileWidth);
            f32 y = tilemap.upperLeftY + (row * tilemap.tileWidth);
            
            RenderRectangle(backBuffer, (u16)x, (u16)y, (u16)tilemap.tileWidth, (u16)tilemap.tileHeight, color);
        }
    }
    
    newPlayerY += (u16)newPlayerYVel;
    
    if(IsTileEmpty(&tilemap, newPlayerX, newPlayerY) &&
       IsTileEmpty(&tilemap, (f32)(newPlayerX + PLAYER_WIDTH - 1), newPlayerY) &&
       IsTileEmpty(&tilemap, newPlayerX, (f32)(newPlayerY + PLAYER_HEIGHT - 1)) &&
       IsTileEmpty(&tilemap, (f32)(newPlayerX + PLAYER_WIDTH), (f32)(newPlayerY + PLAYER_HEIGHT - 1)))
    {
        gameState->playerX = newPlayerX;
        gameState->playerY = newPlayerY;
        isGravityActive = true;
    }
    else
    {
        isGravityActive = false;
    }
    
    newPlayerX = gameState->playerX;
    newPlayerY = gameState->playerY;
    
    // NOTE(winston): Player Render
    RenderRectangle(backBuffer, 
                    gameState->playerX, gameState->playerY, 
                    PLAYER_WIDTH, PLAYER_HEIGHT, 0xffffff00);
}
