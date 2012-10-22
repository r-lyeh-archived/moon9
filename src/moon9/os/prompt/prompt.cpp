/*
 * Simple prompt dialog (based on legolas558's code). MIT license.
 * - rlyeh
 */

#ifdef _WIN32

#include <windows.h>
#include <string>

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")

#pragma message("<moon9/debug/prompt.cpp> says: dialog aware dpi fix (@todo)")

std::string prompt( const std::string &title, const std::string &current_value, const std::string &caption )
{
	class InputBox
	{
		private:

		HWND				hwndParent,
							hwndInputBox,
							hwndQuery,
							hwndOk,
							hwndCancel,
							hwndEditBox;
		LPSTR				szInputText;
		WORD				wInputMaxLength, wInputLength;
		bool				bRegistered,
							bResult;

		HINSTANCE			hThisInstance;

		enum
		{
			CIB_SPAN = 10,
			CIB_LEFT_OFFSET = 6,
			CIB_TOP_OFFSET = 4,
			CIB_WIDTH = 300,
			CIB_HEIGHT = 130,
			CIB_BTN_WIDTH = 60,
			CIB_BTN_HEIGHT = 20
		};

		public:

#		define CIB_CLASS_NAME "CInputBoxA"

		static LRESULT CALLBACK CIB_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			InputBox *self;
			self = (InputBox *)GetWindowLong(hWnd, GWL_USERDATA);

			switch (msg)
			{
				case WM_CREATE:
					self = (InputBox *) ((CREATESTRUCT *)lParam)->lpCreateParams;
					SetWindowLong(hWnd, GWL_USERDATA, (long)self);
					self->create(hWnd);
				break;
				case WM_COMMAND:
					switch(LOWORD(wParam)) {
						case IDOK:
							self->submit();
						case IDCANCEL:
							self->close();
						break;
					}
					break;
				case WM_CLOSE:
					self->hide();
					return 0;
				case WM_DESTROY:
					self->destroy();
					break;
			}
			return(DefWindowProc (hWnd, msg, wParam, lParam));
		}

		InputBox( HINSTANCE hInst ) :
			hwndParent(0),
			hwndInputBox(0),
			hwndQuery(0),
			hwndOk(0),
			hwndCancel(0),
			hwndEditBox(0),
			szInputText(""),
			wInputMaxLength(0), wInputLength(0),
			bRegistered(false),
			bResult(false),
			hThisInstance(0)
		{
			WNDCLASSEX wndInputBox;
			RECT rect;

			memset(&wndInputBox, 0, sizeof(WNDCLASSEX));

			hThisInstance = hInst;

			wndInputBox.cbSize					= sizeof(wndInputBox);
			wndInputBox.lpszClassName			= CIB_CLASS_NAME;
			wndInputBox.style					= CS_HREDRAW | CS_VREDRAW;
			wndInputBox.lpfnWndProc				= CIB_WndProc;
			wndInputBox.lpszMenuName			= NULL;
			wndInputBox.hIconSm					= NULL;
			wndInputBox.cbClsExtra				= 0;
			wndInputBox.cbWndExtra				= 0;
			wndInputBox.hInstance				= hInst;
			wndInputBox.hIcon					= LoadIcon(NULL, IDI_WINLOGO);
			wndInputBox.hCursor					= LoadCursor(NULL, IDC_ARROW);
			wndInputBox.hbrBackground			= (HBRUSH)(COLOR_WINDOW);

			RegisterClassEx(&wndInputBox);

			if (hwndParent)
				GetWindowRect(hwndParent, &rect); //always false?
			else
				GetWindowRect(GetDesktopWindow(), &rect);

			hwndInputBox = CreateWindow(	CIB_CLASS_NAME, "",
							(WS_BORDER | WS_CAPTION), rect.left+(rect.right-rect.left-CIB_WIDTH)/2,
							rect.top+(rect.bottom-rect.top-CIB_HEIGHT)/2,
							CIB_WIDTH, CIB_HEIGHT, hwndParent, NULL,
							hThisInstance, this);
		}

		void destroy()
		{
			EnableWindow(hwndParent, true);
			SendMessage(hwndInputBox, WM_CLOSE/*WM_DESTROY*/, 0, 0);
		}

		~InputBox()
		{
			UnregisterClass(CIB_CLASS_NAME, hThisInstance);
		}

		void submit()
		{
			wInputLength = (int)SendMessage(hwndEditBox, EM_LINELENGTH, 0, 0);
			if (wInputLength) {
				*((LPWORD)szInputText) = wInputMaxLength;
				wInputLength = (WORD)SendMessage(hwndEditBox, EM_GETLINE, 0, (LPARAM)szInputText);
			}
			szInputText[wInputLength] = '\0';
			bResult = true;
		}

		void create(HWND hwndNew)
		{
			static HFONT myFont = NULL;

			if( myFont != NULL )
			{
				DeleteObject( myFont );
				myFont = NULL;
			}

			hwndInputBox = hwndNew;

			NONCLIENTMETRICS ncm;
			ncm.cbSize = sizeof(NONCLIENTMETRICS) - sizeof(ncm.iPaddedBorderWidth);

			if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0))
			{
#if 0
				LOGFONT lf;
				memset(&lf,0,sizeof(LOGFONT));

				lf.lfWeight= FW_NORMAL;
				lf.lfCharSet= ANSI_CHARSET;
				//lf.lfPitchAndFamily = 35;
				lf.lfHeight= 10;
				strcpy(lf.lfFaceName, "Tahoma");
				myFont=CreateFontIndirect(&lf);
#else
				myFont = CreateFontIndirect(&ncm.lfMessageFont);
#endif
			}
			else
			{
				myFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
			}

			//	SetWindowPos(hwndInputBox, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			hwndQuery =	CreateWindow("Static", "", WS_CHILD | WS_VISIBLE,
									CIB_LEFT_OFFSET, CIB_TOP_OFFSET,
									CIB_WIDTH-CIB_LEFT_OFFSET*2, CIB_BTN_HEIGHT*2,
									hwndInputBox, NULL,
									hThisInstance, NULL);
			hwndEditBox = CreateWindow("Edit", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT, CIB_LEFT_OFFSET,
									CIB_TOP_OFFSET + CIB_BTN_HEIGHT*2, CIB_WIDTH-CIB_LEFT_OFFSET*3, CIB_BTN_HEIGHT,
									hwndInputBox,	NULL,
									hThisInstance, NULL);
			hwndOk =		CreateWindow("Button", "OK", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
									CIB_WIDTH/2 - CIB_SPAN*2 - CIB_BTN_WIDTH, CIB_HEIGHT - CIB_TOP_OFFSET*4 - CIB_BTN_HEIGHT*2,
									CIB_BTN_WIDTH, CIB_BTN_HEIGHT, hwndInputBox, (HMENU)IDOK,
									hThisInstance, NULL);
			hwndCancel =	CreateWindow("Button", "Cancel",
									WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
									CIB_WIDTH/2 + CIB_SPAN, CIB_HEIGHT - CIB_TOP_OFFSET*4 - CIB_BTN_HEIGHT*2,  CIB_BTN_WIDTH, CIB_BTN_HEIGHT,
									hwndInputBox, (HMENU)IDCANCEL,
									hThisInstance, NULL);

		//	SendMessage(hwndInputBox,WM_SETFONT,(WPARAM)myFont,FALSE);
			SendMessage(hwndQuery,WM_SETFONT,(WPARAM)myFont,FALSE);
			SendMessage(hwndEditBox,WM_SETFONT,(WPARAM)myFont,FALSE);
			SendMessage(hwndOk,WM_SETFONT,(WPARAM)myFont,FALSE);
			SendMessage(hwndCancel,WM_SETFONT,(WPARAM)myFont,FALSE);
		}

		void close()
		{
			PostMessage(hwndInputBox, WM_CLOSE, 0, 0);
		}

		void hide()
		{
			ShowWindow(hwndInputBox, SW_HIDE);
		}

		void show(LPCSTR lpszTitle, LPCSTR	lpszQuery)
		{
			SetWindowText(hwndInputBox, lpszTitle);
			SetWindowText(hwndEditBox, szInputText);
			SetWindowText(hwndQuery, lpszQuery);
			SendMessage(hwndEditBox, EM_LIMITTEXT, wInputMaxLength, 0);
			SendMessage(hwndEditBox, EM_SETSEL, 0, -1);
			SetFocus(hwndEditBox);
			ShowWindow(hwndInputBox, SW_NORMAL);
		}

		int show(HWND hwndParentWindow, LPCTSTR lpszTitle, LPCTSTR lpszQuery, LPSTR szResult, WORD wMax)
		{
			MSG	msg;
			BOOL	bRet;
			hwndParent = hwndParentWindow;
			szInputText = szResult;
			wInputMaxLength = wMax;

			bResult = false;

		//	EnableWindow(hwndParent, false);

			show(lpszTitle, lpszQuery);

			while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
			{
				if (msg.message==WM_KEYDOWN) {
					switch (msg.wParam) {
					case VK_RETURN:
						submit();
					case VK_ESCAPE:
						close();
						break;
					default:
						TranslateMessage(&msg);
						break;
					}
				} else
		//		if (!IsDialogMessage(hwndInputBox, &msg)) {
					TranslateMessage(&msg);
		//		}
				DispatchMessage(&msg);
				if (msg.message == WM_CLOSE)
					break;
			}

		//	EnableWindow(hwndParent, true);

		    return bResult;
		}

	#	undef CIB_CLASS_NAME
	}
	myinp(GetModuleHandle(0));

	char *result = new char [2048+1];

	memset( result, 0, 2048+1 ); //default value

	strcpy( result, current_value.c_str() );

	myinp.show(0, title.c_str(), caption.size() ? caption.c_str() : title.c_str(), result, 2048);

	std::string _r = result;

	delete [] result;

	return _r;
}

#endif
