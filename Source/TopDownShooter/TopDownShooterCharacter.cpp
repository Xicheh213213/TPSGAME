// Copyright Epic Games, Inc. All Rights Reserved.

#include "TopDownShooterCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"

ATopDownShooterCharacter::ATopDownShooterCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	 //Create a decal in the world to show the cursor's location
	//CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	//CursorToWorld->SetupAttachment(RootComponent);
	//static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	//if (DecalMaterialAsset.Succeeded())
	//{
	//	CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	//}
	//CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	//CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ATopDownShooterCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	//if (CurrentCursor != nullptr)
	//{
	//	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	//	{
	//		if (UWorld* World = GetWorld())
	//				{
	//			FHitResult HitResult;
	//			FCollisionQueryParams Params(NAME_None, FCollisionQueryParams::GetUnknownStatId());
	//			FVector StartLocation = TopDownCameraComponent->GetComponentLocation();
	//		FVector EndLocation = TopDownCameraComponent->GetComponentRotation().Vector() * 2000.0f;
	//					Params.AddIgnoredActor(this);
	//		World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params);
	//		FQuat SurfaceRotation = HitResult.ImpactNormal.ToOrientationRotator().Quaternion();
	//		CurrentCursor->SetWorldLocationAndRotation(HitResult.Location, SurfaceRotation);
	//		}
	//	}
	//	else if (APlayerController* PC = Cast<APlayerController>(GetController()))
	//	{
	//		FHitResult TraceHitResult;
	//				PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
	//		FVector CursorFV = TraceHitResult.ImpactNormal;
	//		FRotator CursorR = CursorFV.Rotation();
	//		CurrentCursor->SetWorldLocation(TraceHitResult.Location);
	//		CurrentCursor->SetWorldRotation(CursorR);
	//	}
	//}
	if(CurrentCursor)
	{
		APlayerController* myPC = Cast<APlayerController>(GetController());
		if(myPC)
		{
			FHitResult TraceHitResult;
			myPC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			CurrentCursor->SetWorldLocation(TraceHitResult.Location);
			CurrentCursor->SetWorldRotation(CursorR);

		}
	}
	MovementTick(DeltaSeconds);
}
void ATopDownShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitWeapon();

	if (CursorMaterial)
	{
		CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
	}
}

void ATopDownShooterCharacter::SetupPlayerInputComponent(UInputComponent* NewInputComponent)
{
	Super::SetupPlayerInputComponent(NewInputComponent);
	NewInputComponent->BindAxis(TEXT("MoveForward"), this, &ATopDownShooterCharacter::InputAxisX);
	NewInputComponent->BindAxis(TEXT("MoveRight"), this, &ATopDownShooterCharacter::InputAxisY);
	NewInputComponent->BindAction(TEXT("FireEvent"), EInputEvent::IE_Pressed, this, &ATopDownShooterCharacter::InputAttackPressed);
	NewInputComponent->BindAction(TEXT("FireEvent"), EInputEvent::IE_Released, this, &ATopDownShooterCharacter::InputAttackReleased);
}

void ATopDownShooterCharacter::InputAxisY(float Value)
{ 
	AxisY = Value;
}

void ATopDownShooterCharacter::InputAxisX(float Value)
{
	AxisX = Value;
}
void ATopDownShooterCharacter::InputAttackPressed()
{
	AttackCharEvent(true);
}

void ATopDownShooterCharacter::InputAttackReleased()
{
	AttackCharEvent(false);
}

void ATopDownShooterCharacter::MovementTick(float Deltatime)
{
	//функция перемещения.
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), AxisX);
	AddMovementInput(FVector(0.0f, 1.0f, 0.0f), AxisY);
   //реализация функции слежение за курсором.
	if (MovementIndex<4)
	{
		APlayerController* myController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (myController)
		{
			FHitResult ResultHit;
			myController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery6, false, ResultHit);// bug was here Config\DefaultEngine.Ini
			float FindRotaterResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
			SetActorRotation(FQuat(FRotator(0.0f, FindRotaterResultYaw, 0.0f)));
		}
	}
}
void ATopDownShooterCharacter::AttackCharEvent(bool bIsFiring)
{
	AWeaponDefault* myWeapon = nullptr;
	myWeapon = GetCurrentWeapon();
	if (myWeapon)
	{
		//ToDo Check melee or range
		myWeapon->SetWeaponStateFire(bIsFiring);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("ATPSCharacter::AttackCharEvent - CurrentWeapon -NULL"));
}


void ATopDownShooterCharacter::CharacterUpdate()
{
	float ResSpeed = 600.f;
	switch (MovementState)
	{
	case EMovementState::Aim_State:
		ResSpeed = MovementInfo.AimSpeed;
		MovementIndex = 1;
		break;
	case EMovementState::Walk_State:
		ResSpeed = MovementInfo.WalkSpeed;
		MovementIndex = 2;
		break;
	case EMovementState::Run_State:
		ResSpeed = MovementInfo.RunState;
		MovementIndex = 3;
		break;
	case EMovementState::Shift_State:
		ResSpeed = MovementInfo.ShiftState;
		MovementIndex = 4;
		break;
	default:
		break;
	}
	GetCharacterMovement()->MaxWalkSpeed = ResSpeed;
}

void ATopDownShooterCharacter::ChangeMovementState(EMovementState NewMovementState)
{
	MovementState = NewMovementState;
	CharacterUpdate();
	//Weapon state update
	AWeaponDefault* myWeapon = GetCurrentWeapon();
	if (myWeapon)
	{
		myWeapon->UpdateStateWeapon(MovementState);
	}
}


AWeaponDefault* ATopDownShooterCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}

void ATopDownShooterCharacter::InitWeapon()
{
	if (InitWeaponClass)
	{
		FVector SpawnLocation = FVector(0);
		FRotator SpawnRotation = FRotator(0);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = GetInstigator();

		AWeaponDefault* myWeapon = Cast<AWeaponDefault>(GetWorld()->SpawnActor(InitWeaponClass, &SpawnLocation, &SpawnRotation, SpawnParams));
		if (myWeapon)
		{
			FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
			myWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketRightHand"));
			CurrentWeapon = myWeapon;

			myWeapon->UpdateStateWeapon(MovementState);
		}
	}
}

UDecalComponent* ATopDownShooterCharacter::GetCursorToWorld()
{
	return CurrentCursor;
}


