


#include "Player/Bag_PlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/HUD/Bag_HUDWidget.h"

ABag_PlayerController::ABag_PlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	TraceLength = 500.0f;
}

void ABag_PlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TraceForItem();
}

void ABag_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (IsValid(Subsystem))
	{
		Subsystem->AddMappingContext(DefaultIMC, 0);
	}

	CreateHUDWidget();

}

void ABag_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
		{
			Subsystem->AddMappingContext(CurrentContext, 0);
		}
	}

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(PrimaryInteractAction, ETriggerEvent::Started, this, &ABag_PlayerController::PrimaryInteract);
}

void ABag_PlayerController::PrimaryInteract()
{
	UE_LOG(LogTemp, Log, TEXT("Primary Interact"));
}

void ABag_PlayerController::CreateHUDWidget()
{
	if (!IsLocalController())
	{
		return;
	}

	HUDWidget = CreateWidget<UBag_HUDWidget>(this, HUDWidgetClass);
	if (IsValid(HUDWidget))
	{
		HUDWidget->AddToViewport();
	}
}

void ABag_PlayerController::TraceForItem()
{
	if (!IsValid(GEngine) || !IsValid(GEngine->GameViewport))
	{
		return;
	}
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	const FVector2D ViewportCenter = ViewportSize * 0.5f;

	FVector TraceStart;
	FVector Forward;
	if (!UGameplayStatics::DeprojectScreenToWorld(this, ViewportCenter, TraceStart, Forward))
	{
		return;
	}

	const FVector TraceEnd = TraceStart + (Forward * TraceLength);
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ItemTraceChannel);

	LastActor = ThisActor;
	ThisActor = HitResult.GetActor();

	if (ThisActor == LastActor)
	{
		return;
	}

	if (ThisActor.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Start trace a new actor"));
	}

	if (LastActor.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Stop trace last actor"));
	}
}
