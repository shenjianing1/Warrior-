// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnExtensionComponentBase.h"
#include "GameplayTagContainer.h"

#include "PawnCombatComponent.generated.h"

class AWarriorWeaponBase;

/**
 * 
 */
UCLASS()
class WARRIOR_API UPawnCombatComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	void RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister, AWarriorWeaponBase* InWeaponToRegister, bool bRegisterAsEquippedWeapon = false);

	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	AWarriorWeaponBase* GetCharacterWeaponByTag(FGameplayTag InWeaponTag) const;

	UPROPERTY(BlueprintReadWrite, Category = "Warrior|Combat")
	FGameplayTagContainer CurrentEqippedWeaponTag;

	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCurrentEquippedWeapon() const;

private:
	// 在 Unreal Engine 中，Actor 类不能直接存储在容器中，必须使用指针或智能指针。
	TMap<FGameplayTag, AWarriorWeaponBase*> CharacterCarriedWeaponMap;
	
};
