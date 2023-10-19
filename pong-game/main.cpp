#include "utils.cpp"
#include <Windows.h>

global_variable bool running = true;

struct Render_State {
	int height, width;
	void* memory;
	BITMAPINFO bitmap_info;

	int getBufferSize() {return height * width * sizeof(u32);}
		
};
global_variable Render_State render_state;
#include "renderer.cpp"
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
	//create window class
	WNDCLASS window_class = {};
	window_class.style = CS_HREDRAW | CS_VREDRAW; //redraw the window horizontally and vertically
	window_class.lpszClassName = "Gamme Window Class"; //string that identifies the window class (not visible to normal user)
	window_class.lpfnWndProc = WindowProc; //act like an event litsener

	//create register class
	RegisterClass(&window_class);

	//create window
	HWND window = CreateWindow(window_class.lpszClassName, "Pong Game", WS_OVERLAPPEDWINDOW|WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720 ,0, 0, hInstance, 0);
	
	//retrieves a handle to a device context (DC) for the client area of a specified window or for the entire screen.
	HDC hdc = GetDC(window);

	while (running) {
		
		//input
		//ask window for any message
		//PM_REMOVE wont ask you to remove message so only read message once
		MSG message;
		while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		
		//stimulate
		clear_screen(0xff5500);
		draw_rect(0, 0, 1, 1, 0x00ff22);
		draw_rect(30, 30, 5, 5, 0x00ff22);
		draw_rect(-20, 20, 8, 3, 0x00ff22);

		//render
		StretchDIBits(hdc, 0, 0, render_state.width, render_state.height, 0, 0, render_state.width, render_state.height, render_state.memory, &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
	}
}