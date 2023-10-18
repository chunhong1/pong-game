#include <Windows.h>

bool running = true;
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

	while (running) {
		
		//input
		//ask window for any message
		MSG message;
		//PM_REMOVE wont ask you to remove message so only read message once
		while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		
		//stimulate

		//render
	}
}