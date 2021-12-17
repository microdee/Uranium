/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"

#include "CefIncludesStart.h"
#include "include/cef_base.h"
#include "CefIncludesEnd.h"

#pragma warning(disable : 4946) // reinterpret_cast used between related classes

struct FCefPtrCounters
{
	TFunction<void()> AddRef;
	TFunction<bool()> Release;
};

/**
 * Alternative to CefRefPtr which would be compatible with forward declaration
 * To keep declaration info minimal we cannot know anything about the target class including its
 * implementation of CEF refcounting. Because of this assignment or conversion from pointer is
 * not directly supported. Please use AsCefPtr(...) and NewCefObject(...) functions when assigning.
 */
template<typename TCefObject>
struct TCefPtr
{
	using FSelf = TCefPtr<TCefObject>;
	using FType = TCefObject;
	
	TCefObject* Object = nullptr;
	TOptional<FCefPtrCounters> Counters;

	bool IsValid() const
	{
		return Object && Counters.IsSet();
	}

	/** return true when this was the last reference and the wrapped object is deleted from memory */
	bool Reset()
	{
		bool result = false;
		if(Object && Counters)
		{
			result = Counters->Release();
		}
		Counters.Reset();
		Object = nullptr;
		return result;
	}

	void Set(TCefObject* object, FCefPtrCounters counters)
	{
		if(IsValid())
		{
			Reset();
		}
		if(object)
		{
			Counters = counters;
			Object = object;
			Counters->AddRef();
		}
	}

	TCefObject* Get() const
	{
		return IsValid()
			? reinterpret_cast<TCefObject*>(Object) : nullptr;
	}

	TCefPtr()
	{
	}
	
	TCefPtr(TCefObject* object, FCefPtrCounters counters)
	{
		Set(object, counters);
	}

	TCefPtr(const FSelf& other)
	{
		Set(other.Get(), other.Counters.Get({}));
	}

	TCefPtr(FSelf&& other) noexcept
	{
		Set(other.Get(), other.Counters.Get({}));
	}

	~TCefPtr()
	{
		Reset();
	}

	FSelf& operator=(const FSelf& rhs)
	{
		Set(rhs.Get(), rhs.Counters.Get({}));
		return *this;
	}
	
	FSelf& operator=(FSelf&& rhs) noexcept
	{
		Set(rhs.Get(), rhs.Counters.Get({}));
		return *this;
	}

	bool operator==(const FSelf& rhs)
	{
		return Get() == rhs.Get();
	}

	bool operator==(TCefObject* rhs)
	{
		return Get() == rhs;
	}

	TCefObject* operator->() const
	{
		return Get();
	}

	operator bool() const
	{
		return IsValid();
	}

	operator TCefObject*()
	{
		return Get();
	}
};

namespace CefPtr_Private
{
	template<typename TCefObject>
	FORCEINLINE void CefAddRef(TCefObject* object)
	{
		object->AddRef();
	}
	
	template<typename TCefObject>
	FORCEINLINE bool CefReleaseRef(TCefObject* object)
	{
		return object->Release();
	}
	
	template<typename TCefObject>
	struct TMakeCefPtr
	{
		TCefObject* Object = nullptr;
		FCefPtrCounters Counters {};
	
		TMakeCefPtr(TCefObject* object) : Object(object)
		{
			static_assert(
				TPointerIsConvertibleFromTo<TCefObject, CefBaseRefCounted>::Value,
				"Target class doesn't implement CefBaseRefCounted"
			);
		
			Counters.AddRef = [object]() { CefAddRef(object); };
			Counters.Release = [object]() { return CefReleaseRef(object); };
		}

		operator TCefPtr<TCefObject>()
		{
			return TCefPtr<TCefObject>(Object, Counters);
		}
	};
}

template<typename TCefObject>
FORCEINLINE CefPtr_Private::TMakeCefPtr<TCefObject> AsCefPtr(TCefObject* object)
{
	return CefPtr_Private::TMakeCefPtr<TCefObject>(object);
}

template<typename TCefObject>
FORCEINLINE CefPtr_Private::TMakeCefPtr<TCefObject> AsCefPtr(CefRefPtr<TCefObject> object)
{
	return CefPtr_Private::TMakeCefPtr<TCefObject>(object.get());
}

template<typename TCefObject, typename ... TArgs>
FORCEINLINE CefPtr_Private::TMakeCefPtr<TCefObject> NewCefObject(TArgs... args)
{
	return CefPtr_Private::TMakeCefPtr<TCefObject>(new TCefObject(args...));
}