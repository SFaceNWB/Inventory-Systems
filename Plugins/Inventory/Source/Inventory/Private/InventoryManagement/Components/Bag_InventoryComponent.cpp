// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/Components/Bag_InventoryComponent.h"

#include "Net/UnrealNetwork.h"
#include "Widgets/Inventory/InventoryBase/Bag_InventoryBase.h"

UBag_InventoryComponent::UBag_InventoryComponent()
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

	if (Result.TotalRoomToFill == 0)
	{
		NoRoomInInventory.Broadcast();
		return;
	}

	if (Result.Item.IsValid() && Result.bStackable)
	{
		// 向背包中添加已经存在的物品添加堆叠数量
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

	// TODO: 让物品组件销毁它的拥有者
}

void UBag_InventoryComponent::Server_AddStacksToItem_Implementation(UBag_ItemComponent* ItemComponent, int32 StackCount,
	int32 Remainder)
{

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

