// Fill out your copyright notice in the Description page of Project Settings.


#include "UraniumContext.h"

FString IUraniumContext::GetCefVersion()
{
	return TEXT(CEF_VERSION);
}

FString IUraniumContext::GetChromiumVersion()
{
	std::wstringstream ver;
	ver << CHROME_VERSION_MAJOR << L"."
		<< CHROME_VERSION_MINOR << L"."
		<< CHROME_VERSION_BUILD << L"."
		<< CHROME_VERSION_PATCH;

	return FString(ver.str().c_str());
}