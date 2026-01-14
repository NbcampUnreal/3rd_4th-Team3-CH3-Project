// Fill out your copyright notice in the Description page of Project Settings.


#include "AK47.h"

AAK47::AAK47()
{
    WeaponType = E_WeaponType::AK47;

    // 기본 총기 특성 설정
    Damage = 30.0f;  // AK47은 30의 피해
    Range = 1200.0f;  // 더 긴 발사 거리
    FireRate = 0.1f;  // 더 빠른 발사 속도
}

void AAK47::BeginPlay()
{
    Super::BeginPlay();
}
