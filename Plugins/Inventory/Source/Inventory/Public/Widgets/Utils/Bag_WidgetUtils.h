// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Bag_WidgetUtils.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORY_API UBag_WidgetUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	static int32 GetIndexFromPosition(const FIntPoint& Position, const int32 Columns);
	static FIntPoint GetPositionFromIndex(const int32 Index, const int32 Columns);
};
