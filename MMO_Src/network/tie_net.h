#pragma once

#include "net_common.h"
#include "net_tsqueue.h"
#include "net_message.h"
#include "net_client.h"
#include "net_server.h"
#include "net_connection.h"

#define ML_NAME 20
#define ML_CHAT 100

namespace tie
{
namespace def
{	
	const int ml_name = ML_NAME;
	const int ml_chat = ML_CHAT;
}
}

enum class GameMsg : uint32_t
{
	Server_GetStatus,
	Server_GetPing,
	Server_RemoveClient,
	Server_RemoveHost,
	Server_RPS_TimeUp,
	Server_RPS_DoneUpdate,
	Server_RPS_IDPlayer_Lose,
	Server_RPS_IDPlayer_Win,
	Server_RPS_AssignOrder,

	Client_Accepted,
	Client_AssignID,
	Client_RegisterWithServer,
	Client_UnregisterWithServer,

	Game_AddPlayer,
	Game_RemovePlayer,
	Game_UpdatePlayer,
	Game_MesStr,

	//Player Required
	PR_SendChat,
	PR_SendChatTyping,
	PR_RPSGame_Choose,

	//Host Required
	RPSGame
};

struct sPlayerDescription
{
	uint32_t nUniqueID = 0;
	char name[ML_NAME];
	bool isHost = false;
	bool isReady = false;
	bool isThisTurn = false;
	bool isFuckUp = false;
	bool isWinner = false;
	int countPress = 0;
	bool isUpdateWithEveryOne = false;
};

struct sPlayerText
{
	uint32_t nUniqueID = 0;
	char chat[100];
	float timerChat = 0.0f;
};

struct sRPSGame
{
	enum class Options : uint8_t
	{
		Rock,
		Paper,
		Scissor,
		GiveUp
	};

	uint32_t owner_id = -1u;
	Options option = Options::GiveUp;
	int order = -1;
	int cur_order = -1;
	bool isAbleStart = false;
	bool isAnyOneNotReady = false;
	float countdown = 0.0f;
	bool timeup = false;
};

