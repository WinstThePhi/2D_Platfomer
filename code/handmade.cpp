
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
            u8 green = y + yOffset;
            u8 blue = x + xOffset;
            *pixel++ = green << 8 | blue;
        }
		row += backBuffer->pitch;
    }
}

internal void 
GameUpdateAndRender(game_back_buffer* backBuffer)
{
    local_persist u16 xOffset;
    local_persist u16 yOffset;
    
    RenderGradient(backBuffer,
                   xOffset, yOffset);
	
    ++xOffset;
    ++yOffset;
}
