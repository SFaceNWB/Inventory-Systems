// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentManagement/Components/Bag_EquipmentComponent.h"

#include "EquipmentManagement/EquipActor/Bag_EquipActor.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "InventoryManagement/Components/Bag_InventoryComponent.h"
#include "InventoryManagement/Utils/Bag_InventoryStatics.h"
#include "Items/Bag_InventoryItem.h"
#include "Items/Fragments/Bag_ItemFragment.h"

void UBag_EquipmentComponent::SetOwningSkeletalMesh(USkeletalMeshComponent* OwningMesh)
{
	OwningSkeletalMesh = OwningMesh;
}

void UBag_EquipmentComponent::InitializeOwner(APlayerController* PlayerController)
{
	if (IsValid(PlayerController))
	{
		OwningPlayerController = PlayerController;
	}
	InitInventoryComponent();
}

void UBag_EquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	InitPlayerController();
}

void UBag_EquipmentComponent::OnItemEquipped(UBag_InventoryItem* EquippedItem)
{
	if (!IsValid(EquippedItem))
	{
		return;
	}
	if (!OwningPlayerController->HasAuthority())
	{
		return;
	}
	FBag_ItemManifest& ItemManifest = EquippedItem->GetItemManifestMutable();
	FBag_EquipmentFragment* EquipmentFragment = ItemManifest.GetFragmentOfTypeMutable<FBag_EquipmentFragment>();
	if (!EquipmentFragment)
	{
		return;
	}
	if (!bIsProxy)
	{
		EquipmentFragment->OnEquip(OwningPlayerController.Get());
	}
	if (!OwningSkeletalMesh.IsValid())
	{
		return;
	}
	ABag_EquipActor* SpawnedEquipActor = SpawnEquipActor(EquipmentFragment, ItemManifest, OwningSkeletalMesh.Get());

	EquippedActors.Add(SpawnedEquipActor);
}

void UBag_EquipmentComponent::OnItemUnequipped(UBag_InventoryItem* UnequippedItem)
{
	if (!IsValid(UnequippedItem))
	{
		return;
	}
	if (!OwningPlayerController->HasAuthority())
	{
		return;
	}
	FBag_ItemManifest& ItemManifest = UnequippedItem->GetItemManifestMutable();
	FBag_EquipmentFragment* EquipmentFragment = ItemManifest.GetFragmentOfTypeMutable<FBag_EquipmentFragment>();
	if (!EquipmentFragment)
	{
		return;
	}
	if (!bIsProxy)
	{
		EquipmentFragment->OnUnequip(OwningPlayerController.Get());
	}
	RemoveEquippedActor(EquipmentFragment->GetEquipmentType());
}

void UBag_EquipmentComponent::InitInventoryComponent()
{
	InventoryComponent = UBag_InventoryStatics::GetInventoryComponent(OwningPlayerController.Get());
	if (!InventoryComponent.IsValid())
	{
		return;
	}

	if (!InventoryComponent->OnItemEquipped.IsAlreadyBound(this, &ThisClass::OnItemEquipped))
	{
		InventoryComponent->OnItemEquipped.AddDynamic(this, &ThisClass::OnItemEquipped);
	}
	if (!InventoryComponent->OnItemUnequipped.IsAlreadyBound(this, &ThisClass::OnItemUnequipped))
	{
		InventoryComponent->OnItemUnequipped.AddDynamic(this, &ThisClass::OnItemUnequipped);
	}
}

ABag_EquipActor* UBag_EquipmentComponent::SpawnEquipActor(FBag_EquipmentFragment* EquipmentFragment,
	const FBag_ItemManifest& Manifest, USkeletalMeshComponent* AttachMesh)
{
	ABag_EquipActor* SpawnedEquipActor = EquipmentFragment->SpawnAttachedActor(AttachMesh);
	SpawnedEquipActor->SetEquipmentType(EquipmentFragment->GetEquipmentType());
	SpawnedEquipActor->SetOwner(GetOwner());
	EquipmentFragment->SetEquippedActor(SpawnedEquipActor);
	return SpawnedEquipActor;
}

ABag_EquipActor* UBag_EquipmentComponent::FindEquippedActor(const FGameplayTag& EquipmentTypeTag)
{
	auto FoundActor = EquippedActors.FindByPredicate(
		[&EquipmentTypeTag](const ABag_EquipActor* EquippedActor)
		{
			return EquippedActor->GetEquipmentType().MatchesTagExact(EquipmentTypeTag);
		});
	return FoundActor ? *FoundActor : nullptr;
}

void UBag_EquipmentComponent::RemoveEquippedActor(const FGameplayTag& EquipmentTypeTag)
{
	
	if (ABag_EquipActor* EquippedActor = FindEquippedActor(EquipmentTypeTag); IsValid(EquippedActor))
	{
		EquippedActors.Remove(EquippedActor);
		EquippedActor->Destroy();
	}
}

void UBag_EquipmentComponent::InitPlayerController()
{
	if (OwningPlayerController = Cast<APlayerController>(GetOwner()); OwningPlayerController.IsValid())
	{
		if (ACharacter* OwnerCharacter = Cast<ACharacter>(OwningPlayerController->GetPawn()); IsValid(OwnerCharacter))
		{
			OnPossessedPawnChange(nullptr, OwnerCharacter);
		}
		else
		{
			OwningPlayerController->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChange);
		}
	}
}

void UBag_EquipmentComponent::OnPossessedPawnChange(APawn* OldPawn, APawn* NewPawn)
{
	if (ACharacter* OwnerCharacter = Cast<ACharacter>(OwningPlayerController->GetPawn()); IsValid(OwnerCharacter))
	{
		OwningSkeletalMesh = OwnerCharacter->GetMesh();
	}
	InitInventoryComponent();
}

