// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MPCamera.h"
#include "MPCameraDefine.generated.h"

class UMPCameraModeBase;

UENUM(BlueprintType)
enum class EMPCameraAutoFollowMode : uint8
{
    LazyFollow,
    FullFollow
};


UENUM(BlueprintType)
enum class EMPCameraMode : uint8
{
    None,
    FreeLook, 
    FocusOn,
    CombatOrbit, 
    AnimDriven,
    MAX,
};

UENUM(BlueprintType)
enum class EMPCameraBlendFunction : uint8
{
    /** Camera does a simple linear interpolation. */
    Linear,
    /** Camera has a slight ease in and ease out, but amount of ease cannot be tweaked. */
    Cubic,
    /** Camera immediately accelerates, but smoothly decelerates into the target.  Ease amount controlled by BlendExp. */
    EaseIn,
    /** Camera smoothly accelerates, but does not decelerate into the target.  Ease amount controlled by BlendExp. */
    EaseOut,
    /** Camera smoothly accelerates and decelerates.  Ease amount controlled by BlendExp. */
    EaseInOut,
    /** Camera Custom blend */
    Custom,
    
    PreBlended,
    
    MAX,
};

USTRUCT(BlueprintType)
struct FMPCameraPositionParams
{
    GENERATED_BODY()
public:
    // Used to calculate Target position
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TargetSocketNameX = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TargetSocketNameY = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TargetSocketNameZ = NAME_None;

    // Used to calculate Pivot position
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FTransform PivotToViewTarget = FTransform::Identity;

    // Used to calculate Camera position
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FTransform CameraToPivot = FTransform::Identity;

    // Used to calculate LookAt position and Camera rotation
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector LookAtOffsetToPivot = FVector::ZeroVector;
    
    
    FMPCameraPositionParams() {}
    
};

/**
 * Struct defining a feeler ray used for camera penetration avoidance.
 */
USTRUCT(BlueprintType)
struct MPCAMERA_API FMPPenetrationAvoidanceRay
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, Category = "Penetration Avoidance")
    FRotator AdjustRotator;

    /** how much this feeler affects the final position if it hits the world */
    UPROPERTY(EditAnywhere, Category = "Penetration Avoidance")
    float WorldWeight = 0.f;

    /** how much this feeler affects the final position if it hits the pawn */
    float PawnWeight = 0.f;
    
    /** extent to use for collision when tracing this feeler */
    UPROPERTY(EditAnywhere, Category = "Penetration Avoidance")
    float Extent = 0.f;

    /** minimum frame interval between traces with this feeler if nothing was hit last frame */
    UPROPERTY(EditAnywhere, Category = "Penetration Avoidance")
    int32 TraceInterval = 0;

    /** number of frames since this feeler was used */
    UPROPERTY(transient)
    int32 FramesUntilNextTrace = 0;

    FMPPenetrationAvoidanceRay() : AdjustRotator(ForceInit) {}
    FMPPenetrationAvoidanceRay(FRotator InAdjustRot, float InWorldWeight, float InPawnWeight, float InExtent, int32 InTraceInterval)
        : AdjustRotator(InAdjustRot)
        , WorldWeight(InWorldWeight)
        , PawnWeight(InPawnWeight)
        , Extent(InExtent)
        , TraceInterval(InTraceInterval)
    {}
    
};


// Camera transition params
USTRUCT(BlueprintType)
struct MPCAMERA_API FMPTransitionParams
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
    float BlendTime = SMALL_NUMBER;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="Transition Blend Type"), Category = "Transition")
    EMPCameraBlendFunction BlendFunction = EMPCameraBlendFunction::Cubic;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="Transition Custom Curve", EditCondition="BlendFunction==EMPCameraBlendFunction::Custom"), Category = "Transition")
    FRuntimeFloatCurve BlendCustomCurve;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
    float BlendExponent = 2.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ToolTip="Blend weight for the transition"), Category = "Transition")
    float BlendWeight = 1.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
    bool bIgnoreCollision;

    /** 
     * If true, lock outgoing viewtarget to last frame's camera POV for the remainder of the blend.
     * This is useful if you plan to teleport the old viewtarget, but don't want to affect the blend. 
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
    uint32 bLockOutgoing : 1;

    FMPTransitionParams()
        : BlendTime(SMALL_NUMBER)
        , BlendFunction(EMPCameraBlendFunction::Cubic)
        , BlendExponent(2.f)
        , BlendWeight(1.f)
        , bIgnoreCollision(false)
        , bLockOutgoing(false)
    {}

    /** For a given linear blend value (blend percentage), return the final blend alpha with the requested function applied */
    float GetBlendAlpha(const float& TimePct) const
    {
        switch (BlendFunction)
        {
        case EMPCameraBlendFunction::Linear:
            return FMath::Lerp(0.f, 1.f, TimePct);
        case EMPCameraBlendFunction::Cubic:
            return FMath::CubicInterp(0.f, 1.f, 0.f, 1.f, TimePct);
        case EMPCameraBlendFunction::EaseIn:
            return FMath::InterpEaseIn(0.f, 1.f, TimePct, BlendExponent);
        case EMPCameraBlendFunction::EaseOut:
            return FMath::InterpEaseOut(0.f, 1.f, TimePct, BlendExponent);
        case EMPCameraBlendFunction::EaseInOut:
            return FMath::InterpEaseInOut(0.f, 1.f, TimePct, BlendExponent);
        case EMPCameraBlendFunction::Custom:
            return BlendCustomCurve.GetRichCurveConst()->Eval(TimePct);
        default:
            break;
        }
        return 1.f;
    }
    
};
