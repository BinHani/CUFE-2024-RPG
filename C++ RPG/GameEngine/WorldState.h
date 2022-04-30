#pragma once

enum worldState : short{

	stateRealTime,
	stateTurnBased,
	stateGameOver,
};

extern worldState _worldState;
