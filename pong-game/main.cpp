#include "utils.cpp"
#include <windows.h>

global_variable bool running = true;

struct Render_State {
	int height, width;
	void* memory;
	BITMAPINFO bitmap_info;

	int getBufferSize() {return height * width * sizeof(u32);}
		
};

global_variable Render_State render_state;

#include "platform_common.cpp"
#include "renderer.cpp"
#include "game.cpp"
/*

LRESULT is an integer value that your program returns to Windows. It contains your program's response to a particular message. The meaning of this value depends on the message code. 
CALLBACK is the calling convention for the function.
hwnd is a handle to the window.
uMsg is the message code; for example, the WM_SIZE message indicates the window was resized.
wParam and lParam contain additional data that pertains to the message. The exact meaning depends on the message code.

*/

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	switch (uMsg) {
		case WM_CLOSE:
		case WM_DESTROY: {
			running = false;
			break;
		} 
		//main window change size
		case WM_SIZE: {
			RECT rect;
			GetClientRect(hwnd, &rect);
			render_state.width = rect.right - rect.left;
			render_state.height = rect.bottom - rect.top;

			//create buffer
			int buffer_size = render_state.getBufferSize();

			//allocate memory in virtual address space, free the memory first if it is existed
			if (render_state.memory)
				VirtualFree(render_state.memory, 0, MEM_RELEASE);

			render_state.memory = VirtualAlloc(0, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			
			render_state.bitmap_info.bmiHeader.biSize = sizeof(render_state.bitmap_info.bmiHeader);
			render_state.bitmap_info.bmiHeader.biWidth = render_state.width;
			render_state.bitmap_info.bmiHeader.biHeight = render_state.height;
			render_state.bitmap_info.bmiHeader.biPlanes = 1;
			render_state.bitmap_info.bmiHeader.biBitCount = 32;
			render_state.bitmap_info.bmiHeader.biCompression = BI_RGB;

			break;
		}

		default:
			result = DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return result;
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	ShowCursor(false);
	//create window class
	WNDCLASS window_class = {};
	window_class.style = CS_HREDRAW | CS_VREDRAW; //redraw the window horizontally and vertically
	window_class.lpszClassName = "Game Window Class"; //string that identifies the window class (not visible to normal user)
	window_class.lpfnWndProc = WindowProc; //act like an event litsener

	//create register class
	RegisterClass(&window_class);

	//create window
	HWND window = CreateWindow(window_class.lpszClassName, "Pong Game", WS_OVERLAPPEDWINDOW|WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720 ,0, 0, hInstance, 0);
	
	//retrieves a handle to a device context (DC) for the client area of a specified window or for the entire screen.
	HDC hdc = GetDC(window);

	Input input = {};

	float delta_time = 0.016666f;
	LARGE_INTEGER frame_begin_time;
	QueryPerformanceCounter(&frame_begin_time);
	float performance_frequency; 
	{
		LARGE_INTEGER perf;
		QueryPerformanceFrequency(&perf);
		performance_frequency = (float)(perf.QuadPart);
	}

	while (running) {
		
		//input
		//ask window for any message
		//PM_REMOVE wont ask you to remove message so only read message once
		MSG message;

		//initialise all button to false first
		for (int i = 0; i < BUTTON_COUNT; i++) {
			input.buttons[i].changed = false;
		}

		while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {

			switch (message.message) {
				case WM_KEYUP:
				case WM_KEYDOWN: {
					
					u32 vk_code = (u32)message.wParam; //extracts the virtual key code from the wParam

					/*
					(1 << 31) is a bitmask that checks the highest bit (bit 31).
					If the highest bit is not set (i.e., it's 0), is_down is set to true, indicating that the key is currently pressed.
					If the highest bit is set (i.e., it's 1), is_down is set to false, indicating that the key has been released.
					*/

					bool is_down = ((message.lParam & (1 << 31)) == 0);

//Macros (code that is specified by a #define) can only have one line by default.
//If we want the macro to extent to the next line, we end the line with the \ symbols
//changed is set to true only if the current is_down is different from the recorded is_down
#define process_button(b, vk)\
case vk: {\
input.buttons[b].changed = is_down != input.buttons[b].is_down;\
input.buttons[b].is_down = is_down;\
} break;
					switch (vk_code) {
						process_button(BUTTON_UP, VK_UP);
						process_button(BUTTON_DOWN, VK_DOWN);
						process_button(BUTTON_W, 'W');
						process_button(BUTTON_S, 'S');
						process_button(BUTTON_A, 'A');
						process_button(BUTTON_D, 'D');
						process_button(BUTTON_LEFT, VK_LEFT);
						process_button(BUTTON_RIGHT, VK_RIGHT);
						process_button(BUTTON_ENTER, VK_RETURN);
					}
					break;
				}
				default: {
					TranslateMessage(&message);
					DispatchMessage(&message);
				}
			}	
		}
		
		//simulate
		simulate_game(&input,delta_time);

		//render
		StretchDIBits(hdc, 0, 0, render_state.width, render_state.height, 0, 0, render_state.width, render_state.height, render_state.memory, &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
		LARGE_INTEGER frame_end_time;
		QueryPerformanceCounter(&frame_end_time);
		delta_time = (float)(frame_end_time.QuadPart - frame_begin_time.QuadPart) / performance_frequency; //get CPU time
		frame_begin_time = frame_end_time;
	}
}