// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetworkTutorialGameMode.h"
#include "NetworkTutorialCharacter.h"
#include "UObject/ConstructorHelpers.h"

ANetworkTutorialGameMode::ANetworkTutorialGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
