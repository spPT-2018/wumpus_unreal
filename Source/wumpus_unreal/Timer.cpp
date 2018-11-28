// Fill out your copyright notice in the Description page of Project Settings.

#include "Timer.h"

long nanoTime() {
	std::chrono::time_point<std::chrono::system_clock> now =
		std::chrono::system_clock::now();

	return std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
}

Timer::Timer() {
}

double Timer::check_time() {
	return (nanoTime() - this->start);
}

void Timer::play() {
	this->start = nanoTime();
}
