// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/Components/Bag_InventoryComponent.h"

#include "Widgets/Inventory/InventoryBase/Bag_InventoryBase.h"

UBag_InventoryComponent::UBag_InventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
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
}

