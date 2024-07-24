// Copyright (C) Thyke. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LODConfig.generated.h"

USTRUCT(BlueprintType)
struct FLODLevelSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LOD")
    float DistanceThreshold;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LOD")
    int32 LODLevel;
};

USTRUCT(BlueprintType)
struct FLODSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LOD")
    float LODUpdateInterval = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LOD")
    float DistantLimbThreshold = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LOD")
    float VeryDistantLimbThreshold = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LOD")
    float SleepThreshold = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LOD")
    TArray<FLODLevelSettings> LODLevels;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LOD")
    bool bEnableVisibilityOptimization = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LOD")
    bool bEnablePhysicsOptimization = true;
};

UCLASS(BlueprintType)
class LODMANAGER_API ULODConfig : public UDataAsset
{
	GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dismemberment|LOD")
    FLODSettings LODSettings;
};
