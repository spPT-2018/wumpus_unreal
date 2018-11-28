// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <chrono>
#include "CoreMinimal.h"

/**
 * 
 */
class WUMPUS_UNREAL_API Timer
{
private:
	long start;
public:
	Timer();
	double check_time();
	void play();
};


