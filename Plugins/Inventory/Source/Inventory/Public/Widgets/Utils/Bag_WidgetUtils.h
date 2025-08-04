// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Bag_WidgetUtils.generated.h"

class UWidget;
/**
 * 
 */
UCLASS()
class INVENTORY_API UBag_WidgetUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static FVector2D GetWidgetPosition(UWidget* Widget);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static FVector2D GetWidgetSize(UWidget* Widget);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static bool IsWithBounds(const FVector2D& BoundaryPosition, const FVector2D& WidgetSize, const FVector2D& MousePosition);

	static int32 GetIndexFromPosition(const FIntPoint& Position, const int32 Columns);
	static FIntPoint GetPositionFromIndex(const int32 Index, const int32 Columns);
};
