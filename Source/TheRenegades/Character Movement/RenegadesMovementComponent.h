// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Everything PBPlayerMovement
#include "CoreMinimal.h"
#include "BaseMovementComponent.h"
#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "NetworkPredictionWorldManager.h"
#include "NetworkPredictionProxy.h"
#include "Components/CapsuleComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "HAL/IConsoleManager.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Sound/SoundCue.h"
#include "ProfilingDebugging/CsvProfiler.h"

// To facilitate A/B testing by disabling CMC if present
#include "NetworkPredictionProxyInit.h"
#include "NetworkPredictionModelDefRegistry.h"
#include "NetworkPredictionProxyWrite.h"
#include "Services/NetworkPredictionInstanceMap.h"
#include "Services/NetworkPredictionService_Finalize.inl"
#include "Services/NetworkPredictionService_Input.inl"
#include "Services/NetworkPredictionService_Interpolate.inl"
#include "Services/NetworkPredictionService_Physics.inl"
#include "Services/NetworkPredictionService_Rollback.inl"
#include "Services/NetworkPredictionService_ServerRPC.inl"
#include "NetworkPredictionReplicationProxy.h"
#include "RenegadesMovementSimulation.h"

#include "RenegadesMovementComponent.generated.h"

#define Print(time, msg) GEngine->AddOnScreenDebugMessage(-1, time, FColor::Cyan, msg);

enum ENetRole : int;
namespace EEndPlayReason { enum Type : int; }

/**
 * 
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class THERENEGADES_API URenegadesMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

protected:

	/** Milliseconds between step sounds */
	float MoveSoundTime;

	/** If we are stepping left, else, right */
	bool StepSide;

	/** The multiplier for acceleration when on ground. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking")
	float GroundAccelerationMultiplier;

	/** The multiplier for acceleration when in air. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking")
	float AirAccelerationMultiplier;

	/* The vector differential magnitude cap when in air. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Jumping / Falling")
	float AirSpeedCap;

	/** Time to crouch on ground in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking")
	float CrouchTime;

	/** Time to uncrouch on ground in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking")
	float UncrouchTime;

	/** Time to crouch in air in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking")
	float CrouchJumpTime;

	/** Time to uncrouch in air in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking")
	float UncrouchJumpTime;

	/** the minimum step height from moving fast */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite)
	float MinStepHeight;

	/** Time (in millis) the player has to rejump without applying friction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Jumping / Falling", meta = (DisplayName = "Rejump Window", ForceUnits = "ms"))
	float BrakingWindow;

	/* Progress checked against the Braking Window, incremented in millis. */
	float BrakingWindowTimeElapsed;

	/** If the player has been on the ground past the Braking Window, start braking. */
	bool bBrakingWindowElapsed;

	/** Wait a frame before crouch speed. */
	bool bCrouchFrameTolerated = false;

	/** If in the crouching transition */
	bool bIsInCrouchTransition = false;

	/** If in the crouching transition */
	bool bInCrouch;

	/** The target ground speed when running. */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float RunSpeed;

	/** The target ground speed when sprinting.  */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SprintSpeed;

	/** The target ground speed when walking slowly. */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float WalkSpeed;

	/** Speed on a ladder */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Ladder")
	float LadderSpeed;

	/** The minimum speed to scale up from for slope movement  */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SpeedMultMin;

	/** The maximum speed to scale up to for slope movement */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SpeedMultMax;

	/** The maximum angle we can roll for camera adjust */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)")
	float RollAngle = 0.0f;

	/** Speed of rolling the camera */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)")
	float RollSpeed = 0.0f;

	/** Speed of rolling the camera */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)")
	float BounceMultiplier = 0.0f;

	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float AxisSpeedLimit = 6667.5f;

	/** Threshold relating to speed ratio and friction which causes us to catch air */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SlideLimit = 0.5f;

	/** Fraction of uncrouch half-height to check for before doing starting an uncrouch. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)")
	float GroundUncrouchCheckFactor = 0.75f;

	bool bShouldPlayMoveSounds = true;

public:

	/*
	

	/** Print pos and vel (Source: cl_showpos) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)")
	uint32 bShowPos : 1;


	URenegadesMovementComponent();

	/* HERE <------------------------

	virtual void InitializeComponent() override;
	void OnRegister() override;

	// Overrides for Source-like movement
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration) override;
	virtual void ApplyVelocityBraking(float DeltaTime, float Friction, float BrakingDeceleration) override;
	void PhysFalling(float deltaTime, int32 Iterations);
	bool ShouldLimitAirControl(float DeltaTime, const FVector& FallAcceleration) const override;
	FVector NewFallVelocity(const FVector& InitialVelocity, const FVector& Gravity, float DeltaTime) const override;

	void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;

	void UpdateSurfaceFriction(bool bIsSliding = false);
	void UpdateCrouching(float DeltaTime, bool bOnlyUnCrouch = false);

	// Overrides for crouch transitions
	virtual void Crouch(bool bClientSimulation = false) override;
	virtual void UnCrouch(bool bClientSimulation = false) override;
	virtual void DoCrouchResize(float TargetTime, float DeltaTime, bool bClientSimulation = false);
	virtual void DoUnCrouchResize(float TargetTime, float DeltaTime, bool bClientSimulation = false);

	bool MoveUpdatedComponentImpl(const FVector& Delta, const FQuat& NewRotation, bool bSweep, FHitResult* OutHit = nullptr, ETeleportType Teleport = ETeleportType::None) override;

	// Jump overrides
	bool CanAttemptJump() const override;
	bool DoJump(bool bClientSimulation) override;

	void TwoWallAdjust(FVector& OutDelta, const FHitResult& Hit, const FVector& OldHitNormal) const override;
	float SlideAlongSurface(const FVector& Delta, float Time, const FVector& Normal, FHitResult& Hit, bool bHandleImpact = false) override;
	FVector ComputeSlideVector(const FVector& Delta, const float Time, const FVector& Normal, const FHitResult& Hit) const override;
	FVector HandleSlopeBoosting(const FVector& SlideResult, const FVector& Delta, const float Time, const FVector& Normal, const FHitResult& Hit) const override;
	bool ShouldCatchAir(const FFindFloorResult& OldFloor, const FFindFloorResult& NewFloor) override;
	bool IsWithinEdgeTolerance(const FVector& CapsuleLocation, const FVector& TestImpactPoint, const float CapsuleRadius) const override;
	bool IsValidLandingSpot(const FVector& CapsuleLocation, const FHitResult& Hit) const override;
	bool ShouldCheckForValidLandingSpot(float DeltaTime, const FVector& Delta, const FHitResult& Hit) const override;

	void TraceCharacterFloor(FHitResult& OutHit);

	// Acceleration
	FORCEINLINE FVector GetAcceleration() const
	{
		return Acceleration;
	}

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode);

	/** Do camera roll effect based on velocity */

	/* HERE <---------------------------------------------------------------

	float GetCameraRoll();

	void SetNoClip(bool bNoClip);

	/** Toggle no clip */

	/* HERE <-----------------------------------------

	void ToggleNoClip();

	bool IsBrakingWindowTolerated() const
	{
		return bBrakingWindowElapsed;
	}

	bool IsInCrouch() const
	{
		return bInCrouch;
	}

	virtual float GetMaxSpeed() const override;

	*/

public:

	/*

	float DefaultStepHeight;
	float DefaultWalkableFloorZ;
	float SurfaceFriction;

	/** The time that the player can remount on the ladder */

	/*

	bool bHasDeferredMovementMode;
	EMovementMode DeferredMovementMode;

public:

	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
	virtual void PreNetReceive() override;
	virtual void EndPlay(const EEndPlayReason::Type Reason);

	// Invoke the ServerRPC, called from UNetworkPredictionWorldManager via the TServerRPCService.
	void CallServerRPC();

protected:

	// Classes must initialize the NetworkPredictionProxy (register with the NetworkPredictionSystem) here. EndPlay will unregister.
	virtual void InitializeNetworkPredictionProxy() { check(false); }

	// Finalizes initialization when NetworkRole changes. Does not need to be overridden.
	virtual void InitializeForNetworkRole(ENetRole Role, const bool bHasNetConnection);

	// Helper: Checks if the owner's role has changed and calls InitializeForNetworkRole if necessary.
	bool CheckOwnerRoleChange();

	// The actual ServerRPC. This needs to be a UFUNCTION but is generic and not dependent on the simulation instance
	UFUNCTION(Server, unreliable, WithValidation)
	void ServerReceiveClientInput(const FServerReplicationRPCParameter& ProxyParameter);

	// Proxy to interface with the NetworkPrediction system
	UPROPERTY(Replicated, transient)
	FNetworkPredictionProxy NetworkPredictionProxy;

	// ReplicationProxies are just pointers to the data/NetSerialize functions within the NetworkSim
	UPROPERTY()
	FReplicationProxy ReplicationProxy_ServerRPC;

private:

	UPROPERTY(Replicated, transient)
	FReplicationProxy ReplicationProxy_Autonomous;

	UPROPERTY(Replicated, transient)
	FReplicationProxy ReplicationProxy_Simulated;

	UPROPERTY(Replicated, transient)
	FReplicationProxy ReplicationProxy_Replay;

protected:

	FReplicationProxySet GetReplicationProxies()
	{
		return FReplicationProxySet{ &ReplicationProxy_ServerRPC, &ReplicationProxy_Autonomous, &ReplicationProxy_Simulated, &ReplicationProxy_Replay };
	}

	*/
};
