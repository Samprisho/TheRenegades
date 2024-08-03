// Fill out your copyright notice in the Description page of Project Settings.


#include "PBBaseCharacter.h"

#include "Net/UnrealNetwork.h"
#include "UObject/CoreNetTypes.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"

APBBaseCharacter::APBBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomPBPlayerMovement>(APBPlayerCharacter::CharacterMovementComponentName))
{

}

UCustomPBPlayerMovement* APBBaseCharacter::GetCustomCharacterMovement() const
{
	return GetCharacterMovement<UCustomPBPlayerMovement>();
}

FCollisionQueryParams APBBaseCharacter::GetIgnoreCharacterParams() const
{

	FCollisionQueryParams Params;

	TArray<AActor*> CharacterChildren;
	GetAllChildActors(CharacterChildren);
	Params.AddIgnoredActors(CharacterChildren);
	Params.AddIgnoredActor(this);

	return Params;
}

void APBBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void APBBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Server only
void APBBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ABasePlayerState* PS = GetPlayerState<ABasePlayerState>();
	if (PS)
	{
		AbilitySystemComponent = Cast<UAbilitySystemComponent>(PS->AbilitySystemComponent);
		PS->AbilitySystemComponent->InitAbilityActorInfo(PS, this);

		if (!HasAuthority() || !IsValid(AbilitySystemComponent))
			return;

		for (auto StartupAbility : Abilities) {
			UE_LOG(LogTemp, Display, TEXT("Attempting to grant ability"))
			AbilitySystemComponent->GiveAbility(
				FGameplayAbilitySpec(StartupAbility, 1, -1, this));
		}
		
		UE_LOG(LogTemp, Display, TEXT("Granted All Abilities!!!!!!!!!"))
	}

}

// Client Only
void APBBaseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	auto PS = GetPlayerState<ABasePlayerState>();
	if (PS)
	{
		AbilitySystemComponent = Cast<UAbilitySystemComponent>(PS->AbilitySystemComponent);

		// Init ASC Actor Info for clients. Server will init its ASC when it possesses a new Actor.
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);
	}

	PlayerStateRep();
}

float APBBaseCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	return ActualDamage;
}

