#include "../../src/KrGui.h"
#include "../../src/krGuiOpenGL.h"

using namespace Kr;

bool gDemoRun = true;

// ==================================================== WINDOW BEGIN
class DemoWindow{
protected:
	int m_x = 0;
	int m_y = 0;
public:
	DemoWindow(){}
	virtual ~DemoWindow(){}
	int GetWindowHeight(){return m_y;}
	int GetWindowWidth(){return m_x;}
	virtual void * GetHandle() = 0;
};

#ifdef KRGUI_PLATFORM_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#ifdef CreateWindow
#undef CreateWindow
#endif
static int gClassNameNumber = 0;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
class DemoWindowWin32 : public DemoWindow{
	HWND m_hWnd = nullptr;
	std::u16string m_className;
	friend LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
public:
	DemoWindowWin32(int x, int y){
		m_x = x;
		m_y = y;
		m_className = u"DemoWindow";
		m_className += ++gClassNameNumber;
		DWORD style = WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_SYSMENU;
		WNDCLASSEX wc;
		ZeroMemory( &wc, sizeof( wc ) );
		wc.cbSize			= sizeof(WNDCLASSEX);
		wc.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wc.lpfnWndProc		= WndProc;
		wc.hInstance		= GetModuleHandle( 0 );
		wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground	= (HBRUSH)(COLOR_MENU+1);
		wc.lpszClassName	= (wchar_t*)m_className.c_str();
		RegisterClassEx(&wc);
		m_hWnd = CreateWindowW( (wchar_t*)m_className.c_str(),L"Window",style,0,0,x,y,nullptr,nullptr,wc.hInstance,this );
		ShowWindow( m_hWnd, SW_SHOWNORMAL  );
		SetForegroundWindow( m_hWnd );
		SetFocus( m_hWnd );
		UpdateWindow( m_hWnd );
		m_dc = GetDC(m_hWnd);
	}
	virtual ~DemoWindowWin32(){
		if( m_hWnd ){
			ReleaseDC(m_hWnd,m_dc);
			DestroyWindow( m_hWnd );
		}
		UnregisterClass( (wchar_t*)m_className.c_str(), GetModuleHandle( 0 ) );
	}
	virtual void * GetHandle(){return m_hWnd;}
	HDC m_dc = nullptr;
};
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	DemoWindowWin32* pD = nullptr;
	int wmId    = LOWORD(wParam);
	if( message == WM_NCCREATE ){
		pD = static_cast<DemoWindowWin32*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
		SetLastError(0);
		if( !SetWindowLongPtr(hWnd, -21, reinterpret_cast<LONG_PTR>(pD)) ){
			if( GetLastError() != 0 ) return FALSE;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}else pD = reinterpret_cast<DemoWindowWin32*>(GetWindowLongPtr(hWnd, -21));

	switch( message ){
	case WM_QUIT:
	case WM_CLOSE:
	case WM_DESTROY:
			gDemoRun = false;
	return 0;
	//case WM_MOUSEWHEEL:
	//	if(pD)
	//	{
	//		if( pD->m_focus )
	//		{
	//			ev.type = kkEventType::MouseWheel;
	//			ev.mouseEvent.state = 0u;
	//			ev.mouseEvent.wheel = int( (f32)GET_WHEEL_DELTA_WPARAM(wParam) / (f32)WHEEL_DELTA );
	//			kkGetMainSystem()->addEvent( ev );
	//		}
	//	}
	//	return 0;
	//case WM_LBUTTONDBLCLK:
	//case WM_RBUTTONDBLCLK:
	//case WM_LBUTTONDOWN:
	//case WM_LBUTTONUP:
	//case WM_RBUTTONDOWN:
	//case WM_RBUTTONUP:
	//case WM_MBUTTONDOWN:
	//case WM_MBUTTONUP:
	//case WM_MOUSEMOVE:
	//{
	//	ev.type = kkEventType::Mouse;
	//	ev.mouseEvent.state = 0u;

	//	static f32 crdX = 0.f;
	//	static f32 crdY = 0.f;
	//	if(pD)
	//	{
	//		if( pD->m_focus )
	//		{
	//			POINT point;
	//			GetCursorPos(&point);
	//			ScreenToClient(hWnd,&point);
	//			crdX = (float)point.x;
	//			crdY = (float)point.y;
	//		}
	//	}
	//	/*ev.mouseEvent.x = LOWORD(lParam);
	//	ev.mouseEvent.y = HIWORD(lParam);*/
	//	ev.mouseEvent.x = crdX;
	//	ev.mouseEvent.y = crdY;

	//	if( wParam & MK_LBUTTON )		ev.mouseEvent.state |= kkEventMouse::kkEventMouseState::MS_LMB_DOWN;
	//	if( wParam & MK_RBUTTON )		ev.mouseEvent.state |= kkEventMouse::kkEventMouseState::MS_RMB_DOWN;
	//	if( wParam & MK_MBUTTON )		ev.mouseEvent.state |= kkEventMouse::kkEventMouseState::MS_MMB_DOWN;
	//	if( wParam & MK_XBUTTON1 )
	//	{
	//		ev.mouseEvent.state |= kkEventMouse::kkEventMouseState::MS_X1MB_DOWN;
	//		switch( message )
	//		{
	//			case WM_XBUTTONUP: ev.mouseEvent.state |= kkEventMouse::kkEventMouseState::MS_X1MB_UP; break;
	//			case WM_XBUTTONDBLCLK: ev.mouseEvent.state |= kkEventMouse::kkEventMouseState::MS_X1MB_DOUBLE; break;
	//		}
	//	}
	//	if( wParam & MK_XBUTTON2 )
	//	{
	//		ev.mouseEvent.state |= kkEventMouse::kkEventMouseState::MS_X2MB_DOWN;
	//		switch( message )
	//		{
	//			case WM_XBUTTONUP: ev.mouseEvent.state |= kkEventMouse::kkEventMouseState::MS_X2MB_UP; break;
	//			case WM_XBUTTONDBLCLK: ev.mouseEvent.state |= kkEventMouse::kkEventMouseState::MS_X2MB_DOUBLE; break;
	//		}
	//	}

	//	switch( message )
	//	{
	//	case WM_LBUTTONUP:{	
	//		ev.mouseEvent.state    |= kkEventMouse::kkEventMouseState::MS_LMB_UP; 
	//	}break;
	//		case WM_RBUTTONUP:	ev.mouseEvent.state    |= kkEventMouse::kkEventMouseState::MS_RMB_UP; break;
	//		case WM_MBUTTONUP:	ev.mouseEvent.state    |= kkEventMouse::kkEventMouseState::MS_MMB_UP; break;
	//		case WM_LBUTTONDBLCLK: ev.mouseEvent.state |= kkEventMouse::kkEventMouseState::MS_LMB_DOUBLE; break;
	//		case WM_RBUTTONDBLCLK: ev.mouseEvent.state |= kkEventMouse::kkEventMouseState::MS_RMB_DOUBLE; break;
	//		case WM_MBUTTONDBLCLK: ev.mouseEvent.state |= kkEventMouse::kkEventMouseState::MS_MMB_DOUBLE; break;
	//	}


	//	kkGetMainSystem()->addEvent( ev );

	//	return 0;
	//}break;

	//case WM_SYSKEYDOWN:
	//case WM_SYSKEYUP:
	//case WM_KEYDOWN:
	//case WM_KEYUP:
	//{
	//	ev.type = kkEventType::Keyboard;
	//	ev.keyboardEvent.key = static_cast<kkKey>( wParam );

	//	if(message == WM_SYSKEYDOWN){ ev.keyboardEvent.state_is_pressed = 1; }
	//	if(message == WM_KEYDOWN){ev.keyboardEvent.state_is_pressed = 1; }
	//	if(message == WM_SYSKEYUP){  ev.keyboardEvent.state_is_relesed = 1; }
	//	if(message == WM_KEYUP){ ev.keyboardEvent.state_is_relesed = 1; }

	//	const UINT MY_MAPVK_VSC_TO_VK_EX = 3;

	//	if( ev.keyboardEvent.key == kkKey::K_SHIFT )
	//	{ // shift -> lshift rshift
	//		ev.keyboardEvent.key = static_cast<kkKey>( MapVirtualKey( ( static_cast<UINT>( lParam >> 16) & 255u ), MY_MAPVK_VSC_TO_VK_EX ) );
	//		ev.keyboardEvent.state_is_shift = TRUE;
	//	}
	//	if( ev.keyboardEvent.key == kkKey::K_CTRL )
	//	{ // ctrl -> lctrl rctrl
	//		ev.keyboardEvent.key = static_cast<kkKey>( MapVirtualKey( ( static_cast<UINT>( lParam >> 16) & 255 ), MY_MAPVK_VSC_TO_VK_EX ) );
	//		ev.keyboardEvent.state_is_ctrl = TRUE;
	//		if( lParam & 0x1000000 )
	//		{
	//			ev.keyboardEvent.key = static_cast<kkKey>( 163 );
	//		}
	//	}

	//	

	//	if( ev.keyboardEvent.key == kkKey::K_ALT )
	//	{ // alt -> lalt ralt
	//		ev.keyboardEvent.key = static_cast<kkKey>( MapVirtualKey( ( static_cast<UINT>( lParam >> 16) & 255 ), MY_MAPVK_VSC_TO_VK_EX ) );
	//		if( lParam & 0x1000000 )
	//		{
	//			ev.keyboardEvent.key = static_cast<kkKey>(165);
	//		}
	//		//printf("alt = %i\n",(int)ev.keyboardEvent.key);
	//		ev.keyboardEvent.state_is_alt = TRUE;
	//	}

	//	

	//	u8 keys[ 256u ];
	//	GetKeyboardState( keys );
	//	WORD chars[ 2 ];

	//	if( ToAsciiEx( (UINT)wParam, HIWORD(lParam), keys, chars, 0, KEYBOARD_INPUT_HKL ) == 1 )
	//	{
	//		WORD uChar;
	//		MultiByteToWideChar( KEYBOARD_INPUT_CODEPAGE, MB_PRECOMPOSED, (LPCSTR)chars,
	//			sizeof(chars), (WCHAR*)&uChar, 1 );
	//		ev.keyboardEvent.character = uChar;
	//	}
	//	else 
	//	{
	//		ev.keyboardEvent.character = 0;
	//	}

	//	kkGetMainSystem()->addEvent( ev );

	//	if( message == WM_SYSKEYDOWN || message == WM_SYSKEYUP )
	//	{
	//		return DefWindowProc( hWnd, message, wParam, lParam );
	//	}
	//	else
	//	{
	//		return 0;
	//	}
	//}break;
	case WM_SYSCOMMAND:
		if( ( wParam & 0xFFF0 ) == SC_SCREENSAVE ||
			( wParam & 0xFFF0 ) == SC_MONITORPOWER ||
			( wParam & 0xFFF0 ) == SC_KEYMENU)return 0;
	}
	return DefWindowProc( hWnd, message, wParam, lParam );
}
#else
#error For Windows
#endif
// ==================================================== WINDOW END

// ==================================================== Graphics Systems
enum class DemoGSType{
	ModernOpenGL
};
class DemoGS{
public:
	DemoGS(){}
	virtual ~DemoGS(){}
	virtual void InitWindow(DemoWindow* window)=0;
	virtual void SetActive(DemoWindow* window)=0;
	virtual void SetClearColor(float red, float green, float blue, float alpha)=0;
	virtual void BeginDraw( bool clear_canvas )=0;
	virtual void EndDraw()=0;
};
class DemoGSModernOpenGL : public DemoGS{
#ifdef KRGUI_PLATFORM_WINDOWS
	HGLRC m_renderingContext = nullptr;
	HDC m_dc = nullptr;
#endif
public:
	DemoGSModernOpenGL(DemoWindow* mainWindow){
		Kr::loadOpenGLProcs();
#ifdef KRGUI_PLATFORM_WINDOWS
		WNDCLASSEX wc;
		memset(&wc,0,sizeof(WNDCLASSEX));
		wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc   = DefWindowProc;
		wc.hInstance     = GetModuleHandle(0);
		wc.lpszClassName = L"OPENGL";
		wc.cbSize        = sizeof(WNDCLASSEX);
		RegisterClassEx(&wc);
		auto tmp_hwnd = CreateWindowEx(WS_EX_APPWINDOW, wc.lpszClassName, L"q", WS_POPUP,0, 0, 128, 128, NULL, NULL, wc.hInstance, NULL);
		ShowWindow(tmp_hwnd, SW_HIDE);
		HDC dc = GetDC( tmp_hwnd );
		PIXELFORMATDESCRIPTOR pf;
		SetPixelFormat(dc, 1, &pf);
		auto rc = gwglCreateContext(dc);
		gwglMakeCurrent(dc, rc);
		Kr::loadOpenGLProcs();
		//int maxSamples = 0;
		//gglGetIntegerv( GL_MAX_SAMPLES, &maxSamples );
		gwglMakeCurrent(NULL, NULL);
		gwglDeleteContext(rc);
		ReleaseDC(tmp_hwnd, dc);
		DestroyWindow(tmp_hwnd);
		InitWindow(mainWindow);
		m_dc   = ((DemoWindowWin32*)mainWindow)->m_dc;
		int v_maj = 4;
		int v_min = 6;
		while(true){
			int attributeList[5];
			attributeList[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
			attributeList[1] = v_maj;
			attributeList[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
			attributeList[3] = v_min;
			attributeList[4] = 0;// Null terminate the attribute list.
			m_renderingContext = gwglCreateContextAttribsARB(m_dc, 0, attributeList);
			if(m_renderingContext)
				break;
			else{
				--v_min;
				if( v_min < 0 ){
					if( v_maj == 3 )
						break;
					v_min = 6;
					--v_maj;
				}
			}
		}
		fprintf(stdout, "Init OpenGL %i.%i\n", v_maj, v_min);
		SetActive(mainWindow);
		gwglSwapIntervalEXT(1);
		glClearDepth(1.0f);
		glEnable(GL_DEPTH_TEST);
		glFrontFace(GL_CW);
		glViewport(0, 0, mainWindow->GetWindowWidth(), mainWindow->GetWindowHeight());
#else
#error For Windows
#endif
	}
	virtual ~DemoGSModernOpenGL(){}
	virtual void InitWindow(DemoWindow* window){
#ifdef KRGUI_PLATFORM_WINDOWS
		DemoWindowWin32 * w = (DemoWindowWin32*)window;
		auto dc   = w->m_dc;
		if(!dc) return;
		int attributeListInt[] = 
		{
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
			WGL_COLOR_BITS_ARB,     32,
			WGL_DEPTH_BITS_ARB,     24,
			WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
			WGL_SWAP_METHOD_ARB,    WGL_SWAP_EXCHANGE_ARB,
			WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
			WGL_STENCIL_BITS_ARB,   8,
			//WGL_SAMPLE_BUFFERS_ARB, 1,
			//WGL_SAMPLES_ARB,        4,//maxSamples,
			0
		};
		int pixelFormat[1];
		unsigned int formatCount;
		auto result = gwglChoosePixelFormatARB(dc, attributeListInt, NULL, 1, pixelFormat, &formatCount);
		if(result != 1) return;
		PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
		result = SetPixelFormat(dc, pixelFormat[0], &pixelFormatDescriptor);
		if(result != 1) return;
#else
#error For Windows
#endif
	}
	virtual void SetActive(DemoWindow* window){
#ifdef KRGUI_PLATFORM_WINDOWS
		DemoWindowWin32 * w = (DemoWindowWin32*)window;
		m_dc   = w->m_dc;
		gwglMakeCurrent(m_dc, m_renderingContext);
#endif
	}
	virtual void SetClearColor(float red, float green, float blue, float alpha){
		glClearColor( red, green, blue, alpha ); 
	}
	virtual void BeginDraw( bool clear_canvas ){
		if( clear_canvas ){
			GLbitfield mask = GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT;
			glClear(mask);
		}
	}
	virtual void EndDraw(){
#ifdef KRGUI_PLATFORM_WINDOWS
		SwapBuffers( m_dc );
#endif
	}
};
// ==================================================== Graphics Systems END



// ==================================================== MAIN SYSTEM
class DemoSystem{
protected:
	std::vector<DemoWindow*> m_windows;
	float m_dt = 0.f;
public:
	DemoSystem(){}
	virtual ~DemoSystem(){
		for(auto w : m_windows){
			delete w;
		}
	}
	float* GetDeltaTime(){return &m_dt;}
	unsigned long long GetTime(){
		static bool isInit = false;
		static unsigned long long baseTime;
		if( !isInit ){
			auto now = std::chrono::high_resolution_clock::now();
			baseTime = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
			isInit = true;
		}
		auto now = std::chrono::high_resolution_clock::now();
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

		return ms - baseTime;
	}
	virtual DemoWindow * CreateWindow(int x, int y) = 0;
	bool Update(){
		UpdateOS();
		static unsigned long long t1 = 0u;
		unsigned long long t2 = this->GetTime();
		float m_tick = float(t2 - t1);
		t1 = t2;

		m_dt = m_tick / 1000.f;
		return gDemoRun;
	};
	virtual void UpdateOS()=0;
	virtual DemoGS* CreateGS(DemoGSType type, DemoWindow* mainWindow){
		switch (type)
		{
		case DemoGSType::ModernOpenGL:
			return new DemoGSModernOpenGL(mainWindow);
			break;
		default:
			fprintf(stderr,"%s\n","GS type not implement");
			break;
		}
		return nullptr;
	}
};
#ifdef KRGUI_PLATFORM_WINDOWS
class DemoSystemWin32 : public DemoSystem{
public:
	DemoSystemWin32(){}
	virtual ~DemoSystemWin32(){}
	virtual DemoWindow * CreateWindow(int x, int y){
		DemoWindowWin32 * new_window = new DemoWindowWin32(x,y);
		m_windows.push_back(new_window);
		return new_window;
	}
	virtual void UpdateOS(){
		MSG msg;
		while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ){
			GetMessage( &msg, NULL, 0, 0 );
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}
	}
};
DemoSystem* Demo_Create(){
	return new DemoSystemWin32;
}
#else
#error For Windows
#endif
// ==================================================== MAIN SYSTEM END

int main()
{
	DemoSystem * demo = Demo_Create();
	DemoWindow * main_window = demo->CreateWindow(800,600);
	
	DemoGS* gs = demo->CreateGS(DemoGSType::ModernOpenGL, main_window);
	gs->SetClearColor(0.232f, 0.4321f, 0.98f, 1.f);

	Gui::GuiSystem * gui = Gui::CreateSystem(Gui::GraphicsSystemType::OpenGL3, "../res/fonts/noto/", "notosans.txt" );
	if(!gui)
	{
		fprintf(stderr, "%s", "Can't init GUI\n");
		return 666;
	}
	Gui::Window guiWindow_main;
	guiWindow_main.OSWindow = main_window->GetHandle();

	auto defaultFont = gui->getCurrentFont();

	Gui::Font * iconFont = gui->createFontFromDDS_RGBA8(u"../res/blender_icons16.dds");
	iconFont->addCharacter(u'1', Gui::Vec4f(67.f, 29.f, 84.f, 47.f), 0, 0, 0, 602, 640 );
	iconFont->addCharacter(u'2', Gui::Vec4f(256.f, 617.f, 273.f, 635.f), 0, 0, 0, 602, 640 ); // check
	iconFont->addCharacter(u'3', Gui::Vec4f(277.f, 617.f, 294.f, 635.f), 0, 0, 0, 602, 640 ); // check

	gui->setCurrentFont(defaultFont);

	float * dt = demo->GetDeltaTime();

	while(demo->Update())
	{
		gs->BeginDraw(true);
		gs->SetActive(main_window);
		
		gui->newFrame(&guiWindow_main, *dt );
			if( gui->addButton() ) printf("Click\n");
			if( gui->addButton(u"Text", nullptr, Gui::Vec2f(100.f,20.f)) ) printf("Click Text\n");
			
			Gui::Style s;
			s.buttonBackgroundAlpha = 0.f;
			s.buttonTextIdleColor = Gui::ColorLime;
			s.buttonTextHoverColor = Gui::ColorRed;
			if( gui->addButton(u"X", &s, Gui::Vec2f(10.f,10.f)) ) printf("X\n");
			if( gui->addButton(u"Round", nullptr, Gui::Vec2f(100.f,20.f), true, true, Gui::Vec4f(4.f,4.f,4.f,4.f)) ) printf("Click Round\n");
			
			gui->setCurrentFont(iconFont);
			
			Gui::Style s2;
			s2.buttonBackgroundAlpha = 0.f;
			s2.buttonTextIdleColor = Gui::ColorWhite;
			if( gui->addButtonSymbol('1', &s2, Gui::Vec2f(18.f,18.f)) ) printf("Monkey\n");
			
			gui->setCurrentFont(defaultFont);

			static bool ch1 = false;
			Gui::Style s3;
			s3.iconFont = iconFont;
			s3.checkboxCheckSymbol = '2';
			s3.checkboxUncheckSymbol = '3';
			s3.checkboxBoxTextOffset = 0.f;
			s3.buttonTextPositionAdd.y = 3.f;
			gui->addCheckBox(&ch1, &s3, u"checkbox");

		gui->render();

		gui->endFrame();
		gs->EndDraw();
	}


	delete demo;
	return 0;
}