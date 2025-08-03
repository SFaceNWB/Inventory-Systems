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
	virtual bool IsSupportedForNetworking() const override { return true; }

	void SetItemManifest(const FBag_ItemManifest& Manifest);
	const FBag_ItemManifest& GetItemManifest() const { return ItemManifest.Get<FBag_ItemManifest>(); }
	FBag_ItemManifest& GetItemManifestMutable() { return ItemManifest.GetMutable<FBag_ItemManifest>(); }
	bool IsStackable() const;
	int32 GetTotalStackCount() const { return TotalStackCount; }
	void SetTotalStackCount(int32 Count) { TotalStackCount = Count; }

private:

	UPROPERTY(VisibleAnywhere, meta = (BaseStruct = "/Script/Inventory.Bag_ItemManifest"), Replicated)
	FInstancedStruct ItemManifest;

	UPROPERTY(Replicated)
	int32 TotalStackCount{ 0 };
};

template <typename FragmentType>
const FragmentType* GetFragment(const UBag_InventoryItem* Item, const FGameplayTag& Tag)
{
	if (!IsValid(Item))
	{
		return nullptr;
	}
	const FBag_ItemManifest& Manifest = Item->GetItemManifest();
	return Manifest.GetFragmentOfTypeWithTag<FragmentType>(Tag);
}




