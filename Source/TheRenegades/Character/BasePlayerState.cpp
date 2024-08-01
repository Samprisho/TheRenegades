// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerState.h"

ABasePlayerState::ABasePlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);


}

UAbilitySystemComponent* ABasePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ABasePlayerState::BeginPlay()
{
	if (IsValid(AbilitySystemComponent))
	{
		BaseAttributes = AbilitySystemComponent->GetSet<UBaseAttributes>();
		UE_LOG(LogTemp, Display, TEXT("Added Base attributes to component"))

	}
	else {
		UE_LOG(LogTemp, Display, TEXT("Nope, failed to validate ability system component"))
	}
}
