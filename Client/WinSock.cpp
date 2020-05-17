#include "WinSock.h"

enum class Client_Message : uint8
{
	Join,		// tell server we're new here
	Leave,		// tell server we're leaving
	StartGame, 	// tell server to start game
	SellProperty,
	BuyProperty,
	BuildHouse,
	NoMoneyDone,
	GiveUp,
	EndTurn 		// tell server our user input
};

enum class Server_Message : uint8
{
	Join_Result,// tell client they're accepted/rejected
	PlayerList, // tell client list of players in server
	StartGame,
	SoldProperty,
	BoughtProperty,
	BuiltHouse,
	PaidMoney,
	ReceivedMoney,
	RentTransfer,
	BuyPropertyOption,
	BuyHouseOption,
	PlayerTurn,
	DiceRoll,
	LandedOn,
	ChanceCard,
	CommunityChestCard,
	NoMoney,
	PlayerBankrupt,
	PlayerLeft,
	Winner
};


struct Timing_Info
{
	LARGE_INTEGER clock_frequency;
	bool32 sleep_granularity_was_set;
};


// todo(jbr) logging system
static void log_warning(const char* fmt, int arg)
{
	char buffer[256];
	sprintf_s(buffer, sizeof(buffer), fmt, arg);
	OutputDebugStringA(buffer);
}

static float32 time_since(LARGE_INTEGER t, LARGE_INTEGER frequency)
{
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);

	return (float32)(now.QuadPart - t.QuadPart) / (float32)frequency.QuadPart;
}

static Timing_Info timing_info_create()
{
	Timing_Info timing_info = {};

	UINT sleep_granularity_ms = 1;
	timing_info.sleep_granularity_was_set = timeBeginPeriod(sleep_granularity_ms) == TIMERR_NOERROR;

	QueryPerformanceFrequency(&timing_info.clock_frequency);

	return timing_info;
}

static void wait_for_tick_end(LARGE_INTEGER tick_start_time, Timing_Info* timing_info)
{
	float32 time_taken_s = time_since(tick_start_time, timing_info->clock_frequency);

	while (time_taken_s < c_seconds_per_tick)
	{
		if (timing_info->sleep_granularity_was_set)
		{
			DWORD time_to_wait_ms = (DWORD)((c_seconds_per_tick - time_taken_s) * 1000);
			if (time_to_wait_ms > 0)
			{
				Sleep(time_to_wait_ms);
			}
		}

		time_taken_s = time_since(tick_start_time, timing_info->clock_frequency);
	}
}