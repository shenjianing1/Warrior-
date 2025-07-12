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

	if (!CharacterStartUpData.IsNull())//检查软引用是否为空，isvalid是检查软引用是否加载
	{
		// 有疑惑
		if (UDataAsset_StartUpDataBase* LoadedData = (CharacterStartUpData.LoadSynchronous()))
		{
			LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent);
		}
	}
}


void AWarriorHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// 确保 InputConfigDataAsset 已经设置
	checkf(InputConfigDataAsset, TEXT("no InputConfigDataAsset"));
	// 确保 PlayerInputComponent 是 UEnhancedInputComponent 类型
	ULocalPlayer* LocalPlayer = GetController<APlayerController>()->GetLocalPlayer();
	// 方式1：通过静态方法获取增强输入子系统（这行实际被下一行覆盖，可删除）
	//UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	// 方式2：通过本地玩家实例获取增强输入子系统（推荐方式）
	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	// 检查增强输入子系统是否存在
	check(Subsystem);
	// 添加默认的输入映射上下文
	Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);
	// 确保输入组件是 UWarriorInputComponent 类型
	UWarriorInputComponent* WarriorInputComponent = CastChecked< UWarriorInputComponent>(PlayerInputComponent);
	// 绑定输入动作
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
	// 获取输入向量
	/*从输入参数 InputActionValue 中提取二维向量 MovementVector。

		X 分量：控制左右移动（如A / D键或手柄左右摇杆）。

		Y 分量：控制前后移动（如W / S键或手柄上下摇杆）。*/
	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	// 计算移动方向
	/*通过控制器获取角色当前的视角方向。

		通过 MovementRotation 剔除俯仰（Pitch）和滚转（Roll），仅保留水平旋转（Yaw），确保移动始终在水平面上。*/
	FRotator Rotator = Controller->GetControlRotation(); // 获取控制器的旋转角度
	const FRotator MovementRotation = FRotator(0.f, Rotator.Yaw, 0.f); // 只使用 Yaw 角度

	const FVector ForwardDirection = FRotationMatrix(MovementRotation).GetUnitAxis(EAxis::X); // 获取前进方向
	const FVector RightDirection = FRotationMatrix(MovementRotation).GetUnitAxis(EAxis::Y); // 获取右侧方向

	const FVector MovementDirection = (ForwardDirection * MovementVector.Y + RightDirection * MovementVector.X).GetSafeNormal(); // 计算移动方向
	if (!MovementDirection.IsNearlyZero())
	{
		// 沿角色前方方向移动，强度由 MovementVector.Y 控制（正值前进，负值后退）。
		AddMovementInput(ForwardDirection, MovementVector.Y); // 添加移动输入
		// 沿角色右方方向移动，强度由 MovementVector.X 控制（正值右移，负值左移）。
		AddMovementInput(RightDirection, MovementVector.X); // 添加移动输入
	}
}

void AWarriorHeroCharacter::Input_Look(const FInputActionValue& InputActionValue)
{
	// 获取输入向量
	const FVector2D LookVector = InputActionValue.Get<FVector2D>();
	
	AddControllerYawInput(LookVector.X);
	AddControllerPitchInput(LookVector.Y);
}

