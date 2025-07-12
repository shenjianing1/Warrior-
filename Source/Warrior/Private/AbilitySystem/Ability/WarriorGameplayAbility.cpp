// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/WarriorGameplayAbility.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Items/Weapons/WarriorHeroWeapon.h"

void UWarriorGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
    // Fix the error by ensuring WarriorHeroWeapon is a subclass type and not an instance.  
    //if (WarriorHeroWeapon)  
    //{  
    //   FActorSpawnParameters SpawnParams;  
    //   SpawnParams.Owner = ActorInfo->OwnerActor.Get();  
    //   SpawnParams.Instigator = Cast<APawn>(ActorInfo->AvatarActor.Get());  
    //   SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;  

    //   // Correctly cast WarriorHeroWeapon to TSubclassOf<AWarriorHeroWeapon>  
    //   TSubclassOf<AWarriorHeroWeapon> WeaponClass = WarriorHeroWeapon->GetClass();  
    //   GetWorld()->SpawnActor<AWarriorHeroWeapon>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);  
    //}
	// �ڸ�������ʱ������
	if (AbilityActivePolicy == EWarriorAbilityActivePolicy::OnGiven)
	{
		// �����������ڸ�������ʱ���߼�
		if (ActorInfo && !Spec.IsActive())
		{
			// �����OnGiven���ԣ����Լ�������
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
		}
	}
}

void UWarriorGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	// �ڽ�������ʱ������
	if (AbilityActivePolicy == EWarriorAbilityActivePolicy::OnGiven)
	{
		if (ActorInfo)
		{
			// �����OnGiven���ԣ���������ʱ�����
			ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
		}
	}
}
