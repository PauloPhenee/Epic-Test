// Fill out your copyright notice in the Description page of Project Settings.

#include "WidgetFunctionLibrary/ScrollingMenuFunctionLibrary.h"
#include "Components/ScrollBox.h"


void UScrollingMenuFunctionLibrary::ScrollBoxToMiddle(UScrollBox* ScrollBox)
{
	if (!ScrollBox)
		return;

	ScrollBox->ForceLayoutPrepass();

	float maxOffset = ScrollBox->GetScrollOffsetOfEnd();

	if (maxOffset > 0)
	{
		float middleOffset = maxOffset / 2.0f;
		ScrollBox->SetScrollOffset(middleOffset);
	}
	else
	{
		float totalContentSize = 0.0f;
		for (int32 i = 0; i < ScrollBox->GetChildrenCount(); i++)
		{
			UWidget* Child = ScrollBox->GetChildAt(i);
			if (Child)
			{
				FVector2D ChildSize = Child->GetDesiredSize();
				totalContentSize += ScrollBox->GetOrientation() == Orient_Vertical ? ChildSize.Y : ChildSize.X;
			}
		}

		FVector2D ViewportSize = ScrollBox->GetCachedGeometry().GetLocalSize();
		float viewportSizeInScrollDirection = ScrollBox->GetOrientation() == Orient_Vertical ? ViewportSize.Y : ViewportSize.X;

		float middleOffset = FMath::Max(0.0f, (totalContentSize - viewportSizeInScrollDirection) / 2.0f);

		ScrollBox->SetScrollOffset(middleOffset);
	}
}
