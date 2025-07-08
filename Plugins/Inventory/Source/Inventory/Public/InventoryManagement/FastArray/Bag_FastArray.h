// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "Bag_FastArray.generated.h"

class UBag_ItemComponent;
class UBag_InventoryComponent;
class UBag_InventoryItem;

/** 在库存中的单个条目 */
USTRUCT(BlueprintType)
struct FBag_InventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FBag_InventoryEntry(){}
private:
	friend struct FBag_InventoryFastArray;
	friend UBag_InventoryComponent;

	UPROPERTY()
	TObjectPtr<UBag_InventoryItem> Item = nullptr;
};

/** 库存项列表 */
USTRUCT(BlueprintType)
struct FBag_InventoryFastArray : public FFastArraySerializer
{
	GENERATED_BODY()

	FBag_InventoryFastArray() : OwnerComponent(nullptr) {}
	FBag_InventoryFastArray(UActorComponent* InOwnerComponent) : OwnerComponent(InOwnerComponent) {}

	TArray<UBag_InventoryItem*> GetAllItems() const;

	// FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	// End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FBag_InventoryEntry, FBag_InventoryFastArray>(Entries, DeltaParams, *this);
	}

	UBag_InventoryItem* AddEntry(UBag_ItemComponent* ItemComponent);
	UBag_InventoryItem* AddEntry(UBag_InventoryItem* Item);
	void RemoveEntry(UBag_InventoryItem* Item);

private:
	friend UBag_InventoryComponent;

	//被复制的列表项
	UPROPERTY()
	TArray<FBag_InventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FBag_InventoryFastArray> : public TStructOpsTypeTraitsBase2<FBag_InventoryFastArray>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};








