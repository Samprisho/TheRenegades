// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomPBPlayerMovement.h"

#include "GameFramework/Character.h"
#include "GameFramework/PhysicsVolume.h"

#include "../Character/PBBaseCharacter.h"
#include "Components/CapsuleComponent.h"

#include "Net/UnrealNetwork.h"
#include "UObject/CoreNetTypes.h"

UCustomPBPlayerMovement::UCustomPBPlayerMovement(const FObjectInitializer& ObjectInitializer)
{
	SetIsReplicatedByDefault(true);

	SetNetworkMoveDataContainer(MoveDataContainer);
}

void UCustomPBPlayerMovement::BeginPlay()
{
	Super::BeginPlay();
	Character = Cast<APBBaseCharacter>(PawnOwner);
}

void UCustomPBPlayerMovement::UpdateFromCompressedFlags(uint8 flags)
{
	Super::UpdateFromCompressedFlags(flags);
}



void FCustomNetworkMoveData::ClientFillNetworkMoveData(const FSavedMove_Character& ClientMove, ENetworkMoveType MoveType)
{
	Super::ClientFillNetworkMoveData(ClientMove, MoveType);

	const FCustomSavedMove& CurrentSavedMove = static_cast<const FCustomSavedMove&>(ClientMove);
}

bool FCustomNetworkMoveData::Serialize(UCharacterMovementComponent& CharacterMovement, FArchive& Ar, UPackageMap* PackageMap, ENetworkMoveType MoveType)
{
	Super::Serialize(CharacterMovement, Ar, PackageMap, MoveType);

	return !Ar.IsError();
}


bool FCustomSavedMove::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	FCustomSavedMove* NewMovePtr = static_cast<FCustomSavedMove*>(NewMove.Get());

	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void FCustomSavedMove::PrepMoveFor(ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	UCustomPBPlayerMovement* CharacterMovement = Cast<UCustomPBPlayerMovement>(Character->GetCharacterMovement());
	if (CharacterMovement) {

	}
}

void FCustomSavedMove::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	UCustomPBPlayerMovement* CharacterMovement = Cast<UCustomPBPlayerMovement>(Character->GetCharacterMovement());
	if (CharacterMovement) {

	}
}


void FCustomSavedMove::Clear()
{
	Super::Clear();
}

FCustomNetworkPredictionData_Client::FCustomNetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{
}

FSavedMovePtr FCustomNetworkPredictionData_Client::AllocateNewMove()
{
	return FSavedMovePtr(new FCustomSavedMove());
}

FCustomCharacterNetworkMoveDataContainer::FCustomCharacterNetworkMoveDataContainer()
{
	NewMoveData = &CustomDefaultMoveData[0];
	PendingMoveData = &CustomDefaultMoveData[1];
	OldMoveData = &CustomDefaultMoveData[2];
}

uint8 FCustomSavedMove::GetCompressedFlags() const
{
	return Super::GetCompressedFlags();
}

FNetworkPredictionData_Client* UCustomPBPlayerMovement::GetPredictionData_Client() const
{
	check(PawnOwner != NULL);

	if (!ClientPredictionData) {
		UCustomPBPlayerMovement* MutableThis = const_cast<UCustomPBPlayerMovement*>(this);

		MutableThis->ClientPredictionData = new FCustomNetworkPredictionData_Client(*this);
	}

	return ClientPredictionData;
}