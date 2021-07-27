// MESO Digital Interiors GmbH. (C) 2020-

#include "UrBrowserView.h"

#include "CefBrowserView.h"
#include "HardwareInfo.h"
#include "UraniumContext.h"
#include "UraniumSubsystem.h"
#include "Uranium.h"
#include "CefBrowserManagedRef.h"
#include "SharedTexture/SharedTextureInterface.h"

void UUrBrowserView::OnBeforeCloseInternal()
{
	Instance = nullptr;
	bBrowserValid = false;

	if (Parent)
	{
		Parent->RemoveChild(this);
		for (auto child : Children)
		{
			Parent->AddChild(child);
		}
		Parent = nullptr;
	}
	else
	{
		for (auto child : Children)
		{
			child->Close();
		}
	}
}

UUrBrowserView::UUrBrowserView(const FObjectInitializer& oi)
	: Super(oi)
{
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

UUrBrowserView* UUrBrowserView::CreateNew(UObject* worldContext)
{
	IUraniumContext* context = UUraniumSubsystem::GetContext();
	if (!context->IsCefRunning())
	{
		UE_LOG(LogUranium, Warning, TEXT("Uranium context is not running. Browser is not created"));
		return nullptr;
	}

	auto res = NewObject<UUrBrowserView>(worldContext);
	res->Instance = NewObject<UCefBrowserManagedRef>(worldContext);
	res->Parent = Cast<UUrBrowserView>(worldContext);
	if (res->Parent)
	{
		res->Parent->AddChild(res);
	}
	return res;
}

UUrBrowserView* UUrBrowserView::CreateNewUraniumBrowser(UObject* worldContext, FOnBeforeCreatedDel onBeforeCreated, FOnAfterCreatedFuncDel onAfterCreated)
{
	IUraniumContext* context = UUraniumSubsystem::GetContext();
	if (!context->IsCefRunning())
	{
		UE_LOG(LogUranium, Warning, TEXT("Uranium context is not running. Browser is not created"));
		return nullptr;
	}

	auto res = NewObject<UUrBrowserView>(worldContext);
	res->Parent = Cast<UUrBrowserView>(worldContext);
	onBeforeCreated.ExecuteIfBound(res);
	if (res->Parent)
	{
		res->Parent->AddChild(res);
	}
	if (onAfterCreated.IsBound())
	{
		res->OnAfterCreated.Add(onAfterCreated);
	}
		
	return res;
}

void UUrBrowserView::DeferredInit(FVector2D initialSize, FString url)
{
	if(Instance->Obj)
	{
		return;
	}
	
	MainSize = initialSize;
	Instance->Obj = FCefBrowserView::CreateNew(initialSize, url, this);
	bBrowserValid = Instance->Obj.get() != nullptr;

	if (bBrowserValid)
	{
		MainTexture = ISharedTexture::CreateSharedTexture();
		NativePopupTexture = ISharedTexture::CreateSharedTexture();
	}
}

void UUrBrowserView::DeferredInit(UCefBrowserManagedRef* fromInstance)
{
	if (Instance->Obj)
	{
		return;
	}
	
	bBrowserValid = fromInstance != nullptr;

	if (bBrowserValid)
	{
		MainSize = fromInstance->Obj->vMainSize;
		fromInstance->Obj->UrWrapper = this;
		Instance = fromInstance;
		MainTexture = ISharedTexture::CreateSharedTexture();
		NativePopupTexture = ISharedTexture::CreateSharedTexture();
	}
}

void UUrBrowserView::Close()
{
	if (bClosing)
	{
		return;
	}
	if (!Instance->IsValidLowLevel())
	{
		return;
	}
	
	bClosing = true;

	if(Instance->IsAvailable())
	{
		Instance->Obj->Close();
	}

	Instance = nullptr;
}

void UUrBrowserView::BeginDestroy()
{
	Super::BeginDestroy();
	if (!bClosing)
	{
		Close();
	}
}

void UUrBrowserView::GetMainSize(FVector2D& desiredMainSize, FVector2D& actualMainSize) const
{
	desiredMainSize = MainSize;
	if(MainTexture)
	{
		actualMainSize = FVector2D(MainTexture->Width, MainTexture->Height);
	}
	else
	{
		actualMainSize = FVector2D::ZeroVector;
	}
}

UTexture2D* UUrBrowserView::GetMainTexture() const
{
	if(!MainTexture)
	{
		return nullptr;
	}
	return MainTexture->GetTexture();
}

UTexture2D* UUrBrowserView::GetNativePopupTexture() const
{
	if (!NativePopupTexture)
	{
		return nullptr;
	}
	return NativePopupTexture->GetTexture();
}

void UUrBrowserView::SetMainSize(FVector2D size, bool force /*= false*/)
{
	if (!Instance->IsAvailable())
	{
		return;
	}
	
	size = FMath::Max(size, FVector2D(128, 128));
	if (MainSize != size || Instance->Obj->vMainSize != size || force)
	{
		MainSize = size;
		Instance->Obj->vMainSize = size;
		Instance->Obj->BrowserHost->WasResized();
	}
}

void UUrBrowserView::Tick(float deltaSeconds, bool recursive /*= false*/)
{
#if SEND_EXTERNAL_BEGINFRAME
	if (!Instance->IsAvailable())
	{
		return;
	}
	Instance->Obj->BrowserHost->SendExternalBeginFrame();
#endif

	if(MainTexture)
	{
		MainTexture->Render();
	}

	if (bPopupShown && NativePopupTexture)
	{
		NativePopupTexture->Render();
	}

	if (recursive)
	{
		for (UUrBrowserView* br : Children)
		{
			br->Tick(deltaSeconds);
		}
	}
}

int UUrBrowserView::GetIdentifier() const
{
	if (!Instance->IsAvailable())
	{
		return 0;
	}
	return Instance->Obj->Browser->GetIdentifier();
}

bool UUrBrowserView::IsSame(UUrBrowserView* other) const
{
	if (!this->IsValidLowLevelFast(false) && !other->IsValidLowLevelFast(false))
	{
		return true;
	}
	
	if (!other->IsValidLowLevelFast(false)) return false;
	if (!this->IsValidLowLevelFast(false)) return false;
	if (!Instance->Obj && !other->Instance->Obj) return true;
	if (!Instance->Obj && other->Instance->Obj) return false;
	if (Instance->Obj && !other->Instance->Obj) return false;
	if (!Instance->IsAvailable() && !other->Instance->IsAvailable()) return true;
	return Instance->Obj->Browser->IsSame(other->Instance->Obj->Browser);
}

bool UUrBrowserView::IsPopup() const
{
	if(!Instance->IsAvailable())
	{
		return false;
	}
	return Instance->Obj->Browser->IsPopup();
}

bool UUrBrowserView::HasDocument() const
{
	if (!Instance->IsAvailable())
	{
		return false;
	}
	return Instance->Obj->Browser->HasDocument();
}

void UUrBrowserView::LoadUrl(FString url)
{
	if (!Instance->IsAvailable())
	{
		return;
	}
	Instance->Obj->Browser->GetMainFrame()->LoadURL(*url);
}

void UUrBrowserView::LoadHtml(const FHtmlContent& htmlContent)
{
	if (!Instance->IsAvailable())
	{
		return;
	}
	auto loadContentOnLoadEnd = [this](int)
	{
		//TODO: use resource handlers implementation
	};
	Instance->Obj->Browser->GetMainFrame()->LoadURL("about:blank");
}

void UUrBrowserView::ExecuteJavaScript(FString code, FString scriptUrl, int startLine /*= 0*/)
{
	if (!Instance->IsAvailable())
	{
		return;
	}
	Instance->Obj->Browser->GetMainFrame()->ExecuteJavaScript(*code, *scriptUrl, startLine);
}

FString UUrBrowserView::GetURL() const
{
	if (!Instance->IsAvailable())
	{
		return TEXT("error:no-browser");
	}
	return FString(Instance->Obj->Browser->GetMainFrame()->GetURL().c_str());
}

void UUrBrowserView::SetZoomLevel(float zoom)
{
	if (!Instance->IsAvailable())
	{
		return;
	}
	Instance->Obj->BrowserHost->SetZoomLevel(zoom);
}

void UUrBrowserView::GoBack()
{
	if (!Instance->IsAvailable())
	{
		return;
	}
	Instance->Obj->Browser->GoBack();
}

void UUrBrowserView::GoForward()
{
	if (!Instance->IsAvailable())
	{
		return;
	}
	Instance->Obj->Browser->GoForward();
}

void UUrBrowserView::Reload()
{
	if (!Instance->IsAvailable())
	{
		return;
	}
	Instance->Obj->Browser->Reload();
}

void UUrBrowserView::ReloadIgnoreCache()
{
	if (!Instance->IsAvailable())
	{
		return;
	}
	Instance->Obj->Browser->ReloadIgnoreCache();
}

bool UUrBrowserView::IsLoading() const
{
	if (!Instance->IsAvailable())
	{
		return true;
	}
	return Instance->Obj->Browser->IsLoading();
}

void UUrBrowserView::ShowDevToolsWindow(FVector2D inspectElementAt)
{
	if (!Instance->IsAvailable())
	{
		return;
	}
	Instance->Obj->ShowDevToolsWindow(inspectElementAt);
}

void UUrBrowserView::ShowDevToolsEmbedded(FVector2D inspectElementAt, FVector2D size)
{
	if (!Instance->IsAvailable())
	{
		return;
	}
	Instance->Obj->ShowDevToolsEmbedded(inspectElementAt, size.X, size.Y);
}

void UUrBrowserView::CloseDevTools()
{
	if (!Instance->IsAvailable())
	{
		return;
	}
	Instance->Obj->BrowserHost->CloseDevTools();
}

bool UUrBrowserView::HasDevTools() const
{
	if (!Instance->IsAvailable())
	{
		return false;
	}
	return Instance->Obj->BrowserHost->HasDevTools();
}

void UUrBrowserView::SetAudioMuted(bool mute)
{
	if (!Instance->IsAvailable())
	{
		return;
	}
	Instance->Obj->BrowserHost->SetAudioMuted(mute);
}

bool UUrBrowserView::IsAudioMuted() const
{
	if (!Instance->IsAvailable())
	{
		return false;
	}
	return Instance->Obj->BrowserHost->IsAudioMuted();
}

void UUrBrowserView::SendKeyEvent(FKeyEvent event, bool isUp, FCefKeyEventFlags modifiers)
{
	SendKeyEventKc(
		event.GetKeyCode(),
		static_cast<TCHAR>(event.GetCharacter()),
		event.IsAltDown(),
		isUp,
		EnforceFlagsFromEvent(event, modifiers)
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

void UUrBrowserView::SendKeyEventKc(uint32 keyCode, TCHAR character, bool isSysKey, bool isUp, FCefKeyEventFlags modifiers)
{
	if (!Instance->IsAvailable())
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

	uint32 scanCode = MapVirtualKeyA(keyCode, MAPVK_VK_TO_VSC);

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
	Instance->Obj->BrowserHost->SendKeyEvent(cefEvent);
}

void UUrBrowserView::SendCharacterKey(FCharacterEvent event, FCefKeyEventFlags modifiers)
{
	SendCharKc(
		event.GetCharacter(),
		event.IsAltDown(),
		EnforceFlagsFromEvent(event, modifiers)
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
	if (!Instance->IsAvailable())
	{
		return;
	}

	uint32 keyCode = VkKeyScanA(character) & 0xff;
	uint32 scanCode = MapVirtualKeyA(keyCode, MAPVK_VK_TO_VSC);
	LPARAM lParam = 1 | ((scanCode & 0xFF) << 16);

	CefKeyEvent cefEvent;
	cefEvent.character = character;
	cefEvent.is_system_key = isSysKey;
	cefEvent.windows_key_code = character;
	cefEvent.native_key_code = lParam;
	cefEvent.type = KEYEVENT_CHAR;
	cefEvent.unmodified_character = character;
	cefEvent.modifiers = modifiers.GetEventFlags();
	Instance->Obj->BrowserHost->SendKeyEvent(cefEvent);
}

CefMouseEvent UUrBrowserView::GetCefMouseEvent(FVector2D location, bool uvSpace, const FCefKeyEventFlags& modifiers) const
{
	CefMouseEvent cefEvent;
	cefEvent.x = uvSpace ? location.X * MainSize.X : location.X;
	cefEvent.y = uvSpace ? location.Y * MainSize.Y : location.Y;
	cefEvent.modifiers = modifiers.GetEventFlags();
	return cefEvent;
}

void UUrBrowserView::SendMouseMove(FVector2D location, bool uvSpace, bool leaving, FCefKeyEventFlags modifiers)
{
	if (!Instance->IsAvailable())
	{
		return;
	}
	Instance->Obj->BrowserHost->SendMouseMoveEvent(GetCefMouseEvent(location, uvSpace, modifiers), leaving);
}

void UUrBrowserView::SendMouseButton(FVector2D location, bool uvSpace, const FKey& button, bool isUp, FCefKeyEventFlags modifiers)
{
	if (!Instance->IsAvailable())
	{
		return;
	}
	if (!UraniumInput::IsValidMouseButton(button))
	{
		return;
	}

	Instance->Obj->BrowserHost->SendMouseClickEvent(
		GetCefMouseEvent(location, uvSpace, modifiers),
		UraniumInput::GetUraniumMouseButton(button),
		isUp, 1
	);
}

void UUrBrowserView::SendMouseWheel(FVector2D location, bool uvSpace, FVector2D wheelDelta, FCefKeyEventFlags modifiers)
{
	if (!Instance->IsAvailable())
	{
		return;
	}
	Instance->Obj->BrowserHost->SendMouseWheelEvent(GetCefMouseEvent(location, uvSpace, modifiers), wheelDelta.X, wheelDelta.Y);
}

void UUrBrowserView::SendTouchEvent(FUraniumPointerEvent event, FCefKeyEventFlags modifiers)
{
	if (!Instance->IsAvailable())
	{
		return;
	}
	CefTouchEvent cefEvent;
	cefEvent.id = event.Id.GetValue();
	if (event.IsUVSpace)
	{
		FVector2D uvloc = event.Location * MainSize;
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
	Instance->Obj->BrowserHost->SendTouchEvent(cefEvent);
}
