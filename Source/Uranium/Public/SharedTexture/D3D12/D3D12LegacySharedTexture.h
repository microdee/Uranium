// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


#include "SharedTexture/SharedTextureInterface.h"

THIRD_PARTY_INCLUDES_START
#include "Windows/AllowWindowsPlatformTypes.h"

#include <wrl/client.h>
#include <d3d11on12.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3d12.h>

#include "Windows/HideWindowsPlatformTypes.h"
THIRD_PARTY_INCLUDES_END

#include "D3D12LegacySharedTexture.generated.h"

using namespace Microsoft::WRL;

UCLASS()
class URANIUM_API UD3D12LegacySharedTexture : public UObject, public ISharedTexture
{
	GENERATED_BODY()
private:

	UPROPERTY()
	UTexture2D* TargetTexture;

	bool bIsInitialized = false;

	static ID3D12Device* GetRhiDevice();
	static ID3D12CommandQueue* GetRhiCommandQueue(FRHICommandListImmediate& RHICmdList);
	static ID3D12GraphicsCommandList* GetRhiGfxCmdList(FRHICommandListImmediate& RHICmdList);
	ID3D12Resource* GetTargetTextureResource();
	bool CreateDD3D11On12TargetTexture();

	void* InitQueuedHandle = nullptr;
	ComPtr<ID3D11Texture2D> D3D11On12TargetTexture;
	ComPtr<ID3D11Texture2D> SharedTexture;
	ComPtr<ID3D11Device> D3D11Device;
	ComPtr<ID3D11On12Device> D3D11On12Device;
	ComPtr<ID3D11DeviceContext> D3D11Context;

public:

#if !(UR_CHROM_COMPAT_USE_NTHANDLE()) || UR_ST_DEVMODE
	virtual void Initialize() override;
	virtual void OnAcceleratedPaint(void* Handle) override;
	virtual void Render() override;
	virtual void InvalidateUeResources(int InWidth, int InHeight, EPixelFormat InFormat) override;
	virtual bool IsApplicable() override;
#endif

	virtual UTexture2D* GetTexture() override { return TargetTexture; }
};