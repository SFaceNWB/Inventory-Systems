// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Bag_Highlightable.h"
#include "Components/StaticMeshComponent.h"
#include "Bag_HighlightableStaticMesh.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORY_API UBag_HighlightableStaticMesh : public UStaticMeshComponent, public IBag_Highlightable
{
	GENERATED_BODY()
public:
	virtual void Highlight_Implementation() override;
	virtual void UnHighlight_Implementation() override;

private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TObjectPtr<UMaterialInterface> HighlightMaterial;
};
