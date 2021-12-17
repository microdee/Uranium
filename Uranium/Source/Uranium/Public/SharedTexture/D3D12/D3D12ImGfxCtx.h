/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"
#include "SharedTexture/ImGfxCtxInterface.h"

#include "../D3DIncludeStart.h"

#include <wrl/client.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_4.h>
#include <dxgi1_6.h>

#include "../D3DIncludeEnd.h"

#include "D3D12ImGfxCtx.generated.h"

using namespace Microsoft::WRL;

UCLASS()
class UD3D12ImGfxCtx : public UObject, public IImGfxCtx
{
	GENERATED_BODY()
private:
	static ID3D12Device* GetRhiDevice();

	bool bIsInitialized = false;

	ComPtr<ID3D12Device> Device;
	ComPtr<ID3D12CommandAllocator> CommandAllocator;
	ComPtr<ID3D12CommandQueue> CommandQueue;
	ComPtr<ID3D12RootSignature> RootSignature;
	ComPtr<ID3D12DescriptorHeap> RtvHeap;
	ComPtr<ID3D12GraphicsCommandList> CommandList;

	ComPtr<ID3D11Device> MutexOnlyDevice;
	ComPtr<ID3D11DeviceContext> MutexOnlyContext;
	ComPtr<ID3D11Device1> MutexOnlyDevice1;

	bool GetRhiAdapter(IDXGIAdapter*& outAdapter);
	void EndFrame();
	void Initialize();
public:
	ComPtr<ID3D12Fence> Fence;
	ComPtr<ID3D12Resource> TargetTexture;

	virtual void OnAcceleratedPaint(void* handle) override;
};