// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "CefBrowserManagedRef.h"

bool UCefBrowserManagedRef::IsAvailable(bool checkBrowserReady)
{
	if(!this)
	{
		return false;
	}
	
	if(checkBrowserReady)
	{
		return IsValidLowLevel() && Obj && Obj->IsBrowserReady();
	}
	
	return IsValidLowLevel() && Obj;
}