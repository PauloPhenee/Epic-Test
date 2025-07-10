// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UMG.h"
#include "Components/ScrollBox.h"
#include "ScrollingMenuFunctionLibrary.generated.h"

/**
 *
 */
UCLASS()
class EPICTEST_API UScrollingMenuFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "User Interface|ScrollBox")
	static void ScrollBoxToMiddle(UScrollBox* ScrollBox);
};
