// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/FastArray/Bag_FastArray.h"

#include "InventoryManagement/Components/Bag_InventoryComponent.h"
#include "Items/Bag_InventoryItem.h"
#include "Items/Components/Bag_ItemComponent.h"

TArray<UBag_InventoryItem*> FBag_InventoryFastArray::GetAllItems() const
{
	TArray<UBag_InventoryItem*> Results;
	Results.Reserve(Entries.Num());
	for (const auto& Entry : Entries)
	{
		if (!IsValid(Entry.Item))
		{
			continue;	
		}
		Results.Add(Entry.Item);
	}
	return Results;
}

void FBag_InventoryFastArray::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	UBag_InventoryComponent* IC = Cast<UBag_InventoryComponent>(OwnerComponent);
	if (!IsValid(IC))
	{
		return;
	}

	for (int32 Index : RemovedIndices)
	{
		IC->OnItemRemoved.Broadcast(Entries[Index].Item);
	}
}

void FBag_InventoryFastArray::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	UBag_InventoryComponent* IC = Cast<UBag_InventoryComponent>(OwnerComponent);
	if (!IsValid(IC))
	{
		return;
	}

	for (int32 Index : AddedIndices)
	{
		IC->OnItemAdded.Broadcast(Entries[Index].Item);
	}
}

UBag_InventoryItem* FBag_InventoryFastArray::AddEntry(UBag_ItemComponent* ItemComponent)
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	UBag_InventoryComponent* IC = Cast<UBag_InventoryComponent>(OwnerComponent);
	if (!IsValid(IC))
	{
		return nullptr;
	}

	FBag_InventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Item = ItemComponent->GetItemManifest().Manifest(OwningActor);

	IC->AddRepSubObj(NewEntry.Item);
	MarkItemDirty(NewEntry);

	return NewEntry.Item;
}

UBag_InventoryItem* FBag_InventoryFastArray::AddEntry(UBag_InventoryItem* Item)
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	FBag_InventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Item = Item;

	MarkItemDirty(NewEntry);
	return Item;
}

void FBag_InventoryFastArray::RemoveEntry(UBag_InventoryItem* Item)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FBag_InventoryEntry& Entry = *EntryIt;
		if (Entry.Item == Item)
		{
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

UBag_InventoryItem* FBag_InventoryFastArray::FindFirstItemByType(const FGameplayTag& ItemType)
{
	auto* FoundItem = Entries.FindByPredicate(
		[ItemType = ItemType](const FBag_InventoryEntry& Entry)
		{
			return IsValid(Entry.Item) && Entry.Item->GetItemManifest().GetItemType().MatchesTagExact(ItemType);
		});
	return FoundItem ? FoundItem->Item : nullptr;
}








