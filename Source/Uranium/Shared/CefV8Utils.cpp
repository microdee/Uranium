#include "CefV8Utils.h"

CefRefPtr<CefV8Value> CefV8Utils::to_v8object(CefRefPtr<CefDictionaryValue> const& dictionary, int reclevel /*= 0 */)
{
    auto const obj = CefV8Value::CreateObject(nullptr, nullptr);
    if (reclevel >= CEFV8_UTILS_MAX_RECLEVEL)
    {
        obj->SetValue("error",
            CefV8Value::CreateString("max recursion level reached"),
            V8_PROPERTY_ATTRIBUTE_NONE);
    }
    else if (dictionary)
    {
        auto const attrib = V8_PROPERTY_ATTRIBUTE_READONLY;
        CefDictionaryValue::KeyList keys;
        dictionary->GetKeys(keys);
        for (auto const& k : keys)
        {
            auto const type = dictionary->GetType(k);
            switch (type)
            {
            case VTYPE_BOOL: obj->SetValue(k,
                CefV8Value::CreateBool(dictionary->GetBool(k)), attrib);
                break;
            case VTYPE_INT: obj->SetValue(k,
                CefV8Value::CreateInt(dictionary->GetInt(k)), attrib);
                break;
            case VTYPE_DOUBLE: obj->SetValue(k,
                CefV8Value::CreateDouble(dictionary->GetDouble(k)), attrib);
                break;
            case VTYPE_STRING: obj->SetValue(k,
                CefV8Value::CreateString(dictionary->GetString(k)), attrib);
                break;
            case VTYPE_DICTIONARY: obj->SetValue(k,
                to_v8object(dictionary->GetDictionary(k), reclevel + 1), attrib);
                break;
            case VTYPE_LIST: obj->SetValue(k,
                to_v8array(dictionary->GetList(k), reclevel + 1), attrib);
                break;

            default: break;
            }
        }
    }
    return obj;
}

CefRefPtr<CefV8Value> CefV8Utils::to_v8array(CefRefPtr<CefListValue> const& list, int reclevel /*= 0 */)
{
    if (reclevel >= CEFV8_UTILS_MAX_RECLEVEL)
    {
        auto const errorArr = CefV8Value::CreateArray(1);
        errorArr[0] = *CefV8Value::CreateString("max recursion level reached");
        return errorArr;
    }
    if(list)
    {
        auto const arr = CefV8Value::CreateArray(static_cast<int>(list->GetSize()));

        for(int i=0; i<arr->GetArrayLength(); i++)
        {
            auto const type = list->GetType(i);

            switch (type)
            {
            case VTYPE_BOOL: arr[i] =
                *CefV8Value::CreateBool(list->GetBool(i));
                break;
            case VTYPE_INT: arr[i] =
                *CefV8Value::CreateInt(list->GetInt(i));
                break;
            case VTYPE_DOUBLE: arr[i] =
                *CefV8Value::CreateDouble(list->GetDouble(i));
                break;
            case VTYPE_STRING: arr[i] =
                *CefV8Value::CreateString(list->GetString(i));
                break;
            case VTYPE_DICTIONARY: arr[i] =
                *to_v8object(list->GetDictionary(i), reclevel + 1);
                break;
            case VTYPE_LIST: arr[i] =
                *to_v8array(list->GetList(i), reclevel + 1);
            default:
                break;
            }
        }

        return arr;
    }
    return CefV8Value::CreateNull();
}

CefRefPtr<CefDictionaryValue> CefV8Utils::to_dictionary(CefRefPtr<CefV8Value> const& obj, int reclevel /*= 0 */)
{
    auto const dict = CefDictionaryValue::Create();
    if (!obj) return dict;

    if (reclevel >= CEFV8_UTILS_MAX_RECLEVEL) dict->SetString("error", "max recursion level reached");
    else if (obj->IsObject())
    {
        std::vector<CefString> keys;
        obj->GetKeys(keys);
        for (const CefString& k : keys)
        {
            auto v = obj->GetValue(k);

            if (v->IsBool()) dict->SetBool(k, v->GetBoolValue());
            if (v->IsDouble()) dict->SetDouble(k, v->GetDoubleValue());
            if (v->IsInt()) dict->SetInt(k, v->GetIntValue());
            if (v->IsString()) dict->SetString(k, v->GetStringValue());
            if (v->IsObject()) dict->SetDictionary(k, to_dictionary(v, reclevel + 1));
            if (v->IsArray()) dict->SetList(k, to_list(v, reclevel + 1));
        }
    }
    return dict;
}

CefRefPtr<CefListValue> CefV8Utils::to_list(CefRefPtr<CefV8Value> const& arr, int reclevel /*= 0 */)
{
    auto const list = CefListValue::Create();
    if (!arr) return list;

    if (reclevel >= CEFV8_UTILS_MAX_RECLEVEL)
    {
        list->SetSize(1);
        list->SetString(0, "max recursion level reached");
    }
    else if(arr->IsArray())
    {
        for(int i=0; i<arr->GetArrayLength(); i++)
        {
            CefRefPtr<CefV8Value> v(&arr[i]);

            if (v->IsBool()) list->SetBool(i, v->GetBoolValue());
            if (v->IsDouble()) list->SetDouble(i, v->GetDoubleValue());
            if (v->IsInt()) list->SetInt(i, v->GetIntValue());
            if (v->IsString()) list->SetString(i, v->GetStringValue());
            if (v->IsObject()) list->SetDictionary(i, to_dictionary(v, reclevel + 1));
            if (v->IsArray()) list->SetList(i, to_list(v, reclevel + 1));
        }
    }
    return list;
}
