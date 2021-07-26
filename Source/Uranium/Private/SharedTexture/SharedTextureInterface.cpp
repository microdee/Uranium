
#include "SharedTexture/SharedTextureInterface.h"

#include "SharedTexture/D3D11/D3D11SharedTexture.h"
#include "SharedTexture/D3D11/D3D11LegacySharedTexture.h"
#include "SharedTexture/D3D12/D3D12SharedTexture.h"
#include "SharedTexture/D3D12/D3D12LegacySharedTexture.h"

#include "HardwareInfo.h"
//#include "Engine/Runtime/Windows/Private/D3D11RHIPrivate.h"

FString ISharedTexture::CurrentRHI()
{
	return FHardwareInfo::GetHardwareInfo(NAME_RHI);
}

EPixelFormat ISharedTexture::FromDXGIFormat(DXGI_FORMAT InFormat)
{
	switch (InFormat)
	{
	case DXGI_FORMAT_UNKNOWN                    : return PF_Unknown;
	case DXGI_FORMAT_R32G32B32A32_TYPELESS      : return PF_R32G32B32A32_UINT;
	case DXGI_FORMAT_R32G32B32A32_FLOAT         : return PF_FloatRGBA;
	case DXGI_FORMAT_R32G32B32A32_UINT          : return PF_R32G32B32A32_UINT;
	case DXGI_FORMAT_R32G32B32A32_SINT          : return PF_Unknown;
	case DXGI_FORMAT_R32G32B32_TYPELESS         : return PF_Unknown;
	case DXGI_FORMAT_R32G32B32_FLOAT            : return PF_FloatRGB;
	case DXGI_FORMAT_R32G32B32_UINT             : return PF_Unknown;
	case DXGI_FORMAT_R32G32B32_SINT             : return PF_Unknown;
	case DXGI_FORMAT_R16G16B16A16_TYPELESS      : return PF_R16G16B16A16_UNORM;
	case DXGI_FORMAT_R16G16B16A16_FLOAT         : return PF_Unknown;
	case DXGI_FORMAT_R16G16B16A16_UNORM         : return PF_R16G16B16A16_UNORM;
	case DXGI_FORMAT_R16G16B16A16_UINT          : return PF_R16G16B16A16_UINT;
	case DXGI_FORMAT_R16G16B16A16_SNORM         : return PF_R16G16B16A16_SNORM;
	case DXGI_FORMAT_R16G16B16A16_SINT          : return PF_R16G16B16A16_SINT;
	case DXGI_FORMAT_R32G32_TYPELESS            : return PF_R32G32_UINT;
	case DXGI_FORMAT_R32G32_FLOAT               : return PF_G32R32F;
	case DXGI_FORMAT_R32G32_UINT                : return PF_R32G32_UINT;
	case DXGI_FORMAT_R32G32_SINT                : return PF_Unknown;
	case DXGI_FORMAT_R32G8X24_TYPELESS          : return PF_Unknown;
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT       : return PF_Unknown;
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS   : return PF_Unknown;
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT    : return PF_Unknown;
	case DXGI_FORMAT_R10G10B10A2_TYPELESS       : return PF_A2B10G10R10;
	case DXGI_FORMAT_R10G10B10A2_UNORM          : return PF_A2B10G10R10;
	case DXGI_FORMAT_R10G10B10A2_UINT           : return PF_Unknown;
	case DXGI_FORMAT_R11G11B10_FLOAT            : return PF_FloatR11G11B10;
	case DXGI_FORMAT_R8G8B8A8_TYPELESS          : return PF_R8G8B8A8;
	case DXGI_FORMAT_R8G8B8A8_UNORM             : return PF_R8G8B8A8;
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB        : return PF_R8G8B8A8;
	case DXGI_FORMAT_R8G8B8A8_UINT              : return PF_R8G8B8A8_UINT;
	case DXGI_FORMAT_R8G8B8A8_SNORM             : return PF_R8G8B8A8_SNORM;
	case DXGI_FORMAT_R8G8B8A8_SINT              : return PF_Unknown;
	case DXGI_FORMAT_R16G16_TYPELESS            : return PF_G16R16;
	case DXGI_FORMAT_R16G16_FLOAT               : return PF_G16R16F;
	case DXGI_FORMAT_R16G16_UNORM               : return PF_G16R16;
	case DXGI_FORMAT_R16G16_UINT                : return PF_R16G16_UINT;
	case DXGI_FORMAT_R16G16_SNORM               : return PF_G16R16;
	case DXGI_FORMAT_R16G16_SINT                : return PF_Unknown;
	case DXGI_FORMAT_R32_TYPELESS               : return PF_R32_UINT;
	case DXGI_FORMAT_D32_FLOAT                  : return PF_R32_FLOAT;
	case DXGI_FORMAT_R32_FLOAT                  : return PF_R32_FLOAT;
	case DXGI_FORMAT_R32_UINT                   : return PF_R32_UINT;
	case DXGI_FORMAT_R32_SINT                   : return PF_R32_SINT;
	case DXGI_FORMAT_R24G8_TYPELESS             : return PF_X24_G8;
	case DXGI_FORMAT_D24_UNORM_S8_UINT          : return PF_X24_G8;
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS      : return PF_X24_G8;
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT       : return PF_X24_G8;
	case DXGI_FORMAT_R8G8_TYPELESS              : return PF_R8G8;
	case DXGI_FORMAT_R8G8_UNORM                 : return PF_R8G8;
	case DXGI_FORMAT_R8G8_UINT                  : return PF_Unknown;
	case DXGI_FORMAT_R8G8_SNORM                 : return PF_R8G8;
	case DXGI_FORMAT_R8G8_SINT                  : return PF_Unknown;
	case DXGI_FORMAT_R16_TYPELESS               : return PF_G16;
	case DXGI_FORMAT_R16_FLOAT                  : return PF_R16F;
	case DXGI_FORMAT_D16_UNORM                  : return PF_G16;
	case DXGI_FORMAT_R16_UNORM                  : return PF_G16;
	case DXGI_FORMAT_R16_UINT                   : return PF_R16_UINT;
	case DXGI_FORMAT_R16_SNORM                  : return PF_G16;
	case DXGI_FORMAT_R16_SINT                   : return PF_R16_SINT;
	case DXGI_FORMAT_R8_TYPELESS                : return PF_G8;
	case DXGI_FORMAT_R8_UNORM                   : return PF_G8;
	case DXGI_FORMAT_R8_UINT                    : return PF_R8_UINT;
	case DXGI_FORMAT_R8_SNORM                   : return PF_G8;
	case DXGI_FORMAT_R8_SINT                    : return PF_Unknown;
	case DXGI_FORMAT_A8_UNORM                   : return PF_A8;
	case DXGI_FORMAT_R1_UNORM                   : return PF_A1;
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP         : return PF_Unknown;
	case DXGI_FORMAT_R8G8_B8G8_UNORM            : return PF_Unknown;
	case DXGI_FORMAT_G8R8_G8B8_UNORM            : return PF_Unknown;
	case DXGI_FORMAT_BC1_TYPELESS               : return PF_DXT1;
	case DXGI_FORMAT_BC1_UNORM                  : return PF_DXT1;
	case DXGI_FORMAT_BC1_UNORM_SRGB             : return PF_DXT1;
	case DXGI_FORMAT_BC2_TYPELESS               : return PF_Unknown;
	case DXGI_FORMAT_BC2_UNORM                  : return PF_Unknown;
	case DXGI_FORMAT_BC2_UNORM_SRGB             : return PF_Unknown;
	case DXGI_FORMAT_BC3_TYPELESS               : return PF_DXT3;
	case DXGI_FORMAT_BC3_UNORM                  : return PF_DXT3;
	case DXGI_FORMAT_BC3_UNORM_SRGB             : return PF_DXT3;
	case DXGI_FORMAT_BC4_TYPELESS               : return PF_BC4;
	case DXGI_FORMAT_BC4_UNORM                  : return PF_BC4;
	case DXGI_FORMAT_BC4_SNORM                  : return PF_BC4;
	case DXGI_FORMAT_BC5_TYPELESS               : return PF_BC5;
	case DXGI_FORMAT_BC5_UNORM                  : return PF_BC5;
	case DXGI_FORMAT_BC5_SNORM                  : return PF_BC5;
	case DXGI_FORMAT_B5G6R5_UNORM               : return PF_R5G6B5_UNORM;
	case DXGI_FORMAT_B5G5R5A1_UNORM             : return PF_Unknown;
	case DXGI_FORMAT_B8G8R8A8_UNORM             : return PF_B8G8R8A8;
	case DXGI_FORMAT_B8G8R8X8_UNORM             : return PF_B8G8R8A8;
	case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM : return PF_A2B10G10R10;
	case DXGI_FORMAT_B8G8R8A8_TYPELESS          : return PF_B8G8R8A8;
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB        : return PF_B8G8R8A8;
	case DXGI_FORMAT_B8G8R8X8_TYPELESS          : return PF_B8G8R8A8;
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB        : return PF_B8G8R8A8;
	case DXGI_FORMAT_BC6H_TYPELESS              : return PF_BC6H;
	case DXGI_FORMAT_BC6H_UF16                  : return PF_BC6H;
	case DXGI_FORMAT_BC6H_SF16                  : return PF_BC6H;
	case DXGI_FORMAT_BC7_TYPELESS               : return PF_BC7;
	case DXGI_FORMAT_BC7_UNORM                  : return PF_BC7;
	case DXGI_FORMAT_BC7_UNORM_SRGB             : return PF_BC7;
	case DXGI_FORMAT_AYUV                       : return PF_Unknown;
	case DXGI_FORMAT_Y410                       : return PF_Unknown;
	case DXGI_FORMAT_Y416                       : return PF_Unknown;
	case DXGI_FORMAT_NV12                       : return PF_NV12;
	case DXGI_FORMAT_P010                       : return PF_Unknown;
	case DXGI_FORMAT_P016                       : return PF_Unknown;
	case DXGI_FORMAT_420_OPAQUE                 : return PF_Unknown;
	case DXGI_FORMAT_YUY2                       : return PF_Unknown;
	case DXGI_FORMAT_Y210                       : return PF_Unknown;
	case DXGI_FORMAT_Y216                       : return PF_Unknown;
	case DXGI_FORMAT_NV11                       : return PF_Unknown;
	case DXGI_FORMAT_AI44                       : return PF_Unknown;
	case DXGI_FORMAT_IA44                       : return PF_Unknown;
	case DXGI_FORMAT_P8                         : return PF_Unknown;
	case DXGI_FORMAT_A8P8                       : return PF_Unknown;
	case DXGI_FORMAT_B4G4R4A4_UNORM             : return PF_Unknown;
	case DXGI_FORMAT_P208                       : return PF_Unknown;
	case DXGI_FORMAT_V208                       : return PF_Unknown;
	case DXGI_FORMAT_V408                       : return PF_Unknown;
	case DXGI_FORMAT_FORCE_UINT                 : return PF_Unknown;
	default                                     : return PF_Unknown;
	}
}

DXGI_FORMAT ISharedTexture::FromUEFormat(EPixelFormat InFormat)
{
	switch (InFormat)
	{
	case PF_Unknown            : return DXGI_FORMAT_UNKNOWN;

	case PF_A32B32G32R32F      : return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case PF_B8G8R8A8           : return DXGI_FORMAT_B8G8R8A8_UNORM;
	case PF_G8                 : return DXGI_FORMAT_R8_UNORM;
	case PF_G16                : return DXGI_FORMAT_R16_UNORM;
	case PF_DXT1               : return DXGI_FORMAT_BC1_UNORM;
	case PF_DXT3               : return DXGI_FORMAT_BC2_UNORM;
	case PF_DXT5               : return DXGI_FORMAT_BC5_UNORM;
	case PF_UYVY               : return DXGI_FORMAT_UNKNOWN;
	case PF_FloatRGB           : return DXGI_FORMAT_R32G32B32_FLOAT;
	case PF_FloatRGBA          : return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case PF_DepthStencil       : return DXGI_FORMAT_D24_UNORM_S8_UINT;
	case PF_ShadowDepth        : return DXGI_FORMAT_D24_UNORM_S8_UINT;
	case PF_R32_FLOAT          : return DXGI_FORMAT_R32_FLOAT;
	case PF_G16R16             : return DXGI_FORMAT_R16G16_UNORM;
	case PF_G16R16F            : return DXGI_FORMAT_R16G16_FLOAT;
	case PF_G16R16F_FILTER     : return DXGI_FORMAT_R16G16_FLOAT;
	case PF_G32R32F            : return DXGI_FORMAT_R32G32_FLOAT;
	case PF_A2B10G10R10        : return DXGI_FORMAT_R10G10B10A2_UNORM;
	case PF_A16B16G16R16       : return DXGI_FORMAT_R16G16B16A16_UNORM;
	case PF_D24                : return DXGI_FORMAT_D24_UNORM_S8_UINT;
	case PF_R16F               : return DXGI_FORMAT_R16_FLOAT;
	case PF_R16F_FILTER        : return DXGI_FORMAT_R16_FLOAT;
	case PF_BC5                : return DXGI_FORMAT_BC5_UNORM;
	case PF_V8U8               : return DXGI_FORMAT_R8G8_UNORM;
	case PF_A1                 : return DXGI_FORMAT_R1_UNORM;
	case PF_FloatR11G11B10     : return DXGI_FORMAT_R11G11B10_FLOAT;
	case PF_A8                 : return DXGI_FORMAT_A8_UNORM;
	case PF_R32_UINT           : return DXGI_FORMAT_R32_UINT;
	case PF_R32_SINT           : return DXGI_FORMAT_R32_SINT;
	case PF_PVRTC2             : return DXGI_FORMAT_UNKNOWN;
	case PF_PVRTC4             : return DXGI_FORMAT_UNKNOWN;
	case PF_R16_UINT           : return DXGI_FORMAT_R16_UINT;
	case PF_R16_SINT           : return DXGI_FORMAT_R16_SINT;
	case PF_R16G16B16A16_UINT  : return DXGI_FORMAT_R16G16B16A16_UINT;
	case PF_R16G16B16A16_SINT  : return DXGI_FORMAT_R16G16B16A16_SINT;
	case PF_R5G6B5_UNORM       : return DXGI_FORMAT_B5G6R5_UNORM;
	case PF_R8G8B8A8           : return DXGI_FORMAT_R8G8B8A8_UNORM;
	case PF_A8R8G8B8           : return DXGI_FORMAT_R8G8B8A8_UNORM;
	case PF_BC4                : return DXGI_FORMAT_BC4_UNORM;
	case PF_R8G8               : return DXGI_FORMAT_R8G8_UNORM;
	case PF_ATC_RGB            : return DXGI_FORMAT_UNKNOWN;
	case PF_ATC_RGBA_E         : return DXGI_FORMAT_UNKNOWN;
	case PF_ATC_RGBA_I         : return DXGI_FORMAT_UNKNOWN;
	case PF_X24_G8             : return DXGI_FORMAT_D24_UNORM_S8_UINT;
	case PF_ETC1               : return DXGI_FORMAT_UNKNOWN;
	case PF_ETC2_RGB           : return DXGI_FORMAT_UNKNOWN;
	case PF_ETC2_RGBA          : return DXGI_FORMAT_UNKNOWN;
	case PF_R32G32B32A32_UINT  : return DXGI_FORMAT_R32G32B32A32_UINT;
	case PF_R16G16_UINT        : return DXGI_FORMAT_R16G16_UINT;
	case PF_ASTC_4x4           : return DXGI_FORMAT_UNKNOWN;
	case PF_ASTC_6x6           : return DXGI_FORMAT_UNKNOWN;
	case PF_ASTC_8x8           : return DXGI_FORMAT_UNKNOWN;
	case PF_ASTC_10x10         : return DXGI_FORMAT_UNKNOWN;
	case PF_ASTC_12x12         : return DXGI_FORMAT_UNKNOWN;
	case PF_BC6H               : return DXGI_FORMAT_BC6H_UF16;
	case PF_BC7                : return DXGI_FORMAT_BC7_UNORM;
	case PF_R8_UINT            : return DXGI_FORMAT_R8_UINT;
	case PF_L8                 : return DXGI_FORMAT_R8_UNORM;
	case PF_XGXR8              : return DXGI_FORMAT_UNKNOWN;
	case PF_R8G8B8A8_UINT      : return DXGI_FORMAT_R8G8B8A8_UINT;
	case PF_R8G8B8A8_SNORM     : return DXGI_FORMAT_R8G8B8A8_UNORM;
	case PF_R16G16B16A16_UNORM : return DXGI_FORMAT_R16G16B16A16_UNORM;
	case PF_R16G16B16A16_SNORM : return DXGI_FORMAT_R16G16B16A16_SNORM;
	case PF_PLATFORM_HDR_0     : return DXGI_FORMAT_UNKNOWN;
	case PF_PLATFORM_HDR_1     : return DXGI_FORMAT_UNKNOWN;
	case PF_PLATFORM_HDR_2     : return DXGI_FORMAT_UNKNOWN;
	case PF_NV12               : return DXGI_FORMAT_NV12;
	case PF_R32G32_UINT        : return DXGI_FORMAT_R32G32_UINT;
	case PF_ETC2_R11_EAC       : return DXGI_FORMAT_UNKNOWN;
	case PF_ETC2_RG11_EAC      : return DXGI_FORMAT_UNKNOWN;
	case PF_MAX                : return DXGI_FORMAT_UNKNOWN;
	default                    : return DXGI_FORMAT_UNKNOWN;
	}
}

template<typename T>
T* GetDefaultObjOfClass()
{
	return T::StaticClass()->template GetDefaultObject<T>();
}

template<typename T>
bool TryCreateSharedTextureImpl(TScriptInterface<ISharedTexture>& OutSharedTex)
{
	if(OutSharedTex) return false;

	if (GetDefaultObjOfClass<T>()->IsApplicable())
	{
		OutSharedTex = NewObject<T>();
		OutSharedTex->Initialize();
		return true;
	}
	return false;
}

TScriptInterface<ISharedTexture> ISharedTexture::CreateSharedTexture()
{
	TScriptInterface<ISharedTexture> Candidate = nullptr;
	TryCreateSharedTextureImpl<UD3D11SharedTexture>(Candidate);
	TryCreateSharedTextureImpl<UD3D11LegacySharedTexture>(Candidate);
	TryCreateSharedTextureImpl<UD3D12SharedTexture>(Candidate);
	TryCreateSharedTextureImpl<UD3D12LegacySharedTexture>(Candidate);
	ensureAlwaysMsgf(Candidate, TEXT("No implementation of shared texture is applicable"));
	return Candidate;
}