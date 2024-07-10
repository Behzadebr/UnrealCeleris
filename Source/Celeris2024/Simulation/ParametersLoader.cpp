#include "ParametersLoader.h"
#include "Misc/FileHelper.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"

bool UParametersLoader::LoadParameters(USimulationParameters* SimulationParametersObject, const FString& JsonFilePath)
{
    if (SimulationParametersObject == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("SimulationParametersObject is null."));
        return false;
    }

    FString JsonString;
    if (!FFileHelper::LoadFileToString(JsonString, *JsonFilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load file: %s"), *JsonFilePath);
        return false;
    }

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        for (const auto& Elem : JsonObject->Values)
        {
            FString PropertyName = Elem.Key;
            FProperty* Property = FindFProperty<FProperty>(SimulationParametersObject->GetClass(), *PropertyName);
            if (Property != nullptr)
            {
                if (FNumericProperty* NumericProperty = CastField<FNumericProperty>(Property))
                {
                    if (NumericProperty->IsFloatingPoint())
                    {
                        double Value = 0;
                        if (Elem.Value->TryGetNumber(Value))
                        {
                            NumericProperty->SetFloatingPointPropertyValue(Property->ContainerPtrToValuePtr<void>(SimulationParametersObject), Value);
                        }
                    }
                    else if (NumericProperty->IsInteger())
                    {
                        int64 Value = 0;
                        if (Elem.Value->Type == EJson::Number)
                        {
                            Value = Elem.Value->AsNumber();
                            NumericProperty->SetIntPropertyValue(Property->ContainerPtrToValuePtr<void>(SimulationParametersObject), Value);
                        }
                        else if (Elem.Value->Type == EJson::String)
                        {
                            FString StringValue = Elem.Value->AsString();
                            Value = FCString::Atoi(*StringValue);
                            NumericProperty->SetIntPropertyValue(Property->ContainerPtrToValuePtr<void>(SimulationParametersObject), Value);
                        }
                    }
                }
                else if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
                {
                    bool Value = Elem.Value->AsBool();
                    BoolProperty->SetPropertyValue(Property->ContainerPtrToValuePtr<void>(SimulationParametersObject), Value);
                }
                else if (FStrProperty* StringProperty = CastField<FStrProperty>(Property))
                {
                    FString Value = Elem.Value->AsString();
                    StringProperty->SetPropertyValue(Property->ContainerPtrToValuePtr<void>(SimulationParametersObject), Value);
                }
            }
        }
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON."));
        return false;
    }
}
