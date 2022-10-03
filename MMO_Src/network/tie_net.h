#pragma once

#include "net_common.h"
#include "net_tsqueue.h"
#include "net_message.h"
#include "net_client.h"
#include "net_server.h"
#include "net_connection.h"

enum class GameMsg : uint32_t
{
	Server_GetStatus,
	Server_GetPing,

	Client_Accepted,
	Client_AssignID,
	Client_RegisterWithServer,
	Client_UnregisterWithServer,

	Game_AddPlayer,
	Game_RemovePlayer,
	Game_UpdatePlayer,
	Game_MesStr
};

struct sPlayerDescription
{
	enum class StateRPS
	{
		Rock,
		Paper,
		Scissor,
		GiveUp
	};

	uint32_t nUniqueID = 0;
	char name[100];
	char chat[100];
	StateRPS sRPS = StateRPS::GiveUp;
	bool isThisTurn = false;
	bool isFuckUp = false;
	bool isWinner = false;
	int countPress = 0;
	bool isUpdateWithEveryOne = false;
};