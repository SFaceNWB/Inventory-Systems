// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Composite/Bag_Composite.h"
#include "Bag_ItemDescription.generated.h"

class USizeBox;
/**
 * 
 */
UCLASS()
class INVENTORY_API UBag_ItemDescription : public UBag_Composite
{
	GENERATED_BODY()
public:

	FVector2D GetBoxSize() const;
	virtual	void SetVisibility(ESlateVisibility InVisibility) override;
private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> SizeBox;
};
