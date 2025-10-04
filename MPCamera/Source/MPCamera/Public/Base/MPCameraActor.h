// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "Component/MPCameraComponent.h"
#include "MPCameraActor.generated.h"

UCLASS()
class MPCAMERA_API AMPCameraActor : public ACameraActor
{
	GENERATED_BODY()

public:
	AMPCameraActor(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;

	UFUNCTION(BlueprintPure, Category = "MP Camera")
	UMPCameraComponent* GetMPCameraComponent() const { return MPCameraComponent; }

	UFUNCTION(BlueprintCallable, Category = "MP Camera")
	void SetAttachedActor(AActor* InActor) { AttachedActor = InActor; }
	UFUNCTION(BlueprintPure, Category = "MP Camera")
	virtual AActor* GetAttachedActor() const { return AttachedActor; }

    virtual void SetCameraMode(TSubclassOf<UMPCameraModeBase> InCameraModeClass);
	
	UFUNCTION(BlueprintCallable, Category = "MP Camera")
	void SetOffset(const FVector& InOffset) { Offset = InOffset; }

	

protected:
    virtual void NextTickInitialize();

	
protected:
	// 自定义摄像机组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MP Camera")
	TObjectPtr<UMPCameraComponent> MPCameraComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MP Camera")
	AActor* AttachedActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MP Camera")
    FVector Offset;
};
