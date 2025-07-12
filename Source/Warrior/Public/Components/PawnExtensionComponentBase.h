// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PawnExtensionComponentBase.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WARRIOR_API UPawnExtensionComponentBase : public UActorComponent
{
	GENERATED_BODY()

protected:
	template<class T>
	T* GetOwningPawn() const
	{
		// Ensure T is a subclass of APawn
		static_assert(TPointerIsConvertibleFromTo<T, APawn>::Value, "T must be a subclass of APawn");
		// Check if the owner is valid and of type APawn
		return CastChecked<T>(GetOwner());
	}
		
	APawn* GetOwningPawn() const
	{
		// Ensure the owner is a valid APawn
		return CastChecked<APawn>(GetOwner());	
	}

	template<class T>
	T* GetController() const
	{
		// Ensure T is a subclass of AController
		static_assert(TPointerIsConvertibleFromTo<T, AController>::Value, "T must be a subclass of AController");
		// Get the owning pawn and return its controller
		return GetOwningPawn<APawn>()->GetController<T>();
	}
};
