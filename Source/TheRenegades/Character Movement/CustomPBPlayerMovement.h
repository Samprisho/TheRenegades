// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/PBPlayerMovement.h"

#include "GameFramework/Character.h"
#include "GameFramework/PhysicsVolume.h"

#include "Components/CapsuleComponent.h"

#include "CustomPBPlayerMovement.generated.h"

class FCustomNetworkMoveData : public FCharacterNetworkMoveData
{

public:

	typedef FCharacterNetworkMoveData Super;

	virtual void ClientFillNetworkMoveData(const FSavedMove_Character& ClientMove, ENetworkMoveType MoveType) override;

	virtual bool Serialize(UCharacterMovementComponent& CharacterMovement, FArchive& Ar, UPackageMap* PackageMap, ENetworkMoveType MoveType) override;

	// Variables here
	bool bWantsToSprintMoveData = false;
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

	// Variables
	bool bWantsToSprintSaved = false;
	bool bWallRunIsRightSaved = false;

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

UENUM(BlueprintType)
enum ECustomMovementMode
{
	MOVE_NONE UMETA(Hidden),
	MOVE_Wallrunning UMETA(DisplayName = "WallRunning"),
};

class APBBaseCharacter;
/**
 * 
 */
UCLASS(Blueprintable)
class THERENEGADES_API UCustomPBPlayerMovement : public UPBPlayerMovement
{
	GENERATED_BODY()

public:

	///////////////////
	// Sprinting
	///////////////////

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sprinting")
	bool bWantsToSprint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Sprinting")
	bool bIsSprinting;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sprinting")
	float CustomMaxSpeed{1000.f};

	UFUNCTION(BlueprintCallable, Category = "Sprinting")
	virtual bool CanSprint() const;

	//Wall Run
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Wallrunning")
	bool bWallRunIsRight;

	/////////////////////
	// Wall run variables
	/////////////////////

	UPROPERTY(EditDefaultsOnly)
	float MinWallRunSpeed = 200.f;

	UPROPERTY(EditDefaultsOnly)
	float MaxWallRunSpeed = 800.f;

	UPROPERTY(EditDefaultsOnly)
	float MaxVerticalWallRunSpeed = 200.f;

	UPROPERTY(EditDefaultsOnly)
	float WallRunPullAwayAngle = 75;

	UPROPERTY(EditDefaultsOnly)
	float WallAttractionForce = 200.f;

	UPROPERTY(EditDefaultsOnly)
	float MinWallRunHeight = 50.f;

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* WallRunGravityScaleCurve;

	UPROPERTY(EditDefaultsOnly)
	float WallJumpForce = 300.f;

	UFUNCTION(BlueprintPure)
	bool IsWallRunning() const { return IsCustomMovementMode(MOVE_Wallrunning); };

	bool IsCustomMovementMode(uint8 TestCustomMovementMode) const;

	// Need this to reset stateful variables upon landing
	virtual void ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations) override;

protected:
	// Helpers
	float OwnerCapsuleRadius() const { return CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius(); };
	float OwnerCapsuleHalfHeight() const { return CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight(); };

	///////////////
	// Wallrunning
	///////////////

	// Attempt wall run
	virtual bool TryWallRun();


	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

	// Actual math and calculations for wall running
	virtual void PhysWallRun(float DeltaTime, int32 Iterations);

	UFUNCTION(BlueprintNativeEvent, Category = "Wall Running")
	void EnterWallRun();
	virtual void EnterWallRun_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Wall Running")
	void ExitWallRun();
	virtual void ExitWallRun_Implementation();

public: // Class setupw

	UCustomPBPlayerMovement(const FObjectInitializer& ObjectInitializer);
	
	friend class FCustomSavedMove;

protected:

	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<APBBaseCharacter> Character;

	virtual void OnMovementModeChanged(EMovementMode PreviousMode, uint8 PreviousCustomMode) override;

public:

	virtual void BeginPlay() override;

	virtual float GetMaxSpeed() const override;

	virtual bool CanAttemptJump() const override;
	virtual bool DoJump(bool bReplayingMoves) override;

	// Update Character state before position change
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	// Update Character state after position change
	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;
	// FInal choice to change logic before next tick
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	FCustomCharacterNetworkMoveDataContainer MoveDataContainer;

	virtual void UpdateFromCompressedFlags(uint8 flags) override;

	virtual void MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags, const FVector& NewAccel) override;
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;
};
