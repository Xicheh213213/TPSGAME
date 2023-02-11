// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Types.generated.h"

UENUM(BlueprintType)
enum class EMovementState :uint8
{
	Aim_State UMETA(DysplayName = "Aim State"),
	Walk_State UMETA(DisplayName="WalkState"),
	Run_State UMETA(DisplayName = "Run State"),
	Shift_State UMETA(DisplayName = "Shift State"),
};

USTRUCT(BlueprintType)
struct FCharacterSpeed
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float AimSpeed = 400.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RunState = 600.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float ShiftState = 800.0f;
};

UCLASS()
class TOPDOWNSHOOTER_API UTypes: public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};

 