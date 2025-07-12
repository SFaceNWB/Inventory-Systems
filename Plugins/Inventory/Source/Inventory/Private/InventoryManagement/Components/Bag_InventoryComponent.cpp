// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/Components/Bag_InventoryComponent.h"

#include "Widgets/Inventory/InventoryBase/Bag_InventoryBase.h"

UBag_InventoryComponent::UBag_InventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBag_InventoryComponent::TryAddItem(UBag_ItemComponent* ItemComponent)
{
	NoRoomInInventory.Broadcast();
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

