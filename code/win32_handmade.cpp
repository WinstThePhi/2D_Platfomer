#include <windows.h>
#include "handmade.h"

#include "win32_handmade.h"

#include "handmade.h"

#define pi32 3.14159265359f

global b8 isRunning;
global back_buffer globalBackBuffer;

inline FILETIME
Win32GetLastWriteTime(char* filename)
{
    FILETIME lastWriteTime  = {0};
    
    WIN32_FIND_DATA findData; 
    HANDLE fileHandle = FindFirstFileA(filename, &findData);
    
    if(fileHandle != INVALID_HANDLE_VALUE)
    {
        lastWriteTime = findData.ftLastWriteTime;
        FindClose(fileHandle);
    }
    
    return lastWriteTime;
}

internal game_code
Win32LoadGameCode(char* filename)
{
    game_code result = {0};
    char* tempDLLName = "handmade_temp.dll";
    
    result.lastWriteTime = Win32GetLastWriteTime(filename);
    
    CopyFile(filename, tempDLLName, FALSE);
    result.gameCode_DLL = LoadLibraryA(tempDLLName);
    if(result.gameCode_DLL)
    {
        result.UpdateAndRender = 
            (game_update_and_render*)GetProcAddress(result.gameCode_DLL, 
                                                    "GameUpdateAndRender");
        result.isValid = {result.UpdateAndRender != 0};
    }
    if(!result.isValid)
    {
        result.UpdateAndRender = GameUpdateAndRenderStub;
    }
    
    return result;
}

internal void
Win32UnloadGameCode(game_code* gameCode)
{
    if(gameCode->gameCode_DLL)
    {
        FreeLibrary(gameCode->gameCode_DLL);
    }
    gameCode->isValid = 0;
    gameCode->UpdateAndRender = GameUpdateAndRenderStub;
}

#if HANDMADE_INTERNAL
b32 
FileExists(char* filename)
{
    DWORD fileAttributes = GetFileAttributes(filename);
    if(fileAttributes == 0xFFFFFFFF)
    {
        return 0;
    }
    return 1;
}

DEBUG_PLATFORM_FREE_FILE_MEMORY(DEBUG_PlatformFreeFileMemory)
{
    VirtualFree(fileMemory, 0, MEM_RELEASE);
}

DEBUG_PLATFORM_READ_ENTIRE_FILE(DEBUG_PlatformReadEntireFile)
{
    Assert(FileExists(filename));
    DEBUG_file_read_result result = {0};
    HANDLE fileHandle = CreateFileA(filename,
                                    GENERIC_READ,
                                    FILE_SHARE_READ,
                                    0,
                                    OPEN_EXISTING,
                                    0,
                                    0);
    
    if(fileHandle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER fileSize;
        if(GetFileSizeEx(fileHandle, &fileSize))
        {
            
            u32 fileSize32 = SafeTruncate_u64(fileSize.QuadPart);
            result.contents = VirtualAlloc(0, 
                                           fileSize32, 
                                           MEM_RESERVE | MEM_COMMIT,
                                           PAGE_READWRITE);
            if(result.contents)
            {
                DWORD bytesRead;
                if((ReadFile(fileHandle, result.contents, 
                             fileSize32, &bytesRead, 0)) && 
                   (fileSize32 == bytesRead))
                {
                    // NOTE(winston): Success
                    result.contentSize = fileSize32;
                }
                else
                {
                    // TODO(winston): logging
                    DEBUG_PlatformFreeFileMemory(result.contents);
                    result.contents = 0;
                }
            }
            else
            {
                // TODO(winston): logging
            }
        }
        else
        {
            // TODO(winston): logging
        }
        CloseHandle(fileHandle);
    } 
    
    return result;
}
DEBUG_PLATFORM_WRITE_ENTIRE_FILE(DEBUG_PlatformWriteEntireFile)
{
    b32 result = false;
    HANDLE fileHandle = CreateFileA(filename,
                                    GENERIC_WRITE,
                                    0,
                                    0,
                                    CREATE_ALWAYS,
                                    0,
                                    0);
    
    if(fileHandle != INVALID_HANDLE_VALUE)
    {
        DWORD bytesWritten;
        if(WriteFile(fileHandle, memory, size, &bytesWritten, 0))
        {
            result = (bytesWritten == size);
        }
        else
        {
            // TODO(winston): logging
        }
    }
    else
    {
        // TODO(winston): logging
    }
    CloseHandle(fileHandle);
    return result;
}

#endif

internal client_dimension
GetClientDimension(HWND window)
{
    RECT clientRect;
    GetClientRect(window, &clientRect);
    
    client_dimension dimensions;
    
    dimensions.height = (u16)(clientRect.bottom - clientRect.top);
    dimensions.width = (u16)(clientRect.right - clientRect.left);
    
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
    i32 offsetX = 10;
    i32 offsetY = 10;
    
    StretchDIBits(deviceContext,
                  offsetX, offsetY, backBuffer->width, backBuffer->height,
                  0, 0, backBuffer->width, backBuffer->height,
                  backBuffer->bitmapMemory,
                  &backBuffer->bitmapInfo,
                  DIB_RGB_COLORS,
                  SRCCOPY);
    
    PatBlt(deviceContext, 0, 0, width, offsetY, BLACKNESS);
    PatBlt(deviceContext, 0, (offsetY + backBuffer->height), width, height, BLACKNESS);
    PatBlt(deviceContext, 0, 0, offsetX, height, BLACKNESS);
    PatBlt(deviceContext, (offsetX + backBuffer->width), 0, width, height, BLACKNESS);
}
#if 0
internal void
Win32ProcessKeyboardMessage(game_button_state* state, b32 isDown)
{
    Assert((isDown != state->endedDown));
    state->endedDown = isDown;
    ++state->halfTransitionCount;
}
#endif

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
                              &globalBackBuffer,
                              dimension.width, dimension.height);
            EndPaint(window, &paintStruct);
        } break;
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            Assert(0 && "gay boi yo key thing no working");
        } break;
        default:
        {
            result = DefWindowProc(window, message, wParam, lParam);
        }
    }
    return result;
}

internal void
Win32ProcessMessageQueue(HWND window, keyboard_input* keyboard)
{
    MSG message;
    while (PeekMessage(&message, window, 0, 0, PM_REMOVE))
    {
        switch(message.message)
        {
            case WM_QUIT:
            {
                isRunning = 0;
            } break;
            case WM_SYSKEYDOWN:
            case WM_KEYDOWN:
            {
                u32 VKcode = (u32)message.wParam;
                
                b32 wasDown = ((message.lParam & (1 << 30)) != 0);
                b32 isDown = ((message.lParam & (1 << 31)) == 0);
                if (VKcode == 'W')
                {
                    Win32PushKeyDown(keyboard, KEY_W);
                }
                else if (VKcode == 'A')
                {
                    Win32PushKeyDown(keyboard, KEY_A);
                }
                else if (VKcode == 'S')
                {
                    Win32PushKeyDown(keyboard, KEY_S);
                }
                else if (VKcode == 'D')
                {
                    Win32PushKeyDown(keyboard, KEY_D);
                }
                else if (VKcode == 'Q')
                {
                    Win32PushKeyDown(keyboard, KEY_Q);
                }
                else if (VKcode == 'E')
                {
                    Win32PushKeyDown(keyboard, KEY_E);
                }
                else if (VKcode == VK_UP)
                {
                    Win32PushKeyDown(keyboard, KEY_UP);
                }
                else if (VKcode == VK_LEFT)
                {
                    Win32PushKeyDown(keyboard, KEY_LEFT);
                }
                else if (VKcode == VK_DOWN)
                {
                    Win32PushKeyDown(keyboard, KEY_DOWN);
                }
                else if (VKcode == VK_RIGHT)
                {
                    Win32PushKeyDown(keyboard, KEY_RIGHT);
                }
                else if (VKcode == VK_ESCAPE)
                {
                    isRunning = 0;
                }
                else if (VKcode == VK_SPACE)
                {
                    Win32PushKeyDown(keyboard, KEY_SPACE);
                }
                
                b32 altKeyDown = (message.lParam & (1 << 29));
                if (VKcode == VK_F4 && altKeyDown)
                {
                    isRunning = 0;
                }
            } break;
            case WM_SYSKEYUP:
            case WM_KEYUP:
            {
                u32 VKcode = (u32)message.wParam;
                
                b32 wasDown = ((message.lParam & (1 << 30)) != 0);
                b32 isDown = ((message.lParam & (1 << 31)) == 0);
                if ((isDown != wasDown))
                {
                    if (VKcode == 'W')
                    {
                        Win32PushKeyUp(keyboard, KEY_W);
                    }
                    else if (VKcode == 'A')
                    {
                        Win32PushKeyUp(keyboard, KEY_A);
                    }
                    else if (VKcode == 'S')
                    {
                        Win32PushKeyUp(keyboard, KEY_S);
                    }
                    else if (VKcode == 'D')
                    {
                        Win32PushKeyUp(keyboard, KEY_D);
                    }
                    else if (VKcode == 'Q')
                    {
                        Win32PushKeyUp(keyboard, KEY_Q);
                    }
                    else if (VKcode == 'E')
                    {
                        Win32PushKeyUp(keyboard, KEY_E);
                    }
                    else if (VKcode == VK_UP)
                    {
                        Win32PushKeyUp(keyboard, KEY_UP);
                    }
                    else if (VKcode == VK_LEFT)
                    {
                        Win32PushKeyUp(keyboard, KEY_LEFT);
                    }
                    else if (VKcode == VK_DOWN)
                    {
                        Win32PushKeyUp(keyboard, KEY_DOWN);
                    }
                    else if (VKcode == VK_RIGHT)
                    {
                        Win32PushKeyUp(keyboard, KEY_RIGHT);
                    }
                    else if (VKcode == VK_ESCAPE)
                    {
                        isRunning = 0;
                    }
                    else if (VKcode == VK_SPACE)
                    {
                        Win32PushKeyUp(keyboard, KEY_SPACE);
                    }
                }
            } break;
            default:
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
            } break;
        }
    }
}

internal f32
Win32GetMSElapsed(i64 start, 
                  i64 end,
                  i64 performanceFrequency)
{
    i64 timeElapsed = end - start;
    Assert(timeElapsed > 0);
    return (1000.0f * (f32)timeElapsed) / (f32)performanceFrequency;
}

//~ Main function

int WinMain(HINSTANCE hInstance, 
            HINSTANCE prevInstance, 
            LPSTR lpCmdLine, 
            int nShowCmd)
{
    setvbuf(stdout, 0, _IONBF, 0);
    
    LARGE_INTEGER performanceFrequencyResult;
    QueryPerformanceFrequency(&performanceFrequencyResult);
    i64 performanceFrequency = performanceFrequencyResult.QuadPart;
    
    Win32InitBackBuffer(&globalBackBuffer,
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
            // NOTE(winston): input stuff
            game_input gameInput[2] = {0};
            game_input* oldGameInput = &gameInput[0];
            game_input* newGameInput = &gameInput[1];
            
            isRunning = 1;
            
            LARGE_INTEGER lastCounter;
            QueryPerformanceCounter(&lastCounter);
            
            game_back_buffer gameBackBuffer = {};
            gameBackBuffer.memory = globalBackBuffer.bitmapMemory;
            gameBackBuffer.bytesPerPixel = globalBackBuffer.bytesPerPixel;
            gameBackBuffer.pitch = globalBackBuffer.pitch;
            gameBackBuffer.width = globalBackBuffer.width;
            gameBackBuffer.height = globalBackBuffer.height;
            
			// NOTE(winston): sum memory alloc
#if HANDMADE_INTERNAL
			LPVOID baseAddress = (LPVOID)Gigabytes((u64)200);
#else
			LPVOID baseAddress = 0;
#endif
			game_memory memory = {};
			memory.permanentStorageSize = Megabytes((u64)64);
            memory.transientStorageSize = Gigabytes((u64)4);
            
            memory.DEBUG_PlatformReadEntireFile = DEBUG_PlatformReadEntireFile;
            memory.DEBUG_PlatformWriteEntireFile = DEBUG_PlatformWriteEntireFile;
            memory.DEBUG_PlatformFreeFileMemory
                = DEBUG_PlatformFreeFileMemory;
            
            u64 totalSize = memory.permanentStorageSize + 
                memory.transientStorageSize;
            
			memory.permanentStorage =
				VirtualAlloc(baseAddress,
							 totalSize,
							 MEM_RESERVE | MEM_COMMIT,
							 PAGE_READWRITE);
			memory.transientStorage = (u8*)memory.permanentStorage + 
                memory.permanentStorageSize;
            u32 counter = 0;
            
            char* gameDLLName = "handmade.dll";
            
            game_code gameCode = Win32LoadGameCode(gameDLLName);
			
            keyboard_input keyboardInput = {};
            
            if (memory.permanentStorage && memory.transientStorage)
            {
				while (isRunning)
				{
                    FILETIME currentWriteTime = Win32GetLastWriteTime(gameDLLName);
                    
                    if(CompareFileTime(&(gameCode.lastWriteTime), &currentWriteTime) != 0)
                    {
                        Win32UnloadGameCode(&gameCode);
                        Sleep(50);
                        gameCode = Win32LoadGameCode(gameDLLName);
                    }
                    
                    game_keyboard_input* oldKeyboardState = 
                        GetController(oldGameInput, 0);
					game_keyboard_input* newKeyboardState = 
                        GetController(newGameInput, 0);
                    
                    
                    *newKeyboardState = {0};
                    
                    for(u32 buttonIndex = 0; 
                        buttonIndex < ArrayCount(newKeyboardState->buttons);
                        ++buttonIndex)
                    {
                        newKeyboardState->buttons[buttonIndex].endedDown
                            = oldKeyboardState->buttons[buttonIndex].endedDown;
                    }
                    
                    Win32ProcessMessageQueue(window, &keyboardInput);
                    
					HDC hdc = GetDC(window);
					client_dimension dimension = GetClientDimension(window);
                    
                    gameCode.UpdateAndRender(&memory,
                                             &keyboardInput,
                                             &gameBackBuffer, 
                                             dimension);
                    
					Win32UpdateWindow(hdc,
									  &globalBackBuffer,
									  dimension.width, dimension.height);
                    
					ReleaseDC(window, hdc);
                    
                    game_input* temp = newGameInput;
                    newGameInput = oldGameInput;
                    oldGameInput = temp;
                    
                    LARGE_INTEGER endCounter;
					QueryPerformanceCounter(&endCounter);
                    
					f32 msPerFrame = Win32GetMSElapsed(lastCounter.QuadPart,
                                                       endCounter.QuadPart,
                                                       performanceFrequency);
                    
#if 0
					char buffer[256];
					sprintf(buffer, "Milliseconds/frame %f ms\n", msPerFrame);
                    
					OutputDebugStringA(buffer);
#endif
					if (msPerFrame < 16.667f)
						Sleep(DWORD(16.667f - msPerFrame));
					lastCounter = endCounter;
                    
				}
            }
            Win32UnloadGameCode(&gameCode);
            DestroyWindow(window);
#if 0
			VirtualFree(memory.permanentStorage,
						0,
						MEM_RELEASE);
			VirtualFree(memory.transientStorage,
						0,
						MEM_RELEASE);
#endif
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
