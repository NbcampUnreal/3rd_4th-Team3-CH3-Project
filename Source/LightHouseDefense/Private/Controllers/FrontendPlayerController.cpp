// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/FrontendPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"

void AFrontendPlayerController::OnPossess(APawn* aPawn)
{
    Super::OnPossess(aPawn); // 부모 클래스의 OnPossess 로직 실행 (기본 처리 유지)

    TArray<AActor*> FoundCameras; // 검색된 카메라 액터들을 담을 배열

    // "Default" 태그가 붙은 모든 ACameraActor를 검색하여 FoundCameras 배열에 저장
    UGameplayStatics::GetAllActorsOfClassWithTag(
        this,                               // 월드 컨텍스트 객체
        ACameraActor::StaticClass(),        // 검색할 클래스 타입 (카메라)
        FName("Default"),                   // 검색할 태그
        FoundCameras                        // 결과를 저장할 배열
    );

    // 검색된 카메라가 있으면 해당 카메라를 뷰 타겟으로 설정
    if (!FoundCameras.IsEmpty())
    {
        SetViewTarget(FoundCameras[0]); // 첫 번째로 찾은 카메라로 시점 변경
    }
}

