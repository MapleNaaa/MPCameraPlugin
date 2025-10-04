// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MPCameraDefine.h"
#include "UObject/Object.h"
#include "MPCameraModeStack.generated.h"

/**
 * 
 */
UCLASS()
class MPCAMERA_API UMPCameraModeStack : public UObject
{
	GENERATED_BODY()

public:
	UMPCameraModeStack();

	void ActivateStack();
	void DeactivateStack();
    bool IsStackActive() const { return bIsActive; }
	bool PushCameraMode(TSubclassOf<UMPCameraModeBase> CameraModeClass);
	void PreEvaluateStack(float DeltaTime);
	bool EvaluateStack(float DeltaTime, struct FMinimalViewInfo& OutResult);

	static FMinimalViewInfo POVBlend(const FMinimalViewInfo A, const FMinimalViewInfo B, float Alpha);

protected:
	int32 FindOrCreateCameraModeInstance(TSubclassOf<UMPCameraModeBase> CameraModeClass);
	void UpdateStack(float DeltaTime, struct FMinimalViewInfo InPOV);
    void BlendStack(float DeltaTime, struct FMinimalViewInfo& OutResult);

protected:

	bool bIsActive;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UMPCameraModeBase>> CameraModeStack;
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<UMPCameraModeBase>> CameraModeInstances;
	
};
