// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "DataAssets/Input/DataAsset_InputConfig.h"

#include "WarriorInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template<class UserObject, typename CallbackFunc>
	void BindNativeInputAction(const UDataAsset_InputConfig* InInputConfig, const FGameplayTag& GameplayTag,
		ETriggerEvent TriggerEvent, UserObject* ContextObject, CallbackFunc Func);
	
};

template<class UserObject, typename CallbackFunc>
inline void UWarriorInputComponent::BindNativeInputAction(const UDataAsset_InputConfig* InInputConfig, const FGameplayTag& GameplayTag, ETriggerEvent TriggerEvent, UserObject* ContextObject, CallbackFunc Func)
{
	check(InInputConfig);
	if (UInputAction* FoundAction = InInputConfig->FindNativeInputActionsByTags(GameplayTag))
	{
		BindAction(FoundAction, TriggerEvent, ContextObject, Func);
	}
}
