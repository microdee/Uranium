// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct ID3D12CommandQueue;
struct ID3D12GraphicsCommandList;
class FRHICommandList;

namespace D3D12CommandQueues
{
	ID3D12CommandQueue* GetCmdQueue(FRHICommandList& cmdList);
	ID3D12GraphicsCommandList* GetGfxCmdQueue(FRHICommandList& cmdList);
}
