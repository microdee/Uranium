/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/


#include "Features/CefJsObjectSerializer.h"

#include "Uranium.h"

void FObjectTraversalState::Add(UObject* object, CefRefPtr<CefDictionaryValue> value)
{
	ObjectValueAssoc.Add(object, value);
	ValueObjectAssoc.Add(value.get(), object);
}

FPinObjects::FPinObjects()
{
}

FPinObjects::FPinObjects(TWeakObjectPtr<UObject> object)
{
	if(object.IsValid())
	{
		With(object.Get());
	}
}

FPinObjects::FPinObjects(UObject* object)
{
	With(object);
}

void FPinObjects::With(UObject* object)
{
	if(!object || !object->IsValidLowLevel())
	{
		return;
	}
	if(object->IsRooted())
	{
		return;
	}
	
	object->AddToRoot();
	Objects.Add(object);
}

FPinObjects::~FPinObjects()
{
	for(auto obj : Objects)
	{
		obj->RemoveFromRoot();
	}
}

CefRefPtr<CefDictionaryValue> FCefJsObjectSerializer::ToCefValue(TWeakObjectPtr<UObject> from)
{
	auto cefMap = CefDictionaryValue::Create();
	if(from.IsValid())
	{
		FPinObjects pin(from);
		FObjectTraversalState traversal {};

		for(
			TPropertyValueIterator<FProperty> it(
				from->GetClass(),
				from.Get(),
				EPropertyValueIteratorFlags::NoRecursion,
				EFieldIteratorFlags::ExcludeDeprecated
			);
			it; ++it
		) {
			FProperty* prop = it.Key();
			const void* val = it.Value();
			cefMap->SetValue(*prop->GetName(), FieldToCefValue(prop, val, traversal));
		}
	}
	return cefMap;
}

void FCefJsObjectSerializer::ToUObject(TWeakObjectPtr<UObject> target, CefRefPtr<CefDictionaryValue> from)
{
	if(target.IsValid())
	{
		FPinObjects pin(target);
		FObjectTraversalState traversal {};

		for(
			TPropertyValueIterator<FProperty> it(
				target->GetClass(),
				target.Get(),
				EPropertyValueIteratorFlags::NoRecursion,
				EFieldIteratorFlags::ExcludeDeprecated
			);
			it; ++it
		)
		{
			FProperty* prop = it.Key();
		
			// removing constness in the hope that modifying the value ptr will not cause issues
			// just like everywhere else regarding working with property values
			void* val = const_cast<void*>(it.Value());
			
			CefValueToField(
				prop, val,
				from->GetValue(*prop->GetName()),
				traversal, pin, target.Get()
			);
		}
	}
}

bool IsStringProperty(FField* field)
{
	if(!field || !field->IsValidLowLevel())
	{
		return false;
	}
	
	return field->IsA<FStrProperty>()
		|| field->IsA<FTextProperty>()
		|| field->IsA<FNameProperty>();
}

bool IsStringOrEnumProperty(FField* field)
{
	return IsStringProperty(field)
		|| field->IsA<FEnumProperty>();
}

FString GetValueOfStringProperty(FField* field, const void* valuePtr)
{
	// ReSharper disable CppDeclarationHidesLocal
	// ReSharper disable CppLocalVariableMayBeConst
#pragma warning( disable : 4456 ) // declaration of 'X' hides previous local declaration // it's intentional here
	if(auto prop = CastField<FStrProperty>(field))
	{
		return prop->GetPropertyValue(valuePtr);
	}
	if(auto prop = CastField<FTextProperty>(field))
	{
		return prop->GetPropertyValue(valuePtr).ToString();
	}
	if(auto prop = CastField<FNameProperty>(field))
	{
		return prop->GetPropertyValue(valuePtr).ToString();
	}
	if(auto prop = CastField<FEnumProperty>(field))
	{
		int64 Val = prop->GetUnderlyingProperty()->GetSignedIntPropertyValue(valuePtr);
		FName Name = prop->GetEnum()->GetNameByIndex(Val);
		return Name.ToString();
	}
#pragma warning( default : 4456 )
	// ReSharper restore CppLocalVariableMayBeConst
	// ReSharper restore CppDeclarationHidesLocal
	
	return {};
}

void SetValueOfStringProperty(FField* field, void* valuePtr, CefRefPtr<CefValue> input)
{
	// ReSharper disable CppDeclarationHidesLocal
	// ReSharper disable CppLocalVariableMayBeConst
#pragma warning( disable : 4456 ) // declaration of 'X' hides previous local declaration // it's intentional here
	FString InputStr(input->GetString().c_str());
	if(auto prop = CastField<FStrProperty>(field))
	{
		prop->SetPropertyValue(valuePtr, InputStr);
	}
	else if(auto prop = CastField<FTextProperty>(field))
	{
		prop->SetPropertyValue(valuePtr, FText::FromString(InputStr));
	}
	else if(auto prop = CastField<FNameProperty>(field))
	{
		prop->SetPropertyValue(valuePtr, FName(*InputStr));
	}
	else if(auto prop = CastField<FEnumProperty>(field))
	{
		FNumericProperty* valProp = prop->GetUnderlyingProperty();
		int valId = prop->GetEnum()->GetIndexByName(FName(*InputStr));
		valProp->SetIntPropertyValue(valuePtr, static_cast<int64>(valId));
	}
#pragma warning( default : 4456 )
	// ReSharper restore CppLocalVariableMayBeConst
	// ReSharper restore CppDeclarationHidesLocal
}

CefRefPtr<CefValue> FCefJsObjectSerializer::FieldToCefValue(
	FField* field,
	const void* valuePtr,
	FObjectTraversalState& traversal
) {
	CefRefPtr<CefValue> target = CefValue::Create();
	
	// Field is valid?
	if(!valuePtr || !field || !field->IsValidLowLevel())
	{
		UE_LOG(LogUranium, Warning, TEXT("[JS Interop] Value or field was nullptr"));
		target->SetNull();
		return target;
	}

	// ReSharper disable CppDeclarationHidesLocal
	// ReSharper disable CppLocalVariableMayBeConst
#pragma warning( disable : 4456 ) // declaration of 'X' hides previous local declaration // it's intentional here
	if(auto prop = CastField<FBoolProperty>(field))
	{
		target->SetBool(prop->GetPropertyValue(valuePtr));
	}
	else if(auto prop = CastField<FNumericProperty>(field))
	{
		if(prop->IsInteger())
			target->SetInt(prop->GetSignedIntPropertyValue(valuePtr));
		if(prop->IsFloatingPoint())
			target->SetDouble(prop->GetFloatingPointPropertyValue(valuePtr));
	}
	else if(IsStringOrEnumProperty(field))
	{
		target->SetString(*GetValueOfStringProperty(field, valuePtr));
	}
	else if(auto prop = CastField<FObjectProperty>(field))
	{
		ObjectPropertyToCefValue(target, prop, valuePtr, traversal);
	}
	else if(auto prop = CastField<FStructProperty>(field))
	{
		StructPropertyToCefValue(target, prop, valuePtr, traversal);
	}
	else if(auto prop = CastField<FMapProperty>(field))
	{
		MapPropertyToCefValue(target, prop, valuePtr, traversal);
	}
	else if(auto prop = CastField<FSetProperty>(field))
	{
		SetPropertyToCefValue(target, prop, valuePtr, traversal);
	}
	else if(auto prop = CastField<FArrayProperty>(field))
	{
		ArrayPropertyToCefValue(target, prop, valuePtr, traversal);
	}
#pragma warning( default : 4456 )
	// ReSharper restore CppLocalVariableMayBeConst
	// ReSharper restore CppDeclarationHidesLocal
	
	return target;
}

void FCefJsObjectSerializer::CefValueToField(
	FField* field,
	void* valuePtr,
	CefRefPtr<CefValue> input,
	FObjectTraversalState& traversal,
	FPinObjects& pins,
	UObject* rootObj
) {
	if(!valuePtr || !field || !field->IsValidLowLevel())
	{
		UE_LOG(LogUranium, Warning, TEXT("[JS Interop] Value destination or field was nullptr"));
		return;
	}

	// ReSharper disable CppDeclarationHidesLocal
	// ReSharper disable CppLocalVariableMayBeConst
#pragma warning( disable : 4456 ) // declaration of 'X' hides previous local declaration // it's intentional here
	if(auto prop = CastField<FBoolProperty>(field))
	{
		prop->SetPropertyValue(valuePtr, input->GetBool());
	}
	else if(auto prop = CastField<FNumericProperty>(field))
	{
		if(prop->IsInteger())
		{
			prop->SetIntPropertyValue(valuePtr, static_cast<int64>(input->GetInt()));
		}
		if(prop->IsFloatingPoint())
		{
			prop->SetFloatingPointPropertyValue(valuePtr, input->GetDouble());
		}
	}
	else if(IsStringOrEnumProperty(field))
	{
		SetValueOfStringProperty(field, valuePtr, input);
	}
	else if(auto prop = CastField<FObjectProperty>(field))
	{
		CefValueToObjectProperty(prop, valuePtr, input, traversal, pins, rootObj);
	}
	else if(auto prop = CastField<FStructProperty>(field))
	{
		CefValueToStructProperty(prop, valuePtr, input, traversal, pins, rootObj);
	}
	else if(auto prop = CastField<FMapProperty>(field))
	{
		CefValueToMapProperty(prop, valuePtr, input, traversal, pins, rootObj);
	}
	else if(auto prop = CastField<FSetProperty>(field))
	{
		CefValueToSetProperty(prop, valuePtr, input, traversal, pins, rootObj);
	}
	else if(auto prop = CastField<FArrayProperty>(field))
	{
		CefValueToArrayProperty(prop, valuePtr, input, traversal, pins, rootObj);
	}
#pragma warning( default : 4456 )
	// ReSharper restore CppLocalVariableMayBeConst
	// ReSharper restore CppDeclarationHidesLocal
}

void FCefJsObjectSerializer::ObjectPropertyToCefValue(
	CefRefPtr<CefValue> target,
	FObjectProperty* inProp,
	const void* valuePtr,
	FObjectTraversalState& traversal
) {
	UObject* obj = inProp->GetObjectPropertyValue(valuePtr);
	if(!obj || !obj->IsValidLowLevel())
	{
		target->SetNull();
		return;
	}

	// We've seen this object before
	if(traversal.ObjectValueAssoc.Contains(obj))
	{
		target->SetDictionary(traversal.ObjectValueAssoc[obj]);
		return;
	}
	
	auto CefMap = CefDictionaryValue::Create();
	traversal.Add(obj, CefMap);

	for(
		TPropertyValueIterator<FProperty> it(
			obj->GetClass(),
			obj,
			EPropertyValueIteratorFlags::NoRecursion,
			EFieldIteratorFlags::ExcludeDeprecated
		);
		it; ++it
	) {
		FProperty* prop = it.Key();
		const void* val = it.Value();
		CefMap->SetValue(*prop->GetName(), FieldToCefValue(prop, val, traversal));
	}
	
	target->SetDictionary(CefMap);
}

void FCefJsObjectSerializer::CefValueToObjectProperty(
	FObjectProperty* inProp,
	void* valuePtr,
	CefRefPtr<CefValue> input,
	FObjectTraversalState& traversal,
	FPinObjects& pins,
	UObject* rootObj
) {
	if(!valuePtr || !inProp || !inProp->IsValidLowLevel())
	{
		UE_LOG(LogUranium, Warning, TEXT("[JS Interop] Object value destination or field was nullptr"));
		return;
	}
	UObject* obj = inProp->GetObjectPropertyValue(valuePtr);
	auto cefMap = input->GetDictionary();

	// If input CEF Value is not a dictionary then assume null
	if(!cefMap)
	{
		inProp->SetObjectPropertyValue(valuePtr, nullptr);
		return;
	}

	// We've seen this object before
	if(traversal.ValueObjectAssoc.Contains(cefMap.get()))
	{
		inProp->SetObjectPropertyValue(valuePtr, traversal.ValueObjectAssoc[cefMap.get()]);
		return;
	}

	// If there's no object allocated yet in target property, create one
	if(!obj || !obj->IsValidLowLevel())
	{
		// TODO: Take care of inheritance problems
		// This is limited to non-abstract classes and can only work with the class known by the property
		obj = NewObject<UObject>(rootObj, inProp->PropertyClass);
		pins.With(obj);
	}

	traversal.Add(obj, cefMap);

	// obviously only commonly present members will be regarded
	for(
		TPropertyValueIterator<FProperty> it(
			obj->GetClass(),
			obj,
			EPropertyValueIteratorFlags::NoRecursion,
			EFieldIteratorFlags::ExcludeDeprecated
		);
		it; ++it
	) {
		FProperty* prop = it.Key();
		
		// removing constness in the hope that modifying the value ptr will not cause issues
		// just like everywhere else regarding working with property values
		void* val = const_cast<void*>(it.Value());
		
		if(cefMap->HasKey(*prop->GetName()))
		{
			auto inVal = cefMap->GetValue(*prop->GetName());
			CefValueToField(prop, val, inVal, traversal, pins, rootObj);
		}
	}
}

void FCefJsObjectSerializer::StructPropertyToCefValue(
	CefRefPtr<CefValue> target,
	FStructProperty* inProp,
	const void* valuePtr,
	FObjectTraversalState& traversal
) {
	UScriptStruct* objStruct = inProp->Struct;
	
	auto cefMap = CefDictionaryValue::Create();
	
	for(
		TPropertyValueIterator<FProperty> it(
			objStruct,
			valuePtr,
			EPropertyValueIteratorFlags::NoRecursion,
			EFieldIteratorFlags::ExcludeDeprecated
		);
		it; ++it
	) {
		FProperty* prop = it.Key();
		const void* val = it.Value();
		cefMap->SetValue(*prop->GetName(), FieldToCefValue(prop, val, traversal));
	}
	
	target->SetDictionary(cefMap);
}

void FCefJsObjectSerializer::CefValueToStructProperty(
	FStructProperty* inProp,
	void* valuePtr,
	CefRefPtr<CefValue> input,
	FObjectTraversalState& traversal,
	FPinObjects& pins,
	UObject* rootObj
) {
	if(!valuePtr || !inProp || !inProp->IsValidLowLevel())
	{
		UE_LOG(LogUranium, Warning, TEXT("[JS Interop] Object value destination or field was nullptr"));
		return;
	}
	auto cefMap = input->GetDictionary();

	// If input CEF Value is not a dictionary then we'll ignore this sctruct
	if(!cefMap)
	{
		// TODO: set default value instead of ignoring
		return;
	}

	// obviously only commonly present members will be regarded
	for(
		TPropertyValueIterator<FProperty> it(
			inProp->Struct,
			valuePtr,
			EPropertyValueIteratorFlags::NoRecursion,
			EFieldIteratorFlags::ExcludeDeprecated
		);
		it; ++it
	) {
		FProperty* prop = it.Key();
		
		// removing constness in the hope that modifying the value ptr will not cause issues
		// just like everywhere else regarding working with property values
		void* val = const_cast<void*>(it.Value());
		
		if(cefMap->HasKey(*prop->GetName()))
		{
			auto inVal = cefMap->GetValue(*prop->GetName());
			CefValueToField(prop, val, inVal, traversal, pins, rootObj);
		}
	}
}

void FCefJsObjectSerializer::MapPropertyToCefValue(
	CefRefPtr<CefValue> target,
	FMapProperty* inProp,
	const void* valuePtr,
	FObjectTraversalState& traversal
) {
	if(!IsStringOrEnumProperty(inProp->KeyProp))
	{
		UE_LOG(LogUranium, Warning, TEXT("[JS Interop] Only String, Name, Text or Enum keys are supported in Maps."));
		target->SetNull();
		return;
	}
	
	auto cefMap = CefDictionaryValue::Create();
	const FScriptMap& inMap = inProp->GetPropertyValue(valuePtr);
	FScriptMapHelper mapHelper(inProp, valuePtr);
	
	for(int i=0; i<inMap.Num(); i++)
	{
		uint8* key = mapHelper.GetKeyPtr(i);
		uint8* value = mapHelper.GetValuePtr(i);
		FString strKey = GetValueOfStringProperty(inProp->KeyProp, key);
		cefMap->SetValue(*strKey, FieldToCefValue(inProp->ValueProp, value, traversal));
	}
	
	target->SetDictionary(cefMap);
}

void FCefJsObjectSerializer::CefValueToMapProperty(
	FMapProperty* inProp,
	void* valuePtr,
	CefRefPtr<CefValue> input,
	FObjectTraversalState& traversal,
	FPinObjects& pins,
	UObject* rootObj
) {
	if(!IsStringOrEnumProperty(inProp->KeyProp))
	{
		UE_LOG(LogUranium, Warning, TEXT("[JS Interop] Only String, Name, Text or Enum keys are supported in Maps."));
		return;
	}
	auto cefMap = input->GetDictionary();
	FScriptMapHelper mapHelper(inProp, valuePtr);

	// Destination map is cleared and built up again from scratch
	// so we wouldn't need key comparison here (more elegant code when going through data)
	mapHelper.EmptyValues();
	
	// If input CEF Value is not a dictionary then we'll ignore this map
	if(!cefMap)
	{
		return;
	}
	
	CefDictionaryValue::KeyList keys;
	cefMap->GetKeys(keys);
	for (auto const& k : keys)
	{
		int index = mapHelper.AddDefaultValue_Invalid_NeedsRehash();
		uint8* key = mapHelper.GetKeyPtr(index);
		uint8* value = mapHelper.GetValuePtr(index);
		auto keyStr = CefValue::Create();
		keyStr->SetString(k);
		
		SetValueOfStringProperty(inProp->KeyProp, key, keyStr);
		CefValueToField(inProp->ValueProp, value, cefMap->GetValue(k), traversal, pins, rootObj);
		
		mapHelper.Rehash();
	}
}

void FCefJsObjectSerializer::SetPropertyToCefValue(
	CefRefPtr<CefValue> target,
	FSetProperty* inProp,
	const void* valuePtr,
	FObjectTraversalState& traversal
) {
	auto cefList = CefListValue::Create();
	const FScriptSet inSet = inProp->GetPropertyValue(valuePtr);
	FScriptSetHelper setHelper(inProp, valuePtr);
	
	cefList->SetSize(inSet.Num());

	for(int i=0; i<inSet.Num(); i++)
	{
		uint8* val = setHelper.GetElementPtr(i);
		cefList->SetValue(i, FieldToCefValue(inProp->ElementProp, val, traversal));
	}
	target->SetList(cefList);
}

void FCefJsObjectSerializer::CefValueToSetProperty(
	FSetProperty* inProp,
	void* valuePtr,
	CefRefPtr<CefValue> input,
	FObjectTraversalState& traversal,
	FPinObjects& pins,
	UObject* rootObj
) {
	auto cefList = input->GetList();
	const FScriptSet inSet = inProp->GetPropertyValue(valuePtr);
	FScriptSetHelper setHelper(inProp, valuePtr);

	// Destination set is cleared and built up again from scratch
	// so we wouldn't need element comparison here (more elegant code when going through data)
	setHelper.EmptyElements();

	// If input CEF Value is not a list then we'll ignore this set
	if(!cefList)
	{
		return;
	}

	for(int i=0; i<cefList->GetSize(); i++)
	{
		int index = setHelper.AddDefaultValue_Invalid_NeedsRehash();
		uint8* value = setHelper.GetElementPtr(index);
		CefValueToField(inProp->ElementProp, value, cefList->GetValue(i), traversal, pins, rootObj);

		setHelper.Rehash();
	}
}

void FCefJsObjectSerializer::ArrayPropertyToCefValue(
	CefRefPtr<CefValue> target,
	FArrayProperty* inProp,
	const void* valuePtr,
	FObjectTraversalState& traversal
) {
	const FScriptArray inArray = inProp->GetPropertyValue(valuePtr);
	FScriptArrayHelper arrayHelper(inProp, valuePtr);

	// If it's an array of bytes just treat it as a binary value
	if(inProp->Inner->IsA<FByteProperty>())
	{
		auto cefBin = CefBinaryValue::Create(arrayHelper.GetRawPtr(), inArray.Num());
		target->SetBinary(cefBin);
		return;
	}
   
	auto cefList = CefListValue::Create(); 
	cefList->SetSize(inArray.Num());

	for(int i=0; i<inArray.Num(); i++)
	{
		uint8* val = arrayHelper.GetRawPtr(i);
		cefList->SetValue(i, FieldToCefValue(inProp->Inner, val, traversal));
	}
	target->SetList(cefList);
}

void FCefJsObjectSerializer::CefValueToArrayProperty(
	FArrayProperty* inProp,
	void* valuePtr,
	CefRefPtr<CefValue> input,
	FObjectTraversalState& traversal,
	FPinObjects& pins,
	UObject* rootObj
) {
	const FScriptArray inArray = inProp->GetPropertyValue(valuePtr);
	FScriptArrayHelper arrayHelper(inProp, valuePtr);
	
	// If it's an array of bytes just treat it as a binary value
	if(inProp->Inner->IsA<FByteProperty>())
	{
		auto cefBin = input->GetBinary();
		if(!cefBin)
		{
			arrayHelper.EmptyValues();
			return;
		}
		arrayHelper.Resize(cefBin->GetSize());
		cefBin->GetData(arrayHelper.GetRawPtr(), cefBin->GetSize(), 0);
		return;
	}

	auto cefList = input->GetList();

	// If input CEF Value is not a list then we'll empty this array
	if(!cefList)
	{
		arrayHelper.EmptyValues();
		return;
	}

	arrayHelper.Resize(cefList->GetSize());

	for(int i=0; i<cefList->GetSize(); i++)
	{
		uint8* val = arrayHelper.GetRawPtr(i);
		CefValueToField(inProp->Inner, val, cefList->GetValue(i), traversal, pins, rootObj);
	}
}