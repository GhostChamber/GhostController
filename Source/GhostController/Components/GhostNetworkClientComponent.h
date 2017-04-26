#pragma once

#include "Components/ActorComponent.h"
#include "GhostNetworkClientComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GHOSTCONTROLLER_API UGhostNetworkClientComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGhostNetworkClientComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
