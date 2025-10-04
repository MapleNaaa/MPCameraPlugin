// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MPCameraActor.h"
#include "MPCameraDefine.h"
#include "Camera/PlayerCameraManager.h"
#include "Mode/MPCameraModeBase.h"
#include "MPPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class MPCAMERA_API AMPPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	AMPPlayerCameraManager(const FObjectInitializer& ObjectInitializer);

	// ~ Begin APlayerCameraManager Interface
	virtual void BeginPlay() override;
	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
	virtual void UpdateMPViewTarget(FTViewTarget& OutVT, float DeltaTime);
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void ApplyCameraModifiers(float DeltaTime, FMinimalViewInfo& OutViewInfo) override;
	virtual void UpdateCameraLensEffects( const FTViewTarget& OutVT ) override;
	virtual void SetViewTarget(class AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams = FViewTargetTransitionParams()) override;
	// ~ End APlayerCameraManager Interface
	
	void SpawnMPCameraActor();
	void SetMPCameraMode(TSubclassOf<UMPCameraModeBase> InCameraModeClass);
	
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ToolTip="Whether to use MPCamera logic, If false will use default camera manager logic"), Category = "MP Camera")
	bool bUseMPCameraLogic = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MP Camera")
	TObjectPtr<AMPCameraActor> MPCameraActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MP Camera")
	TObjectPtr<UMPCameraModeBase> MPCameraMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MP Camera")
	TSubclassOf<UMPCameraModeBase> DefaultCameraModeClass;
};
