/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/
#pragma once

#include "CoreMinimal.h"

#include "UObject/Interface.h"

#include "UrCefEnums.generated.h"

UENUM(BlueprintType)
enum class EUrTextInputMode : uint8
{
	Default,
	None,
	Text,
	Tel,
	Url,
	Email,
	Numeric,
	Decimal,
	Search
};

UENUM(BlueprintType)
enum class EUrLogSeverity : uint8
{
	Default,
	Verbose,
	Debug = Verbose,
	Info,
	Warning,
	Error,
	Fatal,
	Disable = 99
};

UENUM(BlueprintType)
enum class EUrWindowDisposition : uint8
{
	Unknown,
	CurrentTab,
	SingletonTab,
	NewForegroundTab,
	NewBackgroundTab,
	NewPopup,
	NewWindow,
	SaveToDisk,
	OffTheRecord,
	IgnoreAction
};

UENUM(BlueprintType)
enum class EUrGetBrowserSize : uint8
{
	/** Set by a size parameter */
	Manually,

	/**
	 * Set by the desired size of a new window,
	 * or manually when it's a top level browser.
	 */
	FromDesiredOrManually,

	/**
	 * Set by the desired size of a new window,
	 * or from the local size of the widget's geometry when it's a top level browser.
	 */
	FromDesiredOrFromWidgetLocalSize,

	/**
	 * Set by the desired size of a new window,
	 * or from the absolute size of the widget's geometry when it's a top level browser.
	 */
	FromDesiredOrFromWidgetAbsoluteSize,

	/**
	 * Determine size from the local size of the widget's geometry.
	 * This respects scaling of the widget but it won't be pixel-perfect.
	 */
	FromWidgetLocalSize,

	/**
	 * Determine size from the absolute size of the widget's geometry.
	 * This maintains pixel-perfect look, but it won't respect scaling
	 * including scaling from render transformation.
	 */
	FromWidgetAbsoluteSize
};

/**
 * Transition type for a request. Made up of one source value and 0 or more
 * qualifiers.
 */
UENUM(BlueprintType)
enum class EUrTransitionType : uint8
{
	/**
	 * Source is a link click or the JavaScript window.open function. This is
	 * also the default value for requests like sub-resource loads that are not
	 * navigations.
	 */
	Link = 0,

	/**
	 * Source is some other "explicit" navigation. This is the default value for
	 * navigations where the actual type is unknown. See also DirectLoadFlag.
	 */
	Explicit = 1,

	/**
	 * Source is a subframe navigation. This is any content that is automatically
	 * loaded in a non-toplevel frame. For example, if a page consists of several
	 * frames containing ads, those ad URLs will have this transition type.
	 * The user may not even realize the content in these pages is a separate
	 * frame, so may not care about the URL.
	 */
	AutoSubframe = 3,

	/**
	 * Source is a subframe navigation explicitly requested by the user that will
	 * generate new navigation entries in the back/forward list. These are
	 * probably more important than frames that were automatically loaded in
	 * the background because the user probably cares about the fact that this
	 * link was loaded.
	 */
	ManualSubframe = 4,

	/**
	 * Source is a form submission by the user. NOTE: In some situations
	 * submitting a form does not result in this transition type. This can happen
	 * if the form uses a script to submit the contents.
	 */
	FormSubmit = 7,

	/**
	 * Source is a "reload" of the page via the Reload function or by re-visiting
	 * the same URL. NOTE: This is distinct from the concept of whether a
	 * particular load uses "reload semantics" (i.e. bypasses cached data).
	 */
	Reload = 8,

	/*
	 * Qualifiers.
	 * Any of the core values above can be augmented by one or more qualifiers.
	 * These qualifiers further define the transition.
	 * TODO: expose them to BP
	 */
};

USTRUCT(BlueprintType)
struct FUrCefErrorCode
{
	GENERATED_BODY()

private:
	static TMap<int, FString> ErrorCodes;

public:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Uranium")
	int Code = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Uranium")
	FString Output;

	FUrCefErrorCode() { }

	FUrCefErrorCode(int input)
	{
		Code = input;
		Output = ErrorCodes[input];
	}
};