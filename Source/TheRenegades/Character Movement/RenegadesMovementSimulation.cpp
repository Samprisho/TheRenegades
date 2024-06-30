#include "RenegadesMovementSimulation.h"

void FRenegadesMovementSimulation::SimulationTick(const FNetSimTimeStep& TimeStep, const TNetSimInput<CharacterMotionStateTypes>& Input, const TNetSimOutput<CharacterMotionStateTypes>& Output)
{
	// InvalidateCache();
	*Output.Sync = *Input.Sync;

	const float DeltaSeconds = (float)TimeStep.StepMS / 1000.f;
}

/*

void FRenegadesMovementSimulation::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void FRenegadesMovementSimulation::InvalidateCache()
{

	CachedFloor.Clear();
}

bool FRenegadesMovementSimulation::IsExceedingMaxSpeed(const FVector& Velocity, float InMaxSpeed) const
{
	return false;
}

FVector FRenegadesMovementSimulation::ComputeSlideVector(const FVector& Delta, const float Time, const FVector& Normal, const FHitResult& Hit) const
{
	return FVector();
}

void FRenegadesMovementSimulation::TwoWallAdjust(FVector& OutDelta, const FHitResult& Hit, const FVector& OldHitNormal) const
{
}

float FRenegadesMovementSimulation::SlideAlongSurface(const FVector& Delta, float Time, const FQuat Rotation, const FVector& Normal, FHitResult& Hit, bool bHandleImpact)
{
	return 0.0f;
}

void FRenegadesMovementSimulation::PerformMovement(float DeltaSeconds, const TNetSimInput<CharacterMotionStateTypes>& Input, const TNetSimOutput<CharacterMotionStateTypes>& Output, const FVector& LocalSpaceMovementInput)
{
}

void FRenegadesMovementSimulation::Movement_Walking(float DeltaSeconds, const TNetSimInput<CharacterMotionStateTypes>& Input, const TNetSimOutput<CharacterMotionStateTypes>& Output, const FVector& LocalSpaceMovementInput)
{
}

void FRenegadesMovementSimulation::Movement_Falling(float DeltaSeconds, const TNetSimInput<CharacterMotionStateTypes>& Input, const TNetSimOutput<CharacterMotionStateTypes>& Output, const FVector& LocalSpaceMovementInput)
{
}

void FRenegadesMovementSimulation::SetMovementMode(ECharacterMovementMode MovementMode, const TNetSimInput<CharacterMotionStateTypes>& Input, const TNetSimOutput<CharacterMotionStateTypes>& Output)
{
}

FRotator FRenegadesMovementSimulation::ComputeLocalRotation(const FNetSimTimeStep& TimeStep, const TNetSimInput<CharacterMotionStateTypes>& Input, const TNetSimOutput<CharacterMotionStateTypes>& Output) const
{
	return FRotator();
}

FVector FRenegadesMovementSimulation::ComputeLocalInput(const FNetSimTimeStep& TimeStep, const TNetSimInput<CharacterMotionStateTypes>& Input, const TNetSimOutput<CharacterMotionStateTypes>& Output) const
{
	return FVector();
}

FVector FRenegadesMovementSimulation::ComputeVelocity(float DeltaSeconds, const FVector& InitialVelocity, const TNetSimInput<CharacterMotionStateTypes>& Input, const TNetSimOutput<CharacterMotionStateTypes>& Output, const FVector& LocalSpaceMovementInput) const
{
	return FVector();
}

FVector FRenegadesMovementSimulation::ComputeGravity(float DeltaSeconds, const TNetSimInput<CharacterMotionStateTypes>& Input, const TNetSimOutput<CharacterMotionStateTypes>& Output, const FVector& Delta) const
{
	return FVector();
}

void FRenegadesMovementSimulation::FindFloor(const TNetSimInput<CharacterMotionStateTypes>& Input, const TNetSimOutput<CharacterMotionStateTypes>& Output, const FVector& Location, FFloorTestResult& OutFloorResult) const
{
}

void FRenegadesMovementSimulation::ComputeFloorDist(const TNetSimInput<CharacterMotionStateTypes>& Input, const TNetSimOutput<CharacterMotionStateTypes>& Output, const FVector& Location, FFloorTestResult& OutFloorResult, float SweepDistance) const
{
}

bool FRenegadesMovementSimulation::FloorSweepTest(FHitResult& OutHit, const FVector& Start, const FVector& End, ECollisionChannel TraceChannel, const FCollisionShape& CollisionShape, const FCollisionQueryParams& Params, const FCollisionResponseParams& ResponseParam) const
{
	return false;
}

bool FRenegadesMovementSimulation::IsWalkable(const FHitResult& Hit, const TNetSimInput<CharacterMotionStateTypes>& Input, const TNetSimOutput<CharacterMotionStateTypes>& Output) const
{
	return false;
}

bool FRenegadesMovementSimulation::IsValidLandingSpot(const FVector& Location, const FHitResult& Hit, const TNetSimInput<CharacterMotionStateTypes>& Input, const TNetSimOutput<CharacterMotionStateTypes>& Output, const FVector& Delta) const
{
	return false;
}
*/