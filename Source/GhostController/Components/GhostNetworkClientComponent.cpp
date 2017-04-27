#include "GhostController.h"
#include "GhostNetworkClientComponent.h"


const int32 UGhostNetworkClientComponent::NETWORK_PORT = 4000;
const FString UGhostNetworkClientComponent::BROADCAST_CODE = "GHOST-CONTROLLER";
const FString UGhostNetworkClientComponent::BROADCAST_SOCKET = "GhostControllerBroadcast";
const FString UGhostNetworkClientComponent::LISTENER_SOCKET = "GhostControllerListener";
const FString UGhostNetworkClientComponent::DATA_SOCKET = "GhostControllerData";


UGhostNetworkClientComponent::UGhostNetworkClientComponent() :
	mConnectionSocket(nullptr), NetworkPort(NETWORK_PORT)
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


bool UGhostNetworkClientComponent::Discover()
{
	// Setup listener socket
	TSharedPtr<FInternetAddr> listenAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	listenAddress->SetPort(NetworkPort + 1);
	FSocket* receiveSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_DGram, LISTENER_SOCKET);
	receiveSocket->SetNonBlocking(true);
	int32 newSize = 0;
	receiveSocket->SetReceiveBufferSize(1024, newSize);
	receiveSocket->Bind(*listenAddress);
	receiveSocket->Listen(8);

	// Setup timer to listen from server
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, [receiveSocket, this]()
	{
		GetServerAddress(receiveSocket);
	}, 1, false);


	// Broadcast server query request
	TSharedPtr<FInternetAddr> broadcastAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	broadcastAddress->SetBroadcastAddress();
	broadcastAddress->SetPort(NetworkPort);
	FSocket* socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_DGram, BROADCAST_SOCKET);
	socket->SetBroadcast(true);
	SendMessageToSocketAndAddress(BROADCAST_CODE, socket, broadcastAddress);
	socket->Close();

	return true;
}


bool UGhostNetworkClientComponent::Connect(const FString& IP)
{
	if (mConnectionSocket == nullptr)
	{
		mRemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		bool bIsValid;
		mRemoteAddress->SetIp(*IP, bIsValid);
		mRemoteAddress->SetPort(NetworkPort);
		if (!bIsValid)
		{
			UE_LOG(LogTemp, Error, TEXT("IP address was not valid! : %s"), *IP);
			return false;
		}
		mConnectionSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_DGram, DATA_SOCKET);
		return true;
	}
	UE_LOG(LogTemp, Warning, TEXT("Unable to connect to %s:%d"), *IP, NetworkPort);
	return false;
}


void UGhostNetworkClientComponent::Disconnect()
{
	mConnectionSocket->Close();
	mConnectionSocket = nullptr;
}


void UGhostNetworkClientComponent::SendMessage(FString Message) const
{
	SendMessageToSocketAndAddress(Message, mConnectionSocket, mRemoteAddress);
}


void UGhostNetworkClientComponent::SendMessageToSocketAndAddress(FString Message, FSocket* Socket, TSharedPtr<FInternetAddr> RemoteAddress) const
{
	if (Socket != nullptr)
	{
		//Set Send Buffer Size
		int32 sendSize = Message.Len();
		int32 newSize;
		Socket->SetSendBufferSize(sendSize, newSize);
		if (newSize != sendSize)
		{
			UE_LOG(LogTemp, Error, TEXT("Tried to set SendSize as %u but the system set it to %u"), sendSize, newSize);
		}

		int32 bytesSent = 0;
		Socket->SendTo(reinterpret_cast<uint8*>(TCHAR_TO_UTF8(*Message)), sendSize, bytesSent, *RemoteAddress);
		if (bytesSent < Message.Len())
		{
			UE_LOG(LogTemp, Error, TEXT("Error sending message. Only %d bytes sent when the message was %d bytes long"), bytesSent, sendSize);
		}
	}
}


void UGhostNetworkClientComponent::GetServerAddress(FSocket* ReceiveSocket)
{
	bool pending;
	if (ReceiveSocket->HasPendingConnection(pending) && pending)
	{
		TSharedPtr<FInternetAddr> serverAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		const uint32 bufferSize = 1024;
		uint8 buffer[bufferSize];
		int32 bytesRead;
		ReceiveSocket->RecvFrom(buffer, bufferSize, bytesRead, *serverAddress);
		ReceiveSocket->Close();
		UE_LOG(LogTemp, Warning, TEXT("Received reply = %d bytes, address = %s"), bytesRead, *(serverAddress->ToString(false)));

		int32 port = 0;
		serverAddress->GetPort(port);
		FString ServerIP(serverAddress->ToString(false));
		UE_LOG(LogTemp, Warning, TEXT("IP = %s, Port = %d"), *ServerIP, NetworkPort);
		Connect(ServerIP);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("No pending data"));
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, [ReceiveSocket, this]()
		{
			GetServerAddress(ReceiveSocket);
		}, 0.1, false);
	}
}
