// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Uranium.h"

THIRD_PARTY_INCLUDES_START
#include "Windows/AllowWindowsPlatformTypes.h"

#define InterlockedDecrement _InterlockedDecrement
#define InterlockedIncrement _InterlockedIncrement
#include <comdef.h>
#undef InterlockedDecrement
#undef InterlockedIncrement

#include "Windows/HideWindowsPlatformTypes.h"
THIRD_PARTY_INCLUDES_END

inline FString GetHResultErrorString(HRESULT Hr)
{
	_com_error ComError(Hr);
	FString ErrorStr(ComError.ErrorMessage());
	FString ErrorDesc(ComError.Description().GetBSTR());
	
	return FString::Printf(TEXT("%#8x | %s \n  (%s)"), Hr, *ErrorStr, *ErrorDesc);
}

inline bool HrFail_Impl(HRESULT Hr, FString M, bool& bFailureMember)
{
	if(SUCCEEDED(Hr)) return bFailureMember = false;

	FString HrStr = GetHResultErrorString(Hr);
	UE_LOG(LogUranium, Error, TEXT("Shared Texture error: %s"), *M);
	UE_LOG(LogUranium, Error, TEXT("HRESULT: %s"), *HrStr);
	PLATFORM_BREAK();
	return bFailureMember = true;
}

#define HrFail(hr, m) HrFail_Impl(hr, m, bFailure)
#define HrFail_R(hr, m) if(HrFail(hr, m)) return