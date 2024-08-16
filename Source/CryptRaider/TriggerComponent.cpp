// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerComponent.h"

UTriggerComponent::UTriggerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UTriggerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTriggerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* AcceptableActor = GetAcceptableActor();
	if(AcceptableActor != nullptr)
	{
		if(Mover != nullptr)
		{
			UPrimitiveComponent* Component = Cast<UPrimitiveComponent>(AcceptableActor->GetRootComponent());

			if(Component != nullptr)
			{
				Component->SetSimulatePhysics(false);
			}

			AcceptableActor->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);

			Mover->SetShouldMove(true);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Trigger requires a UMover."));
		}
	}
	else
	{
		Mover->SetShouldMove(false);
	}
}

void UTriggerComponent::SetMover(UMover* NewMover)
{
	Mover = NewMover;
	UE_LOG(LogTemp, Warning, TEXT("New mover added to trigger."));
}

AActor* UTriggerComponent::GetAcceptableActor() const
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		//UE_LOG(LogTemp, Display, TEXT("Overlaped Actor: %s"), *Actor->GetActorNameOrLabel());
		if (Actor->ActorHasTag(AcceptableActorTag) && !Actor->ActorHasTag("Grabbed"))
		{
			//UE_LOG(LogTemp, Warning, TEXT("Unlocking Door!"));
			return Actor;
		}
	}

	return nullptr;
}
