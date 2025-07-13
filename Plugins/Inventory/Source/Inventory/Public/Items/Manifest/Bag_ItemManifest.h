// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/Bag_GridTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "Bag_ItemManifest.generated.h"

enum class EBag_ItemCategory : uint8;
class UBag_InventoryItem;
/**
 * 创建新背包物品必须的数据的物品清单类。
 */
USTRUCT(BlueprintType)
struct INVENTORY_API FBag_ItemManifest
{
	GENERATED_BODY()

	UBag_InventoryItem* Manifest(UObject* NewOuter);
	EBag_ItemCategory GetIemCategory() const { return ItemCategory; }

private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	EBag_ItemCategory ItemCategory{ EBag_ItemCategory::None };
};
