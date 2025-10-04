// Fill out your copyright notice in the Description page of Project Settings.


#include "Mode/MPCameraModeBase.h"
#include "GameFramework/Character.h"
#include "Component/MPCameraComponent.h"
#include "GameFramework/CameraBlockingVolume.h"

UMPCameraModeBase::UMPCameraModeBase()
{
	
	PenetrationAvoidanceRays.Add(FMPPenetrationAvoidanceRay(FRotator(+00.0f, +00.0f, 0.0f), 1.00f, 1.00f, 14.f, 0));
	PenetrationAvoidanceRays.Add(FMPPenetrationAvoidanceRay(FRotator(+00.0f, +16.0f, 0.0f), 0.75f, 0.75f, 00.f, 3));
	PenetrationAvoidanceRays.Add(FMPPenetrationAvoidanceRay(FRotator(+00.0f, -16.0f, 0.0f), 0.75f, 0.75f, 00.f, 3));
	PenetrationAvoidanceRays.Add(FMPPenetrationAvoidanceRay(FRotator(+00.0f, +32.0f, 0.0f), 0.50f, 0.50f, 00.f, 5));
	PenetrationAvoidanceRays.Add(FMPPenetrationAvoidanceRay(FRotator(+00.0f, -32.0f, 0.0f), 0.50f, 0.50f, 00.f, 5));
	PenetrationAvoidanceRays.Add(FMPPenetrationAvoidanceRay(FRotator(+20.0f, +00.0f, 0.0f), 1.00f, 1.00f, 00.f, 4));
	PenetrationAvoidanceRays.Add(FMPPenetrationAvoidanceRay(FRotator(-20.0f, +00.0f, 0.0f), 0.50f, 0.50f, 00.f, 4));
}

bool UMPCameraModeBase::ShouldLockOutgoingPOV() const
{
    return CurrentTransParams.bLockOutgoing;
}

void UMPCameraModeBase::SetBlendWeight(float InWeight)
{
	BlendWeight = FMath::Clamp(InWeight, 0.f, 1.f);

	BlendAlpha = CurrentTransParams.GetBlendAlpha(BlendWeight);
}

void UMPCameraModeBase::OnRemovedFromStack()
{
	
}

void UMPCameraModeBase::OnBecomeActive()
{
	
}

void UMPCameraModeBase::OnDeactivated()
{
	
}

void UMPCameraModeBase::PreUpdateCamera(float DeltaTime)
{
	LastPOV = CurrentPOV;
	LastPivotToWorld = PivotToWorld;
	TargetToWorld = ViewTarget->GetActorTransform();
}

void UMPCameraModeBase::UpdatePivotToWorld()
{
	ACharacter* Character = Cast<ACharacter>(ViewTarget);
	APlayerController* PC = Cast<APlayerController>(Character->GetController());
	
    PivotToWorld = PositionParams.PivotToViewTarget * TargetToWorld;
	PivotToWorld.SetRotation(PC->GetControlRotation().Quaternion());
}

void UMPCameraModeBase::UpdateLookAtToWorld()
{
	const FVector LookAtOffset = PositionParams.LookAtOffsetToPivot;

	FTransform LookAtToPivot = FTransform::Identity;
	LookAtToPivot.SetTranslation(LookAtOffset);

	LookAtToWorld = LookAtToPivot * PivotToWorld;
}

void UMPCameraModeBase::UpdateCameraToWorld()
{
	FTransform TempCameraToPivot = FTransform::Identity;
	TempCameraToPivot.SetLocation(PositionParams.CameraToPivot.GetLocation());

	CameraToWorld = TempCameraToPivot * PivotToWorld;

	// CameraRotation
	const FVector CameraLookAtPos = LookAtToWorld.GetLocation();
	const FVector DesiredLookAtDir = (CameraLookAtPos - CameraToWorld.GetLocation()).GetSafeNormal();
	const FQuat CameraRot = FRotationMatrix::MakeFromXZ(DesiredLookAtDir, FVector::UpVector).Rotator().Quaternion();
	CameraToWorld.SetRotation(CameraRot);
}

void UMPCameraModeBase::UpdatePreventPenetration(float DeltaTime)
{
	if (!bPreventPenetration) return;
	ViewTarget = GetAttachedActor();

	const UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(ViewTarget->GetRootComponent());
    if (!PrimitiveComp) return;

	if (PenetrationAvoidanceRays.Num() == 0) return;

	FVector ClosePointOnLineToCapsuleCenter = FVector::ZeroVector;
	FVector SafeLocation = ViewTarget->GetActorLocation();
	FMath::PointDistToLine(SafeLocation, CameraToWorld.GetRotation().Vector(), CameraToWorld.GetLocation(), ClosePointOnLineToCapsuleCenter);

	const float PushInDistance = PenetrationAvoidanceRays[0].Extent + CollisionPushOutDistance;
    const float MaxHalfHeight = ViewTarget->GetSimpleCollisionHalfHeight() - PushInDistance;
	SafeLocation.Z = FMath::Clamp(ClosePointOnLineToCapsuleCenter.Z, SafeLocation.Z - MaxHalfHeight, SafeLocation.Z + MaxHalfHeight);

	float DistanceSqr = 0;
	PrimitiveComp->GetSquaredDistanceToCollision(ClosePointOnLineToCapsuleCenter,  DistanceSqr, SafeLocation);
	SafeLocation += (SafeLocation - ClosePointOnLineToCapsuleCenter).GetSafeNormal() * PushInDistance;

	const bool bSingleRayPenetration = !bDoPredictiveAvoidance;

	FVector CameraLoc = CameraToWorld.GetLocation();
    PreventCameraPenetration(ViewTarget, SafeLocation, CameraLoc, DeltaTime, DistanceSqr, bSingleRayPenetration);
	CameraToWorld.SetLocation(CameraLoc);
}

void UMPCameraModeBase::UpdatePOV(FMinimalViewInfo InPOV)
{
	CurrentPOV = InPOV;
	CurrentPOV.Rotation = CameraToWorld.GetRotation().Rotator();
	CurrentPOV.Location = CameraToWorld.GetLocation();
}

void UMPCameraModeBase::UpdateBlendWeight(float DeltaTime)
{
	if (GetBlendTime() > 0.f)
	{
        BlendAlpha += DeltaTime / GetBlendTime();
		BlendAlpha = FMath::Clamp(BlendAlpha, 0.f, 1.f);
	}
	else
	{
		BlendAlpha = 1.f;
	}

	BlendWeight = CurrentTransParams.GetBlendAlpha(BlendAlpha);
	
}

void UMPCameraModeBase::PostUpdateCamera(float DeltaTime)
{
	const APlayerController* PC = Cast<APlayerController>(ViewTarget->GetInstigatorController());
	LastControlRotation = PC->GetControlRotation();
}


void UMPCameraModeBase::UpdateCamera(float DeltaTime, struct FMinimalViewInfo InPOV)
{
    if ((ViewTarget = GetAttachedActor()) == nullptr) return;
	
	PreUpdateCamera(DeltaTime);
	UpdatePivotToWorld();
	UpdateLookAtToWorld();
	UpdateCameraToWorld();
	UpdatePreventPenetration(DeltaTime);
	UpdatePOV(InPOV);
	PostUpdateCamera(DeltaTime);

	UpdateBlendWeight(DeltaTime);
}


UMPCameraComponent* UMPCameraModeBase::GetCameraComponent() const
{
	return CastChecked<UMPCameraComponent>(GetOuter()->GetOuter());
}

AActor* UMPCameraModeBase::GetAttachedActor() const
{
	return GetCameraComponent()->GetAttachedActor();
}

void UMPCameraModeBase::PreventCameraPenetration(class AActor* NowTarget, FVector const& SafeLoc, FVector& CameraLoc, float const& DeltaTime, float& DistBlockedPct, bool bSingleRayOnly)
{
	float HardBlockPct = DistBlockedPct;
	float SoftBlockPct = DistBlockedPct;

	FVector BaseRay = CameraLoc - SafeLoc;
	FRotationMatrix BaseRayMatrix(BaseRay.Rotation());
	FVector BaseRayLocalUp, BaseRayLocalRight, BaseRayLocalForward;

    BaseRayMatrix.GetScaledAxes(BaseRayLocalForward, BaseRayLocalRight, BaseRayLocalUp);

	float DistBlockedPctThisFrame = 1.f;

    int32 const NumRaysToShoot = bSingleRayOnly ? FMath::Min(1, PenetrationAvoidanceRays.Num()) : PenetrationAvoidanceRays.Num();
	FCollisionQueryParams SphereParams(SCENE_QUERY_STAT(CameraPen), false, nullptr/*PlayerCamera*/);
	SphereParams.AddIgnoredActor(NowTarget);

	FCollisionShape SphereShape = FCollisionShape::MakeSphere(0.f);
	UWorld* World = NowTarget->GetWorld();
	if (!World) return;

	for (int32 RayIdx = 0; RayIdx < NumRaysToShoot; ++RayIdx)
	{
		FMPPenetrationAvoidanceRay& Ray = PenetrationAvoidanceRays[RayIdx];

		if (Ray.FramesUntilNextTrace <= 0)
		{
			FVector RayTarget;
			{
				FVector RotatedRay = BaseRay.RotateAngleAxis(Ray.AdjustRotator.Yaw, BaseRayLocalUp);
				RotatedRay = RotatedRay.RotateAngleAxis(Ray.AdjustRotator.Pitch, BaseRayLocalRight);
				RayTarget = SafeLoc + RotatedRay;
			}

			SphereShape.Sphere.Radius = Ray.Extent;
			ECollisionChannel TraceChannel = ECC_Camera;

			FHitResult OutResult;
			const bool bHit = World->SweepSingleByChannel(OutResult, SafeLoc, RayTarget, FQuat::Identity, TraceChannel, SphereShape, SphereParams);

			Ray.FramesUntilNextTrace = Ray.TraceInterval;

			const AActor* HitActor = bHit ? OutResult.GetActor() : nullptr;

			if (bHit && HitActor)
			{
				bool bIgnoreHit = false;

				if (HitActor->ActorHasTag(TEXT("IgnoreCameraPenetration")))
				{
					bIgnoreHit = true;
					SphereParams.AddIgnoredActor(HitActor);
				}

				if (!bIgnoreHit && HitActor->IsA(ACameraBlockingVolume::StaticClass()))
				{
					const FVector ViewTargetForwardXY = NowTarget->GetActorForwardVector().GetSafeNormal2D();
                    const FVector ViewTargetLocation = NowTarget->GetActorLocation();
                    const FVector HitOffset = OutResult.Location - ViewTargetLocation;
					const FVector HitDirectionXY = HitOffset.GetSafeNormal2D();
					const float DotHitDirection = FVector::DotProduct(ViewTargetForwardXY, HitDirectionXY);
                    if (DotHitDirection < 0.0f)
                    {
                        bIgnoreHit = true;
                        SphereParams.AddIgnoredActor(HitActor);
                    }
				}

				if (!bIgnoreHit)
				{
                    float const Weight = Cast<APawn>(OutResult.GetActor()) ? Ray.PawnWeight : Ray.WorldWeight;
                    float NewBlockPct = OutResult.Time;
                    NewBlockPct += (1.f - NewBlockPct) * (1.f - Weight);

                    // Recompute blocked pct taking into account pushout distance.
                    NewBlockPct = ((OutResult.Location - SafeLoc).Size() - CollisionPushOutDistance) / (RayTarget - SafeLoc).Size();
					DistBlockedPctThisFrame = FMath::Min(NewBlockPct, DistBlockedPctThisFrame);

					// This feeler got a hit, so do another trace next frame
                    Ray.FramesUntilNextTrace = 0;
				}
			}

			if (RayIdx == 0)
			{
				HardBlockPct = DistBlockedPctThisFrame;
			}
			else
			{
				SoftBlockPct = DistBlockedPctThisFrame;
			}
			
		}
		else
		{
			--Ray.FramesUntilNextTrace;
		}
	}

	if (bResetInterpolation)
	{
		DistBlockedPct = DistBlockedPctThisFrame;
	}
	else if (DistBlockedPct < DistBlockedPctThisFrame)
	{
		if (PenetrationBlendOutTime > DeltaTime)
		{
			DistBlockedPct = DistBlockedPct + DeltaTime / PenetrationBlendOutTime * (DistBlockedPctThisFrame - DistBlockedPct);
		}
		else
		{
			DistBlockedPct = DistBlockedPctThisFrame;
		}
	}
	else
	{
		if (DistBlockedPct > HardBlockPct)
		{
			DistBlockedPct = HardBlockPct;
		}
		else if (DistBlockedPct > SoftBlockPct)
		{
			// interpolate smoothly in
			if (PenetrationBlendInTime > DeltaTime)
			{
				DistBlockedPct = DistBlockedPct - DeltaTime / PenetrationBlendInTime * (DistBlockedPct - SoftBlockPct);
			}
			else
			{
				DistBlockedPct = SoftBlockPct;
			}
		}
	}

	DistBlockedPct = FMath::Clamp<float>(DistBlockedPct, 0.f, 1.f);
	if (DistBlockedPct < (1.f - ZERO_ANIMWEIGHT_THRESH))
	{
		CameraLoc = SafeLoc + (CameraLoc - SafeLoc) * DistBlockedPct;
	}
}
