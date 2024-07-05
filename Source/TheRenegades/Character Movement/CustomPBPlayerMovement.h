// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/PBPlayerMovement.h"
#include "CustomPBPlayerMovement.generated.h"

class FCustomNetworkMoveData : public FCharacterNetworkMoveData
{

public:

	typedef FCharacterNetworkMoveData Super;

	virtual void ClientFillNetworkMoveData(const FSavedMove_Character& ClientMove, ENetworkMoveType MoveType) override;

	virtual bool Serialize(UCharacterMovementComponent& CharacterMovement, FArchive& Ar, UPackageMap* PackageMap, ENetworkMoveType MoveType) override;

	// Variables here
};

class FCustomCharacterNetworkMoveDataContainer : public FCharacterNetworkMoveDataContainer
{

public:

	FCustomCharacterNetworkMoveDataContainer();

	FCustomNetworkMoveData CustomDefaultMoveData[3];
};

class FCustomSavedMove : public FSavedMove_Character
{
public:

	typedef FSavedMove_Character Super;


	/** Returns a byte containing encoded special movement information (jumping, crouching, etc.)	 */
	virtual uint8 GetCompressedFlags() const override;

	/** Returns true if this move can be combined with NewMove for replication without changing any behaviour.
	* Just determines if any variables were modified between moves for optimisation purposes. If nothing changed, combine moves to save time.
	*/
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;

	/** Called to set up this saved move (when initially created) to make a predictive correction. */
	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
	/** Called before ClientUpdatePosition uses this SavedMove to make a predictive correction	 */
	virtual void PrepMoveFor(class ACharacter* Character) override;

	/** Clear saved move properties, so it can be re-used. */
	virtual void Clear() override;
};

//Class Prediction Data
class FCustomNetworkPredictionData_Client : public FNetworkPredictionData_Client_Character
{
public:
	FCustomNetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement);

	typedef FNetworkPredictionData_Client_Character Super;

	///Allocates a new copy of our custom saved move
	virtual FSavedMovePtr AllocateNewMove() override;
};

class APBBaseCharacter;
/**
 * 
 */
UCLASS(Blueprintable)
class THERENEGADES_API UCustomPBPlayerMovement : public UPBPlayerMovement
{
	GENERATED_BODY()

	// Variables
public:


	// Sprinting
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Input | Sprinting")
	bool bWantsToSprint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Input | Sprinting")
	bool bIsSprinting;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Input | Sprinting")
	float CustomMaxSpeed{1000.f};

	UFUNCTION(BlueprintCallable, Category = "Sprinting")
	virtual bool CanSprint() const;

	virtual float GetMaxSpeed() const override;

	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;


public: // Class setup

	UCustomPBPlayerMovement(const FObjectInitializer& ObjectInitializer);
	
	friend class FCustomSavedMove;

protected:

	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<APBBaseCharacter> Character;

public:

	virtual void BeginPlay() override;

public:
	FCustomCharacterNetworkMoveDataContainer MoveDataContainer;

	virtual void UpdateFromCompressedFlags(uint8 flags) override;

	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;
};
