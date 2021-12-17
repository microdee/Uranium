/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/ 

#pragma once
#include "UrRenderHandler.h"

#include "UrDefaultRenderHandler.generated.h"

class ISharedTexture;

class URANIUM_API FCefDefaultRenderHandler : public ICefRenderHandler
{
	friend class UUrDefaultRenderHandler;
public:
	
	IMPLEMENT_REFCOUNTING(FCefDefaultRenderHandler);
};

UCLASS(NotBlueprintable, BlueprintType)
class URANIUM_API UUrDefaultRenderResourceHandler
	: public UObject
	, public IUrRenderResourceHandler
	, public IUrHandler
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	TScriptInterface<ISharedTexture> MainTexture;

	UPROPERTY()
	TScriptInterface<ISharedTexture> NativePopupTexture;
	
	FVector2D MainSize {};
	float ScaleFactor = 1.0;
	
public:
	virtual void PostInitProperties() override;

	virtual void OnPersistentBrowserSet_Implementation() override;
	
	virtual void OnAcceleratedPaint(
		CefRefPtr<CefBrowser> browser,
		void* sharedHandle,
		const CefRenderHandler::RectList& dirtyRects,
		bool isNativePopup
	) override;
	
	virtual UTexture2D* GetMainTexture() const override;
	virtual UTexture2D* GetNativePopupTexture() const override;
	
	virtual bool IsTextureVerticallyMirrored() const override
	{
		return true;
	}
	
	virtual float GetScaleFactor() const override
	{
		return ScaleFactor;
	}
	virtual void SetScaleFactor(float scale) override
	{
		ScaleFactor = scale;
	}
	
	virtual FVector2D GetMainSize() const override;
	virtual void SetMainSize(FVector2D size) override;

	virtual void TickRenderResource() override;
};

UCLASS(Blueprintable, BlueprintType)
class URANIUM_API UUrDefaultRenderEventsHandler
	: public UObject
	, public IUrRenderEventsHandler
	, public IUrHandler
{
	GENERATED_BODY()
	
protected:
	FVector2D NativePopupSize;
	FVector2D NativePopupLocation;
	bool bIsNativePopupVisible;

	FVector2D ScrollOffset;
	
public:
	virtual void OnNativePopupShow_Implementation(UUrBrowserView* browser, bool show) override;
	virtual void OnNativePopupSize_Implementation(UUrBrowserView* browser, FVector2D location, FVector2D size) override;
	virtual void OnScrollOffsetChanged_Implementation(UUrBrowserView* browser, FVector2D offset) override;
	virtual bool IsNativePopupVisible_Implementation() override;
	virtual FVector2D GetNativePopupLocation_Implementation() override;
	virtual FVector2D GetNativePopupSize_Implementation() override;
	virtual FVector2D GetScrollOffset_Implementation() override;
};