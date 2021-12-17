/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "Launch/Resources/Version.h"

#define MAKE_UE_VERSION(Major, Minor) (((Major) * 1000) + ((Minor) * 10))
#define MAKE_UE_VERSION_P(Major, Minor, Patch) (MAKE_UE_VERSION(Major, Minor) + (Patch))
#define UE_VERSION MAKE_UE_VERSION_P(ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ENGINE_PATCH_VERSION)