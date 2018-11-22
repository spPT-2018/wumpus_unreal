// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <functional>
#include <vector>

class WUMPUS_UNREAL_API WumpusVoidEvent
{
public:
	void add_handler(std::function<void(void)> handler) {
		handlers.push_back(handler);
	}
	void invoke() {
		for (auto h : handlers) {
			h();
		}
	}

private:
	std::vector<std::function<void(void)>> handlers;
};

template <typename T>
/**
 * 
 */
class WUMPUS_UNREAL_API WumpusEvent
{
public:
	void add_handler(std::function<void(T)> handler) {
		handlers.push_back(handler);
	}
	void invoke(T argument) {
		for (auto h : handlers) {
			h(argument);
		}
	}

private:
	std::vector<std::function<void(T)>> handlers;
};
