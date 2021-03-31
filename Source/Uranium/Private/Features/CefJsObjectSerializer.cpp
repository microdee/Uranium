// Fill out your copyright notice in the Description page of Project Settings.


#include "CefJsObjectSerializer.h"

#include "Uranium.h"

void FObjectTraversalState::Add(UObject* InObject, CefRefPtr<CefDictionaryValue> InValue)
{
    ObjectValueAssoc.Add(InObject, InValue);
    ValueObjectAssoc.Add(InValue.get(), InObject);
}

FPinObjects::FPinObjects()
{
}

FPinObjects::FPinObjects(TWeakObjectPtr<UObject> InObject)
{
    if(InObject.IsValid()) With(InObject.Get());
}

FPinObjects::FPinObjects(UObject* InObject)
{
    With(InObject);
}

void FPinObjects::With(UObject* InObject)
{
    if(!InObject->IsValidLowLevel()) return;
    if(InObject->IsRooted()) return;
    InObject->AddToRoot();
    Objects.Add(InObject);
}

FPinObjects::~FPinObjects()
{
    for(auto Obj : Objects)
    {
        Obj->RemoveFromRoot();
    }
}

CefRefPtr<CefDictionaryValue> FCefJsObjectSerializer::ToCefValue(TWeakObjectPtr<UObject> From)
{
    auto CefMap = CefDictionaryValue::Create();
    if(From.IsValid())
    {
        FPinObjects Pin(From);
        FObjectTraversalState Traversal {};

        for(
            TPropertyValueIterator<FProperty> It(
                From->GetClass(),
                From.Get(),
                EPropertyValueIteratorFlags::NoRecursion,
                EFieldIteratorFlags::ExcludeDeprecated
            );
            It; ++It
        ) {
            auto Prop = It.Key();
            auto Val = It.Value();
            CefMap->SetValue(*Prop->GetName(), FieldToCefValue(Prop, Val, Traversal));
        }
    }
    return CefMap;
}

void FCefJsObjectSerializer::ToUObject(TWeakObjectPtr<UObject> Target, CefRefPtr<CefDictionaryValue> From)
{
    if(Target.IsValid())
    {
        FPinObjects Pin(Target);
        FObjectTraversalState Traversal {};

        for(
            TPropertyValueIterator<FProperty> It(
                Target->GetClass(),
                Target.Get(),
                EPropertyValueIteratorFlags::NoRecursion,
                EFieldIteratorFlags::ExcludeDeprecated
            );
            It; ++It
        )
        {
            auto Prop = It.Key();
        
            // removing constness in the hope that modifying the value ptr will not cause issues
            // just like everywhere else regarding working with property values
            auto Val = const_cast<void*>(It.Value());
            
            CefValueToField(
                Prop, Val,
                From->GetValue(*Prop->GetName()),
                Traversal, Pin, Target.Get()
            );
        }
    }
}

bool IsStringProperty(FField* InField)
{
    if(!InField->IsValidLowLevel()) return false;
    return InField->IsA<FStrProperty>()
        || InField->IsA<FTextProperty>()
        || InField->IsA<FNameProperty>();
}

bool IsStringOrEnumProperty(FField* InField)
{
    return IsStringProperty(InField)
        || InField->IsA<FEnumProperty>();
}

FString GetValueOfStringProperty(FField* InField, const void* ValuePtr)
{
    // ReSharper disable CppDeclarationHidesLocal
    // ReSharper disable CppLocalVariableMayBeConst
    if(auto Prop = CastField<FStrProperty>(InField))
    {
        return Prop->GetPropertyValue(ValuePtr);
    }
    if(auto Prop = CastField<FTextProperty>(InField))
    {
        return Prop->GetPropertyValue(ValuePtr).ToString();
    }
    if(auto Prop = CastField<FNameProperty>(InField))
    {
        return Prop->GetPropertyValue(ValuePtr).ToString();
    }
    if(auto Prop = CastField<FEnumProperty>(InField))
    {
        auto Val = Prop->GetUnderlyingProperty()->GetSignedIntPropertyValue(ValuePtr);
        auto Name = Prop->GetEnum()->GetNameByIndex(Val);
        return Name.ToString();
    }
    // ReSharper restore CppLocalVariableMayBeConst
    // ReSharper restore CppDeclarationHidesLocal
    
    return {};
}

void SetValueOfStringProperty(FField* InField, void* ValuePtr, CefRefPtr<CefValue> Input)
{
    // ReSharper disable CppDeclarationHidesLocal
    // ReSharper disable CppLocalVariableMayBeConst
    FString InputStr(Input->GetString().c_str());
    if(auto Prop = CastField<FStrProperty>(InField))
    {
        Prop->SetPropertyValue(ValuePtr, InputStr);
    }
    else if(auto Prop = CastField<FTextProperty>(InField))
    {
        Prop->SetPropertyValue(ValuePtr, FText::FromString(InputStr));
    }
    else if(auto Prop = CastField<FNameProperty>(InField))
    {
        Prop->SetPropertyValue(ValuePtr, FName(*InputStr));
    }
    else if(auto Prop = CastField<FEnumProperty>(InField))
    {
        auto ValProp = Prop->GetUnderlyingProperty();
        int ValId = Prop->GetEnum()->GetIndexByName(FName(*InputStr));
        ValProp->SetIntPropertyValue(ValuePtr, static_cast<int64>(ValId));
    }
    // ReSharper restore CppLocalVariableMayBeConst
    // ReSharper restore CppDeclarationHidesLocal
}

CefRefPtr<CefValue> FCefJsObjectSerializer::FieldToCefValue(
    FField* InField,
    const void* ValuePtr,
    FObjectTraversalState& Traversal
) {
    CefRefPtr<CefValue> Target = CefValue::Create();
    
    // Field is valid?
    if(!ValuePtr || !InField->IsValidLowLevel())
    {
        UE_LOG(LogUranium, Warning, TEXT("[JS Interop] Value or field was nullptr"));
        Target->SetNull();
        return Target;
    }

    // ReSharper disable CppDeclarationHidesLocal
    // ReSharper disable CppLocalVariableMayBeConst
    if(auto Prop = CastField<FBoolProperty>(InField))
    {
        Target->SetBool(Prop->GetPropertyValue(ValuePtr));
    }
    else if(auto Prop = CastField<FNumericProperty>(InField))
    {
        if(Prop->IsInteger())
            Target->SetInt(Prop->GetSignedIntPropertyValue(ValuePtr));
        if(Prop->IsFloatingPoint())
            Target->SetDouble(Prop->GetFloatingPointPropertyValue(ValuePtr));
    }
    else if(IsStringOrEnumProperty(InField))
    {
        Target->SetString(*GetValueOfStringProperty(InField, ValuePtr));
    }
    else if(auto Prop = CastField<FObjectProperty>(InField))
    {
        ObjectPropertyToCefValue(Target, Prop, ValuePtr, Traversal);
    }
    else if(auto Prop = CastField<FStructProperty>(InField))
    {
        StructPropertyToCefValue(Target, Prop, ValuePtr, Traversal);
    }
    else if(auto Prop = CastField<FMapProperty>(InField))
    {
        MapPropertyToCefValue(Target, Prop, ValuePtr, Traversal);
    }
    else if(auto Prop = CastField<FSetProperty>(InField))
    {
        SetPropertyToCefValue(Target, Prop, ValuePtr, Traversal);
    }
    else if(auto Prop = CastField<FArrayProperty>(InField))
    {
        ArrayPropertyToCefValue(Target, Prop, ValuePtr, Traversal);
    }
    // ReSharper restore CppLocalVariableMayBeConst
    // ReSharper restore CppDeclarationHidesLocal
    
    return Target;
}

void FCefJsObjectSerializer::CefValueToField(
    FField* InField,
    void* ValuePtr,
    CefRefPtr<CefValue> Input,
    FObjectTraversalState& Traversal,
    FPinObjects& Pins,
    UObject* RootObj
) {
    if(!ValuePtr || !InField->IsValidLowLevel())
    {
        UE_LOG(LogUranium, Warning, TEXT("[JS Interop] Value destination or field was nullptr"));
        return;
    }

    // ReSharper disable CppDeclarationHidesLocal
    // ReSharper disable CppLocalVariableMayBeConst
    if(auto Prop = CastField<FBoolProperty>(InField))
    {
        Prop->SetPropertyValue(ValuePtr, Input->GetBool());
    }
    else if(auto Prop = CastField<FNumericProperty>(InField))
    {
        if(Prop->IsInteger())
            Prop->SetIntPropertyValue(ValuePtr, static_cast<int64>(Input->GetInt()));
        if(Prop->IsFloatingPoint())
            Prop->SetFloatingPointPropertyValue(ValuePtr, Input->GetDouble());
    }
    else if(IsStringOrEnumProperty(InField))
    {
        SetValueOfStringProperty(InField, ValuePtr, Input);
    }
    else if(auto Prop = CastField<FObjectProperty>(InField))
    {
        CefValueToObjectProperty(Prop, ValuePtr, Input, Traversal, Pins, RootObj);
    }
    else if(auto Prop = CastField<FStructProperty>(InField))
    {
        CefValueToStructProperty(Prop, ValuePtr, Input, Traversal, Pins, RootObj);
    }
    else if(auto Prop = CastField<FMapProperty>(InField))
    {
        CefValueToMapProperty(Prop, ValuePtr, Input, Traversal, Pins, RootObj);
    }
    else if(auto Prop = CastField<FSetProperty>(InField))
    {
        CefValueToSetProperty(Prop, ValuePtr, Input, Traversal, Pins, RootObj);
    }
    else if(auto Prop = CastField<FArrayProperty>(InField))
    {
        CefValueToArrayProperty(Prop, ValuePtr, Input, Traversal, Pins, RootObj);
    }
    // ReSharper restore CppLocalVariableMayBeConst
    // ReSharper restore CppDeclarationHidesLocal
}

void FCefJsObjectSerializer::ObjectPropertyToCefValue(
    CefRefPtr<CefValue> Target,
    FObjectProperty* InProp,
    const void* ValuePtr,
    FObjectTraversalState& Traversal
) {
    auto Obj = InProp->GetObjectPropertyValue(ValuePtr);
    if(!Obj->IsValidLowLevel())
    {
        Target->SetNull();
        return;
    }

    // We've seen this object before
    if(Traversal.ObjectValueAssoc.Contains(Obj))
    {
        Target->SetDictionary(Traversal.ObjectValueAssoc[Obj]);
        return;
    }
    
    auto CefMap = CefDictionaryValue::Create();
    Traversal.Add(Obj, CefMap);

    for(
        TPropertyValueIterator<FProperty> It(
            Obj->GetClass(),
            Obj,
            EPropertyValueIteratorFlags::NoRecursion,
            EFieldIteratorFlags::ExcludeDeprecated
        );
        It; ++It
    ) {
        auto Prop = It.Key();
        auto Val = It.Value();
        CefMap->SetValue(*Prop->GetName(), FieldToCefValue(Prop, Val, Traversal));
    }
    
    Target->SetDictionary(CefMap);
}

void FCefJsObjectSerializer::CefValueToObjectProperty(
    FObjectProperty* InProp,
    void* ValuePtr,
    CefRefPtr<CefValue> Input,
    FObjectTraversalState& Traversal,
    FPinObjects& Pins,
    UObject* RootObj
) {
    if(!ValuePtr || !InProp->IsValidLowLevel())
    {
        UE_LOG(LogUranium, Warning, TEXT("[JS Interop] Object value destination or field was nullptr"));
        return;
    }
    auto Obj = InProp->GetObjectPropertyValue(ValuePtr);
    auto CefMap = Input->GetDictionary();

    // If input CEF Value is not a dictionary then assume null
    if(!CefMap)
    {
        InProp->SetObjectPropertyValue(ValuePtr, nullptr);
        return;
    }

    // We've seen this object before
    if(Traversal.ValueObjectAssoc.Contains(CefMap.get()))
    {
        InProp->SetObjectPropertyValue(ValuePtr, Traversal.ValueObjectAssoc[CefMap.get()]);
        return;
    }

    // If there's no object allocated yet in target property, create one
    if(!Obj->IsValidLowLevel())
    {
        // TODO: Take care of inheritance problems
        // This is limited to non-abstract classes and can only work with the class known by the property
        auto InClass = InProp->PropertyClass;
        Obj = NewObject<UObject>(RootObj, InProp->PropertyClass);
        Pins.With(Obj);
    }

    Traversal.Add(Obj, CefMap);

    // obviously only commonly present members will be regarded
    for(
        TPropertyValueIterator<FProperty> It(
            Obj->GetClass(),
            Obj,
            EPropertyValueIteratorFlags::NoRecursion,
            EFieldIteratorFlags::ExcludeDeprecated
        );
        It; ++It
    ) {
        auto Prop = It.Key();
        
        // removing constness in the hope that modifying the value ptr will not cause issues
        // just like everywhere else regarding working with property values
        auto Val = const_cast<void*>(It.Value());
        
        if(CefMap->HasKey(*Prop->GetName()))
        {
            auto InVal = CefMap->GetValue(*Prop->GetName());
            CefValueToField(Prop, Val, InVal, Traversal, Pins, RootObj);
        }
    }
}

void FCefJsObjectSerializer::StructPropertyToCefValue(
    CefRefPtr<CefValue> Target,
    FStructProperty* InProp,
    const void* ValuePtr,
    FObjectTraversalState& Traversal
) {
    auto ObjStruct = InProp->Struct;
    
    auto CefMap = CefDictionaryValue::Create();
    
    for(
        TPropertyValueIterator<FProperty> It(
            ObjStruct,
            ValuePtr,
            EPropertyValueIteratorFlags::NoRecursion,
            EFieldIteratorFlags::ExcludeDeprecated
        );
        It; ++It
    ) {
        auto Prop = It.Key();
        auto Val = It.Value();
        CefMap->SetValue(*Prop->GetName(), FieldToCefValue(Prop, Val, Traversal));
    }
    
    Target->SetDictionary(CefMap);
}

void FCefJsObjectSerializer::CefValueToStructProperty(
    FStructProperty* InProp,
    void* ValuePtr,
    CefRefPtr<CefValue> Input,
    FObjectTraversalState& Traversal,
    FPinObjects& Pins,
    UObject* RootObj
) {
    if(!ValuePtr || !InProp->IsValidLowLevel())
    {
        UE_LOG(LogUranium, Warning, TEXT("[JS Interop] Object value destination or field was nullptr"));
        return;
    }
    auto CefMap = Input->GetDictionary();

    // If input CEF Value is not a dictionary then we'll ignore this sctruct
    if(!CefMap)
    {
        // TODO: set default value instead of ignoring
        return;
    }

    // obviously only commonly present members will be regarded
    for(
        TPropertyValueIterator<FProperty> It(
            InProp->Struct,
            ValuePtr,
            EPropertyValueIteratorFlags::NoRecursion,
            EFieldIteratorFlags::ExcludeDeprecated
        );
        It; ++It
    ) {
        auto Prop = It.Key();
        
        // removing constness in the hope that modifying the value ptr will not cause issues
        // just like everywhere else regarding working with property values
        auto Val = const_cast<void*>(It.Value());
        
        if(CefMap->HasKey(*Prop->GetName()))
        {
            auto InVal = CefMap->GetValue(*Prop->GetName());
            CefValueToField(Prop, Val, InVal, Traversal, Pins, RootObj);
        }
    }
}

void FCefJsObjectSerializer::MapPropertyToCefValue(
    CefRefPtr<CefValue> Target,
    FMapProperty* InProp,
    const void* ValuePtr,
    FObjectTraversalState& Traversal
) {
    if(!IsStringOrEnumProperty(InProp->KeyProp)) {
        UE_LOG(LogUranium, Warning, TEXT("[JS Interop] Only String, Name, Text or Enum keys are supported in Maps."));
        Target->SetNull();
        return;
    }
    
    auto CefMap = CefDictionaryValue::Create();
    const FScriptMap& InMap = InProp->GetPropertyValue(ValuePtr);
    FScriptMapHelper MapHelper(InProp, ValuePtr);
    
    for(int i=0; i<InMap.Num(); i++)
    {
        uint8* Key = MapHelper.GetKeyPtr(i);
        uint8* Value = MapHelper.GetValuePtr(i);
        FString StrKey = GetValueOfStringProperty(InProp->KeyProp, Key);
        CefMap->SetValue(*StrKey, FieldToCefValue(InProp->ValueProp, Value, Traversal));
    }
    
    Target->SetDictionary(CefMap);
}

void FCefJsObjectSerializer::CefValueToMapProperty(
    FMapProperty* InProp,
    void* ValuePtr,
    CefRefPtr<CefValue> Input,
    FObjectTraversalState& Traversal,
    FPinObjects& Pins,
    UObject* RootObj
) {
    if(!IsStringOrEnumProperty(InProp->KeyProp)) {
        UE_LOG(LogUranium, Warning, TEXT("[JS Interop] Only String, Name, Text or Enum keys are supported in Maps."));
        return;
    }
    auto CefMap = Input->GetDictionary();
    const FScriptMap& InMap = InProp->GetPropertyValue(ValuePtr);
    FScriptMapHelper MapHelper(InProp, ValuePtr);

    // Destination map is cleared and built up again from scratch
    // so we wouldn't need key comparison here (more elegant code when going through data)
    MapHelper.EmptyValues();
    
    // If input CEF Value is not a dictionary then we'll ignore this map
    if(!CefMap) return;
    
    CefDictionaryValue::KeyList keys;
    CefMap->GetKeys(keys);
    for (auto const& k : keys)
    {
        int Index = MapHelper.AddDefaultValue_Invalid_NeedsRehash();
        uint8* Key = MapHelper.GetKeyPtr(Index);
        uint8* Value = MapHelper.GetValuePtr(Index);
        auto KeyStr = CefValue::Create();
        KeyStr->SetString(k);
        
        SetValueOfStringProperty(InProp->KeyProp, Key, KeyStr);
        CefValueToField(InProp->ValueProp, Value, CefMap->GetValue(k), Traversal, Pins, RootObj);
        
        MapHelper.Rehash();
    }
}

void FCefJsObjectSerializer::SetPropertyToCefValue(
    CefRefPtr<CefValue> Target,
    FSetProperty* InProp,
    const void* ValuePtr,
    FObjectTraversalState& Traversal
) {
    auto CefList = CefListValue::Create();
    const FScriptSet InSet = InProp->GetPropertyValue(ValuePtr);
    FScriptSetHelper SetHelper(InProp, ValuePtr);
    
    CefList->SetSize(InSet.Num());

    for(int i=0; i<InSet.Num(); i++)
    {
        auto Val = SetHelper.GetElementPtr(i);
        CefList->SetValue(i, FieldToCefValue(InProp->ElementProp, Val, Traversal));
    }
    Target->SetList(CefList);
}

void FCefJsObjectSerializer::CefValueToSetProperty(
    FSetProperty* InProp,
    void* ValuePtr,
    CefRefPtr<CefValue> Input,
    FObjectTraversalState& Traversal,
    FPinObjects& Pins,
    UObject* RootObj
) {
    auto CefList = Input->GetList();
    const FScriptSet InSet = InProp->GetPropertyValue(ValuePtr);
    FScriptSetHelper SetHelper(InProp, ValuePtr);

    // Destination set is cleared and built up again from scratch
    // so we wouldn't need element comparison here (more elegant code when going through data)
    SetHelper.EmptyElements();

    // If input CEF Value is not a list then we'll ignore this set
    if(!CefList) return;

    for(int i=0; i<CefList->GetSize(); i++)
    {
        int Index = SetHelper.AddDefaultValue_Invalid_NeedsRehash();
        uint8* Value = SetHelper.GetElementPtr(Index);
        CefValueToField(InProp->ElementProp, Value, CefList->GetValue(i), Traversal, Pins, RootObj);

        SetHelper.Rehash();
    }
}

void FCefJsObjectSerializer::ArrayPropertyToCefValue(
    CefRefPtr<CefValue> Target,
    FArrayProperty* InProp,
    const void* ValuePtr,
    FObjectTraversalState& Traversal
) {
    const FScriptArray InArray = InProp->GetPropertyValue(ValuePtr);
    FScriptArrayHelper ArrayHelper(InProp, ValuePtr);

    // If it's an array of bytes just treat it as a binary value
    if(InProp->Inner->IsA<FByteProperty>())
    {
        auto CefBin = CefBinaryValue::Create(ArrayHelper.GetRawPtr(), InArray.Num());
        Target->SetBinary(CefBin);
        return;
    }
   
    auto CefList = CefListValue::Create(); 
    CefList->SetSize(InArray.Num());

    for(int i=0; i<InArray.Num(); i++)
    {
        auto Val = ArrayHelper.GetRawPtr(i);
        CefList->SetValue(i, FieldToCefValue(InProp->Inner, Val, Traversal));
    }
    Target->SetList(CefList);
}

void FCefJsObjectSerializer::CefValueToArrayProperty(
    FArrayProperty* InProp,
    void* ValuePtr,
    CefRefPtr<CefValue> Input,
    FObjectTraversalState& Traversal,
    FPinObjects& Pins,
    UObject* RootObj
) {
    const FScriptArray InArray = InProp->GetPropertyValue(ValuePtr);
    FScriptArrayHelper ArrayHelper(InProp, ValuePtr);
    
    // If it's an array of bytes just treat it as a binary value
    if(InProp->Inner->IsA<FByteProperty>())
    {
        auto CefBin = Input->GetBinary();
        if(!CefBin)
        {
            ArrayHelper.EmptyValues();
            return;
        }
        ArrayHelper.Resize(CefBin->GetSize());
        CefBin->GetData(ArrayHelper.GetRawPtr(), CefBin->GetSize(), 0);
        return;
    }

    auto CefList = Input->GetList();

    // If input CEF Value is not a list then we'll empty this array
    if(!CefList)
    {
        ArrayHelper.EmptyValues();
        return;
    }

    ArrayHelper.Resize(CefList->GetSize());

    for(int i=0; i<CefList->GetSize(); i++)
    {
        auto Val = ArrayHelper.GetRawPtr(i);
        CefValueToField(InProp->Inner, Val, CefList->GetValue(i), Traversal, Pins, RootObj);
    }
}
