// Fill out your copyright notice in the Description page of Project Settings.


#include "Base/MPCameraActor.h"
#include "Component/MPCameraComponent.h"
#include "Kismet/GameplayStatics.h"

AMPCameraActor::AMPCameraActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UMPCameraComponent>(TEXT("CameraComponent")))
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMPCameraActor::BeginPlay()
{
	Super::BeginPlay();
	
	// 获取替换后的摄像机组件
	MPCameraComponent = Cast<UMPCameraComponent>(GetCameraComponent());
	NextTickInitialize();
}

void AMPCameraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMPCameraActor::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult)
{
	if (AttachedActor && MPCameraComponent)
	{
        MPCameraComponent->GetCameraView(DeltaTime, OutResult);
		SetActorLocationAndRotation(OutResult.Location, OutResult.Rotation);
	}
	else
	{
		Super::CalcCamera(DeltaTime, OutResult);
	}
	
}

void AMPCameraActor::SetCameraMode(TSubclassOf<UMPCameraModeBase> InCameraModeClass)
{
	MPCameraComponent->SetCameraMode(InCameraModeClass);
}

void AMPCameraActor::NextTickInitialize()
{
	APlayerController* TempPC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!TempPC || !TempPC->GetPawn())
    {
        GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AMPCameraActor::NextTickInitialize);
        return;
    }
    AttachedActor = TempPC->GetPawn();
}