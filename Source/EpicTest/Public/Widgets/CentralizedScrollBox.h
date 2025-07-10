// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ScrollBox.h"
#include "Kismet/KismetMathLibrary.h"
#include "CentralizedScrollBox.generated.h"

// Delegate for when scroll value changes
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScrollOffsetChanged, float, CurrentOffset);


UCLASS()
class EPICTEST_API UCentralizedScrollBox : public UScrollBox
{
	GENERATED_BODY()

public:
	UCentralizedScrollBox();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scroll Animation")
	float MinScrollDuration = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scroll Animation")
	float MinScrollDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scroll Animation")
	float MaxScrollDuration = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scroll Animation")
	float MaxScrollDistance = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scroll Animation")
	TEnumAsByte<EEasingFunc::Type> ScrollEasingType = EEasingFunc::Linear;

	// Event fired when scroll offset changes programmatically
	UPROPERTY(BlueprintAssignable, Category = "User Interface|ScrollBox")
	FOnScrollOffsetChanged OnScrollOffsetChanged;

	// Smoothly scrolls to center a specific element in the scroll box
	UFUNCTION(BlueprintCallable, Category = "User Interface|ScrollBox")
	void ScrollToElementCentered(int32 ElementIndex);

	float CalculateScrollOffsetToCenter(UWidget* Widget, int32 WidgetIndex);

private:
	FTimerHandle ScrollTimerHandle;

	float StartScrollOffset;
	float TargetScrollOffset;
	float ScrollStartTime;
	float ScrollDuration;
	TEnumAsByte<EEasingFunc::Type> CurrentEasingType;

	void TickScrollAnimation();
};
