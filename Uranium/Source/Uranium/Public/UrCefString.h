// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CefIncludesStart.h"

#include "include/cef_base.h"

#include "CefIncludesEnd.h"

namespace UrCefString
{
	inline bool Set(cef_string_t* target, FString input)
	{
		return CefString(target).FromString(*input, input.Len(), true);
	}
}