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

struct level_info
{
    tile_map tilemap;
    
    u16 startingX;
    u16 startingY;
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
        isEmpty = (tileValue == 0 || tileValue == 2);
    }
    return isEmpty;
}

internal b32 
IsWinningTile(tile_map* tilemap, f32 x, f32 y)
{
    b32 isWinningTile = false;
    
    u32 playerTileX = Truncate_F32ToI32((x - tilemap->upperLeftX) / tilemap->tileWidth);
    u32 playerTileY = Truncate_F32ToI32((y - tilemap->upperLeftY) / tilemap->tileHeight); 
    
    if(playerTileX >= 0 && playerTileX < tilemap->tileMapCountX &&
       playerTileY >= 0 && playerTileY < tilemap->tileMapCountY)
    {
        u32 tileValue = tilemap->tiles[playerTileY * tilemap->tileMapCountX + playerTileX];
        isWinningTile = (tileValue == 2);
    }
    return isWinningTile;
}

internal b32 
IsDeathTile(tile_map* tilemap, f32 x, f32 y)
{
    b32 isDeathTile = false;
    
    u32 playerTileX = Truncate_F32ToI32((x - tilemap->upperLeftX) / tilemap->tileWidth);
    u32 playerTileY = Truncate_F32ToI32((y - tilemap->upperLeftY) / tilemap->tileHeight); 
    
    if(playerTileX >= 0 && playerTileX < tilemap->tileMapCountX &&
       playerTileY >= 0 && playerTileY < tilemap->tileMapCountY)
    {
        u32 tileValue = tilemap->tiles[playerTileY * tilemap->tileMapCountX + playerTileX];
        isDeathTile = (tileValue == 3);
    }
    return isDeathTile;
}

#define TILEMAP_WIDTH 16
#define TILEMAP_HEIGHT 9
#define TILE_WIDTH 75
#define TILE_HEIGHT 75

global u32 tilemap1Data[TILEMAP_HEIGHT][TILEMAP_WIDTH] = 
{
    {1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1},
    {1, 0, 0, 0,  0, 1, 1, 0,  0, 1, 1, 0,  0, 0, 0, 1},
    {1, 0, 0, 0,  0, 1, 1, 0,  0, 0, 0, 0,  0, 0, 0, 2},
    {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 2},
    {1, 0, 0, 0,  0, 0, 0, 0,  0, 1, 1, 0,  0, 0, 0, 1},
    {1, 0, 0, 0,  0, 0, 0, 0,  0, 1, 1, 0,  0, 0, 0, 1},
    {1, 0, 0, 0,  0, 1, 1, 0,  0, 1, 1, 0,  0, 1, 0, 1},
    {1, 0, 0, 0,  0, 1, 1, 0,  0, 1, 1, 0,  0, 1, 0, 1},
    {1, 1, 1, 1,  1, 1, 1, 0,  0, 1, 1, 1,  1, 1, 1, 1},
};

global u32 tilemap2Data[TILEMAP_HEIGHT][TILEMAP_WIDTH] = 
{
    {1, 1, 1, 1,  1, 2, 2, 1,  1, 1, 1, 1,  1, 1, 1, 1},
    {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
    {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
    {1, 0, 0, 0,  0, 1, 1, 1,  0, 0, 0, 0,  0, 0, 0, 1},
    {1, 0, 0, 0,  0, 0, 0, 0,  0, 1, 1, 0,  0, 0, 0, 1},
    {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 1, 0,  0, 0, 0, 1},
    {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  1, 1, 0, 1},
    {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  1, 1, 0, 1},
    {1, 1, 1, 1,  1, 1, 1, 0,  0, 1, 1, 1,  1, 1, 1, 1},
};

global u32 tilemap3Data[TILEMAP_HEIGHT][TILEMAP_WIDTH] = 
{
    {1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1},
    {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
    {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
    {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 1, 0,  0, 0, 0, 1},
    {1, 0, 0, 0,  0, 0, 0, 1,  0, 0, 0, 0,  0, 0, 0, 1},
    {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
    {1, 0, 0, 0,  1, 1, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
    {1, 0, 0, 0,  1, 1, 0, 0,  0, 0, 0, 0,  0, 2, 0, 1},
    {1, 1, 1, 1,  1, 1, 3, 3,  3, 3, 3, 3,  3, 2, 3, 1},
};
// NOTE(winston): example tilemap
#if 0
global u32 tilemap[NUMBERHERE]Data[TILEMAP_HEIGHT][TILEMAP_WIDTH] = 
{
    {1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1},
    {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
    {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
    {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
    {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
    {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
    {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
    {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
    {1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1},
};
#endif

global u32 tilemap4Data[TILEMAP_HEIGHT][TILEMAP_WIDTH] = 
{
    {1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1},
    {2, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
    {2, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
    {1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  0, 0, 0, 1},
    {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
    {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 1, 3, 1},
    {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
    {1, 1, 1, 0,  0, 1, 0, 0,  1, 0, 0, 1,  0, 0, 0, 1},
    {1, 3, 3, 3,  3, 3, 3, 3,  3, 3, 3, 3,  3, 3, 3, 1},
};

internal level_info
GenerateLevel(u32* tilemapData,
              u16 startingX,
              u16 startingY)
{
    level_info level = {};
    
    level.tilemap.tileWidth = TILE_WIDTH;
    level.tilemap.tileHeight = TILE_HEIGHT;
    level.tilemap.upperLeftX = 0;
    level.tilemap.upperLeftY = 0;
    level.tilemap.tileMapCountX = TILEMAP_WIDTH;
    level.tilemap.tileMapCountY = TILEMAP_HEIGHT;
    level.tilemap.tiles = tilemapData;
    level.startingX = startingX;
    level.startingY = startingY;
    
    return level;
}

internal void
RestartLevel(level_info level, game_state* gameState)
{
    gameState->playerX = level.startingX;
    gameState->playerY = level.startingY;
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
        
        gameState->playerX = 0;
        gameState->playerY = 0;
        
        gameState->level = 1;
        
        memory->isInitialized = true;
    }
    
    local_persist u16 newPlayerX = gameState->playerX;
    local_persist u16 newPlayerY = gameState->playerY;
    local_persist f32 newPlayerYVel;
    local_persist b32 isGravityActive = false;
    
    local_persist level_info currentLevel = {};
    local_persist level_info level1 = GenerateLevel((u32*)tilemap1Data, 100, 400);
    local_persist level_info level2 = GenerateLevel((u32*)tilemap2Data, 100, 400);
    local_persist level_info level3 = GenerateLevel((u32*)tilemap3Data, 100, 400);
    local_persist level_info level4 = GenerateLevel((u32*)tilemap4Data, 100, 400);
    
    local_persist b32 changeLevel = true;
    
    if(changeLevel)
    {
        switch(gameState->level)
        {
            case 1:
            {
                currentLevel = level1;
                newPlayerX = currentLevel.startingX;
                newPlayerY = currentLevel.startingY;
            } break;
            case 2:
            {
                currentLevel = level2;
                newPlayerX = currentLevel.startingX;
                newPlayerY = currentLevel.startingY;
            } break;
            case 3:
            {
                currentLevel = level3;
                newPlayerX = currentLevel.startingX;
                newPlayerY = currentLevel.startingY;
            } break;
            case 4:
            {
                currentLevel = level4;
                newPlayerX = currentLevel.startingX;
                newPlayerY = currentLevel.startingY;
            } break;
            default:
            {
                char buffer[50];
                sprintf_s(buffer, 50, "Level %d not found", (i32)gameState->level);
                memory->DEBUG_PlatformWriteEntireFile("../data/error_log.txt", sizeof(buffer), buffer);
            } break;
        }
#if 0
        if(gameState->level == 1)
        {
            currentLevel = level1;
            newPlayerX = currentLevel.startingX;
            newPlayerY = currentLevel.startingY;
        }
        else if(gameState->level == 2)
        {
            currentLevel = level2;
            newPlayerX = currentLevel.startingX;
            newPlayerY = currentLevel.startingY;
        }
        else if(gameState->level == 3)
        {
            currentLevel = level3;
            newPlayerX = currentLevel.startingX;
            newPlayerY = currentLevel.startingY;
        }
#endif
        changeLevel = false;
    }
    
#define PLAYER_WIDTH 50
#define PLAYER_HEIGHT 75
    
    
    b32 canJump = (!IsTileEmpty(&currentLevel.tilemap, newPlayerX, (f32)(newPlayerY + PLAYER_HEIGHT + 4)) ||
                   !IsTileEmpty(&currentLevel.tilemap, (f32)(newPlayerX + PLAYER_WIDTH), (f32)(newPlayerY + PLAYER_HEIGHT + 4)));
    if(input->key[KEY_ESC].isDown)
    {
        RestartLevel(currentLevel, gameState);
    }
#define GRAVITY .333f
#define JUMP_POWER 12
#define PLAYER_SPEED 7
#if 1
    if(input->key[KEY_W].isDown && canJump)
    {
        newPlayerYVel = -1 * JUMP_POWER;
    }
#endif
    if(input->key[KEY_A].isDown)
    {
        newPlayerX -= PLAYER_SPEED;
    }
    if(input->key[KEY_D].isDown)
    {
        newPlayerX += PLAYER_SPEED;
    }
    if(input->key[KEY_SPACE].isDown && canJump)
    {
        newPlayerYVel = -1 * JUMP_POWER;
    }
    if(input->key[KEY_UP].isDown && canJump)
    {
        newPlayerYVel = -1 * JUMP_POWER;
    }
    if(input->key[KEY_LEFT].isDown)
    {
        newPlayerX -= PLAYER_SPEED;
    }
    if(input->key[KEY_RIGHT].isDown)
    {
        newPlayerX += PLAYER_SPEED;
    }
    
    if(input->key[KEY_RIGHT].isDown && input->key[KEY_D].isDown)
    {
        newPlayerX -= PLAYER_SPEED;
    }
    if(input->key[KEY_LEFT].isDown && input->key[KEY_A].isDown)
    {
        newPlayerX += PLAYER_SPEED;
    }
    
    if(isGravityActive)
        newPlayerYVel += GRAVITY;
    else
        newPlayerYVel = 0;
    
    for(i32 row = 0; row < 9; ++row)
    {
        for(i32 column = 0; column < 16; ++column)
        {
            //u32 tileID = tilemap.tiles[row][column];
            u32 tileID = currentLevel.tilemap.tiles[column + row * currentLevel.tilemap.tileMapCountX];
            u32 color = 0xff808080;
            
            switch(tileID)
            {
                case 1:
                {
                    color = 0xff6a0dad;
                } break;
                case 2:
                {
                    color = 0xffffff00;
                } break;
                case 3:
                {
                    color = 0xffff0000;
                } break;
            }
#if 0
            if(tileID == 1)
            {
                //color = 0xffffffff;
                color = 0xffa9ba9d;
            }
            else if(tileID == 2)
            {
                //color = 0xffffffff;
                color = 0xffffff00;
            }
            if(tileID == 2)
            {
                //color = 0xffffffff;
                color = 0xffffff00;
            }
#endif
            f32 x = 
                currentLevel.tilemap.upperLeftX + 
                (column * currentLevel.tilemap.tileWidth);
            f32 y = currentLevel.tilemap.upperLeftY + 
                (row * currentLevel.tilemap.tileWidth);
            
            RenderRectangle(backBuffer, (u16)x, (u16)y, (u16)currentLevel.tilemap.tileWidth, (u16)currentLevel.tilemap.tileHeight, color);
        }
    }
    
    newPlayerY += (u16)newPlayerYVel;
    
    if(IsTileEmpty(&currentLevel.tilemap, newPlayerX, newPlayerY) &&
       IsTileEmpty(&currentLevel.tilemap, (f32)(newPlayerX + PLAYER_WIDTH), newPlayerY) &&
       IsTileEmpty(&currentLevel.tilemap, newPlayerX, (f32)(newPlayerY + PLAYER_HEIGHT - 1)) &&
       IsTileEmpty(&currentLevel.tilemap, (f32)(newPlayerX + PLAYER_WIDTH), (f32)(newPlayerY + PLAYER_HEIGHT - 1)))
    {
        gameState->playerX = newPlayerX;
        gameState->playerY = newPlayerY;
        
        if((newPlayerY + PLAYER_HEIGHT > backBuffer->height - 50))
        {
            RestartLevel(currentLevel, gameState);
        }
        isGravityActive = true;
    }
    else
    {
        isGravityActive = false;
    }
    
    if(IsDeathTile(&currentLevel.tilemap, newPlayerX, newPlayerY) ||
       IsDeathTile(&currentLevel.tilemap, (f32)(newPlayerX + PLAYER_WIDTH - 3), newPlayerY) ||
       IsDeathTile(&currentLevel.tilemap, newPlayerX, (f32)(newPlayerY + PLAYER_HEIGHT + 3)) ||
       IsDeathTile(&currentLevel.tilemap, (f32)(newPlayerX + PLAYER_WIDTH - 3), (f32)(newPlayerY + PLAYER_HEIGHT) + 3))
    {
        RestartLevel(currentLevel, gameState);
    }
    else if(IsWinningTile(&currentLevel.tilemap, gameState->playerX, gameState->playerY))
    {
        ++gameState->level;
        changeLevel = true;
    }
    
    newPlayerX = gameState->playerX;
    newPlayerY = gameState->playerY;
    
    // NOTE(winston): Player Render
    RenderRectangle(backBuffer, 
                    gameState->playerX, gameState->playerY, 
                    PLAYER_WIDTH, PLAYER_HEIGHT, 0xff0000ff);
}
