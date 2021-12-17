/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"

#include "UrRenderHandler.h"
#include "UObject/Object.h"
#include "SlowRenderResourceHandler.generated.h"

/**
 * A simplistic class to keep a persistent image buffer
 */
UCLASS()
class URANIUM_API UWebTexture : public UObject
{
	GENERATED_BODY()
	
private:
	static constexpr int BytesPerPixel = 4;

	TArray<FUpdateTextureRegion2D> Regions;
	TArray<uint8> Buffer;
	
	UPROPERTY()
	UTexture2D* Texture;
	
public:

	void Update(const void* buffer, const CefRenderHandler::RectList& dirtyRects, int width, int height);

	UTexture2D* GetTexture() const;
};

/**
 * Render resource handler using legacy texture retrieval method.
 */
UCLASS()
class URANIUM_API USlowRenderResourceHandler
	: public UObject
	, public IUrRenderResourceHandler
	, public IUrHandler
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	UWebTexture* MainTexture;

	UPROPERTY()
	UWebTexture* NativePopupTexture;
	
	FVector2D MainSize {};
	float ScaleFactor = 1.0;
	
public:
	virtual void PostInitProperties() override;

	virtual void OnPersistentBrowserSet_Implementation() override;

	virtual void OnPaint(
		CefRefPtr<CefBrowser> browser,
		const void* buffer,
		const CefRenderHandler::RectList& dirtyRects,
		int width, int height,
		bool isNativePopup
	) override;
	
	virtual UTexture2D* GetMainTexture() const override;
	virtual UTexture2D* GetNativePopupTexture() const override;
	
	virtual bool IsTextureVerticallyMirrored() const override
	{
		return false;
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
