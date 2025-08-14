// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Composite/Bag_CompositeBase.h"
#include "Bag_Composite.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORY_API UBag_Composite : public UBag_CompositeBase
{
	GENERATED_BODY()
public:
	virtual void NativeOnInitialized() override;
	virtual void ApplyFunction(FuncType Function) override;
	virtual void Collapse() override;

private:
	UPROPERTY()
	TArray<TObjectPtr<UBag_CompositeBase>> Children;
};
