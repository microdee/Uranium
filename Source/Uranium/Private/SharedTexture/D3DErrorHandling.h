// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Uranium.h"

THIRD_PARTY_INCLUDES_START
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/AllowWindowsPlatformAtomics.h"
#include "Windows/PreWindowsApi.h"

#include <comdef.h>

#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformAtomics.h"
#include "Windows/HideWindowsPlatformTypes.h"
THIRD_PARTY_INCLUDES_END

inline FString GetHResultErrorString(HRESULT hr)
{
	_com_error comError(hr);
	FString errorStr(comError.ErrorMessage());
	FString errorDesc(comError.Description().GetBSTR());
	
	return FString::Printf(TEXT("%#8x | %s \n  (%s)"), hr, *errorStr, *errorDesc);
}

inline bool HrFail_Impl(HRESULT hr, FString m, bool& failureMember)
{
	if(SUCCEEDED(hr))
	{
		return failureMember = false;
	}

	FString hrStr = GetHResultErrorString(hr);
	UE_LOG(LogUranium, Error, TEXT("Shared Texture error: %s"), *m);
	UE_LOG(LogUranium, Error, TEXT("HRESULT: %s"), *hrStr);
	PLATFORM_BREAK();
	return failureMember = true;
}

#define HrFail(hr, m) HrFail_Impl(hr, m, bFailure)
#define HrFail_R(hr, m) if(HrFail(hr, m)) return