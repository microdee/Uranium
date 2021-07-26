// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "CefBrowserManagedRef.h"

bool UCefBrowserManagedRef::IsAvailable(bool bCheckBrowserReady)
{
	if(!this) return false;
	if(bCheckBrowserReady)
		return IsValidLowLevel() && Obj && Obj->IsBrowserReady();
	return IsValidLowLevel() && Obj;
}