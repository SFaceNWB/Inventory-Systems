

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Bag_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORY_API ABag_PlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
	
};
