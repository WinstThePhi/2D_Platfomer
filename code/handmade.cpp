// (255,165,0)

#include "handmade.h"

void
RenderGradient(game_back_buffer* backBuffer,
               u16 xOffset, u16 yOffset)
{
    u8* row = (u8*)backBuffer->bitmapMemory;
    
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
        
        memory->isInitialized = true;
    }
    
    if((input->keyboard).moveLeft.endedDown)
    {
        --gameState->xOffset;
    }
    else if((input->keyboard).moveRight.endedDown)
    {
        ++gameState->xOffset;
    }
    else if((input->keyboard).moveUp.endedDown)
    {
        --gameState->yOffset;
    }
    else if((input->keyboard).moveDown.endedDown)
    {
        ++gameState->yOffset;
    }
    RenderGradient(backBuffer, gameState->xOffset, gameState->yOffset);
    //RenderGradient(backBuffer, x, y);
    char gayBoi[] = "mama no play the tien len";
    memory->DEBUG_PlatformWriteEntireFile("../data/output.txt",
                                          ArrayCount(gayBoi),
                                          gayBoi);
    //++gameState->xOffset;
    //++gameState->yOffset;
}
