#pragma once

#include "Components/ActorComponent.h"
#include "Networking.h"
#include "GhostNetworkClientComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GHOSTCONTROLLER_API UGhostNetworkClientComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGhostNetworkClientComponent();
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Ghost")
	bool Discover();

	UFUNCTION(BlueprintCallable, Category = "Ghost")
	bool Connect(const FString& IP);

	UFUNCTION(BlueprintCallable, Category = "Ghost")
	void Disconnect();

	UFUNCTION(BlueprintCallable, Category = "Ghost")
	void SendMessage(FString Message) const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ghost")
	int32 NetworkPort;

protected:
	void BeginPlay() override;

private:
	void GetServerAddress(FSocket* ReceiveSocket);

	void SendMessageToSocketAndAddress(FString Message, FSocket* Socket, TSharedPtr<FInternetAddr> RemoteAddress) const;


	FSocket* mConnectionSocket;
	TSharedPtr<FInternetAddr> mRemoteAddress;

	static const int32 NETWORK_PORT;
	static const FString BROADCAST_CODE;
	static const FString BROADCAST_SOCKET;
	static const FString LISTENER_SOCKET;
	static const FString DATA_SOCKET;
};
