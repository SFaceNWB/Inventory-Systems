

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Bag_PlayerController.generated.h"

class UBag_HUDWidget;
class UInputMappingContext;
class UInputAction;

/**
 * 
 */
UCLASS()
class INVENTORY_API ABag_PlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TArray<UInputMappingContext*> DefaultMappingContexts;

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:

	void PrimaryInteract();
	void CreateHUDWidget();

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UInputMappingContext> DefaultIMC;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UInputAction> PrimaryInteractAction;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UBag_HUDWidget> HUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UBag_HUDWidget> HUDWidget;
};
