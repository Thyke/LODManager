// Copyright (C) Thyke. All Rights Reserved.

#include "CharacterLODManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/PlayerCameraManager.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterLODManagerComponent)

UCharacterLODManagerComponent::UCharacterLODManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UCharacterLODManagerComponent::RegisterSkeletalMeshComponent(USkeletalMeshComponent* NewMesh)
{
	if (NewMesh && !ManagedMeshes.Contains(NewMesh))
	{
		ManagedMeshes.Add(NewMesh);
		UE_LOG(LogTemp, Log, TEXT("Registered new mesh for LOD management: %s"), *NewMesh->GetName());
	}
}

void UCharacterLODManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	LoadLODConfig();
	RegisterSkeletalMeshComponents();
}

void UCharacterLODManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}


void UCharacterLODManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!LoadedLODConfig) return;

	LODUpdateTimer += DeltaTime;
	if (LODUpdateTimer >= LoadedLODConfig->LODSettings.LODUpdateInterval)
	{
		UpdateMeshOptimizations();
		LODUpdateTimer = 0.0f;
	}
}

void UCharacterLODManagerComponent::RegisterSkeletalMeshComponents()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	TArray<USkeletalMeshComponent*> SkeletalMeshes;
	Owner->GetComponents<USkeletalMeshComponent>(SkeletalMeshes);

	for (USkeletalMeshComponent* Mesh : SkeletalMeshes)
	{
		if (Mesh)
		{
			ManagedMeshes.AddUnique(Mesh);
		}
	}
}

void UCharacterLODManagerComponent::LoadLODConfig()
{
	if (LODConfig.IsValid())
	{
		LoadedLODConfig = LODConfig.LoadSynchronous();
	}
}

void UCharacterLODManagerComponent::UpdateMeshOptimizations()
{
	if (!LoadedLODConfig)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateMeshOptimizations: LoadedLODConfig is null"));
		return;
	}

	const FLODSettings& Settings = LoadedLODConfig->LODSettings;

	UE_LOG(LogTemp, Log, TEXT("UpdateMeshOptimizations: Starting optimization for %d meshes"), ManagedMeshes.Num());

	for (USkeletalMeshComponent* MeshComponent : ManagedMeshes)
	{
		if (!IsValid(MeshComponent))
		{
			UE_LOG(LogTemp, Warning, TEXT("UpdateMeshOptimizations: Invalid MeshComponent"));
			continue;
		}

		float DistanceToCamera = GetDistanceToCamera(MeshComponent);

		int32 CurrentLOD = MeshComponent->GetPredictedLODLevel();

		UE_LOG(LogTemp, Log, TEXT("Mesh: %s, Distance: %.2f, Current LOD: %d"),
			*MeshComponent->GetName(), DistanceToCamera, CurrentLOD);

		UpdateMeshLOD(MeshComponent, DistanceToCamera);

		int32 NewLOD = MeshComponent->GetPredictedLODLevel();

		if (CurrentLOD != NewLOD)
		{
			UE_LOG(LogTemp, Log, TEXT("LOD Changed for %s: %d -> %d"),
				*MeshComponent->GetName(), CurrentLOD, NewLOD);
		}

		if (Settings.bEnableVisibilityOptimization)
		{
			bool bWasVisible = MeshComponent->IsVisible();
			UpdateMeshVisibility(MeshComponent, DistanceToCamera);
			bool bIsVisible = MeshComponent->IsVisible();

			if (bWasVisible != bIsVisible)
			{
				UE_LOG(LogTemp, Log, TEXT("Visibility Changed for %s: %s -> %s"),
					*MeshComponent->GetName(), bWasVisible ? TEXT("Visible") : TEXT("Hidden"),
					bIsVisible ? TEXT("Visible") : TEXT("Hidden"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Visibility optimization disabled for %s"), *MeshComponent->GetName());
		}

		if (Settings.bEnablePhysicsOptimization)
		{
			bool bWasSimulatingPhysics = MeshComponent->IsSimulatingPhysics();
			UpdateMeshPhysics(MeshComponent, DistanceToCamera);
			bool bIsSimulatingPhysics = MeshComponent->IsSimulatingPhysics();

			if (bWasSimulatingPhysics != bIsSimulatingPhysics)
			{
				UE_LOG(LogTemp, Log, TEXT("Physics Simulation Changed for %s: %s -> %s"),
					*MeshComponent->GetName(), bWasSimulatingPhysics ? TEXT("On") : TEXT("Off"),
					bIsSimulatingPhysics ? TEXT("On") : TEXT("Off"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Physics optimization disabled for %s"), *MeshComponent->GetName());
		}
	}

	UE_LOG(LogTemp, Log, TEXT("UpdateMeshOptimizations: Finished optimization"));
}

void UCharacterLODManagerComponent::UpdateMeshLOD(USkeletalMeshComponent* MeshComponent, float DistanceToCamera)
{
	const FLODSettings& Settings = LoadedLODConfig->LODSettings;
	int32 DesiredLOD = 0;
	for (const FLODLevelSettings& LODLevel : Settings.LODLevels)
	{
		if (DistanceToCamera > LODLevel.DistanceThreshold)
		{
			DesiredLOD = LODLevel.LODLevel;
		}
		else
		{
			break;
		}
	}
	MeshComponent->SetForcedLOD(FMath::Clamp(DesiredLOD, 1, MeshComponent->GetNumLODs()));
}

void UCharacterLODManagerComponent::UpdateMeshVisibility(USkeletalMeshComponent* MeshComponent, float DistanceToCamera)
{
	const FLODSettings& Settings = LoadedLODConfig->LODSettings;
	float MaxLODDistance = Settings.LODLevels.Last().DistanceThreshold;
	float DistantThreshold = MaxLODDistance * Settings.DistantLimbThreshold;
	float VeryDistantThreshold = MaxLODDistance * Settings.VeryDistantLimbThreshold;

	if (DistanceToCamera > VeryDistantThreshold)
	{
		MeshComponent->SetVisibility(false);
	}
	else
	{
		MeshComponent->SetVisibility(true);
		MeshComponent->SetUpdateClothInEditor(DistanceToCamera <= DistantThreshold);
		MeshComponent->bDisableClothSimulation = (DistanceToCamera > DistantThreshold);
		MeshComponent->SetCastShadow(DistanceToCamera <= DistantThreshold);
	}
}

void UCharacterLODManagerComponent::UpdateMeshPhysics(USkeletalMeshComponent* MeshComponent, float DistanceToCamera)
{
	const FLODSettings& Settings = LoadedLODConfig->LODSettings;
	float MaxLODDistance = Settings.LODLevels.Last().DistanceThreshold;
	float VeryDistantThreshold = MaxLODDistance * Settings.VeryDistantLimbThreshold;

	if (DistanceToCamera > VeryDistantThreshold)
	{
		MeshComponent->SetSimulatePhysics(false);
	}
	else
	{
		MeshComponent->SetSimulatePhysics(true);
		if (MeshComponent->IsSimulatingPhysics() &&
			MeshComponent->GetPhysicsLinearVelocity().SizeSquared() < FMath::Square(Settings.SleepThreshold) &&
			MeshComponent->GetPhysicsAngularVelocityInDegrees().SizeSquared() < FMath::Square(Settings.SleepThreshold))
		{
			MeshComponent->PutAllRigidBodiesToSleep();
		}
	}
}

float UCharacterLODManagerComponent::GetDistanceToCamera(const USkeletalMeshComponent* MeshComponent)
{
	if (UWorld* World = GetWorld())
	{
		APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
		if (CameraManager)
		{
			FVector CameraLocation = CameraManager->GetCameraLocation();
			return FVector::Distance(MeshComponent->GetComponentLocation(), CameraLocation);
		}
	}
	return 0.f;
}