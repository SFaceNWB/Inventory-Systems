// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/Components/Bag_InventoryComponent.h"

#include "Items/Components/Bag_ItemComponent.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/Inventory/InventoryBase/Bag_InventoryBase.h"
#include "Items/Bag_InventoryItem.h"
#include "Items/Fragments/Bag_ItemFragment.h"

UBag_InventoryComponent::UBag_InventoryComponent()
	:InventoryList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
	bInventoryMenuOpen = false;
}

void UBag_InventoryComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
}


void UBag_InventoryComponent::TryAddItem(UBag_ItemComponent* ItemComponent)
{
	FBag_SlotAvailabilityResult Result = InventoryMenu->HasRoomForItem(ItemComponent);

	UBag_InventoryItem* FoundItem = InventoryList.FindFirstItemByType(ItemComponent->GetItemManifest().GetItemType());
	Result.Item = FoundItem;

	if (Result.TotalRoomToFill == 0)
	{
		NoRoomInInventory.Broadcast();
		return;
	}

	if (Result.Item.IsValid() && Result.bStackable)
	{
		// 向背包中添加已经存在的物品添加堆叠数量
		OnStackChange.Broadcast(Result);
		Server_AddStacksToItem(ItemComponent, Result.TotalRoomToFill, Result.Remainder);
	}
	else if (Result.TotalRoomToFill > 0)
	{
		// 背包中不存在这个物品， 创建并更新背包中的相关槽位
		Server_AddNewItem(ItemComponent, Result.bStackable ? Result.TotalRoomToFill : 0);
	}
}

void UBag_InventoryComponent::Server_AddNewItem_Implementation(UBag_ItemComponent* ItemComponent, int32 StackCount)
{
	UBag_InventoryItem* NewItem = InventoryList.AddEntry(ItemComponent);
	NewItem->SetTotalStackCount(StackCount);

	if (GetOwner()->GetNetMode() == NM_ListenServer || GetOwner()->GetNetMode() == NM_Standalone)
	{
		OnItemAdded.Broadcast(NewItem);
	}

	ItemComponent->PickedUp();
}

void UBag_InventoryComponent::Server_AddStacksToItem_Implementation(UBag_ItemComponent* ItemComponent, int32 StackCount,
	int32 Remainder)
{
	const FGameplayTag& ItemType = IsValid(ItemComponent) ? ItemComponent->GetItemManifest().GetItemType() : FGameplayTag::EmptyTag;
	UBag_InventoryItem* Item = InventoryList.FindFirstItemByType(ItemType);
	if (!IsValid(Item))
	{
		return;
	}

	Item->SetTotalStackCount(Item->GetTotalStackCount() + StackCount);

	if (Remainder == 0)
	{
		ItemComponent->PickedUp();
	}
	else if (FBag_StackableFragment* StackableFragment = ItemComponent->GetItemManifest().GetFragmentOfTypeMutable<FBag_StackableFragment>())
	{
		StackableFragment->SetStackCount(Remainder);
	}
}

void UBag_InventoryComponent::Server_DropItem_Implementation(UBag_InventoryItem* Item, int32 StackCount)
{
	const int32 NewStackCount = Item->GetTotalStackCount() - StackCount;
	if (NewStackCount <= 0)
	{
		InventoryList.RemoveEntry(Item);
	}
	else
	{
		Item->SetTotalStackCount(NewStackCount);
	}
	SpawnDroppedItem(Item, StackCount);
}

void UBag_InventoryComponent::Server_ConsumeItem_Implementation(UBag_InventoryItem* Item)
{
	const int32 NewStackCount = Item->GetTotalStackCount() - 1;
	if (NewStackCount <= 0)
	{
		InventoryList.RemoveEntry(Item);
	}
	else
	{
		Item->SetTotalStackCount(NewStackCount);
	}
	if (FBag_ConsumableFragment* ConsumableFragment = Item->GetItemManifestMutable().GetFragmentOfTypeMutable<FBag_ConsumableFragment>())
	{
		ConsumableFragment->OnConsume(OwningController.Get());
	}
}

void UBag_InventoryComponent::ToggleInventoryMenu()
{
	if (bInventoryMenuOpen)
	{
		CloseInventoryMenu();
	}
	else
	{
		OpenInventoryMenu();
	}
}

void UBag_InventoryComponent::AddRepSubObj(UObject* SubObj)
{
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && IsValid(SubObj))
	{
		AddReplicatedSubObject(SubObj);
	}
}

void UBag_InventoryComponent::SpawnDroppedItem(UBag_InventoryItem* Item, int32 StackCount)
{
	const APawn* OwningPawn = OwningController->GetPawn();
	FVector RotatedForward = OwningPawn->GetActorForwardVector();
	RotatedForward = RotatedForward.RotateAngleAxis(FMath::FRandRange(DropSpawnAngleMin, DropSpawnAngleMax), FVector::UpVector);
	FVector SpawnLocation = OwningPawn->GetActorLocation() + RotatedForward * FMath::FRandRange(DropSpawnDistanceMin, DropSpawnDistanceMax);
	SpawnLocation.Z -= RelativeSpawnElevation;
	const FRotator SpawnrRotation = FRotator::ZeroRotator;

	FBag_ItemManifest& ItemManifest = Item->GetItemManifestMutable();
	if (FBag_StackableFragment* StackableFragment = ItemManifest.GetFragmentOfTypeMutable<FBag_StackableFragment>())
	{
		StackableFragment->SetStackCount(StackCount);
	}
	ItemManifest.SpawnPickUpActor(this, SpawnLocation, SpawnrRotation);
}

void UBag_InventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	ConstructInventory();
}

void UBag_InventoryComponent::ConstructInventory()
{
	OwningController = Cast<APlayerController>(GetOwner());
	checkf(OwningController.IsValid(), TEXT("Inventory Component should have a Player Controller as Owner."));
	if (!OwningController->IsLocalController())
	{
		return;
	}

	InventoryMenu = CreateWidget<UBag_InventoryBase>(OwningController.Get(), InventoryMenuClass);
	InventoryMenu->AddToViewport();
	CloseInventoryMenu();
}

void UBag_InventoryComponent::OpenInventoryMenu()
{
	if (!IsValid(InventoryMenu))
	{
		return;
	}

	InventoryMenu->SetVisibility(ESlateVisibility::Visible);
	bInventoryMenuOpen = true;

	if (!OwningController.IsValid())
	{
		return;
	}

	FInputModeGameAndUI InputMode;
	OwningController->SetInputMode(InputMode);
	OwningController->SetShowMouseCursor(true);
}

void UBag_InventoryComponent::CloseInventoryMenu()
{
	if (!IsValid(InventoryMenu))
	{
		return;
	}

	InventoryMenu->SetVisibility(ESlateVisibility::Collapsed);
	bInventoryMenuOpen = false;

	if (!OwningController.IsValid())
	{
		return;
	}

	FInputModeGameOnly InputMode;
	OwningController->SetInputMode(InputMode);
	OwningController->SetShowMouseCursor(false);
}

