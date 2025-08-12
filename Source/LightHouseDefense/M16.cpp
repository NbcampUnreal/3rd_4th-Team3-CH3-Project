// Fill out your copyright notice in the Description page of Project Settings.


#include "M16.h"

AM16::AM16()
{
    // 기본 총기 특성 설정
    Damage = 20.0f;  // M16은 AK47보다 적은 피해
    Range = 1000.0f;  // M16의 발사 거리
    FireRate = 0.2f;  // M16은 AK47보다 느린 발사 속도
}

void AM16::BeginPlay()
{
    Super::BeginPlay();
}

