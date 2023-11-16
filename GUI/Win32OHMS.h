/**
* @file    Win32OHMS.h
* @author  Tyler Parret True (OwlHowlinMornSky) <mysteryworldgod@outlook.com>
*
* @section LICENSE
*
*                   GNU AFFERO GENERAL PUBLIC LICENSE
*                      Version 3, 19 November 2007
*
*    Copyright (c) 2023  Tyler Parret True
* 
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU Affero General Public License as published
*    by the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Affero General Public License for more details.
*
*    You should have received a copy of the GNU Affero General Public License
*    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*
*/
#pragma once

#include <functional>
#include <Windows.h>

//#define TIMER_SLAPSE (33)
#define TIMER_SLAPSE (17)

std::function<void()> OnIdle;
std::function<void()> OnEnterSYSLOOP;
std::function<void()> OnExitSYSLOOP;
std::function<void()> OnSizing;

LRESULT CALLBACK myWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

bool MyRegistClass(HINSTANCE hInst) {
	WNDCLASSEXW wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.hbrBackground = CreateSolidBrush(RGB(0x77, 0x77, 0x77));// (HBRUSH)(COLOR_WINDOWTEXT);
	wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wcex.hInstance = hInst;
	wcex.lpfnWndProc = myWndProc;
	wcex.lpszClassName = L"WINDOWS_OHMS_CLASS_PVZANIMATION";
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	return (RegisterClassExW(&wcex) != 0);
}

void CALLBACK TimerProc(HWND hWnd, UINT message, UINT_PTR nTimerid, DWORD systemTick) {
	OnIdle();
	return;
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
	case WM_CREATE: {
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

	case WM_GETMINMAXINFO: {
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
			case HTMAXBUTTON: {
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
