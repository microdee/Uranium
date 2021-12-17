/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#include "Handlers/UrDefaultRenderHandler.h"

#include "UrBrowserView.h"
#include "SharedTexture/SharedTextureInterface.h"

void UUrDefaultRenderResourceHandler::OnAcceleratedPaint(
    CefRefPtr<CefBrowser> browser,
    void* sharedHandle,
    const CefRenderHandler::RectList& dirtyRects,
    bool isNativePopup
) {
    // TODO: use dirty rects when copying resources, might has better performance
    if(isNativePopup && NativePopupTexture)
    {
        NativePopupTexture->OnAcceleratedPaint(sharedHandle);
    }
    else if(MainTexture)
    {
        MainTexture->OnAcceleratedPaint(sharedHandle);
    }
}

void UUrDefaultRenderResourceHandler::PostInitProperties()
{
    Super::PostInitProperties();
    CefRenderHandler = new FCefDefaultRenderHandler();
    CefRenderHandler->UrRenderResourceHandler = TScriptInterface<IUrRenderResourceHandler>(this);
}

void UUrDefaultRenderResourceHandler::OnPersistentBrowserSet_Implementation()
{
    if(Browser)
    {
        MainTexture = ISharedTexture::CreateSharedTexture();
        NativePopupTexture = ISharedTexture::CreateSharedTexture();
    }
}

UTexture2D* UUrDefaultRenderResourceHandler::GetMainTexture() const
{
    return MainTexture ? MainTexture->GetTexture() : nullptr;
}

UTexture2D* UUrDefaultRenderResourceHandler::GetNativePopupTexture() const
{
    return NativePopupTexture ? NativePopupTexture->GetTexture() : nullptr;
}

FVector2D UUrDefaultRenderResourceHandler::GetMainSize() const
{
    return MainSize;
}

void UUrDefaultRenderResourceHandler::SetMainSize(FVector2D size)
{
    size = FMath::Max(size, FVector2D(128, 128));
    if (MainSize != size)
    {
        MainSize = size;
        if(Browser)
        {
            Browser->GetHost()->WasResized();
        }
    }
}

void UUrDefaultRenderResourceHandler::TickRenderResource()
{
    if(!GetBrowserView().IsValid() || !GetBrowserView()->RenderEventsHandler)
    {
        return;
    }
    
    bool popupShown = IUrRenderEventsHandler::Execute_IsNativePopupVisible(GetBrowserView()->RenderEventsHandler);

    if(MainTexture)
    {
        MainTexture->Render();
    }

    if(popupShown && NativePopupTexture)
    {
        NativePopupTexture->Render();
    }
}

void UUrDefaultRenderEventsHandler::OnNativePopupShow_Implementation(UUrBrowserView* browser, bool show)
{
    bIsNativePopupVisible = show;
}

void UUrDefaultRenderEventsHandler::OnNativePopupSize_Implementation(UUrBrowserView* browser, FVector2D location, FVector2D size)
{
    NativePopupLocation = location;
    NativePopupSize = size;
}

void UUrDefaultRenderEventsHandler::OnScrollOffsetChanged_Implementation(UUrBrowserView* browser, FVector2D offset)
{
    ScrollOffset = offset;
}

bool UUrDefaultRenderEventsHandler::IsNativePopupVisible_Implementation()
{
    return bIsNativePopupVisible;
}

FVector2D UUrDefaultRenderEventsHandler::GetNativePopupLocation_Implementation()
{
    return NativePopupLocation;
}

FVector2D UUrDefaultRenderEventsHandler::GetNativePopupSize_Implementation()
{
    return NativePopupSize;
}

FVector2D UUrDefaultRenderEventsHandler::GetScrollOffset_Implementation()
{
    return ScrollOffset;
}
