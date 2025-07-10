// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/CentralizedScrollBox.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include <UMG.h>

UCentralizedScrollBox::UCentralizedScrollBox()
{
	StartScrollOffset = 0.0f;
	TargetScrollOffset = 0.0f;
	ScrollStartTime = 0.0f;
	ScrollDuration = 0.0f;
	CurrentEasingType = EEasingFunc::Linear;
}

float UCentralizedScrollBox::CalculateScrollOffsetToCenter(UWidget* Widget, int32 WidgetIndex)
{
	if (!Widget || !this)
		return 0.0f;

	ForceLayoutPrepass();

	float CurrentScrollOffset = GetScrollOffset();

	UPanelSlot* WidgetSlot = Widget->Slot;
	if (!WidgetSlot)
	{
		return CurrentScrollOffset;
	}

	FGeometry ScrollBoxGeometry = GetCachedGeometry();
	FGeometry WidgetGeometry = Widget->GetCachedGeometry();

	float ScrollBoxSize = GetOrientation() == Orient_Vertical ?
		ScrollBoxGeometry.GetLocalSize().Y :
		ScrollBoxGeometry.GetLocalSize().X;

	float WidgetSize = GetOrientation() == Orient_Vertical ?
		WidgetGeometry.GetLocalSize().Y :
		WidgetGeometry.GetLocalSize().X;

	float WidgetPosition = 0.0f;
	for (int32 i = 0; i < WidgetIndex; i++)
	{
		UWidget* PrevWidget = GetChildAt(i);
		if (PrevWidget)
		{
			UScrollBoxSlot* PrevSlot = Cast<UScrollBoxSlot>(PrevWidget->Slot);

			if (GetOrientation() == Orient_Vertical)
			{
				WidgetPosition += PrevWidget->GetCachedGeometry().GetLocalSize().Y;

				if (PrevSlot)
				{
					FMargin Padding = PrevSlot->GetPadding();
					WidgetPosition += Padding.Top + Padding.Bottom;
				}
			}
			else
			{
				WidgetPosition += PrevWidget->GetCachedGeometry().GetLocalSize().X;

				if (PrevSlot)
				{
					FMargin Padding = PrevSlot->GetPadding();
					WidgetPosition += Padding.Left + Padding.Right;
				}
			}
		}
	}

	UScrollBoxSlot* CurrentSlot = Cast<UScrollBoxSlot>(Widget->Slot);
	if (CurrentSlot)
	{
		FMargin Padding = CurrentSlot->GetPadding();
		if (GetOrientation() == Orient_Vertical)
		{
			WidgetPosition += Padding.Top;
		}
		else
		{
			WidgetPosition += Padding.Left;
		}
	}

	float TargetOffset = WidgetPosition + (WidgetSize / 2.0f) - (ScrollBoxSize / 2.0f);

	// Clamp to valid scroll range
	float MaxScrollOffset = GetScrollOffsetOfEnd();
	return FMath::Clamp(TargetOffset, 0.0f, MaxScrollOffset);
}

void UCentralizedScrollBox::ScrollToElementCentered(int32 ElementIndex)
{
	// Account for spacer by adding 1 to the index
	int32 ActualIndex = ElementIndex + 1;

	if (ActualIndex < 0 || ActualIndex >= GetChildrenCount())
	{
		return;
	}

	UWidget* Widget = GetChildAt(ActualIndex);
	if (!Widget)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
		return;

	ForceLayoutPrepass();

	float CurrentScrollOffset = GetScrollOffset();
	TargetScrollOffset = CalculateScrollOffsetToCenter(Widget, ActualIndex);

	float Distance = FMath::Abs(TargetScrollOffset - CurrentScrollOffset);

	float Duration = FMath::GetMappedRangeValueClamped(
		FVector2D(MinScrollDistance, MaxScrollDistance),
		FVector2D(MinScrollDuration, MaxScrollDuration),
		Distance);

	StartScrollOffset = CurrentScrollOffset;
	this->TargetScrollOffset = TargetScrollOffset;
	ScrollStartTime = World->GetTimeSeconds();
	ScrollDuration = Duration;
	CurrentEasingType = ScrollEasingType;

	if (World->GetTimerManager().IsTimerActive(ScrollTimerHandle))
	{
		World->GetTimerManager().ClearTimer(ScrollTimerHandle);
	}

	World->GetTimerManager().SetTimer(
		ScrollTimerHandle,
		this,
		&UCentralizedScrollBox::TickScrollAnimation,
		0.016f, // ~60fps update
		true);
}

void UCentralizedScrollBox::TickScrollAnimation()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	float CurrentTime = World->GetTimeSeconds() - ScrollStartTime;
	float Alpha = FMath::Clamp(CurrentTime / ScrollDuration, 0.0f, 1.0f);

	float EasedAlpha = UKismetMathLibrary::Ease(0.0f, 1.0f, Alpha, CurrentEasingType);

	float NewScrollOffset = FMath::Lerp(StartScrollOffset, TargetScrollOffset, EasedAlpha);
	SetScrollOffset(NewScrollOffset);

	// Broadcast the OnScrollOffsetChanged event with the current offset value
	OnScrollOffsetChanged.Broadcast(NewScrollOffset);

	if (Alpha >= 1.0f)
	{
		World->GetTimerManager().ClearTimer(ScrollTimerHandle);
	}
}
