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

	void Add(UObject* object, CefRefPtr<CefDictionaryValue> value);
};

class FPinObjects
{
	TArray<UObject*> Objects;
public:
	FPinObjects();
	explicit FPinObjects(TWeakObjectPtr<UObject> object);
	explicit FPinObjects(UObject* object);
	~FPinObjects();
	
	void With(UObject* object);
};

/**
 * 
 */
class URANIUM_API FCefJsObjectSerializer
{
public:
	static CefRefPtr<CefDictionaryValue> ToCefValue(TWeakObjectPtr<UObject> from);
	static void ToUObject(TWeakObjectPtr<UObject> target, CefRefPtr<CefDictionaryValue> from);

private:
	static CefRefPtr<CefValue> FieldToCefValue(
		FField* field,
		const void* valuePtr,
		FObjectTraversalState& traversal
	);
	
	static void CefValueToField(
		FField* field,
		void* valuePtr,
		CefRefPtr<CefValue> input,
		FObjectTraversalState& traversal,
		FPinObjects& pins,
		UObject* rootObj
	);
	
	static void ObjectPropertyToCefValue(
		CefRefPtr<CefValue> target,
		FObjectProperty* inProp,
		const void* valuePtr,
		FObjectTraversalState& traversal
	);
	
	static void CefValueToObjectProperty(
		FObjectProperty* inProp,
		void* valuePtr,
		CefRefPtr<CefValue> input,
		FObjectTraversalState& traversal,
		FPinObjects& pins,
		UObject* rootObj
	);
	
	static void StructPropertyToCefValue(
		CefRefPtr<CefValue> target,
		FStructProperty* inProp,
		const void* valuePtr,
		FObjectTraversalState& traversal
	);
	
	static void CefValueToStructProperty(
		FStructProperty* inProp,
		void* valuePtr,
		CefRefPtr<CefValue> input,
		FObjectTraversalState& traversal,
		FPinObjects& pins,
		UObject* rootObj
	);
	
	static void MapPropertyToCefValue(
		CefRefPtr<CefValue> target,
		FMapProperty* inProp,
		const void* valuePtr,
		FObjectTraversalState& traversal
	);
	
	static void CefValueToMapProperty(
		FMapProperty* inProp,
		void* valuePtr,
		CefRefPtr<CefValue> input,
		FObjectTraversalState& traversal,
		FPinObjects& pins,
		UObject* rootObj
	);
	
	static void SetPropertyToCefValue(
		CefRefPtr<CefValue> target,
		FSetProperty* inProp,
		const void* valuePtr,
		FObjectTraversalState& traversal
	);
	
	static void CefValueToSetProperty(
		FSetProperty* inProp,
		void* valuePtr,
		CefRefPtr<CefValue> input,
		FObjectTraversalState& traversal,
		FPinObjects& pins,
		UObject* rootObj
	);
	
	static void ArrayPropertyToCefValue(
		CefRefPtr<CefValue> target,
		FArrayProperty* inProp,
		const void* valuePtr,
		FObjectTraversalState& traversal
	);
	
	static void CefValueToArrayProperty(
		FArrayProperty* inProp,
		void* valuePtr,
		CefRefPtr<CefValue> input,
		FObjectTraversalState& traversal,
		FPinObjects& pins,
		UObject* rootObj
	);
};