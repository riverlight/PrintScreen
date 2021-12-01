// hook.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <Windows.h>
#include <iostream>
#include <string.h>

#include "utils.h"

const char* g_szDir = "d:/workroom/testroom/snapshot";
const char* g_szPrefix = "lol";
int g_nStartCount = 0;


class MyHook {
public:
	//single ton
	static MyHook& Instance() {
		static MyHook myHook;
		return myHook;
	}
	void InstallHook(); // function to install our hook
	void UninstallHook(); // function to uninstall our hook

	MSG msg; // struct with information about all messages in our queue
	int Messsages(); // function to "deal" with our messages 

	HHOOK keyboardhook;
};

LRESULT WINAPI MyKeyBoardCallback(int nCode, WPARAM wParam, LPARAM lParam);

 
int MyHook::Messsages() {
	while (msg.message != WM_QUIT) { //while we do not close our application
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		Sleep(1);
	}
	UninstallHook(); //if we close, let's uninstall our hook
	return (int)msg.wParam; //return the messages
}
void MyHook::InstallHook() {
	/*
	SetWindowHookEx(
	WM_MOUSE_LL = mouse low level hook type,
	MyMouseCallback = our callback function that will deal with system messages about mouse
	NULL, 0);
	c++ note: we can check the return SetWindowsHookEx like this because:
	If it return NULL, a NULL value is 0 and 0 is false.
	*/
	
	if (!(keyboardhook == SetWindowsHookEx(WH_KEYBOARD_LL, MyKeyBoardCallback, NULL, 0)))
	{
		printf_s("Error: %x \n", GetLastError());
	}
}

void MyHook::UninstallHook() {
	/*
	uninstall our hook using the hook handle
	*/
	UnhookWindowsHookEx(keyboardhook);
}

#define  IsKeyPressed(nVirtKey)     ((GetKeyState(nVirtKey) & (1<<(sizeof(SHORT)*8-1))) != 0)

LRESULT WINAPI MyKeyBoardCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	KBDLLHOOKSTRUCT* pKeyStruct = (KBDLLHOOKSTRUCT*)lParam;

	if (nCode == 0)
	{
		if (pKeyStruct)
		{
			//printf_s("Virtual Key Code: %d \n", pKeyStruct->vkCode);
		}

		if (WM_KEYUP == wParam || WM_SYSKEYUP == wParam)
		{
			if (pKeyStruct->vkCode == VK_SNAPSHOT)
			{
				if (IsKeyPressed(VK_MENU))
				{
					HBITMAP hBmp;
					//HDIB hDIB;
					if (OpenClipboard(NULL)) {
						if ((hBmp = (HBITMAP)GetClipboardData(CF_BITMAP)) != NULL) {
							//将这个hBmp写入文件，你总会吧。
							char szFilename[200] = "\0";
							sprintf_s(szFilename, "%s/%s_%d.bmp", g_szDir, g_szPrefix, g_nStartCount++);
							printf_s("save to %s...\n", szFilename);

                            SaveBitmapToFile(hBmp, szFilename);
							CloseClipboard();
						}
					}
					printf_s("snapshot...\n");
				} else
				{
					
					printf_s("no alt\n");
				}
			}
		}
	}
	return CallNextHookEx(MyHook::Instance().keyboardhook, nCode, wParam, lParam);
}

int main()
{
    std::cout << "Hello World!\n";

	MyHook::Instance().InstallHook();
	return MyHook::Instance().Messsages();

}

