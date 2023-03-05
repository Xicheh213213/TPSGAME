// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Types.h"
#include "WeaponDefault.h"
#include "TopDownShooterCharacter.generated.h"

UCLASS(Blueprintable)
class ATopDownShooterCharacter : public ACharacter
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
public:
	ATopDownShooterCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;
	
	// позволяет переопределить любые кнопки.
	virtual void SetupPlayerInputComponent(class UInputComponent* InputConponent) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	//FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//class UDecalComponent* CursorToWorld;


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		EMovementState MovementState = EMovementState::Run_State;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		FCharacterSpeed MovementInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
		UMaterialInterface* CursorMaterial = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
		FVector CursorSize = FVector(20.f, 40.f, 40.f);
	AWeaponDefault* CurrentWeapon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo")
		TSubclassOf<AWeaponDefault> InitWeaponClass = nullptr;
	UDecalComponent* CurrentCursor = nullptr;
	UFUNCTION()
		void InputAxisY(float Value);
	UFUNCTION()
		void InputAxisX(float Value);
	float AxisX = 0.0f;
	float AxisY = 0.0f;
//TickFunction
	UFUNCTION()
	void MovementTick(float Deltatime);
	UFUNCTION(BlueprintCallable)
		void CharacterUpdate();
	UFUNCTION(BlueprintCallable)
		void ChangeMovementState(EMovementState NewMovementState);
	UPROPERTY()
		int MovementIndex = 0;

	UFUNCTION(BlueprintCallable)
		AWeaponDefault* GetCurrentWeapon();
	UFUNCTION(BlueprintCallable)
		void InitWeapon();
	UFUNCTION(BlueprintCallable)
		UDecalComponent* GetCursorToWorld();

};

