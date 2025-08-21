// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Bag_EquipmentComponent.generated.h"

struct FBag_ItemManifest;
struct FBag_EquipmentFragment;
class ABag_EquipActor;
class UBag_InventoryItem;
class UBag_InventoryComponent;
class APlayerController;
class USkeletalMeshComponent;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class INVENTORY_API UBag_EquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	void SetOwningSkeletalMesh(USkeletalMeshComponent* OwningMesh);
	void SetIsProxy(bool bProxy) { bIsProxy = bProxy; }
	void InitializeOwner(APlayerController* PlayerController);

protected:
	virtual void BeginPlay() override;

private:

	TWeakObjectPtr<UBag_InventoryComponent> InventoryComponent;
	TWeakObjectPtr<APlayerController> OwningPlayerController;
	TWeakObjectPtr<USkeletalMeshComponent> OwningSkeletalMesh;

	UFUNCTION()
	void OnItemEquipped(UBag_InventoryItem* EquippedItem);

	UFUNCTION()
	void OnItemUnequipped(UBag_InventoryItem* UnequippedItem);
		
	void InitInventoryComponent();
	ABag_EquipActor* SpawnEquipActor(FBag_EquipmentFragment* EquipmentFragment, const FBag_ItemManifest& Manifest, USkeletalMeshComponent* AttachMesh);

	UPROPERTY()
	TArray<TObjectPtr<ABag_EquipActor>> EquippedActors;

	ABag_EquipActor* FindEquippedActor(const FGameplayTag& EquipmentTypeTag);
	void RemoveEquippedActor(const FGameplayTag& EquipmentTypeTag);
	void InitPlayerController();

	UFUNCTION()
	void OnPossessedPawnChange(APawn* OldPawn, APawn* NewPawn);

	bool bIsProxy{ false };
};
