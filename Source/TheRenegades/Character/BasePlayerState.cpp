// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerState.h"

ABasePlayerState::ABasePlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);

	AbilitySystemComponent->AddAttributeSetSubobject<UBaseAttributes>(BaseAttributes);
}

void ABasePlayerState::BeginPlay()
{
	
}
