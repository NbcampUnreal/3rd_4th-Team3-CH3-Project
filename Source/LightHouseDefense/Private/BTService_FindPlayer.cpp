// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_FindPlayer.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

UBTService_FindPlayer::UBTService_FindPlayer()
{
    NodeName = TEXT("Find Player");
    Interval = 1.0f; //1초마다 플레이어를 찾도록 설정
    RandomDeviation = 0.1f; //랜덤 편차를 0.1초로 설정
}

void UBTService_FindPlayer::TickNode(UBehaviorTreeComponent&OwnerComp, uint8*NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (BlackboardComp == nullptr)
    {
        return;
    }
    //플레이어 캐릭터 찾기
    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

    if (PlayerCharacter)
    {
        //플레이어를 찾으면 블랙보드에 설정
        BlackboardComp->SetValueAsObject(TargetActorKey.SelectedKeyName, PlayerCharacter);
    }
    else
    {
        //플레이어를 찾지 못하면 블랙보드에서 키를 제거
        BlackboardComp->ClearValue(TargetActorKey.SelectedKeyName);
    }
}
