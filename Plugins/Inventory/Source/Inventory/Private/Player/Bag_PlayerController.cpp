


#include "Player/Bag_PlayerController.h"
#include "Inventory.h"


void ABag_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogInventory, Log, TEXT("BeginPlay for PlayerController"));

}
