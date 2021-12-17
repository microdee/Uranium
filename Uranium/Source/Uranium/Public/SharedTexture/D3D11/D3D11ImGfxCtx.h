/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"
#include "SharedTexture/ImGfxCtxInterface.h"

#include "../D3DIncludeStart.h"

#include <wrl/client.h>
#include <d3d11.h>
#include <d3d11_1.h>

#include "../D3DIncludeEnd.h"

#include "D3D11ImGfxCtx.generated.h"

using namespace Microsoft::WRL;

UCLASS()
class UD3D11ImGfxCtx : public UObject, public IImGfxCtx
{
	GENERATED_BODY()
private:
	static ID3D11Device* GetRhiDevice();

	bool bIsInitialized = false;

	ComPtr<ID3D11Device> Device;
	ComPtr<ID3D11Device1> Device1;
	ComPtr<ID3D11DeviceContext> DeviceContext;

	bool GetRhiAdapter(IDXGIAdapter*& outAdapter);
	void Initialize();
public:
	ComPtr<ID3D11Texture2D> TargetTexture;
	HANDLE TargetSharedHandle = NULL;
	virtual void OnAcceleratedPaint(void* handle) override;
};