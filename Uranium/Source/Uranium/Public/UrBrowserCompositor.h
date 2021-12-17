/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "CoreMinimal.h"

#include "UObject/Interface.h"
#include "UrBrowserCompositor.generated.h"

class UUrBrowserComposition;

// This class does not need to be modified.
UINTERFACE()
class UUrBrowserCompositor : public UInterface
{
	GENERATED_BODY()
};

/**
 * Simple interface which just provides a browser composition for browsers created by the implementing class
 */
class URANIUM_API IUrBrowserCompositor
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category="Uranium")
	UUrBrowserComposition* GetComposition();
};
