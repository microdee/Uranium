/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"
#include "HAL/Event.h"

#include "UraniumContext.h"
#include "UraniumSubsystem.h"
#include "UObject/Interface.h"

#include "UrCefHandler.generated.h"

class UUrBrowserView;
class CefBrowser;

// This class does not need to be modified.
UINTERFACE()
class UUrHandler : public UInterface
{
	GENERATED_BODY()
};

/**
* Base interface for all Unreal facing handlers;
*/
class URANIUM_API IUrHandler
{
	GENERATED_BODY()
protected:
	TWeakObjectPtr<UUrBrowserView> BrowserView;
	TCefPtr<CefBrowser> Browser;
	
public:

	template<class T>
	class TBadge
	{
		friend T;
	private: TBadge() {}
	};

	void SetBrowserView(TBadge<UUrBrowserView> badge, UUrBrowserView* view);
	void SetBrowser(TBadge<UUrBrowserView> badge, TCefPtr<CefBrowser> browser);

	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	void OnPersistentBrowserViewSet();
	
	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	void OnPersistentBrowserSet();
	
	TWeakObjectPtr<UUrBrowserView> GetBrowserView()
	{
		return BrowserView;
	}
	TCefPtr<CefBrowser> GetBrowser()
	{
		return Browser;
	}
};

template<typename TUrHandler>
IUrHandler* AsUrHandler(TUrHandler* input)
{
	auto output = Cast<IUrHandler>(input);
	checkf(output, TEXT("Uranium Handlers must implement UrHandler interface."));
	return output;
}

template<typename TUrHandler, typename TBodyFunc, typename TReturn>
TReturn InvokeLatentReturn(TUrHandler urHandler, TBodyFunc body, TReturn defaultReturn)
{
	auto retVal = defaultReturn;
	FEvent* semaphore = FPlatformProcess::GetSynchEventFromPool();
	AsyncTask(ENamedThreads::GameThread, [urHandler, body, &retVal, semaphore]()
	{
		if (urHandler.IsValid())
		{
			retVal = body();
		}
		semaphore->Trigger();
	});
	semaphore->Wait();
	FPlatformProcess::ReturnSynchEventToPool(semaphore);
	return retVal;
}

template<typename TUrHandler>
void InvokeLatent(TUrHandler urHandler, TFunction<void()> body)
{
	AsyncTask(ENamedThreads::GameThread, [=]()
	{
		if (urHandler.IsValid())
		{
			body();
		}
	});
}

template<typename TUrHandler, typename TBodyFunc, typename TReturn>
TReturn InvokeLatentWithBrowserReturn(TUrHandler urHandler, TBodyFunc body, TReturn defaultReturn = {})
{
	if(urHandler.IsValid())
	{
		auto handler = Cast<IUrHandler>(urHandler.GetObject());
		check(handler);
		if(UUrBrowserView* view = handler->GetBrowserView().Get())
		{
			return InvokeLatentReturn(urHandler, [view, body]() { return body(view); }, defaultReturn);
		}
	}
	return defaultReturn;
}

template<typename TUrHandler>
void InvokeLatentWithBrowser(TUrHandler urHandler, TFunction<void(UUrBrowserView*)> body)
{
	if(urHandler.IsValid())
	{
		auto handler = Cast<IUrHandler>(urHandler.GetObject());
		check(handler);
		if(UUrBrowserView* view = handler->GetBrowserView().Get())
		{
			InvokeLatent(urHandler, [view, body]() { body(view); });
		}
	}
}

#define EXPAND_PARENTHESIS(...) __VA_ARGS__
#define EXTRA_ARGS_(...) , __VA_ARGS__
#define EXTRA_ARGS__

#define CEF_REDIRECT_EVENT_NO_BROWSER_BODY_I(handlerInterface, handlerMember, handlerEvents, eventName, arguments) \
	IUr##handlerInterface::Execute_##eventName(Ur##handlerMember->_getUObject() EXTRA_ARGS_##arguments); \
	if(auto eventsObject = Get##handlerEvents()) \
	{ \
		eventsObject->eventName##Static.Broadcast(EXPAND_PARENTHESIS##arguments); \
		/*eventsObject->eventName.Broadcast(EXPAND_PARENTHESIS##arguments);*/ \
	} \
	else \
	{ \
		UE_LOG(LogUranium, Warning, TEXT(#handlerEvents) TEXT(" wasn't available. ") TEXT(#eventName) TEXT(" did not get invoked.")); \
	} \

#define CEF_REDIRECT_EVENT_NO_BROWSER_BODY(handlerName, eventName, arguments) \
	CEF_REDIRECT_EVENT_NO_BROWSER_BODY_I(handlerName##Handler, handlerName##Handler, handlerName##Events, eventName, arguments)

#define CEF_REDIRECT_EVENT_NO_BROWSER(handlerName, eventName, arguments) \
	InvokeLatent(Ur##handlerName##Handler, [=]() \
	{ \
		CEF_REDIRECT_EVENT_NO_BROWSER_BODY(handlerName, eventName, arguments) \
	}) \
	
#define CEF_REDIRECT_EVENT_NO_BROWSER_RETURN(handlerName, eventName, arguments, defValue) \
	InvokeLatentReturn(Ur##handlerName##Handler, [=]() \
	{ \
		auto retVal = CEF_REDIRECT_EVENT_NO_BROWSER_BODY(handlerName, eventName, arguments) \
		return retVal; \
	}, defValue) \
	
#define CEF_REDIRECT_EVENT_BODY_I(handlerInterface, handlerMember, handlerEvents, eventName, arguments) \
	IUr##handlerInterface::Execute_##eventName(Ur##handlerMember->_getUObject(), view EXTRA_ARGS_##arguments); \
	if(auto eventsObject = Get##handlerEvents()) \
	{ \
		eventsObject->eventName##Static.Broadcast(view EXTRA_ARGS_##arguments); \
		/*eventsObject->eventName.Broadcast(view EXTRA_ARGS_##arguments);*/ \
	} \
	else \
	{ \
		UE_LOG(LogUranium, Warning, TEXT(#handlerEvents) TEXT(" wasn't available. ") TEXT(#eventName) TEXT(" did not get invoked.")); \
	} \

#define CEF_REDIRECT_EVENT_BODY(handlerName, eventName, arguments) \
	CEF_REDIRECT_EVENT_BODY_I(handlerName##Handler, handlerName##Handler, handlerName##Events, eventName, arguments)

#define CEF_REDIRECT_EVENT(handlerName, eventName, arguments) \
	InvokeLatentWithBrowser(Ur##handlerName##Handler, [=](UUrBrowserView* view) \
	{ \
		CEF_REDIRECT_EVENT_BODY(handlerName, eventName, arguments) \
	}) \

#define CEF_REDIRECT_EVENT_RETURN(handlerName, eventName, arguments, defValue) \
	InvokeLatentWithBrowserReturn(Ur##handlerName##Handler, [=](UUrBrowserView* view) \
	{ \
		auto retVal = CEF_REDIRECT_EVENT_BODY(handlerName, eventName, arguments) \
		return retVal; \
	}, defValue)
