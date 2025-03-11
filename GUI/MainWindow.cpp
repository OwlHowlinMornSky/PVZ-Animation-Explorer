#include "MainWindow.h"

#include <functional>
#include <Windows.h>
#include "resource.h"

namespace {

//#define TIMER_SLAPSE (33)
#define TIMER_SLAPSE (17)

std::function<void()> OnIdle;
std::function<void()> OnEnterSYSLOOP;
std::function<void()> OnExitSYSLOOP;
std::function<void()> OnSizing;
std::function<void(bool, std::wstring_view)> OnOpenClose;

LRESULT CALLBACK myWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

bool MyRegistClass(HINSTANCE hInst) {
	WNDCLASSEXW wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.hbrBackground = CreateSolidBrush(RGB(0x77, 0x77, 0x77));// (HBRUSH)(COLOR_WINDOWTEXT);
	wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wcex.hInstance = hInst;
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDR_MENU1);
	wcex.lpfnWndProc = myWndProc;
	wcex.lpszClassName = L"WINDOWS_OHMS_CLASS_PVZANIMATION";
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	return (RegisterClassExW(&wcex) != 0);
}

void CALLBACK TimerProc(HWND hWnd, UINT message, UINT_PTR nTimerid, DWORD systemTick) {
	OnIdle();
	return;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

LRESULT CALLBACK myWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static WPARAM l_lastHitNC = HTNOWHERE;
	static UINT_PTR l_timerID = 0;
	static POINT l_mintrsize = { 120, 90 };
	static POINT l_maxtrsize = { 1920, 1080 };

	// Don't forward the menu system command, so that pressing ALT or F10 doesn't steal the focus
	if ((message == WM_SYSCOMMAND) && (wParam == SC_KEYMENU))
		return 0;

	switch (message) {
	case WM_CREATE:
	{
		RECT rect = { 0, 0, l_maxtrsize.x, l_maxtrsize.y };
		AdjustWindowRect(&rect, static_cast<DWORD>(GetWindowLongPtr(hWnd, GWL_STYLE)), false);
		l_maxtrsize = { rect.right - rect.left, rect.bottom - rect.top };
		rect = { 0, 0, l_mintrsize.x, l_mintrsize.y };
		AdjustWindowRect(&rect, static_cast<DWORD>(GetWindowLongPtr(hWnd, GWL_STYLE)), false);
		l_mintrsize = { rect.right - rect.left, rect.bottom - rect.top };
		break;
	}

	case WM_CLOSE:
		break;

	case WM_ERASEBKGND:
		break;

	case WM_GETMINMAXINFO:
	{
		MINMAXINFO* info = (MINMAXINFO*)lParam;
		info->ptMinTrackSize = l_mintrsize;
		info->ptMaxTrackSize = l_maxtrsize;
		break;
	}

	case WM_NCMOUSEMOVE:
		if (l_lastHitNC == HTCAPTION) {
			l_lastHitNC = HTNOWHERE;
			PostMessageW(hWnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, lParam);
			PostMessageW(hWnd, WM_MOUSEMOVE, MK_LBUTTON, 0);
		}
		else return DefWindowProcW(hWnd, message, wParam, lParam);
		break;

	case WM_NCLBUTTONDOWN:
		if (l_lastHitNC == HTNOWHERE && (wParam == HTCAPTION || wParam == HTCLOSE ||
			wParam == HTMAXBUTTON || wParam == HTMINBUTTON || wParam == HTHELP)) {
			l_lastHitNC = wParam;
			PostMessageW(hWnd, WM_ACTIVATE, WA_CLICKACTIVE, 0);
		}
		else {
			l_lastHitNC = HTNOWHERE;
			return DefWindowProcW(hWnd, message, wParam, lParam);
		}
		break;

	case WM_NCLBUTTONUP:
		if (l_lastHitNC == wParam) {
			l_lastHitNC = HTNOWHERE;
			switch (wParam) {
			case HTMAXBUTTON:
			{
				bool wndMaxed = GetWindowLongPtrW(hWnd, GWL_STYLE) & WS_MAXIMIZE;
				PostMessageW(hWnd, WM_SYSCOMMAND, wndMaxed ? SC_RESTORE : SC_MAXIMIZE, lParam);
				break;
			}
			case HTMINBUTTON:
				PostMessageW(hWnd, WM_SYSCOMMAND, SC_MINIMIZE, lParam);
				break;
			case HTCLOSE:
				PostMessageW(hWnd, WM_SYSCOMMAND, SC_CLOSE, lParam);
				break;
			case HTHELP:
				break;
			default:
				return DefWindowProcW(hWnd, message, wParam, lParam);
			}
		}
		else {
			l_lastHitNC = HTNOWHERE;
			return DefWindowProcW(hWnd, message, wParam, lParam);
		}
		break;

	case WM_NCRBUTTONDOWN:
		if (l_lastHitNC == HTNOWHERE && (wParam == HTCAPTION || wParam == HTCLOSE ||
			wParam == HTMAXBUTTON || wParam == HTMINBUTTON || wParam == HTHELP)) {
			l_lastHitNC = wParam | 0x8000;
			PostMessageW(hWnd, WM_ACTIVATE, WA_CLICKACTIVE, 0);
		}
		else
			l_lastHitNC = HTNOWHERE;
		break;

	case WM_NCRBUTTONUP:
		if (l_lastHitNC & 0x8000) {
			l_lastHitNC = HTNOWHERE;
			PostMessageW(hWnd, WM_CONTEXTMENU, 0, lParam);
		}
		else
			l_lastHitNC = HTNOWHERE;
		break;

	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 分析菜单选择:
		switch (wmId) {
		case ID_40001:
		{
			WCHAR buffer[MAX_PATH] = L"\0";
			OPENFILENAMEW of{};
			of.lStructSize = sizeof(of);
			of.hwndOwner = hWnd;
			of.hInstance = GetModuleHandleW(NULL);
			of.lpstrFilter = L"PVZ\0*.reanim;\0Any File\0*.*\0\0";
			of.lpstrFile = buffer;
			of.nMaxFile = MAX_PATH;
			of.Flags = OFN_EXPLORER;
			if (0 == GetOpenFileNameW(&of)) {
				break;
			}
			OnOpenClose(true, buffer);
			break;
		}
		case ID_40002:
			OnOpenClose(false, L"");
			break;
		case ID_40003:
			break;
		case ID_40004:
			DialogBoxW(GetModuleHandleW(NULL), MAKEINTRESOURCE(IDD_DIALOG1), hWnd, About);
			break;
		//case IDM_EXIT:
		//	DestroyWindow(hWnd);
		//	break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;

	case WM_ENTERMENULOOP:
		if (l_timerID) {
			KillTimer(0, l_timerID);
		}
		l_timerID = SetTimer(0, 0, TIMER_SLAPSE, (TIMERPROC)TimerProc);
		OnEnterSYSLOOP();
		break;

	case WM_EXITMENULOOP:
		if (l_timerID) {
			KillTimer(0, l_timerID);
		}
		l_timerID = 0;
		OnExitSYSLOOP();
		break;

	case WM_MOVING:
		break;

	case WM_ENTERSIZEMOVE:
		if (l_timerID) {
			KillTimer(0, l_timerID);
		}
		l_timerID = SetTimer(0, 0, TIMER_SLAPSE, (TIMERPROC)TimerProc);
		OnEnterSYSLOOP();
		break;

	case WM_EXITSIZEMOVE:
		if (l_timerID) {
			KillTimer(0, l_timerID);
		}
		l_timerID = 0;
		OnExitSYSLOOP();
		break;

	case WM_NCMOUSELEAVE:
		l_lastHitNC = HTNOWHERE;
		return DefWindowProcW(hWnd, message, wParam, lParam);
		break;

	default:
		return DefWindowProcW(hWnd, message, wParam, lParam);
	}
	return 0;
}

}

MainWindow::MainWindow() {
	test = nullptr;
}

MainWindow::~MainWindow() {
	delete test;
	test = nullptr;
}

void MainWindow::init() {
	initialize_callback();
	initialize_font_text();
	initialize_animTrans();
	initialize_texture_coord();
	update_textTrans();
	update_textPos(INFINITY, 0.0f);
	timeScale = 10;
	update_textTimeScale();

	initialize_window();
}

void MainWindow::run() {
	// Message Pump
	MSG msg = { 0 };
	running = true;
	sfclock.restart();
	while (running) {
		if (PeekMessageW(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		else {
			myIdle();
		}
	}
	app.close();
}

void MainWindow::initialize_callback() {
	OnIdle = std::bind(&MainWindow::myIdle, this);
	OnEnterSYSLOOP = std::bind(&MainWindow::myEnterSYSLOOP, this);
	OnExitSYSLOOP = std::bind(&MainWindow::myExitSYSLOOP, this);
	OnOpenClose = std::bind(&MainWindow::onOpenClose, this, std::placeholders::_1, std::placeholders::_2);
}

void MainWindow::initialize_font_text() {
	fontMono.loadFromFile("CascadiaMono-Regular.otf");
	textFrame.setFont(fontMono);
	textFrame.setFillColor(sf::Color::White);
	textFrame.setCharacterSize(24);
	textFrame.setPosition(12.0f, 7.0f);
	textFrame.setOutlineColor(sf::Color::Black);
	textFrame.setOutlineThickness(2.5f);

	textTrans.setFont(fontMono);
	textTrans.setFillColor(sf::Color::White);
	textTrans.setCharacterSize(18);
	textTrans.setPosition(14.0f, 36.0f);
	textTrans.setOutlineColor(sf::Color::Black);
	textTrans.setOutlineThickness(2.0f);

	textTimeScale.setFont(fontMono);
	textTimeScale.setFillColor(sf::Color::White);
	textTimeScale.setCharacterSize(18);
	textTimeScale.setPosition(14.0f, 78.0f);
	textTimeScale.setOutlineColor(sf::Color::Black);
	textTimeScale.setOutlineThickness(2.0f);

	textPos.setFont(fontMono);
	textPos.setFillColor(sf::Color::White);
	textPos.setCharacterSize(18);
	textPos.setPosition(14.0f, 104.0f);
	textPos.setOutlineColor(sf::Color::Black);
	textPos.setOutlineThickness(2.0f);

	return;
}

void MainWindow::initialize_window() {
	HINSTANCE hInst = GetModuleHandleW(NULL);
	if (!MyRegistClass(hInst)) {
		MessageBoxW(0, L"Regist Class: Failed!", L"Error", MB_ICONERROR);
		throw std::exception();
	}
	HWND hWnd0 = CreateWindowW(L"WINDOWS_OHMS_CLASS_PVZANIMATION", L"PVZ Animation Explorer",
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
		0, 0, hInst, 0);
	if (!hWnd0) {
		MessageBoxW(NULL, L"Create Window: Failed!", L"Error", MB_ICONERROR);
		throw std::exception();
	}

	app.create(hWnd0, sf::ContextSettings(0, 0, 8));
	app.setSize({ 800, 600 });
	app.setView(sf::View({ 0, 0, 800.0f, 600.0f }));

	update_view();
	return myExitSYSLOOP();
}

void MainWindow::initialize_texture_coord() {
	sf::Image tmp;
	tmp.create(100, 100, sf::Color(0, 0, 0, 0));
	for (int i = 1; i < 10; ++i) {
		for (int j = 0; j < 100; ++j) {
			tmp.setPixel(i * 10, j, sf::Color(0, 0, 0, 0x22));
			tmp.setPixel(j, i * 10, sf::Color(0, 0, 0, 0x22));
		}
	}
	for (int i = 0; i < 100; ++i) {
		tmp.setPixel(50, i, sf::Color(0, 0, 0, 0x55));
		tmp.setPixel(i, 50, sf::Color(0, 0, 0, 0x55));
	}
	for (int i = 0; i < 100; ++i) {
		tmp.setPixel(0, i, sf::Color(0, 0, 0, 0xcc));
		tmp.setPixel(i, 0, sf::Color(0, 0, 0, 0xcc));
	}
	texBackgnd.loadFromImage(tmp);
	texBackgnd.setRepeated(true);
	texBackgnd.generateMipmap();
	return;
}

void MainWindow::initialize_animTrans() {
	animTrans = sf::Transform::Identity;
	animTrans.translate(100.0f, 120.0f);
	scaleRatio = 100.0f;
	rotateRatio = 0.0f;
	return update_textTrans();
}

void MainWindow::update_textTrans() {
	wchar_t tmp[64];
	swprintf_s(tmp, 64, L"Scale: %.1f%%.\nRotate: %.1f°.", scaleRatio, rotateRatio);
	return textTrans.setString(tmp);
}

void MainWindow::update_textPos(float px, float py) {
	if (px == INFINITY && py == 0.0f)
		return textPos.setString("x: -, y:-.");
	char tmp[64];
	sprintf_s(tmp, 64, "x: %.2f, y: %.2f.", px, py);
	return textPos.setString(tmp);
}

void MainWindow::update_textTimeScale() {
	char tmp[64];
	sprintf_s(tmp, 64, "Speed: %.0f%%.", timeScale / 0.1f);
	return textTimeScale.setString(tmp);
}

void MainWindow::update_view() {
	viewSize = app.getSize();
}

void MainWindow::onOpenClose(bool isOpen, std::wstring_view file) {
	if (isOpen) {
		testdata.load(file);
		testdata.listControlTrack();
		printf_s("LIST:\n\n");
		testdata.listTrack();
		test = testdata.create(true);
	}
	else {
		delete this->test;
		this->test = nullptr;
		testdata.clear();
	}
}

void MainWindow::myEnterSYSLOOP() {
	return app.setFramerateLimit(0);
}

void MainWindow::myExitSYSLOOP() {
	return app.setFramerateLimit(60);
}

void MainWindow::myIdle() {
	static sf::Vector2f l_mouseLastPos;
	static bool l_pressing = false;

	sf::Event evt;
	while (app.pollEvent(evt)) {
		switch (evt.type) {
		case sf::Event::Closed:
			running = false;
			break;
		case sf::Event::Resized:
			app.setView(sf::View({ 0, 0, (float)evt.size.width, (float)evt.size.height }));
			update_view();
			break;
		case sf::Event::KeyPressed:
			switch (evt.key.code) {
			case sf::Keyboard::R:
				initialize_animTrans();
				break;
			case sf::Keyboard::Left:
				if (!test)
					break;
				if (timeScale > 0)
					--timeScale;
				test->setTimeScale(timeScale / 10.0f);
				update_textTimeScale();
				break;
			case sf::Keyboard::Right:
				if (!test)
					break;
				if (timeScale < 100)
					++timeScale;
				test->setTimeScale(timeScale / 10.0f);
				update_textTimeScale();
				break;
			default:
				break;
			}
			break;
		case sf::Event::MouseWheelScrolled:
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
				sf::Vector2f mPos((float)evt.mouseWheelScroll.x, (float)evt.mouseWheelScroll.y);
				mPos = animTrans.getInverse().transformPoint(mPos);
				float ratio = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ? 2.0f : 1.0f;
				ratio = -3.0f * ratio * evt.mouseWheelScroll.delta;
				animTrans.rotate(ratio, mPos);
				rotateRatio -= ratio;
				if (rotateRatio <= -0.0f) rotateRatio += 360.0f;
				rotateRatio = std::fmodf(rotateRatio, 360.0f);
			}
			else {
				sf::Vector2f mPos((float)evt.mouseWheelScroll.x, (float)evt.mouseWheelScroll.y);
				mPos = animTrans.getInverse().transformPoint(mPos);
				float ratio = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ? 1.21f : 1.1f;
				ratio = std::powf(ratio, evt.mouseWheelScroll.delta);
				float fratio = scaleRatio * ratio;
				if (fratio > 10000.0f) {
					fratio = 10000.0f;
					ratio = 10000.0f / scaleRatio;
				}
				else if (fratio < 5.0f) {
					fratio = 5.0f;
					ratio = 5.0f / scaleRatio;
				}
				animTrans.scale(sf::Vector2f(ratio, ratio), mPos);
				scaleRatio = fratio;
			}
			update_textTrans();
			break;
		}
		case sf::Event::MouseButtonPressed:
			if (evt.mouseButton.button == sf::Mouse::Left) {
				l_mouseLastPos.x = (float)evt.mouseButton.x;
				l_mouseLastPos.y = (float)evt.mouseButton.y;
				l_pressing = true;
			}
			break;
		case sf::Event::MouseButtonReleased:
			if (evt.mouseButton.button == sf::Mouse::Left) {
				l_pressing = false;
			}
			break;
		case sf::Event::MouseMoved:
		{
			sf::Vector2f mPos((float)evt.mouseMove.x, (float)evt.mouseMove.y);
			sf::Transform IV = animTrans.getInverse();
			sf::Vector2f truePos(IV.transformPoint(mPos));
			if (l_pressing) {
				animTrans.translate(truePos - IV.transformPoint(l_mouseLastPos));
				l_mouseLastPos = mPos;
			}
			update_textPos(truePos.x, truePos.y);
			break;
		}
		case sf::Event::MouseLeft:
			update_textPos(INFINITY, 0.0f);
			break;
		default:
			break;
		}
	}

	float dt = sfclock.restart().asSeconds();

	if (test) {
		if (timeScale > 0)
			test->update(dt);

		textFrame.setString(test->getFrameInfoString());
	}

	app.clear(sf::Color(0xCC, 0xCC, 0xCC));

	{
		sf::Vector2f p[4];
		p[0] = { 0.0f, 0.0f };
		p[1] = { (float)viewSize.x, 0.0f };
		p[2] = { (float)viewSize.x, (float)viewSize.y };
		p[3] = { 0.0f, (float)viewSize.y };
		sf::VertexArray va;
		for (int i = 0; i < 4; ++i) {
			sf::Vertex vertex;
			vertex.position = p[i];
			vertex.texCoords = animTrans.getInverse().transformPoint(p[i]) + sf::Vector2f(0.5f, 0.5f);
			va.append(vertex);
		}
		va.setPrimitiveType(sf::Quads);
		if (scaleRatio >= 500.0f)
			texBackgnd.setSmooth(false);
		else
			texBackgnd.setSmooth(true);
		app.draw(va, sf::RenderStates(&texBackgnd));
	}

	if (test) {
		app.draw(*test, { animTrans });
	}

	app.draw(textFrame);
	app.draw(textTrans);
	app.draw(textTimeScale);
	app.draw(textPos);

	return app.display();
}
