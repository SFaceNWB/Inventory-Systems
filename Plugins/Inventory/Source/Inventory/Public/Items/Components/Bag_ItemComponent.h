// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Bag_ItemComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class INVENTORY_API UBag_ItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBag_ItemComponent();

	FString GetPickupMessage() const { return PickupMessage; }
protected:

private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FString PickupMessage;
};
