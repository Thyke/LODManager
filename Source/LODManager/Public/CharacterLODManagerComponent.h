// Copyright (C) Thyke. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LODConfig.h"
#include "CharacterLODManagerComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LODMANAGER_API UCharacterLODManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UCharacterLODManagerComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LOD")
	TSoftObjectPtr<ULODConfig> LODConfig;

    UFUNCTION(BlueprintCallable, Category = "LOD Manager")
	void RegisterSkeletalMeshComponent(USkeletalMeshComponent* NewMesh);
protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    UPROPERTY()
    TArray<USkeletalMeshComponent*> ManagedMeshes;

    UPROPERTY()
    ULODConfig* LoadedLODConfig;

    float LODUpdateTimer = 0.0f;

    void RegisterSkeletalMeshComponents();
    void LoadLODConfig();

    void UpdateMeshOptimizations();
    void UpdateMeshLOD(USkeletalMeshComponent* MeshComponent, float DistanceToCamera);
    void UpdateMeshVisibility(USkeletalMeshComponent* MeshComponent, float DistanceToCamera);
    void UpdateMeshPhysics(USkeletalMeshComponent* MeshComponent, float DistanceToCamera);
    float GetDistanceToCamera(const USkeletalMeshComponent* MeshComponent);
};
