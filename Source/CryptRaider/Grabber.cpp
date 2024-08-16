// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"

#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();

	if(PhysicsHandle && PhysicsHandle->GrabbedComponent)
	{
		const FVector HoldPoint = GetComponentLocation() + (HoldDistance * GetForwardVector());

		PhysicsHandle->SetTargetLocationAndRotation(HoldPoint, GetComponentRotation());
	}
}

void UGrabber::Grab()
{
	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();

	if (PhysicsHandle == nullptr)
	{
		return;
	}

	FHitResult HitResult;

	if (GetGrabbableInReach(HitResult))
	{
		/*DrawDebugSphere(GetWorld(), HitResult.Location, 10, 10, FColor::Blue, false, 5);
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10, 10, FColor::Green, false, 5);
		UE_LOG(LogTemp, Warning, TEXT("Hit Result: %s"), *(HitResult.GetActor()->GetActorNameOrLabel()));*/

		UPrimitiveComponent* HitComponent = HitResult.GetComponent();

		HitComponent->SetSimulatePhysics(true);
		HitComponent->WakeAllRigidBodies();

		AActor* HitActor = HitResult.GetActor();

		HitActor->Tags.Add("Grabbed");
		HitActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		PhysicsHandle->GrabComponentAtLocationWithRotation(
			HitComponent,
			NAME_None,
			HitResult.ImpactPoint,
			GetComponentRotation()
		);

	}
}

void UGrabber::Release()
{
	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();

	UPrimitiveComponent* GrabbedComponent = PhysicsHandle->GetGrabbedComponent();

	if(PhysicsHandle && GrabbedComponent)
	{
		GrabbedComponent->WakeAllRigidBodies();
		PhysicsHandle->ReleaseComponent();

		GrabbedComponent->GetOwner()->Tags.Remove("Grabbed");
	}
}

UPhysicsHandleComponent* UGrabber::GetPhysicsHandle() const
{
	UPhysicsHandleComponent* Handle = GetOwner()->GetComponentByClass<UPhysicsHandleComponent>();
	if(Handle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Grabber requires a UPhysicsHandleComponent."));
	}

	return Handle;
}

bool UGrabber::GetGrabbableInReach(FHitResult& OutHitResult) const
{
	const FVector StartPoint = GetComponentLocation();
	const FVector EndPoint = StartPoint + (MaxGrabDistance * GetForwardVector());

	//DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor::Red, false, 5);

	FCollisionShape Sphere = FCollisionShape::MakeSphere(GrabRadius);

	return GetWorld()->SweepSingleByChannel(
		OutHitResult,
		StartPoint,
		EndPoint,
		FQuat::Identity,
		ECC_GameTraceChannel2,
		Sphere
	);;
}

