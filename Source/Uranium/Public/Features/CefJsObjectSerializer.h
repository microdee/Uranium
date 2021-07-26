// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "CefIncludesStart.h"
#include "include/cef_v8.h"
#include "CefIncludesEnd.h"

class FObjectTraversalState
{
public:
	TMap<void*, CefRefPtr<CefDictionaryValue>> ObjectValueAssoc;
	TMap<void*, UObject*> ValueObjectAssoc;

	void Add(UObject* InObject, CefRefPtr<CefDictionaryValue> InValue);
};

class FPinObjects
{
	TArray<UObject*> Objects;
public:
	FPinObjects();
	explicit FPinObjects(TWeakObjectPtr<UObject> InObject);
	explicit FPinObjects(UObject* InObject);
	~FPinObjects();
	
	void With(UObject* InObject);
};

/**
 * 
 */
class URANIUM_API FCefJsObjectSerializer
{
public:
	static CefRefPtr<CefDictionaryValue> ToCefValue(TWeakObjectPtr<UObject> From);
	static void ToUObject(TWeakObjectPtr<UObject> Target, CefRefPtr<CefDictionaryValue> From);

private:
	static CefRefPtr<CefValue> FieldToCefValue(
		FField* InField,
		const void* ValuePtr,
		FObjectTraversalState& Traversal
	);
	
	static void CefValueToField(
		FField* InField,
		void* ValuePtr,
		CefRefPtr<CefValue> Input,
		FObjectTraversalState& Traversal,
		FPinObjects& Pins,
		UObject* RootObj
	);
	
	static void ObjectPropertyToCefValue(
		CefRefPtr<CefValue> Target,
		FObjectProperty* InProp,
		const void* ValuePtr,
		FObjectTraversalState& Traversal
	);
	
	static void CefValueToObjectProperty(
		FObjectProperty* InProp,
		void* ValuePtr,
		CefRefPtr<CefValue> Input,
		FObjectTraversalState& Traversal,
		FPinObjects& Pins,
		UObject* RootObj
	);
	
	static void StructPropertyToCefValue(
		CefRefPtr<CefValue> Target,
		FStructProperty* InProp,
		const void* ValuePtr,
		FObjectTraversalState& Traversal
	);
	
	static void CefValueToStructProperty(
		FStructProperty* InProp,
		void* ValuePtr,
		CefRefPtr<CefValue> Input,
		FObjectTraversalState& Traversal,
		FPinObjects& Pins,
		UObject* RootObj
	);
	
	static void MapPropertyToCefValue(
		CefRefPtr<CefValue> Target,
		FMapProperty* InProp,
		const void* ValuePtr,
		FObjectTraversalState& Traversal
	);
	
	static void CefValueToMapProperty(
		FMapProperty* InProp,
		void* ValuePtr,
		CefRefPtr<CefValue> Input,
		FObjectTraversalState& Traversal,
		FPinObjects& Pins,
		UObject* RootObj
	);
	
	static void SetPropertyToCefValue(
		CefRefPtr<CefValue> Target,
		FSetProperty* InProp,
		const void* ValuePtr,
		FObjectTraversalState& Traversal
	);
	
	static void CefValueToSetProperty(
		FSetProperty* InProp,
		void* ValuePtr,
		CefRefPtr<CefValue> Input,
		FObjectTraversalState& Traversal,
		FPinObjects& Pins,
		UObject* RootObj
	);
	
	static void ArrayPropertyToCefValue(
		CefRefPtr<CefValue> Target,
		FArrayProperty* InProp,
		const void* ValuePtr,
		FObjectTraversalState& Traversal
	);
	
	static void CefValueToArrayProperty(
		FArrayProperty* InProp,
		void* ValuePtr,
		CefRefPtr<CefValue> Input,
		FObjectTraversalState& Traversal,
		FPinObjects& Pins,
		UObject* RootObj
	);
};