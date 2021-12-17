/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#ifdef UR_D3D_INCLUDE_REGION
#error Direct3D include regions cannot be nested, one region is already opened before
#endif
#define UR_D3D_INCLUDE_REGION 1

#pragma warning( push )
#pragma warning( disable : 4005 )

THIRD_PARTY_INCLUDES_START
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/AllowWindowsPlatformAtomics.h"