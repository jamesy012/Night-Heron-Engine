#include "Window.h"

#include "Graphics/API/GFXAPI.h"
#include "Singletons.h"

#include <Dependency/ImGui/imgui.h>
#include <Dependency/ImGui/examples\imgui_impl_win32.h>
IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT Window::WndProc(HWND hWnd, UINT uMsg, WPARAM  wParam, LPARAM  lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
		return 0;
	}

	switch (uMsg) {
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE) {
				if (MessageBox(NULL, "Are you sure you want to exit?", "Exit", MB_YESNO | MB_ICONQUESTION) == IDYES) {
					DestroyWindow(hWnd);
				}
			}
			return 0;
		case WM_SIZE:
			{
				if (_CGraphics == nullptr) {
					return 1;
				}
				int width = LOWORD(lParam);
				int height = HIWORD(lParam);

				_CGraphics->ResizeWindow(width, height);

				_CGraphics->m_Window->m_WindowWidth = width;
				_CGraphics->m_Window->m_WindowHeight = height;
			}
			return 0;
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void Window::ImGuiInit() {
	ImGui_ImplWin32_Init(m_HWnd);
}

void Window::ImGuiNewFrame() {
	ImGui_ImplWin32_NewFrame();
}


bool Window::CreateMainWindow() {

	unsigned int      PixelFormat;                        // Holds The Results After Searching For A Match
	WNDCLASS    wc;                         // Windows Class Structure
	DWORD       dwExStyle;                      // Window Extended Style
	DWORD       dwStyle;                        // Window Style

	RECT WindowRect;                            // Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left = (long)0;                        // Set Left Value To 0
	WindowRect.right = (long)m_WindowWidth;                       // Set Right Value To Requested Width
	WindowRect.top = (long)0;                         // Set Top Value To 0
	WindowRect.bottom = (long)m_WindowHeight;                    // Set Bottom Value To Requested Height

	m_HInstance = GetModuleHandle(NULL);            // Grab An Instance For Our Window

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;       // Redraw On Move, And Own DC For Window
	wc.lpfnWndProc = (WNDPROC)WndProc;                // WndProc Handles Messages
	wc.cbClsExtra = 0;                        // No Extra Window Data
	wc.cbWndExtra = 0;                        // No Extra Window Data
	wc.hInstance = m_HInstance;                    // Set The Instance
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);          // Load The Default Icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);          // Load The Arrow Pointer
	wc.hbrBackground = NULL;                     // No Background Required For GL
	wc.lpszMenuName = NULL;                     // We Don't Want A Menu
	wc.lpszClassName = m_WindowClassName.c_str();                 // Set The Class Name

	if (!RegisterClass(&wc)) {
		MessageBox(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Exit And Return FALSE
	}

	//if (ourWindow->fullscreen) {
	//	DEVMODE dmScreenSettings; // Device Mode
	//	memset(&dmScreenSettings, 0, sizeof(DEVMODE)); // make sure memory is cleared
	//
	//	dmScreenSettings.dmSize = sizeof(DEVMODE); // size of structure
	//	dmScreenSettings.dmPelsWidth = width; // width of screen
	//	dmScreenSettings.dmPelsHeight = height; // height of screen
	//	dmScreenSettings.dmBitsPerPel = m_Bits; // bits
	//	dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	//
	//	if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
	//		if (MessageBox(NULL, "The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?", "NeHe GL", MB_YESNO | MB_ICONEXCLAMATION) == IDYES) {
	//			ourWindow->fullscreen = false;
	//		} else {
	//			// Pop Up A Message Box Letting User Know The Program Is Closing.
	//			MessageBox(NULL, "Program Will Now Close.", "ERROR", MB_OK | MB_ICONSTOP);
	//			return FALSE;                   // Exit And Return FALSE
	//		}
	//	}
	//}

	//if (ourWindow->fullscreen) { // are we still there?
	//	dwExStyle = WS_EX_APPWINDOW;
	//	dwStyle = WS_POPUP;
	//	ShowCursor(FALSE);
	//} else {
	dwExStyle = WS_EX_APPWINDOW | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE;
	//dwStyle = WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_MINIMIZEBOX;
	dwStyle = WS_OVERLAPPEDWINDOW;
	//}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

	//SetPropW(ourWindow->hWnd, L"OpenGLWindow", ourWindow);

	m_HWnd = CreateWindowEx(dwExStyle, m_WindowClassName.c_str(), m_WindowTitle.c_str(), dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top, NULL, NULL, m_HInstance, NULL);

	if (!m_HWnd) {
		//DestroyOpenGLWindow();
		MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Return FALSE
	}



	static  PIXELFORMATDESCRIPTOR pfd =                  // pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),                  // Size Of This Pixel Format Descriptor
		1,                              // Version Number
		PFD_DRAW_TO_WINDOW |                        // Format Must Support Window
		PFD_SUPPORT_OPENGL |                        // Format Must Support OpenGL
		PFD_DOUBLEBUFFER,                       // Must Support Double Buffering
		PFD_TYPE_RGBA,                          // Request An RGBA Format
		m_Bits,                               // Select Our Color Depth
		0, 0, 0, 0, 0, 0,                       // Color Bits Ignored
		0,                              // No Alpha Buffer
		0,                              // Shift Bit Ignored
		0,                              // No Accumulation Buffer
		0, 0, 0, 0,                         // Accumulation Bits Ignored
		24,                             // 16Bit Z-Buffer (Depth Buffer)
		8,                              // No Stencil Buffer
		0,                              // No Auxiliary Buffer
		PFD_MAIN_PLANE,                         // Main Drawing Layer
		0,                              // Reserved
		0, 0, 0                             // Layer Masks Ignored
	};

	m_HDC = GetDC(m_HWnd);
	if (!m_HDC) {
		//DestroyOpenGLWindow();                         // Reset The Display
		MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Return FALSE
	}

	PixelFormat = ChoosePixelFormat(m_HDC, &pfd);
	if (!PixelFormat)             // Did Windows Find A Matching Pixel Format?
	{
		//DestroyOpenGLWindow();                         // Reset The Display
		MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Return FALSE
	}

	if (!SetPixelFormat(m_HDC, PixelFormat, &pfd))               // Are We Able To Set The Pixel Format?
	{
		//DestroyOpenGLWindow();                         // Reset The Display
		MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Return FALSE
	}


	//SetWindowLongPtr(m_HWnd, GWLP_USERDATA, (LONG_PTR)ourWindow);

	ShowWindow(m_HWnd, SW_SHOW);
	SetForegroundWindow(m_HWnd);
	SetFocus(m_HWnd);


	return true;
}

void Window::DestrowMainWindow() {

	if (m_HDC && !ReleaseDC(m_HWnd, m_HDC)) {
		//failed
		MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		m_HDC = NULL;
	}
	if (m_HWnd && !DestroyWindow(m_HWnd)) {
		MessageBox(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		m_HWnd = NULL;                          // Set hWnd To NULL
	}
	if (!UnregisterClass(m_WindowClassName.c_str(), m_HInstance))               // Are We Able To Unregister Class
	{
		MessageBox(NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		m_HInstance = NULL;                         // Set hInstance To NULL
	}

	ImGui_ImplWin32_Shutdown();
}
