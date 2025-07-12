// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WarriorBaseAnimInstance.generated.h"

class AWarriorBaseCharacter;
class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	// the below functions are the native overrides for each phase
	// Native initialization override point
	virtual void NativeInitializeAnimation() override;

	// Native thread safe update override point. Executed on a worker thread just prior to graph update 
	// for linked anim instances, only called when the hosting node(s) are relevant
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY()
	AWarriorBaseCharacter* OwningCharacter;

	UPROPERTY()
	UCharacterMovementComponent* OwningMovementComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float GroundSpeed;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	bool bHasAccleration;
};
