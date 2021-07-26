// MESO Digital Interiors GmbH. (C) 2020-

#include "UrBrowserView.h"

#include "CefBrowserView.h"
#include "HardwareInfo.h"
#include "UraniumContext.h"
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

UUrBrowserView::UUrBrowserView(const FObjectInitializer& Oi)
	: Super(Oi)
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
	auto context = GetSingletonUraniumContext();
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
	auto context = GetSingletonUraniumContext();
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
		res->OnAfterCreated.Add(onAfterCreated);
		
	return res;
}

void UUrBrowserView::DeferredInit(FVector2D InitialSize, FString Url)
{
	if(Instance->Obj) return;
	MainSize = InitialSize;
	Instance->Obj = FCefBrowserView::CreateNew(InitialSize, Url, this);
	bBrowserValid = Instance->Obj.get() != nullptr;

	if (bBrowserValid)
	{
		MainTexture = ISharedTexture::CreateSharedTexture();
		NativePopupTexture = ISharedTexture::CreateSharedTexture();
	}
}

void UUrBrowserView::DeferredInit(UCefBrowserManagedRef* FromInstance)
{
	if (Instance->Obj) return;
	bBrowserValid = FromInstance != nullptr;

	if (bBrowserValid)
	{
		MainSize = FromInstance->Obj->vMainSize;
		FromInstance->Obj->UrWrapper = this;
		Instance = FromInstance;
		MainTexture = ISharedTexture::CreateSharedTexture();
		NativePopupTexture = ISharedTexture::CreateSharedTexture();
	}
}

void UUrBrowserView::Close()
{
	if (bClosing) return;
	if (!Instance->IsValidLowLevel()) return;
	bClosing = true;

	if(Instance->IsAvailable())
		Instance->Obj->Close();

	Instance = nullptr;
}

void UUrBrowserView::BeginDestroy()
{
	Super::BeginDestroy();
	if (!bClosing) Close();
}

void UUrBrowserView::GetMainSize(FVector2D& desiredMainSize, FVector2D& actualMainSize) const
{
	desiredMainSize = MainSize;
	if(MainTexture) 
		actualMainSize = FVector2D(MainTexture->Width, MainTexture->Height);
	else actualMainSize = FVector2D::ZeroVector;
}

UTexture2D* UUrBrowserView::GetMainTexture() const
{
	if(!MainTexture) return nullptr;
	return MainTexture->GetTexture();
}

UTexture2D* UUrBrowserView::GetNativePopupTexture() const
{
	if (!NativePopupTexture) return nullptr;
	return NativePopupTexture->GetTexture();
}

void UUrBrowserView::SetMainSize(FVector2D size, bool force /*= false*/)
{
	if (!Instance->IsAvailable()) return;
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
	if (!Instance->IsAvailable()) return;
	Instance->Obj->BrowserHost->SendExternalBeginFrame();
#endif

	if(MainTexture)
		MainTexture->Render();

	if (bPopupShown && NativePopupTexture)
		NativePopupTexture->Render();

	if (recursive)
	{
		for (auto br : Children)
		{
			br->Tick(deltaSeconds);
		}
	}
}

int UUrBrowserView::GetIdentifier() const
{
	if (!Instance->IsAvailable()) return 0;
	return Instance->Obj->Browser->GetIdentifier();
}

bool UUrBrowserView::IsSame(UUrBrowserView* other) const
{
	if (!this->IsValidLowLevelFast(false) && !other->IsValidLowLevelFast(false))
		return true;
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
	if(!Instance->IsAvailable()) return false;
	return Instance->Obj->Browser->IsPopup();
}

bool UUrBrowserView::HasDocument() const
{
	if (!Instance->IsAvailable()) return false;
	return Instance->Obj->Browser->HasDocument();
}

void UUrBrowserView::LoadUrl(FString url)
{
	if (!Instance->IsAvailable()) return;
	Instance->Obj->Browser->GetMainFrame()->LoadURL(*url);
}

void UUrBrowserView::LoadHtml(const FHtmlContent& HtmlContent)
{
	if (!Instance->IsAvailable()) return;
	auto LoadContentOnLoadEnd = [this](int)
	{
		//TODO: use resource handlers implementation
	};
	Instance->Obj->Browser->GetMainFrame()->LoadURL("about:blank");
}

void UUrBrowserView::ExecuteJavaScript(FString Code, FString ScriptUrl, int StartLine /*= 0*/)
{
	if (!Instance->IsAvailable()) return;
	Instance->Obj->Browser->GetMainFrame()->ExecuteJavaScript(*Code, *ScriptUrl, StartLine);
}

FString UUrBrowserView::GetURL() const
{
	if (!Instance->IsAvailable()) return TEXT("error:no-browser");
	return FString(Instance->Obj->Browser->GetMainFrame()->GetURL().c_str());
}

void UUrBrowserView::SetZoomLevel(float Zoom)
{
	if (!Instance->IsAvailable()) return;
	Instance->Obj->BrowserHost->SetZoomLevel(Zoom);
}

void UUrBrowserView::GoBack()
{
	if (!Instance->IsAvailable()) return;
	Instance->Obj->Browser->GoBack();
}

void UUrBrowserView::GoForward()
{
	if (!Instance->IsAvailable()) return;
	Instance->Obj->Browser->GoForward();
}

void UUrBrowserView::Reload()
{
	if (!Instance->IsAvailable()) return;
	Instance->Obj->Browser->Reload();
}

void UUrBrowserView::ReloadIgnoreCache()
{
	if (!Instance->IsAvailable()) return;
	Instance->Obj->Browser->ReloadIgnoreCache();
}

bool UUrBrowserView::IsLoading() const
{
	if (!Instance->IsAvailable()) return true;
	return Instance->Obj->Browser->IsLoading();
}

void UUrBrowserView::ShowDevToolsWindow(FVector2D InspectElementAt)
{
	if (!Instance->IsAvailable()) return;
	Instance->Obj->ShowDevToolsWindow(InspectElementAt);
}

void UUrBrowserView::ShowDevToolsEmbedded(FVector2D InspectElementAt, FVector2D Size)
{
	if (!Instance->IsAvailable()) return;
	Instance->Obj->ShowDevToolsEmbedded(InspectElementAt, Size.X, Size.Y);
}

void UUrBrowserView::CloseDevTools()
{
	if (!Instance->IsAvailable()) return;
	Instance->Obj->BrowserHost->CloseDevTools();
}

bool UUrBrowserView::HasDevTools() const
{
	if (!Instance->IsAvailable()) return false;
	return Instance->Obj->BrowserHost->HasDevTools();
}

void UUrBrowserView::SetAudioMuted(bool Mute)
{
	if (!Instance->IsAvailable()) return;
	Instance->Obj->BrowserHost->SetAudioMuted(Mute);
}

bool UUrBrowserView::IsAudioMuted() const
{
	if (!Instance->IsAvailable()) return false;
	return Instance->Obj->BrowserHost->IsAudioMuted();
}

void UUrBrowserView::SendKeyEvent(FKeyEvent Event, bool IsUp, FCefKeyEventFlags Modifiers)
{
	SendKeyEventKc(
		Event.GetKeyCode(),
		static_cast<TCHAR>(Event.GetCharacter()),
		Event.IsAltDown(),
		IsUp,
		EnforceFlagsFromEvent(Event, Modifiers)
	);
}

void UUrBrowserView::SendKey(int WindowsKeyCode, FString Character, bool IsUp, FCefKeyEventFlags Modifiers)
{
	SendKeyEventKc(
		static_cast<uint32>(WindowsKeyCode),
		Character.IsEmpty() ? '\0' : Character[0],
		Modifiers.AltDown,
		IsUp,
		Modifiers
	);
}

void UUrBrowserView::SendKeyEventKc(uint32 KeyCode, TCHAR Char, bool IsSysKey, bool IsUp, FCefKeyEventFlags Modifiers)
{
	if (!Instance->IsAvailable()) return;

	// Some languages use AltGr for special characters, which registers as Alt + Ctrl keypresses
	// In order to avoid triggering shortcuts like Ctrl+V while typing text, when the right Alt(Gr)
	// is pressed, do not send explicit key events (only send character)
	if (Modifiers.AltGrDown) return;

	uint32 ScanCode = MapVirtualKeyA(KeyCode, MAPVK_VK_TO_VSC);

	LPARAM lParam = 1 | (ScanCode & 0xFF) << 16; // Scan Code in lParam bits 16..23
	if (IsUp)
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
	cefEvent.windows_key_code = KeyCode;
	cefEvent.character = Char;
	cefEvent.is_system_key = IsSysKey;
	cefEvent.native_key_code = lParam;
	cefEvent.type = IsUp ? KEYEVENT_KEYUP : KEYEVENT_KEYDOWN;
	cefEvent.unmodified_character = Char;
	cefEvent.modifiers = Modifiers.GetEventFlags();
	Instance->Obj->BrowserHost->SendKeyEvent(cefEvent);
}

void UUrBrowserView::SendCharacterKey(FCharacterEvent Event, FCefKeyEventFlags Modifiers)
{
	SendCharKc(
		Event.GetCharacter(),
		Event.IsAltDown(),
		EnforceFlagsFromEvent(Event, Modifiers)
	);
}

void UUrBrowserView::SendKeyString(FString Input, FCefKeyEventFlags Modifiers)
{
	for(const TCHAR& Char : Input)
	{
		SendCharKc(Char, Modifiers.AltDown, Modifiers);
	}
}

void UUrBrowserView::SendCharKc(TCHAR Char, bool IsSysKey, FCefKeyEventFlags Modifiers)
{
	if (!Instance->IsAvailable()) return;

	uint32 KeyCode = VkKeyScanA(Char) & 0xff;
	uint32 ScanCode = MapVirtualKeyA(KeyCode, MAPVK_VK_TO_VSC);
	LPARAM lParam = 1 | ((ScanCode & 0xFF) << 16);

	CefKeyEvent cefEvent;
	cefEvent.character = Char;
	cefEvent.is_system_key = IsSysKey;
	cefEvent.windows_key_code = Char;
	cefEvent.native_key_code = lParam;
	cefEvent.type = KEYEVENT_CHAR;
	cefEvent.unmodified_character = Char;
	cefEvent.modifiers = Modifiers.GetEventFlags();
	Instance->Obj->BrowserHost->SendKeyEvent(cefEvent);
}

CefMouseEvent UUrBrowserView::GetCefMouseEvent(FVector2D Location, bool UVSpace, const FCefKeyEventFlags& Modifiers) const
{
	CefMouseEvent cefEvent;
	cefEvent.x = UVSpace ? Location.X * MainSize.X : Location.X;
	cefEvent.y = UVSpace ? Location.Y * MainSize.Y : Location.Y;
	cefEvent.modifiers = Modifiers.GetEventFlags();
	return cefEvent;
}

void UUrBrowserView::SendMouseMove(FVector2D Location, bool UVSpace, bool bLeaving, FCefKeyEventFlags Modifiers)
{
	if (!Instance->IsAvailable()) return;
	Instance->Obj->BrowserHost->SendMouseMoveEvent(GetCefMouseEvent(Location, UVSpace, Modifiers), bLeaving);
}

void UUrBrowserView::SendMouseButton(FVector2D Location, bool UVSpace, const FKey& Button, bool isUp, FCefKeyEventFlags Modifiers)
{
	if (!Instance->IsAvailable()) return;
	if (!UraniumInput::IsValidMouseButton(Button)) return;

	Instance->Obj->BrowserHost->SendMouseClickEvent(
		GetCefMouseEvent(Location, UVSpace, Modifiers),
		UraniumInput::GetUraniumMouseButton(Button),
		isUp, 1
	);
}

void UUrBrowserView::SendMouseWheel(FVector2D Location, bool UVSpace, FVector2D WheelDelta, FCefKeyEventFlags Modifiers)
{
	if (!Instance->IsAvailable()) return;
	Instance->Obj->BrowserHost->SendMouseWheelEvent(GetCefMouseEvent(Location, UVSpace, Modifiers), WheelDelta.X, WheelDelta.Y);
}

void UUrBrowserView::SendTouchEvent(FUraniumPointerEvent Event, FCefKeyEventFlags Modifiers)
{
	if (!Instance->IsAvailable()) return;
	CefTouchEvent cefEvent;
	cefEvent.id = Event.Id.GetValue();
	if (Event.IsUVSpace)
	{
		FVector2D uvloc = Event.Location * MainSize;
		cefEvent.x = uvloc.X;
		cefEvent.y = uvloc.Y;
	}
	else
	{
		cefEvent.x = Event.Location.X;
		cefEvent.y = Event.Location.Y;
	}
	cefEvent.radius_x = Event.Size.X;
	cefEvent.radius_y = Event.Size.Y;
	cefEvent.pressure = Event.Pressure;
	cefEvent.rotation_angle = FMath::DegreesToRadians(Event.RotationDegrees);
	cefEvent.pointer_type = static_cast<cef_pointer_type_t>(Event.PointerType);
	cefEvent.type = static_cast<cef_touch_event_type_t>(Event.EventType);
	cefEvent.modifiers = Modifiers.GetEventFlags();
	Instance->Obj->BrowserHost->SendTouchEvent(cefEvent);
}
