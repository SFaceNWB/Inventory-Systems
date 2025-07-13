// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Manifest/Bag_ItemManifest.h"
#include "Bag_InventoryItem.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORY_API UBag_InventoryItem : public UObject
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void SetItemManifest(const FBag_ItemManifest& Manifest);
private:

	UPROPERTY(VisibleAnywhere, meta = (BaseStruct = "/Script/Inventory.Bag_ItemManifest"), Replicated)
	FInstancedStruct ItemManifest;
};
