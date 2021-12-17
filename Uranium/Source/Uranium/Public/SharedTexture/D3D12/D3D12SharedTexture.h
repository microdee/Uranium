/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"


#include "SharedTexture/SharedTextureInterface.h"

#include "../D3DIncludeStart.h"

#include <wrl/client.h>
#include <d3d11on12.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3d12.h>

#include "../D3DIncludeEnd.h"

#include "D3D12SharedTexture.generated.h"

using namespace Microsoft::WRL;

class UD3D11ImGfxCtx;

UCLASS()
class URANIUM_API UD3D12SharedTexture : public UObject, public ISharedTexture
{
	GENERATED_BODY()
private:

	UPROPERTY()
	UTexture2D* TargetTexture;

	UPROPERTY()
	UD3D11ImGfxCtx* ImGfxCtx;

	static ID3D12Device* GetRhiDevice();
	ID3D12Resource* GetTargetTextureResource();
	bool CreateDD3D11On12TargetTexture();

	bool bIsInitialized = false;

	void* InitQueuedHandle = nullptr;
	HANDLE PrevImGfxCtxHandle = nullptr;
	ComPtr<ID3D11Texture2D> D3D11SharedTextureFromImGfxCtx;
	ComPtr<ID3D11Texture2D> D3D11On12TargetTexture;
	ComPtr<ID3D11Device> D3D11Device;
	ComPtr<ID3D11On12Device> D3D11On12Device;
	ComPtr<ID3D11DeviceContext> D3D11Context;

public:

#if (UR_CHROM_COMPAT_USE_NTHANDLE()) || UR_ST_DEVMODE
	virtual void Initialize() override;
	virtual void OnAcceleratedPaint(void* handle) override;
	virtual void Render() override;
	virtual void InvalidateUeResources(int width, int height, EPixelFormat format) override;
	virtual bool IsApplicable() override;
#endif

	virtual UTexture2D* GetTexture() override { return TargetTexture; }
};