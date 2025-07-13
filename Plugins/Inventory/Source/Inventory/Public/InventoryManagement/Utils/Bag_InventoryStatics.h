// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Manifest/Bag_ItemManifest.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Bag_InventoryStatics.generated.h"

class UBag_ItemComponent;
class UBag_InventoryComponent;
/**
 * 
 */
UCLASS()
class INVENTORY_API UBag_InventoryStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static UBag_InventoryComponent* GetInventoryComponent(const APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static EBag_ItemCategory GetItemCategoryFromItemComponent(UBag_ItemComponent* ItemComponent);
};
