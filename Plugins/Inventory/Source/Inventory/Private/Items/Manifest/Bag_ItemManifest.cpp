// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Manifest/Bag_ItemManifest.h"

#include "Items/Bag_InventoryItem.h"
#include "Items/Components/Bag_ItemComponent.h"

UBag_InventoryItem* FBag_ItemManifest::Manifest(UObject* NewOuter)
{
	UBag_InventoryItem* Item = NewObject<UBag_InventoryItem>(NewOuter, UBag_InventoryItem::StaticClass());
	Item->SetItemManifest(*this);

	return Item;
}

void FBag_ItemManifest::SpawnPickUpActor(const UObject* WorldContextObject, const FVector& SpawnLocation,
	const FRotator& SpawnRotation)
{
	if (!IsValid(PickUpActorClass) || !IsValid(WorldContextObject))
	{
		return;
	}

	AActor* SpawnedActor = WorldContextObject->GetWorld()->SpawnActor<AActor>(PickUpActorClass, SpawnLocation, SpawnRotation);
	if (!IsValid(SpawnedActor))
	{
		return;
	}

	// 设置项清单，项类型，项类别
	UBag_ItemComponent* ItemComponent = SpawnedActor->FindComponentByClass<UBag_ItemComponent>();
	check(ItemComponent);

	ItemComponent->InitItemManifest(*this);
}
