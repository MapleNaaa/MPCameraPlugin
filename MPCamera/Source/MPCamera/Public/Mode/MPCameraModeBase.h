// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/MPCameraDefine.h"
#include "Engine/DataAsset.h"
#include "Help/MPCameraInterpolators.h"
#include "MPCameraModeBase.generated.h"

class UMPCameraComponent;
/**
 * 
 */
UCLASS(Abstract)
class MPCAMERA_API UMPCameraModeBase : public UObject
{
	GENERATED_BODY()

	friend class AMPPlayerCameraManager;
	
public:
    UMPCameraModeBase();


    virtual bool ShouldLockOutgoingPOV() const;

    virtual void SetBlendWay(bool bInOrOut) { bIsBlendIn = bInOrOut;  CurrentTransParams = bInOrOut ? TransInParams : TransOutParams; }
	// virtual FMPTransitionParams GetBlendParams() const { return CurrentTransParams; }
    virtual float GetBlendTime() const { return CurrentTransParams.BlendTime; }
	virtual void SetBlendWeight(float InWeight);
    virtual float GetBlendWeight() const { return BlendWeight; }
	
    virtual void OnRemovedFromStack();

	virtual void OnBecomeActive();
    virtual void OnDeactivated();

	virtual void UpdateCamera(float DeltaTime, struct FMinimalViewInfo InPOV);
	virtual void PreUpdateCamera(float DeltaTime);
	virtual void PostUpdateCamera(float DeltaTime);
	virtual FMinimalViewInfo GetPOV() const { return CurrentPOV; }

	virtual UMPCameraComponent* GetCameraComponent() const;
	virtual AActor* GetAttachedActor() const;

	
    void PreventCameraPenetration(AActor* ViewTarget, FVector const& SafeLoc, FVector& CameraLoc, float const& DeltaTime, float& DistBlockedPct, bool bSingleRayOnly);
	
protected:
	// Compute the camera's position
	virtual void UpdatePivotToWorld();
	virtual void UpdateLookAtToWorld();
	virtual void UpdateCameraToWorld();
	virtual void UpdatePreventPenetration(float DeltaTime);
	virtual void UpdatePOV(FMinimalViewInfo InPOV);
	virtual void UpdateBlendWeight(float DeltaTime);

protected:

	 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Position")
	FMPCameraPositionParams PositionParams;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="过渡引入参数", Tooltip="相机过渡引入参数"), Category="Transition")
	FMPTransitionParams TransInParams;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="过渡退出参数", Tooltip="相机过渡退出参数"), Category="Transition")
	FMPTransitionParams TransOutParams;
	FMPTransitionParams CurrentTransParams;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Transition", meta=(DisplayName="锁定引入", Tooltip="勾选此项，会使混入过程中其他相机处于停滞状态，适用于锁住混入的相机点"))
	uint32 bLockComeIn : 1;

	// Penetration prevention
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collision")
	float PenetrationBlendInTime = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collision")
	float PenetrationBlendOutTime = 0.15f;

	/** If true, does collision checks to keep the camera out of the world. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collision")
	bool bPreventPenetration = true;

	/** If true, try to detect nearby walls and move the camera in anticipation.  Helps prevent popping. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collision")
	bool bDoPredictiveAvoidance = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	float CollisionPushOutDistance = 2.f;

	/** When the camera's distance is pushed into this percentage of its full distance due to penetration */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	float ReportPenetrationPercent = 0.f;

	/**
	 * These are the feeler rays that are used to find where to place the camera.
	 * Index: 0  : This is the normal feeler we use to prevent collisions.
	 * Index: 1+ : These feelers are used if you bDoPredictiveAvoidance=true, to scan for potential impacts if the player
	 *             were to rotate towards that direction and primitively collide the camera so that it pulls in before
	 *             impacting the occluder.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Collision")
	TArray<FMPPenetrationAvoidanceRay> PenetrationAvoidanceRays;

	UPROPERTY(Transient)
	float AimLineToDesiredPosBlockedPct;

	UPROPERTY(Transient)
	TArray<TObjectPtr<const AActor>> DebugActorsHitDuringCameraPenetration;


	
	/** When true, any blending of the block percentage when calculating penetration avoidance is skipped */
	bool bSkipNextPredictivePenetrationAvoidanceBlend = false;

	bool bIsActive = false;
	bool bIsBlendIn = false;
	float BlendWeight = 0.f;
	float BlendAlpha = 0.f;

	UPROPERTY(transient)
	TObjectPtr<AActor> ViewTarget = nullptr;

	// Some internal variables used for Position
	UPROPERTY(BlueprintReadOnly)
	FRotator LastControlRotation = FRotator::ZeroRotator;
	UPROPERTY(BlueprintReadOnly)
	FTransform TargetToWorld = FTransform::Identity;
	UPROPERTY(BlueprintReadOnly)
	FTransform PivotToWorld = FTransform::Identity;
	UPROPERTY(BlueprintReadOnly)
	FTransform LastPivotToWorld = FTransform::Identity;
	UPROPERTY(BlueprintReadWrite)
	FTransform CameraToWorld = FTransform::Identity;
	UPROPERTY(BlueprintReadOnly)
	FTransform LookAtToWorld = FTransform::Identity;

	// Last FMinimalViewInfo
	UPROPERTY(Transient, BlueprintReadOnly)
	FMinimalViewInfo LastPOV;
	UPROPERTY(Transient, BlueprintReadOnly)
	FMinimalViewInfo CurrentPOV;


	/** If true, skips all interpolation and puts camera in ideal location.  Automatically set to false next frame. */
	UPROPERTY(transient)
	uint32 bResetInterpolation:1;
	
};
