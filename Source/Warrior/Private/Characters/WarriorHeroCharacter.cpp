// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/WarriorHeroCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DataAssets/Input/DataAsset_InputConfig.h"
#include "Components/Input/WarriorInputComponent.h"
#include "WarriorGameplayTags.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "DataAssets/StartUpData/DataAsset_HeroStartUpData.h"
#include "Components/Combat/HeroCombatComponent.h"

#include "WarriorDebugHelper.h"

AWarriorHeroCharacter::AWarriorHeroCharacter()
{
	GetMesh()->AddLocalRotation(FRotator(0.f, -90.f, 0.f)); // Rotate the mesh to face forward correctly
	GetMesh()->AddLocalOffset(FVector(0.f, 0.f, -96.f)); // Adjust the mesh position to align with the capsule component

	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.f); // Initialize capsule size
	bUseControllerRotationPitch = false; // Disable controller rotation pitch
	bUseControllerRotationRoll = false; // Disable controller rotation roll
	bUseControllerRotationYaw = false; // Disable controller rotation yaw

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom")); // Create the camera boom component
	CameraBoom->SetupAttachment(GetRootComponent()); // Attach the camera boom to the root component
	CameraBoom->TargetArmLength = 200.f; // Distance from the character to the camera
	CameraBoom->SocketOffset = FVector(0.f, 55.f, 65.f); // Offset the camera position
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller's rotation
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera")); // Create the camera component
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the camera boom
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate with the arm
	// Set the character's movement properties
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character will rotate to movement direction
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // Rotation rate for the character
	GetCharacterMovement()->MaxWalkSpeed = 400.f; // Maximum walking speed
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f; // Deceleration when braking while walking
	
	// Create and initialize the combat component
	HeroCombatComponent = CreateDefaultSubobject<UHeroCombatComponent>(TEXT("HeroCombatComponent"));
}

void AWarriorHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	/*Debug::Print(TEXT("Warrior Hero Character Possessed By Controller"));
	const FString s = FString::Printf(TEXT("Avatar Actor: %s"), *WarriorAbilitySystemComponent->GetAvatarActor()->GetActorLabel());
	const FString s1 = FString::Printf(TEXT("Owner Actor: %s"), *WarriorAbilitySystemComponent->GetOwnerActor()->GetActorLabel());
	Debug::Print(s);
	Debug::Print(s1);*/

	if (!CharacterStartUpData.IsNull())//����������Ƿ�Ϊ�գ�isvalid�Ǽ���������Ƿ����
	{
		// ���ɻ�
		if (UDataAsset_StartUpDataBase* LoadedData = (CharacterStartUpData.LoadSynchronous()))
		{
			LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent);
		}
	}
}


void AWarriorHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// ȷ�� InputConfigDataAsset �Ѿ�����
	checkf(InputConfigDataAsset, TEXT("no InputConfigDataAsset"));
	// ȷ�� PlayerInputComponent �� UEnhancedInputComponent ����
	ULocalPlayer* LocalPlayer = GetController<APlayerController>()->GetLocalPlayer();
	// ��ʽ1��ͨ����̬������ȡ��ǿ������ϵͳ������ʵ�ʱ���һ�и��ǣ���ɾ����
	//UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	// ��ʽ2��ͨ���������ʵ����ȡ��ǿ������ϵͳ���Ƽ���ʽ��
	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	// �����ǿ������ϵͳ�Ƿ����
	check(Subsystem);
	// ���Ĭ�ϵ�����ӳ��������
	Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);
	// ȷ����������� UWarriorInputComponent ����
	UWarriorInputComponent* WarriorInputComponent = CastChecked< UWarriorInputComponent>(PlayerInputComponent);
	// �����붯��
	WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset, WarriorGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
	WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset, WarriorGameplayTags::InputTag_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);
}

void AWarriorHeroCharacter::BeginPlay()
{
	Super::BeginPlay();
	Debug::Print(TEXT("Warrior Hero Character Begin Play"));
}


void AWarriorHeroCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	// ��ȡ��������
	/*��������� InputActionValue ����ȡ��ά���� MovementVector��

		X ���������������ƶ�����A / D�����ֱ�����ҡ�ˣ���

		Y ����������ǰ���ƶ�����W / S�����ֱ�����ҡ�ˣ���*/
	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	// �����ƶ�����
	/*ͨ����������ȡ��ɫ��ǰ���ӽǷ���

		ͨ�� MovementRotation �޳�������Pitch���͹�ת��Roll����������ˮƽ��ת��Yaw����ȷ���ƶ�ʼ����ˮƽ���ϡ�*/
	FRotator Rotator = Controller->GetControlRotation(); // ��ȡ����������ת�Ƕ�
	const FRotator MovementRotation = FRotator(0.f, Rotator.Yaw, 0.f); // ֻʹ�� Yaw �Ƕ�

	const FVector ForwardDirection = FRotationMatrix(MovementRotation).GetUnitAxis(EAxis::X); // ��ȡǰ������
	const FVector RightDirection = FRotationMatrix(MovementRotation).GetUnitAxis(EAxis::Y); // ��ȡ�Ҳ෽��

	const FVector MovementDirection = (ForwardDirection * MovementVector.Y + RightDirection * MovementVector.X).GetSafeNormal(); // �����ƶ�����
	if (!MovementDirection.IsNearlyZero())
	{
		// �ؽ�ɫǰ�������ƶ���ǿ���� MovementVector.Y ���ƣ���ֵǰ������ֵ���ˣ���
		AddMovementInput(ForwardDirection, MovementVector.Y); // ����ƶ�����
		// �ؽ�ɫ�ҷ������ƶ���ǿ���� MovementVector.X ���ƣ���ֵ���ƣ���ֵ���ƣ���
		AddMovementInput(RightDirection, MovementVector.X); // ����ƶ�����
	}
}

void AWarriorHeroCharacter::Input_Look(const FInputActionValue& InputActionValue)
{
	// ��ȡ��������
	const FVector2D LookVector = InputActionValue.Get<FVector2D>();
	
	AddControllerYawInput(LookVector.X);
	AddControllerPitchInput(LookVector.Y);
}

