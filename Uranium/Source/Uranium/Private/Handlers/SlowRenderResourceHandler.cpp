// Fill out your copyright notice in the Description page of Project Settings.


#include "Handlers/SlowRenderResourceHandler.h"

#include "Handlers/UrDefaultRenderHandler.h"

void UWebTexture::Update(const void* buffer, const CefRenderHandler::RectList& dirtyRects, int width, int height)
{
	int bufSize = width * height * BytesPerPixel;
	if(!Texture || Texture->GetSizeX() != width || Texture->GetSizeY() != height)
	{
		Texture = UTexture2D::CreateTransient(width, height);
		Texture->UpdateResource();
		Buffer.SetNum(bufSize);
	}

	FMemory::Memcpy(Buffer.GetData(), buffer, bufSize);

	Regions.SetNum(dirtyRects.size());
	for(int i=0; i<dirtyRects.size(); i++)
	{
		CefRect rect = dirtyRects[i];
		Regions[i] = FUpdateTextureRegion2D{
			static_cast<uint32>(rect.x),
			static_cast<uint32>(rect.y),
			rect.x, rect.y,
			static_cast<uint32>(rect.width),
			static_cast<uint32>(rect.height)
		};
	}
	
	Texture->UpdateTextureRegions(
		0, Regions.Num(), Regions.GetData(), width * BytesPerPixel, BytesPerPixel,
		Buffer.GetData(),
		[](uint8*, const FUpdateTextureRegion2D*) {}
	);
}

UTexture2D* UWebTexture::GetTexture() const
{
	return Texture;
}

void USlowRenderResourceHandler::PostInitProperties()
{
	Super::PostInitProperties();
	CefRenderHandler = new FCefDefaultRenderHandler();
	CefRenderHandler->UrRenderResourceHandler = TScriptInterface<IUrRenderResourceHandler>(this);
	
	MainTexture = NewObject<UWebTexture>();
	NativePopupTexture = NewObject<UWebTexture>();
}

void USlowRenderResourceHandler::OnPersistentBrowserSet_Implementation()
{
}

void USlowRenderResourceHandler::OnPaint(
	CefRefPtr<CefBrowser> browser,
	const void* buffer,
	const CefRenderHandler::RectList& dirtyRects,
	int width, int height,
	bool isNativePopup
) {
	if(isNativePopup)
	{
		NativePopupTexture->Update(buffer, dirtyRects, width, height);
	}
	else
	{
		MainTexture->Update(buffer, dirtyRects, width, height);
	}
}

UTexture2D* USlowRenderResourceHandler::GetMainTexture() const
{
	return MainTexture->GetTexture();
}

UTexture2D* USlowRenderResourceHandler::GetNativePopupTexture() const
{
	return NativePopupTexture->GetTexture();
}

FVector2D USlowRenderResourceHandler::GetMainSize() const
{
    return MainSize;
}

void USlowRenderResourceHandler::SetMainSize(FVector2D size)
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

void USlowRenderResourceHandler::TickRenderResource()
{
}
