// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Utils/Bag_WidgetUtils.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/Widget.h"

FVector2D UBag_WidgetUtils::GetWidgetPosition(UWidget* Widget)
{
	const FGeometry Geometry = Widget->GetCachedGeometry();

	FVector2D PixelPosition;
	FVector2D ViewportPosition;
	USlateBlueprintLibrary::LocalToViewport(Widget ,Geometry, USlateBlueprintLibrary::GetLocalTopLeft(Geometry), PixelPosition, ViewportPosition);
	return ViewportPosition;
}

FVector2D UBag_WidgetUtils::GetWidgetSize(UWidget* Widget)
{
	const FGeometry Geometry = Widget->GetCachedGeometry();
	return Geometry.GetLocalSize();
}

bool UBag_WidgetUtils::IsWithBounds(const FVector2D& BoundaryPosition, const FVector2D& WidgetSize,
                                    const FVector2D& MousePosition)
{
	return MousePosition.X >= BoundaryPosition.X && MousePosition.X <= BoundaryPosition.X + WidgetSize.X &&
		MousePosition.Y >= BoundaryPosition.Y && MousePosition.Y <= BoundaryPosition.Y + WidgetSize.Y;
}

FVector2D UBag_WidgetUtils::GetClampedWidgetPosition(const FVector2D& Boundary, const FVector2D& WidgetSize,
	const FVector2D& MousePos)
{
	FVector2D ClampedPosition = MousePos;

	// 调整水平位置确保小部件在边界内
	if (MousePos.X + WidgetSize.X > Boundary.X)//如果超出右边界
	{
		ClampedPosition.X = Boundary.X - WidgetSize.X;
	}
	if (MousePos.X < 0.0f)
	{
		ClampedPosition.X = 0.0f;
	}

	// 调整垂直位置确保小部件在边界内
	if (MousePos.Y + WidgetSize.Y > Boundary.Y)//如果超出下边界
	{
		ClampedPosition.Y = Boundary.Y - WidgetSize.Y;
	}
	if (MousePos.Y < 0.0f)
	{
		ClampedPosition.Y = 0.0f;
	}
	return ClampedPosition;
}

int32 UBag_WidgetUtils::GetIndexFromPosition(const FIntPoint& Position, const int32 Columns)
{
	return  Position.X + Position.Y * Columns;
}

FIntPoint UBag_WidgetUtils::GetPositionFromIndex(const int32 Index, const int32 Columns)
{
	return FIntPoint(Index % Columns, Index / Columns);
}
