/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#if !defined(__UNREAL__)

#include "UraniumCefApp.h"
#include "CefModuleLoader.h"
#include <filesystem>
#include <Windows.h>
#include <winternl.h>

int WINAPI WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd )
{
	FScopedCefModuleLoader moduleLoader;
	int result = UraniumCefApp::Main(hInstance);
	return result;
}

#endif