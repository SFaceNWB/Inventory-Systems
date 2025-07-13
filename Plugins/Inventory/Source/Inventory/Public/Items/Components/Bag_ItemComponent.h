// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/Manifest/Bag_ItemManifest.h"
#include "Bag_ItemComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class INVENTORY_API UBag_ItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBag_ItemComponent();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	FBag_ItemManifest GetItemManifest()const { return ItemManifest; }

	FString GetPickupMessage() const { return PickupMessage; }
protected:

private:
	UPROPERTY(Replicated ,EditAnywhere, Category = "Inventory")
	FBag_ItemManifest ItemManifest;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FString PickupMessage;
};
