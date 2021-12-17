/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/


#include "D3D12CommandQueues.h"
#include "UeVersion.h"

#if UE_VERSION >= MAKE_UE_VERSION(4, 26)

#include "D3D12RHIBridge.h"

#else

#define NV_AFTERMATH 0
#include "D3D12RHI/Private/D3D12RHIPrivate.h"

#endif


ID3D12CommandQueue* D3D12CommandQueues::GetCmdQueue(FRHICommandList& cmdList)
{
#if UE_VERSION >= MAKE_UE_VERSION(4, 26)
	
	void* cmdQueue = nullptr;
	void* gfxQueue = nullptr;
	D3D12RHI::GetGfxCommandListAndQueue(cmdList, gfxQueue, cmdQueue);
	return static_cast<ID3D12CommandQueue*>(cmdQueue);
	
#else
	
	FD3D12CommandContext& cmdCtx = static_cast<FD3D12CommandContext&>(cmdList.GetContext());
	return cmdCtx.CommandListHandle.GetCommandListManager()->GetD3DCommandQueue();
	
#endif
}

ID3D12GraphicsCommandList* D3D12CommandQueues::GetGfxCmdQueue(FRHICommandList& cmdList)
{
#if UE_VERSION >= MAKE_UE_VERSION(4, 26)
	
	void* cmdQueue = nullptr;
	void* gfxQueue = nullptr;
	D3D12RHI::GetGfxCommandListAndQueue(cmdList, gfxQueue, cmdQueue);
	return static_cast<ID3D12GraphicsCommandList*>(gfxQueue);
	
#else
	
	FD3D12CommandContext& cmdCtx = static_cast<FD3D12CommandContext&>(cmdList.GetContext());
	return cmdCtx.CommandListHandle.GraphicsCommandList();
	
#endif
}
