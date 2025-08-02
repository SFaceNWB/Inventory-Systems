// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/Bag_GridTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "GameplayTagContainer.h"
#include "Bag_ItemManifest.generated.h"

struct FBag_ItemFragment;
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
	EBag_ItemCategory GetItemCategory() const { return ItemCategory; }
	FGameplayTag GetItemType() const { return ItemType; }

	template<typename T> requires std::derived_from<T, FBag_ItemFragment>
	const T* GetFragmentOfTypeWithTag(const FGameplayTag& FragmentTag) const;

	template<typename T> requires std::derived_from<T, FBag_ItemFragment>
	const T* GetFragmentOfType() const;

private:

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ExcludeBaseStruct))
	TArray<TInstancedStruct<FBag_ItemFragment>> Fragments;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	EBag_ItemCategory ItemCategory{ EBag_ItemCategory::None };

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag ItemType;
};


template<typename T>
requires std::derived_from<T, FBag_ItemFragment>
const T* FBag_ItemManifest::GetFragmentOfTypeWithTag(const FGameplayTag& FragmentTag) const
{
	for (const TInstancedStruct<FBag_ItemFragment>& Fragment : Fragments)
	{
		if (const T* FragmentPtr = Fragment.GetPtr<T>())
		{
			if (!FragmentPtr->GetFragmentTag().MatchesTagExact(FragmentTag))
			{
				continue;
			}
			return FragmentPtr;
		}
	}
	return nullptr;
}

template <typename T>
requires std::derived_from<T, FBag_ItemFragment>
const T* FBag_ItemManifest::GetFragmentOfType() const
{
	for (const TInstancedStruct<FBag_ItemFragment>& Fragment : Fragments)
	{
		if (const T* FragmentPtr = Fragment.GetPtr<T>())
		{
			return FragmentPtr;
		}
	}
	return nullptr;
}








