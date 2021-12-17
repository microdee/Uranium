/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#include "UrBrowserView.h"

#include "CefBrowserClient.h"
#include "HardwareInfo.h"
#include "UraniumContext.h"
#include "UraniumSubsystem.h"
#include "Uranium.h"
#include "UrBrowserComposition.h"
#include "Handlers/UrAudioHandler.h"
#include "Handlers/UrCefHandler.h"
#include "Handlers/UrContextMenuHandler.h"
#include "Handlers/UrDevToolsHandler.h"
#include "Handlers/UrDialogHandler.h"
#include "Handlers/UrDisplayHandler.h"
#include "Handlers/UrDownloadHandler.h"
#include "Handlers/UrDragHandler.h"
#include "Handlers/UrFindHandler.h"
#include "Handlers/UrFocusHandler.h"
#include "Handlers/UrJSDialogHandler.h"
#include "Handlers/UrKeyboardHandler.h"
#include "Handlers/UrLifeSpanHandler.h"
#include "Handlers/UrLoadHandler.h"
#include "Handlers/UrRenderHandler.h"
#include "Handlers/UrRequestHandler.h"
#include "Handlers/UrWindowedDevToolsHandler.h"

struct FUrBrowserViewDetails
{
	template<typename TUrHandler>
	TUrHandler* GetHandler(const UUrBrowserView* self)
	{
		using UClassType = typename TUrHandler::UClassType;
		if(ensureAlways(self->Handlers.Contains(UClassType::StaticClass())))
		{
			return Cast<TUrHandler>(self->Handlers[UClassType::StaticClass()]);
		}
		return nullptr;
	}
};

UObject* UUrBrowserView::CreateHandler(UClass* handlerClass, UClass* ofInterface)
{
	checkf(handlerClass->ImplementsInterface(UUrHandler::StaticClass()),
		TEXT("Handlers must implement UrHandler interface")
	);
	
	checkf(handlerClass->ImplementsInterface(ofInterface),
		TEXT("%s must implement %s interface"),
		*handlerClass->GetName(),
		*ofInterface->GetName()
	);
	
	UObject* handler = nullptr;

	if(UniqueHandlers.Contains(handlerClass))
	{
		handler = UniqueHandlers[handlerClass];
		Handlers.Add(ofInterface, handler);
		return handler;
	}
	
	handler = NewObject<UObject>(this, handlerClass);
	
	UniqueHandlers.Add(handlerClass, handler);
	Handlers.Add(ofInterface, handler);
	
	auto handlerInterface = Cast<IUrHandler>(handler);
	handlerInterface->SetBrowserView({}, this);

	OnHandlerCreated(handler, ofInterface);
	
	return handler;
}

void UUrBrowserView::OnCreated_Implementation()
{
}

void UUrBrowserView::OnHandlerCreated_Implementation(UObject* handler, UClass* ofInterface)
{
}

void UUrBrowserView::OnCreatedInternal(UObject* parent, UUrBrowserComposition* composition)
{
	Parent = parent;
	Composition = composition;

	if(auto browserParent = Cast<UUrBrowserView>(parent))
	{
		browserParent->AddChild(this);
	}

	AudioHandler = CreateHandler(composition->AudioHandler, UUrAudioHandler::StaticClass());
	ContextMenuHandler = CreateHandler(composition->ContextMenuHandler, UUrContextMenuHandler::StaticClass());
	DialogHandler = CreateHandler(composition->DialogHandler, UUrDialogHandler::StaticClass());
	DisplayHandler = CreateHandler(composition->DisplayHandler, UUrDisplayHandler::StaticClass());
	DownloadHandler = CreateHandler(composition->DownloadHandler, UUrDownloadHandler::StaticClass());
	DragHandler = CreateHandler(composition->DragHandler, UUrDragHandler::StaticClass());
	FindHandler = CreateHandler(composition->FindHandler, UUrFindHandler::StaticClass());
	FocusHandler = CreateHandler(composition->FocusHandler, UUrFocusHandler::StaticClass());
	JSDialogHandler = CreateHandler(composition->JSDialogHandler, UUrJSDialogHandler::StaticClass());
	KeyboardHandler = CreateHandler(composition->KeyboardHandler, UUrKeyboardHandler::StaticClass());
	LifeSpanHandler = CreateHandler(composition->LifeSpanHandler, UUrLifeSpanHandler::StaticClass());
	LoadHandler = CreateHandler(composition->LoadHandler, UUrLoadHandler::StaticClass());
	RenderResourceHandler = CreateHandler(composition->RenderResourceHandler, UUrRenderResourceHandler::StaticClass());
	RenderEventsHandler = CreateHandler(composition->RenderEventsHandler, UUrRenderEventsHandler::StaticClass());
	RequestHandler = CreateHandler(composition->RequestHandler, UUrRequestHandler::StaticClass());
	DevToolsHandler = CreateHandler(composition->DevToolsHandler, UUrDevToolsHandler::StaticClass());

	if(auto renderEventsHandler = Cast<IUrRenderEventsHandler>(RenderEventsHandler))
	{
		if(auto renderResourceHandler = Cast<IUrRenderResourceHandler>(RenderResourceHandler))
		{
			renderEventsHandler->AssignCefRenderHandler(renderResourceHandler->CefRenderHandler);
		}
	}

	FCoreDelegates::OnEndFrame.AddUObject(this, &UUrBrowserView::HandleEndFrame);

	OnCreated();
}

void UUrBrowserView::HandleEndFrame()
{
	bIsAlreadyTicked.store(false);
}

void UUrBrowserView::OnAfterDeferredInitialized_Implementation()
{
}

UUrBrowserView::UUrBrowserView(const FObjectInitializer& oi)
	: Super(oi)
{
}

TScriptInterface<IUrAudioHandler> UUrBrowserView::GetAudioInterface() const
{
	return AudioHandler;
}

TScriptInterface<IUrContextMenuHandler> UUrBrowserView::GetContextMenuInterface() const
{
	return ContextMenuHandler;
}

TScriptInterface<IUrDialogHandler> UUrBrowserView::GetDialogInterface() const
{
	return DialogHandler;
}

TScriptInterface<IUrDisplayHandler> UUrBrowserView::GetDisplayInterface() const
{
	return DisplayHandler;
}

TScriptInterface<IUrDownloadHandler> UUrBrowserView::GetDownloadInterface() const
{
	return DownloadHandler;
}

TScriptInterface<IUrDragHandler> UUrBrowserView::GetDragInterface() const
{
	return DragHandler;
}

TScriptInterface<IUrFindHandler> UUrBrowserView::GetFindInterface() const
{
	return FindHandler;
}

TScriptInterface<IUrFocusHandler> UUrBrowserView::GetFocusInterface() const
{
	return FocusHandler;
}

TScriptInterface<IUrJSDialogHandler> UUrBrowserView::GetJSDialogInterface() const
{
	return JSDialogHandler;
}

TScriptInterface<IUrKeyboardHandler> UUrBrowserView::GetKeyboardInterface() const
{
	return KeyboardHandler;
}

TScriptInterface<IUrLifeSpanHandler> UUrBrowserView::GetLifeSpanInterface() const
{
	return LifeSpanHandler;
}

TScriptInterface<IUrLoadHandler> UUrBrowserView::GetLoadInterface() const
{
	return LoadHandler;
}

TScriptInterface<IUrRenderResourceHandler> UUrBrowserView::GetRenderResourceInterface() const
{
	return RenderResourceHandler;
}

TScriptInterface<IUrRenderEventsHandler> UUrBrowserView::GetRenderEventsInterface() const
{
	return RenderEventsHandler;
}

TScriptInterface<IUrRequestHandler> UUrBrowserView::GetRequestInterface() const
{
	return RequestHandler;
}

TScriptInterface<IUrDevToolsHandler> UUrBrowserView::GetDevToolsInterface() const
{
	return DevToolsHandler;
}

void UUrBrowserView::SetCefBrowser(TBadge<IUrLifeSpanHandler> badge, TCefPtr<CefBrowser> browser)
{
	Browser = browser;
	bBrowserValid = true;

	for(auto handler : UniqueHandlers)
	{
		auto handlerInterface = Cast<IUrHandler>(handler.Value);
		handlerInterface->SetBrowser({}, Browser);
	}
}

void UUrBrowserView::InvalidateBrowser(TBadge<IUrLifeSpanHandler> badge)
{
	if (auto parentBrowser = Cast<UUrBrowserView>(Parent.Get()))
	{
		parentBrowser->RemoveChild(this);
		for (auto child : Children)
		{
			parentBrowser->AddChild(child);
		}
		Parent.Reset();
	}
	
	bBrowserValid = false;
	Browser.Reset();
}

void UUrBrowserView::RemoveChild(UUrBrowserView* child)
{
	Children.Remove(child);
}

void UUrBrowserView::AddChild(UUrBrowserView* child)
{
	Children.Add(child);
	child->Parent = this;
}

UUrBrowserView* UUrBrowserView::CreateNew(
	UObject* parent,
	TSubclassOf<UUrBrowserComposition> compositionClass,
	TSubclassOf<UUrBrowserView> explicitViewClass,
	UUrBrowserView* templateObject
) {
	IUraniumContext* context = UUraniumSubsystem::GetContext();
	if (!context->IsCefRunning())
	{
		UE_LOG(LogUranium, Error, TEXT("Uranium context is not running. Browser is not created"));
		return nullptr;
	}
	
	if(!parent)
	{
		parent = UUraniumSubsystem::Get();
	}
	UUrBrowserComposition* compositionTemplate = nullptr;
	
	if(!compositionClass)
	{
		if(parent->Implements<UUrBrowserCompositor>())
		{
			compositionTemplate = IUrBrowserCompositor::Execute_GetComposition(parent);
		}
		else
		{
			compositionTemplate = UUraniumSubsystem::Get()->GetDefaultBrowserComposition();
		}
	}
	else
	{
		compositionTemplate = GetMutableDefault<UUrBrowserComposition>(compositionClass);
	}

	checkf(compositionTemplate, TEXT("Couldn't determine browser composition"));
	
	auto composition = NewObject<UUrBrowserComposition>(
		parent,
		compositionTemplate->GetClass(),
		NAME_None, RF_NoFlags,
		compositionTemplate
	);
	
	if(explicitViewClass)
	{
		composition->BrowserView = explicitViewClass;
	}

	auto newBrowser = NewObject<UUrBrowserView>(
		parent,
		composition->BrowserView,
		NAME_None, RF_NoFlags,
		templateObject
	);
	newBrowser->OnCreatedInternal(parent, composition);
	return newBrowser;
}

UUrBrowserView* UUrBrowserView::CreateNewUraniumBrowser(
	FOnBeforeCreatedFunc onBeforeCreated,
	FOnAfterCreatedFunc onAfterCreated,
	UObject* parentIn,
	TSubclassOf<UUrBrowserComposition> compositionClass,
	TSubclassOf<UUrBrowserView> explicitViewClass,
	UUrBrowserView* templateObject
) {
	IUraniumContext* context = UUraniumSubsystem::GetContext();
	if (!context->IsCefRunning())
	{
		UE_LOG(LogUranium, Error, TEXT("Uranium context is not running. Browser is not created"));
		return nullptr;
	}
	
	UUrBrowserView* result = CreateNew(parentIn, compositionClass, explicitViewClass, templateObject);
	
	onBeforeCreated.ExecuteIfBound(result);
	UUrLifeSpanEvents::GetEvents(result->LifeSpanHandler)->OnAfterCreatedStatic.AddLambda([=](UUrBrowserView* view)
	{
		onAfterCreated.ExecuteIfBound(view);
	});
	return result;
}

void UUrBrowserView::DeferredInit(FVector2D initialSize, FString url)
{
	DeferredInit(initialSize, url, [=](FVector2D, FString, TCefPtr<FCefBrowserClient>)
	{
		FUrBrowserViewDetails details;
		if(auto cefLifeSpan = details.GetHandler<IUrLifeSpanHandler>(this)->CefLifeSpanHandler)
		{
			if(cefLifeSpan->CreateNewCefBrowser(this, initialSize, url, CefClient))
			{
				UE_LOG(LogUranium, Verbose, TEXT("Created CEF browser. (%s)"), *url);
			}
			else
			{
				UE_LOG(LogUranium, Error, TEXT("Couldn't create CEF browser. (%s) This browser view will not function correctly"), *url);
			}
		}
	});
}

void UUrBrowserView::DeferredInit(
	FVector2D initialSize, FString url,
	TFunction<void(FVector2D, FString, TCefPtr<FCefBrowserClient>)> browserCreation
) {
	if(CefClient)
	{
		return;
	}

	FUrBrowserViewDetails details;

	CefClient = NewCefObject<FCefBrowserClient>(FCefHandlers {
		AsCefPtr(details.GetHandler<IUrAudioHandler>(this)->CefAudioHandler),
		AsCefPtr(details.GetHandler<IUrContextMenuHandler>(this)->CefContextMenuHandler),
		AsCefPtr(details.GetHandler<IUrDialogHandler>(this)->CefDialogHandler),
		AsCefPtr(details.GetHandler<IUrDisplayHandler>(this)->CefDisplayHandler),
		AsCefPtr(details.GetHandler<IUrDownloadHandler>(this)->CefDownloadHandler),
		AsCefPtr(details.GetHandler<IUrDragHandler>(this)->CefDragHandler),
		AsCefPtr(details.GetHandler<IUrFindHandler>(this)->CefFindHandler),
		AsCefPtr(details.GetHandler<IUrFocusHandler>(this)->CefFocusHandler),
		AsCefPtr(details.GetHandler<IUrJSDialogHandler>(this)->CefJSDialogHandler),
		AsCefPtr(details.GetHandler<IUrKeyboardHandler>(this)->CefKeyboardHandler),
		AsCefPtr(details.GetHandler<IUrLifeSpanHandler>(this)->CefLifeSpanHandler),
		AsCefPtr(details.GetHandler<IUrLoadHandler>(this)->CefLoadHandler),
		AsCefPtr(details.GetHandler<IUrRenderResourceHandler>(this)->CefRenderHandler),
		AsCefPtr(details.GetHandler<IUrRequestHandler>(this)->CefRequestHandler)
	}, FOnProcessMessageReceived() /* TODO: handle process messages */);

	details.GetHandler<IUrRenderResourceHandler>(this)->SetMainSize(initialSize);

	browserCreation(initialSize, url, CefClient);
	OnAfterDeferredInitialized();
}

void UUrBrowserView::Close(bool force)
{
	if(Browser)
	{
		bClosing = true;
		Browser->GetHost()->CloseBrowser(force);
	}
}

UUrBrowserComposition* UUrBrowserView::GetComposition_Implementation()
{
	return Composition ? Composition : UUraniumSubsystem::Get()->GetDefaultBrowserComposition();
}

void UUrBrowserView::BeginDestroy()
{
	Super::BeginDestroy();
	
	if (!bClosing)
	{
		Close();
	}
}

bool UUrBrowserView::IsReadyForFinishDestroy()
{
	if(Browser)
	{
		if(auto lsUrHandler = Cast<IUrLifeSpanHandler>(LifeSpanHandler))
		{
			if(auto lsCefHandler = lsUrHandler->CefLifeSpanHandler)
			{
				return lsCefHandler->IsClosed();
			}
		}
	}
	return true;
}

bool UUrBrowserView::IsDestructionThreadSafe() const
{
	return true;
}

bool UUrBrowserView::IsSupportedForNetworking() const
{
	return false;
}

void UUrBrowserView::GetMainSize(FVector2D& desiredMainSize, FVector2D& actualMainSize) const
{
	if(Browser)
	{
		FUrBrowserViewDetails details;
		auto renderHandler = details.GetHandler<IUrRenderResourceHandler>(this);
		desiredMainSize = renderHandler->GetMainSize();
		if(UTexture2D* texture = renderHandler->GetMainTexture())
		{
			actualMainSize = {
				static_cast<float>(texture->GetSizeX()),
				static_cast<float>(texture->GetSizeY())
			};
		}
		else
		{
			actualMainSize = {0,0};
		}
	}
}

FVector2D UUrBrowserView::GetNativePopupSize() const
{
	if(Browser)
	{
		return IUrRenderEventsHandler::Execute_GetNativePopupSize(RenderEventsHandler);
	}
	return {0,0};
}

FVector2D UUrBrowserView::GetNativePopupLocation() const
{
	if(Browser)
	{
		return IUrRenderEventsHandler::Execute_GetNativePopupLocation(RenderEventsHandler);
	}
	return {0,0};
}

bool UUrBrowserView::GetNativePopupShown() const
{
	if(Browser)
	{
		return IUrRenderEventsHandler::Execute_IsNativePopupVisible(RenderEventsHandler);
	}
	return false;
}

UTexture2D* UUrBrowserView::GetMainTexture() const
{
	if(Browser)
	{
		FUrBrowserViewDetails details;
		auto renderHandler = details.GetHandler<IUrRenderResourceHandler>(this);
		return renderHandler->GetMainTexture();
	}
	return nullptr;
}

UTexture2D* UUrBrowserView::GetNativePopupTexture() const
{
	if(Browser)
	{
		FUrBrowserViewDetails details;
		auto renderHandler = details.GetHandler<IUrRenderResourceHandler>(this);
		return renderHandler->GetNativePopupTexture();
	}
	return nullptr;
}

bool UUrBrowserView::IsBrowserValid() const
{
	return Browser.IsValid();
}

bool UUrBrowserView::IsTextureVerticallyMirrored() const
{
	if(RenderResourceHandler)
	{
		return GetRenderResourceInterface()->IsTextureVerticallyMirrored();
	}
	return UR_WITH_SHARED_TEXTURE;
}

void UUrBrowserView::SetMainSize(FVector2D size)
{
	if(Browser)
	{
		size = FMath::Max(size, FVector2D(128, 128));
		FUrBrowserViewDetails details;
		auto renderHandler = details.GetHandler<IUrRenderResourceHandler>(this);
		renderHandler->SetMainSize(size);
	}
}

void UUrBrowserView::Tick(float deltaSeconds, bool recursive /*= false*/)
{

	if(!bIsAlreadyTicked.load())
	{
#if 0 // TODO: expose to be more modular
		if(Browser)
		{
			Browser->GetHost()->SendExternalBeginFrame();
		}
#endif
		
		bIsAlreadyTicked.store(true);

		if(auto renderHandler = Cast<IUrRenderResourceHandler>(RenderResourceHandler))
		{
			renderHandler->TickRenderResource();
		}
	}

	if (recursive)
	{
		for (UUrBrowserView* br : Children)
		{
			br->Tick(deltaSeconds, true);
		}
	}
}

int UUrBrowserView::GetIdentifier() const
{
	return Browser ? Browser->GetIdentifier() : 0;
}

bool UUrBrowserView::IsSame(UUrBrowserView* other) const
{
	if(this == other) return true;
	if (!this->IsValidLowLevelFast(false) && !other->IsValidLowLevelFast(false))
	{
		return true;
	}
	
	if (!other || !other->IsValidLowLevelFast(false)) return false;
	if (!this || !this->IsValidLowLevelFast(false)) return false;
	return Browser->IsSame(other->Browser.Get());
}

bool UUrBrowserView::IsPopup() const
{
	return Browser ? Browser->IsPopup() : false;
}

bool UUrBrowserView::HasDocument() const
{
	return Browser ? Browser->HasDocument() : false;
}

void UUrBrowserView::LoadUrl(FString url)
{
	if (Browser)
	{
		Browser->GetMainFrame()->LoadURL(*url);
	}
}

void UUrBrowserView::LoadHtml(const FHtmlContent& htmlContent)
{
	if (Browser)
	{
		auto loadContentOnLoadEnd = [this](int)
		{
			//TODO: use resource handlers implementation
		};
		Browser->GetMainFrame()->LoadURL("about:blank");
	}
}

void UUrBrowserView::ExecuteJavaScript(FString code, FString scriptUrl, int startLine /*= 0*/)
{
	if (Browser)
	{
		Browser->GetMainFrame()->ExecuteJavaScript(*code, *scriptUrl, startLine);
	}
}

FString UUrBrowserView::GetURL() const
{
	return Browser
		? FString(Browser->GetMainFrame()->GetURL().c_str())
		: TEXT("error:no-browser");
}

void UUrBrowserView::SetZoomLevel(float zoom)
{
	if (Browser)
	{
		Browser->GetHost()->SetZoomLevel(zoom);
	}
}

void UUrBrowserView::GoBack()
{
	if (Browser)
	{
		Browser->GoBack();
	}
}

void UUrBrowserView::GoForward()
{
	if (Browser)
	{
		Browser->GoForward();
	}
}

void UUrBrowserView::Reload()
{
	if (Browser)
	{
		Browser->Reload();
	}
}

void UUrBrowserView::ReloadIgnoreCache()
{
	if (Browser)
	{
		Browser->ReloadIgnoreCache();
	}
}

bool UUrBrowserView::IsLoading() const
{
	return Browser ? Browser->IsLoading() : true;
}

void UUrBrowserView::ShowDevTools(FVector2D inspectElementAt, FVector2D size)
{
	if (Browser)
	{
		IUrDevToolsHandler::Execute_ShowDevTools(DevToolsHandler,
			this,
			inspectElementAt,
			static_cast<int>(size.X),
			static_cast<int>(size.Y)
		);
	}
}

void UUrBrowserView::ShowDevToolsWindow(FVector2D inspectElementAt)
{
	if(Browser && GEngine->IsEditor())
	{
		// we can immediately discard this object
		auto windowedDevtoolsHandler = NewObject<UUrWindowedDevToolsHandler>();
	
		auto handlerInterface = Cast<IUrHandler>(windowedDevtoolsHandler);
		handlerInterface->SetBrowserView({}, this);
		handlerInterface->SetBrowser({}, Browser);
		IUrDevToolsHandler::Execute_ShowDevTools(windowedDevtoolsHandler, this, inspectElementAt, 0, 0);
	}
}

void UUrBrowserView::CloseDevTools()
{
	if (Browser)
	{
		Browser->GetHost()->CloseDevTools();
	}
}

bool UUrBrowserView::HasDevTools() const
{
	return Browser ? Browser->GetHost()->HasDevTools() : false;
}

void UUrBrowserView::SetAudioMuted(bool mute)
{
	if (Browser)
	{
		Browser->GetHost()->SetAudioMuted(mute);
	}
}

bool UUrBrowserView::IsAudioMuted() const
{
	return Browser ? Browser->GetHost()->IsAudioMuted() : false;
}

void UUrBrowserView::SendKeyEvent(FKeyEvent event, bool isUp, FCefKeyEventFlags modifiers)
{
	SendKeyEventKc(
		event.GetKeyCode(),
		static_cast<TCHAR>(event.GetCharacter()),
		event.IsAltDown(),
		isUp,
		UrInput::EnforceFlagsFromEvent(event, modifiers)
	);
}

void UUrBrowserView::SendKey(int windowsKeyCode, FString character, bool isUp, FCefKeyEventFlags modifiers)
{
	SendKeyEventKc(
		static_cast<uint32>(windowsKeyCode),
		character.IsEmpty() ? '\0' : character[0],
		modifiers.AltDown,
		isUp,
		modifiers
	);
}

#ifndef MAPVK_VK_TO_VSC
#define MAPVK_VK_TO_VSC (0)
#endif

void UUrBrowserView::SendKeyEventKc(uint32 keyCode, TCHAR character, bool isSysKey, bool isUp, FCefKeyEventFlags modifiers)
{
	if (!Browser)
	{
		return;
	}

	// Some languages use AltGr for special characters, which registers as Alt + Ctrl keypresses
	// In order to avoid triggering shortcuts like Ctrl+V while typing text, when the right Alt(Gr)
	// is pressed, do not send explicit key events (only send character)
	if (modifiers.AltGrDown)
	{
		return;
	}

	uint32 scanCode = MapVirtualKey(keyCode, MAPVK_VK_TO_VSC);

	LPARAM lParam = 1 | (scanCode & 0xFF) << 16; // Scan Code in lParam bits 16..23
	if (isUp)
	{
		// (bit 30) The previous key state. The value is always 1 for a WM_KEYUP message.
		// (bit 31) The transition state. The value is always 1 for a WM_KEYUP message.
		lParam |= (3 << 30);
	}
	else
	{
		// (bit 30) The previous key state.
		//		The value is 1 if the key is down before the message is sent,
		//		or it is zero if the key is up.
		// (bit 31) The transition state. The value is always 0 for a WM_KEYDOWN message.
		lParam |= (1 << 30);
	}

	CefKeyEvent cefEvent;
	cefEvent.windows_key_code = keyCode;
	cefEvent.character = character;
	cefEvent.is_system_key = isSysKey;
	cefEvent.native_key_code = lParam;
	cefEvent.type = isUp ? KEYEVENT_KEYUP : KEYEVENT_KEYDOWN;
	cefEvent.unmodified_character = character;
	cefEvent.modifiers = modifiers.GetEventFlags();
	Browser->GetHost()->SendKeyEvent(cefEvent);
}

void UUrBrowserView::SendCharacterKey(FCharacterEvent event, FCefKeyEventFlags modifiers)
{
	SendCharKc(
		event.GetCharacter(),
		event.IsAltDown(),
		UrInput::EnforceFlagsFromEvent(event, modifiers)
	);
}

void UUrBrowserView::SendKeyString(FString input, FCefKeyEventFlags modifiers)
{
	for(const TCHAR& Char : input)
	{
		SendCharKc(Char, modifiers.AltDown, modifiers);
	}
}

void UUrBrowserView::SendCharKc(TCHAR character, bool isSysKey, FCefKeyEventFlags modifiers)
{
	if (!Browser)
	{
		return;
	}

	CefKeyEvent cefEvent;

#if PLATFORM_WINDOWS
	uint32 keyCode = VkKeyScan(character) & 0xff;
 	uint32 scanCode = MapVirtualKey(keyCode, MAPVK_VK_TO_VSC);
	LPARAM lParam = 1 | ((scanCode & 0xFF) << 16);
	cefEvent.native_key_code = lParam;
#endif
	
	cefEvent.character = character;
	cefEvent.is_system_key = isSysKey;
	cefEvent.windows_key_code = character;
	cefEvent.type = KEYEVENT_CHAR;
	cefEvent.unmodified_character = character;
	cefEvent.modifiers = modifiers.GetEventFlags();
	Browser->GetHost()->SendKeyEvent(cefEvent);
}

CefMouseEvent UUrBrowserView::GetCefMouseEvent(FVector2D location, bool uvSpace, const FCefKeyEventFlags& modifiers) const
{
	FVector2D mainSize = {1, 1};
	if(Browser)
	{
		if(auto renderHandler = Cast<IUrRenderResourceHandler>(RenderResourceHandler))
		{
			mainSize = renderHandler->GetMainSize();
		}
	}
	
	CefMouseEvent cefEvent;
	cefEvent.x = uvSpace ? location.X * mainSize.X : location.X;
	cefEvent.y = uvSpace ? location.Y * mainSize.Y : location.Y;
	cefEvent.modifiers = modifiers.GetEventFlags();
	return cefEvent;
}

void UUrBrowserView::SendMouseMove(FVector2D location, bool uvSpace, bool leaving, FCefKeyEventFlags modifiers)
{
	if (Browser)
	{
		Browser->GetHost()->SendMouseMoveEvent(GetCefMouseEvent(location, uvSpace, modifiers), leaving);
	}
}

void UUrBrowserView::SendMouseButton(FVector2D location, bool uvSpace, const FKey& button, bool isUp, FCefKeyEventFlags modifiers)
{
	if (Browser && UrInput::IsValidMouseButton(button))
	{
		Browser->GetHost()->SendMouseClickEvent(
			GetCefMouseEvent(location, uvSpace, modifiers),
			static_cast<CefBrowserHost::MouseButtonType>(UrInput::GetUraniumMouseButton(button)),
			isUp, 1
		);
	}
}

void UUrBrowserView::SendMouseWheel(FVector2D location, bool uvSpace, FVector2D wheelDelta, FCefKeyEventFlags modifiers)
{
	if (Browser)
	{
		Browser->GetHost()->SendMouseWheelEvent(GetCefMouseEvent(location, uvSpace, modifiers), wheelDelta.X, wheelDelta.Y);
	}
}

void UUrBrowserView::SendTouchEvent(FUrPointerEvent event, FCefKeyEventFlags modifiers)
{
	if (!Browser)
	{
		return;
	}
	
	FVector2D mainSize = {1, 1};
	if(auto renderHandler = Cast<IUrRenderResourceHandler>(RenderResourceHandler))
	{
		mainSize = renderHandler->GetMainSize();
	}
	
	CefTouchEvent cefEvent;
	cefEvent.id = event.Id.GetValue();
	if (event.IsUVSpace)
	{
		FVector2D uvloc = event.Location * mainSize;
		cefEvent.x = uvloc.X;
		cefEvent.y = uvloc.Y;
	}
	else
	{
		cefEvent.x = event.Location.X;
		cefEvent.y = event.Location.Y;
	}
	cefEvent.radius_x = event.Size.X;
	cefEvent.radius_y = event.Size.Y;
	cefEvent.pressure = event.Pressure;
	cefEvent.rotation_angle = FMath::DegreesToRadians(event.RotationDegrees);
	cefEvent.pointer_type = static_cast<cef_pointer_type_t>(event.PointerType);
	cefEvent.type = static_cast<cef_touch_event_type_t>(event.EventType);
	cefEvent.modifiers = modifiers.GetEventFlags();
	Browser->GetHost()->SendTouchEvent(cefEvent);
}
