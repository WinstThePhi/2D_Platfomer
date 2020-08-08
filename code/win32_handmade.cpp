#include <windows.h>
#include <stdio.h>

#include "layer.h"
#include "win32_handmade.h"
#include "handmade.h"

#include "handmade.cpp"

#define pi32 3.14159265359f

global b8 isRunning;
global back_buffer backBuffer;
 
internal client_dimension
GetClientDimension(HWND window)
{
    RECT clientRect;
    GetClientRect(window, &clientRect);
    
    client_dimension dimensions;

    dimensions.height = clientRect.bottom - clientRect.top;
    dimensions.width = clientRect.right - clientRect.left;
    
    return dimensions;
}

internal void
Win32InitBackBuffer(back_buffer* backBuffer,
                    u16 width, u16 height)
{
    backBuffer->bytesPerPixel = 4;
     
    backBuffer->width = width;
    backBuffer->height = height;
    
    backBuffer->bitmapInfo.bmiHeader.biSize = sizeof(backBuffer->bitmapInfo.bmiHeader);
    backBuffer->bitmapInfo.bmiHeader.biWidth = width;
    backBuffer->bitmapInfo.bmiHeader.biHeight = -height;
    backBuffer->bitmapInfo.bmiHeader.biPlanes = 1;
    backBuffer->bitmapInfo.bmiHeader.biBitCount = 32;
    backBuffer->bitmapInfo.bmiHeader.biCompression = BI_RGB;
    
    u32 sizeToAlloc = (height * width) * backBuffer->bytesPerPixel;
	
    backBuffer->pitch = width * backBuffer->bytesPerPixel;
    backBuffer->bitmapMemory = VirtualAlloc(0, sizeToAlloc, MEM_COMMIT, PAGE_READWRITE);
}

internal void
Win32UpdateWindow(HDC deviceContext,
                  back_buffer* backBuffer,
                  u16 width, u16 height)
{
    StretchDIBits(deviceContext,
                  0, 0, width, height,
                  0, 0, backBuffer->width, backBuffer->height,
                  backBuffer->bitmapMemory,
                  &backBuffer->bitmapInfo,
                  DIB_RGB_COLORS,
                  SRCCOPY);
}

internal LRESULT CALLBACK
Win32MainWindowCallback(HWND window, 
                        UINT message,
                        WPARAM wParam, 
                        LPARAM lParam)
{
    LRESULT result = {};
    switch(message)
    {
        case WM_CLOSE:
        {
            isRunning = 0;
        } break;
        case WM_PAINT:
        {
            PAINTSTRUCT paintStruct;
            HDC hdc = BeginPaint(window, &paintStruct);
	    
            client_dimension dimension = GetClientDimension(window);
            Win32UpdateWindow(hdc,
                              &backBuffer,
                              dimension.width, dimension.height);
            EndPaint(window, &paintStruct);
        } break;
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            u32 VKcode = wParam;
            bool wasDown = ((lParam & (1 << 30)) != 0);
            bool isDown = ((lParam & (1 << 31)) == 0);
            if (isDown != wasDown)
            {
                if (VKcode == 'W')
                {

                }
                else if (VKcode == 'A')
                {

                }
                else if (VKcode == 'S')
                {

                }
                else if (VKcode == 'D')
                {

                }
                else if (VKcode == 'Q')
                {
                    
                }
                else if (VKcode == VK_UP)
                {
                    
                }
                else if (VKcode == VK_LEFT)
                {
                    
                }
                else if (VKcode == VK_DOWN)
                {
                    
                }
                else if (VKcode == VK_RIGHT)
                {
                    
                }
                else if (VKcode == VK_ESCAPE)
                {
                    if (isDown)
                    {
                        OutputDebugStringA("Escape key is down!\n");
                    }
                    else if (wasDown)
                    {
                        OutputDebugStringA("Escape key was down!\n");
                    }
                }
                else if (VKcode == VK_SPACE)
                {
                    
                }
            }

            b32 altKeyDown = (lParam & (1 << 29));
            if (VKcode == VK_F4 && altKeyDown)
            {
                isRunning = 0;
            }
        } break;
        default:
        {
            result = DefWindowProc(window, message, wParam, lParam);
        }
    }
    return result;
}

int WinMain(HINSTANCE hInstance,
            HINSTANCE prevInstance,
            LPSTR lpCmdLine,
            int nShowCmd)
{
    LARGE_INTEGER performanceFrequencyResult;
    QueryPerformanceFrequency(&performanceFrequencyResult);
    i64 performanceFrequency = performanceFrequencyResult.QuadPart;
    
    Win32InitBackBuffer(&backBuffer,
                        1280, 720);
    
    WNDCLASSA windowClass = {};
    
    windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = Win32MainWindowCallback;
    windowClass.hInstance = hInstance;
    windowClass.lpszClassName = "HandmadeHero";
    windowClass.hCursor = LoadCursor(0, IDC_ARROW);
    
    if (RegisterClassA(&windowClass))
    {
        HWND window = CreateWindowEx(0,
                                     "HandmadeHero",
                                     "Handmade Hero",
                                     WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     1280,
                                     720,
                                     0,
                                     0,
                                     hInstance,
                                     0);
        if (window)
        {
            isRunning = 1;
            
            LARGE_INTEGER lastCounter;
            QueryPerformanceCounter(&lastCounter);
            
            game_back_buffer gameBackBuffer = {};
            gameBackBuffer.bitmapMemory = backBuffer.bitmapMemory;
            gameBackBuffer.bytesPerPixel = backBuffer.bytesPerPixel;
            gameBackBuffer.pitch = backBuffer.pitch;
            gameBackBuffer.width = backBuffer.width;
            gameBackBuffer.height = backBuffer.height;

	    game_memory memory = {};
	    memory.permanentStorageSize = Megabytes(64);
	    memory.permanentStorage =
		VirtualAlloc(0,
			     memory.permanentStorageSize,
			     MEM_RESERVE | MEM_COMMIT,
			     PAGE_READWRITE);

	    if (memory.permanentStorage)
	    {
		while (isRunning)
		{
		    MSG message;
		    while (PeekMessage(&message, window, 0, 0, PM_REMOVE))
		    {
			if (message.message == WM_QUIT)
			{
			    isRunning = 0;
			}
			TranslateMessage(&message);
			DispatchMessage(&message);
		    }
                
		    GameUpdateAndRender(&memory,
					&gameBackBuffer);
                
		    HDC hdc = GetDC(window);
		    client_dimension dimension = GetClientDimension(window);
                
		    Win32UpdateWindow(hdc,
				      &backBuffer,
				      dimension.width, dimension.height);
                
		    ReleaseDC(window, hdc);
                
		    LARGE_INTEGER endCounter;
		    QueryPerformanceCounter(&endCounter);
                
		    i64 timeElapsed = endCounter.QuadPart - lastCounter.QuadPart;
                
		    f32 msPerFrame = (1000.0f * (f32)timeElapsed) / (f32)performanceFrequency;
                
#if 0
		    char buffer[256];
		    sprintf(buffer, "Milliseconds/frame %f ms\n", msPerFrame);
                
		    OutputDebugStringA(buffer);
#endif
		    if (msPerFrame < 16.667)
			Sleep(16.667 - msPerFrame);
                
		    lastCounter = endCounter;
		}
            }
            DestroyWindow(window);
        }
        else
        {
            OutputDebugStringA("Failed to create window\n");
            // TODO(winston): logging
        }
    }
    else
    {
        OutputDebugStringA("Failed to register window class\n");
        // TODO(winston): logging
    }
    return 0;
}
