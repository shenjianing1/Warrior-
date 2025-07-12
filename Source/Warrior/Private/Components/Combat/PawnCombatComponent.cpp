// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/PawnCombatComponent.h"

void UPawnCombatComponent::RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister, AWarriorWeaponBase* InWeaponToRegister, bool bRegisterAsEquippedWeapon)
{
}

AWarriorWeaponBase* UPawnCombatComponent::GetCharacterWeaponByTag(FGameplayTag InWeaponTag) const
{
	return nullptr;
}

AWarriorWeaponBase* UPawnCombatComponent::GetCharacterCurrentEquippedWeapon() const
{
	return nullptr;
}
