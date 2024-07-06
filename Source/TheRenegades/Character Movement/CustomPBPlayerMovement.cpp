// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomPBPlayerMovement.h"

#include "../Character/PBBaseCharacter.h"

#include "Net/UnrealNetwork.h"
#include "UObject/CoreNetTypes.h"

UCustomPBPlayerMovement::UCustomPBPlayerMovement(const FObjectInitializer& ObjectInitializer)
{
	SetIsReplicatedByDefault(true);

	SetNetworkMoveDataContainer(MoveDataContainer);
}

void UCustomPBPlayerMovement::OnMovementModeChanged(EMovementMode PreviousMode, uint8 PreviousCustomMode)
{
	if (!HasValidData())
		return;


	// Call Exit codes
	if (PreviousCustomMode == MOVE_Custom)
		switch (PreviousCustomMode)
		{
		case MOVE_Wallrunning:
			ExitWallRun();
		}

	// Call Entry code
	if (MovementMode == MOVE_Custom)
		switch (CustomMovementMode)
		{
		case MOVE_Wallrunning:
			EnterWallRun();
		}

	Super::OnMovementModeChanged(PreviousMode, PreviousCustomMode);
}

void UCustomPBPlayerMovement::BeginPlay()
{
	Super::BeginPlay();
	Character = Cast<APBBaseCharacter>(PawnOwner);
}

float UCustomPBPlayerMovement::GetMaxSpeed() const
{
	if (MovementMode == MOVE_Custom) {
		switch (CustomMovementMode)
		{
		case MOVE_Wallrunning:
			return MaxWallRunSpeed;
		}
	}

	return bIsSprinting ? CustomMaxSpeed : Super::GetMaxSpeed();
}

bool UCustomPBPlayerMovement::CanAttemptJump() const
{
	return Super::CanAttemptJump() || IsWallRunning();
}

bool UCustomPBPlayerMovement::DoJump(bool bReplayingMoves)
{
	bool bWasWallRunning = IsWallRunning();
	if (Super::DoJump(bReplayingMoves))
	{
		if (bWasWallRunning && Character)
		{
			FVector Start = UpdatedComponent->GetComponentLocation();
			FVector CastDelta = UpdatedComponent->GetRightVector() * OwnerCapsuleRadius() * 2;
			FVector End = bWallRunIsRight ? Start + CastDelta : Start - CastDelta;
			auto Params = Character->GetIgnoreCharacterParams();
			FHitResult WallHit;
			GetWorld()->LineTraceSingleByProfile(WallHit, Start, End, "BlockAll", Params);
			Velocity += WallHit.Normal * WallJumpForce;
		}
		return true;
	}
	return false;
}


bool UCustomPBPlayerMovement::CanSprint() const
{
	// This can't run on a simulated proxy, since bWantsToSprint is only simulated
	if (Character && IsMovingOnGround() && bWantsToSprint) {
		FVector Forward = CharacterOwner->GetActorForwardVector();
		FVector MoveDirection = Velocity.GetSafeNormal();

		float VelocityDot = FVector::DotProduct(Forward, MoveDirection);
		return VelocityDot > 0.7f;
	}

	return false;
}


void UCustomPBPlayerMovement::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);

	// Gaurd code, if we are a simulated proxy, then we don't need to run this
	if (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)
		return;
	
	bIsSprinting = CanSprint();

	if (IsFalling())
		TryWallRun();
}

void UCustomPBPlayerMovement::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateAfterMovement(DeltaSeconds);


}

void UCustomPBPlayerMovement::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
}

bool UCustomPBPlayerMovement::IsCustomMovementMode(uint8 TestCustomMovementMode) const
{
	return MovementMode == EMovementMode::MOVE_Custom && CustomMovementMode == TestCustomMovementMode;
}

void UCustomPBPlayerMovement::ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations)
{
	Super::ProcessLanded(Hit, remainingTime, Iterations);
}

bool UCustomPBPlayerMovement::TryWallRun()
{
	if (!IsFalling()) return false;
	if (Velocity.SizeSquared2D() < pow(MinWallRunSpeed, 2)) return false;
	if (Velocity.Z < -MaxVerticalWallRunSpeed) return false;
	if (!Character) return false;

	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector LeftEnd = Start - UpdatedComponent->GetRightVector() * OwnerCapsuleRadius() * 2;
	FVector RightEnd = Start + UpdatedComponent->GetRightVector() * OwnerCapsuleRadius() * 2;
	auto Params = Character->GetIgnoreCharacterParams();
	FHitResult FloorHit, WallHit;
	// Check Player Height
	if (GetWorld()->LineTraceSingleByProfile(FloorHit, Start, Start + FVector::DownVector * (OwnerCapsuleHalfHeight() + MinWallRunHeight), "BlockAll", Params))
	{
		return false;
	}

	// Left Cast
	GetWorld()->LineTraceSingleByProfile(WallHit, Start, LeftEnd, "BlockAll", Params);
	if (WallHit.IsValidBlockingHit() && (Velocity | WallHit.Normal) < 0)
	{
		bWallRunIsRight = false;
	}
	// Right Cast
	else
	{
		GetWorld()->LineTraceSingleByProfile(WallHit, Start, RightEnd, "BlockAll", Params);
		if (WallHit.IsValidBlockingHit() && (Velocity | WallHit.Normal) < 0)
		{
			bWallRunIsRight = true;
		}
		else
		{
			return false;
		}
	}
	FVector ProjectedVelocity = FVector::VectorPlaneProject(Velocity, WallHit.Normal);
	if (ProjectedVelocity.SizeSquared2D() < pow(MinWallRunSpeed, 2)) return false;

	// Passed all conditions
	Velocity = ProjectedVelocity;
	Velocity.Z = FMath::Clamp(Velocity.Z, 0.f, MaxVerticalWallRunSpeed);
	SetMovementMode(MOVE_Custom, MOVE_Wallrunning);
	return true;
}

void UCustomPBPlayerMovement::PhysCustom(float deltaTime, int32 Iterations)
{
	if (GetOwner()->GetLocalRole() == ROLE_SimulatedProxy)
		return;

	switch (CustomMovementMode)
	{
		case MOVE_Wallrunning:
			PhysWallRun(deltaTime, Iterations);
			break;
		default:
			UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"));
	}

	Super::PhysCustom(deltaTime, Iterations);
}

void UCustomPBPlayerMovement::PhysWallRun(float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME)
	{
		return;
	}
	if (!Character || (!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)))
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}

	bJustTeleported = false;
	float remainingTime = DeltaTime;
	// Perform the move
	while ((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)))
	{
		Iterations++;
		bJustTeleported = false;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();

		FVector Start = UpdatedComponent->GetComponentLocation();
		FVector CastDelta = UpdatedComponent->GetRightVector() * OwnerCapsuleRadius() * 2;
		FVector End = bWallRunIsRight ? Start + CastDelta : Start - CastDelta;
		auto Params = Character->GetIgnoreCharacterParams();
		float SinPullAwayAngle = FMath::Sin(FMath::DegreesToRadians(WallRunPullAwayAngle));
		FHitResult WallHit;
		GetWorld()->LineTraceSingleByProfile(WallHit, Start, End, "BlockAll", Params);
		bool bWantsToPullAway = WallHit.IsValidBlockingHit() && !Acceleration.IsNearlyZero() && (Acceleration.GetSafeNormal() | WallHit.Normal) > SinPullAwayAngle;
		if (!WallHit.IsValidBlockingHit() || bWantsToPullAway)
		{
			SetMovementMode(MOVE_Falling);
			StartNewPhysics(remainingTime, Iterations);
			return;
		}
		// Clamp Acceleration
		Acceleration = FVector::VectorPlaneProject(Acceleration, WallHit.Normal);
		Acceleration.Z = 0.f;
		// Apply acceleration
		CalcVelocity(timeTick, 0.f, false, GetMaxBrakingDeceleration());
		Velocity = FVector::VectorPlaneProject(Velocity, WallHit.Normal);
		float TangentAccel = Acceleration.GetSafeNormal() | Velocity.GetSafeNormal2D();
		bool bVelUp = Velocity.Z > 0.f;
		Velocity.Z += GetGravityZ() * (WallRunGravityScaleCurve ? WallRunGravityScaleCurve->GetFloatValue(bVelUp ? 0.f : TangentAccel) * timeTick : 0.0f);
		if (Velocity.SizeSquared2D() < pow(MinWallRunSpeed, 2) || Velocity.Z < -MaxVerticalWallRunSpeed)
		{
			SetMovementMode(MOVE_Falling);
			StartNewPhysics(remainingTime, Iterations);
			return;
		}

		// Compute move parameters
		const FVector Delta = timeTick * Velocity; // dx = v * dt
		const bool bZeroDelta = Delta.IsNearlyZero();
		if (bZeroDelta)
		{
			remainingTime = 0.f;
		}
		else
		{
			FHitResult Hit;
			SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, Hit);
			FVector WallAttractionDelta = -WallHit.Normal * WallAttractionForce * timeTick;
			SafeMoveUpdatedComponent(WallAttractionDelta, UpdatedComponent->GetComponentQuat(), true, Hit);
		}
		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			remainingTime = 0.f;
			break;
		}
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / timeTick; // v = dx / dt
	}


	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector CastDelta = UpdatedComponent->GetRightVector() * OwnerCapsuleRadius() * 2;
	FVector End = bWallRunIsRight ? Start + CastDelta : Start - CastDelta;
	auto Params = Character->GetIgnoreCharacterParams();
	FHitResult FloorHit, WallHit;
	GetWorld()->LineTraceSingleByProfile(WallHit, Start, End, "BlockAll", Params);
	GetWorld()->LineTraceSingleByProfile(FloorHit, Start, Start + FVector::DownVector * (OwnerCapsuleHalfHeight() + MinWallRunHeight * .5f), "BlockAll", Params);
	if (FloorHit.IsValidBlockingHit() || !WallHit.IsValidBlockingHit() || Velocity.SizeSquared2D() < pow(MinWallRunSpeed, 2))
	{
		SetMovementMode(MOVE_Falling);
	}
}

void UCustomPBPlayerMovement::EnterWallRun_Implementation()
{
}

void UCustomPBPlayerMovement::ExitWallRun_Implementation()
{
}

void UCustomPBPlayerMovement::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, bIsSprinting, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(ThisClass, bWallRunIsRight, COND_SimulatedOnly);
}

void UCustomPBPlayerMovement::UpdateFromCompressedFlags(uint8 flags)
{
	Super::UpdateFromCompressedFlags(flags);
}

void FCustomNetworkMoveData::ClientFillNetworkMoveData(const FSavedMove_Character& ClientMove, ENetworkMoveType MoveType)
{
	Super::ClientFillNetworkMoveData(ClientMove, MoveType);

	const FCustomSavedMove& CurrentSavedMove = static_cast<const FCustomSavedMove&>(ClientMove);

	bWantsToSprintMoveData = CurrentSavedMove.bWantsToSprintSaved;
}

bool FCustomNetworkMoveData::Serialize(UCharacterMovementComponent& CharacterMovement, FArchive& Ar, UPackageMap* PackageMap, ENetworkMoveType MoveType)
{
	Super::Serialize(CharacterMovement, Ar, PackageMap, MoveType);

	SerializeOptionalValue<bool>(Ar.IsSaving(), Ar, bWantsToSprintMoveData, false);

	return !Ar.IsError();
}


void UCustomPBPlayerMovement::MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags, const FVector& NewAccel)
{
	FCustomNetworkMoveData* CurrentMoveData = static_cast<FCustomNetworkMoveData*>(GetCurrentNetworkMoveData());
	if (CurrentMoveData == nullptr)
		return;

	bWantsToSprint = CurrentMoveData->bWantsToSprintMoveData;

	Super::MoveAutonomous(ClientTimeStamp, DeltaTime, CompressedFlags, NewAccel);

}


bool FCustomSavedMove::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	FCustomSavedMove* NewMovePtr = static_cast<FCustomSavedMove*>(NewMove.Get());

	if (bWantsToSprintSaved != NewMovePtr->bWantsToSprintSaved) {
		return false;
	}

	if (bWallRunIsRightSaved != NewMovePtr->bWallRunIsRightSaved) {
		return false;
	}

	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

// Called when packet is dropped, resets compressed flag to saved state
void FCustomSavedMove::PrepMoveFor(ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	UCustomPBPlayerMovement* CharacterMovement = Cast<UCustomPBPlayerMovement>(Character->GetCharacterMovement());
	if (CharacterMovement) {
		CharacterMovement->bWantsToSprint = bWantsToSprintSaved;
		CharacterMovement->bWallRunIsRight = bWallRunIsRightSaved;
	}
}

// Server move before using
void FCustomSavedMove::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	UCustomPBPlayerMovement* CharacterMovement = Cast<UCustomPBPlayerMovement>(Character->GetCharacterMovement());
	if (CharacterMovement) {
		bWantsToSprintSaved = CharacterMovement->bWantsToSprint;
		bWallRunIsRightSaved = CharacterMovement->bWallRunIsRight;
	}
}

void FCustomSavedMove::Clear()
{
	Super::Clear();

	bWantsToSprintSaved = false;
	bWallRunIsRightSaved = false;

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
