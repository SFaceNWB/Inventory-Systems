// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Bag_Highlightable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UBag_Highlightable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class INVENTORY_API IBag_Highlightable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, Category = "Inventory")
	void Highlight();

	UFUNCTION(BlueprintNativeEvent, Category = "Inventory")
	void UnHighlight();
};
