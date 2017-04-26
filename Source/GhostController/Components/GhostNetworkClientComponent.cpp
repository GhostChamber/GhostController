#include "GhostController.h"
#include "GhostNetworkClientComponent.h"


UGhostNetworkClientComponent::UGhostNetworkClientComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UGhostNetworkClientComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UGhostNetworkClientComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
