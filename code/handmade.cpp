// (255,165,0)
internal void
RenderGradient(game_back_buffer* backBuffer,
               u16 xOffset, u16 yOffset)
{
    u8* row = (u8*)backBuffer->bitmapMemory;

    for(u16 y = 0; y < backBuffer->height; ++y)
    {
        u32* pixel = (u32*)row;
        for(u16 x = 0; x < backBuffer->width; ++x)
        {
	    u8 red = 255;
            u8 green = 165;
            u8 blue = 0;
            *pixel++ = red << 16 | green << 8 | blue;
        }
	row += backBuffer->pitch;
    }
}

internal void 
GameUpdateAndRender(game_memory* memory,
		    game_back_buffer* backBuffer)
{
    game_state* gameState = (game_state*)memory->permanentStorage;

    if (!memory->isInitialized)
    {
	gameState->xOffset = 0;
	gameState->yOffset = 0;

	memory->isInitialized = 1;
    }

    RenderGradient(backBuffer,
                   gameState->xOffset, gameState->yOffset);

    ++gameState->xOffset;
    ++gameState->yOffset;
}
