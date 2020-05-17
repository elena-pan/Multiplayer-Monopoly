#include "WinSock.h"
#include "WinSock.cpp"
#include "Net.cpp"

#include "Position.h"
#include "Player.h"
#include "Card.h"
#include "Board.h"
#include <iostream>
#include <string>
#include <random>
#include <time.h>
#include <vector>

bool recvError = false;

int main()
{
	if (!Net::init())
	{
		printf("Net::init failed\n");
		return 0;
	}

	Net::Socket sock;
	if (!Net::socket_create(&sock))
	{
		printf("Socket create failed\n");
		return 0;
	}

	Net::IP_Endpoint local_endpoint = {};
	local_endpoint.address = INADDR_ANY;
	local_endpoint.port = c_port;
	if (!Net::socket_bind(&sock, &local_endpoint))
	{
		printf("Socket bind failed");
		return 0;
	}

	uint8 buffer[c_socket_buffer_size];
	uint8 writeBuffer[c_socket_buffer_size];

	struct clients_sockets
	{
		sockaddr_in	address;
		Net::Socket socket;
		bool        connected;
	};

	clients_sockets client_sockets[c_max_clients];

	for (uint16 i = 0; i < c_max_clients; ++i)
	{
		client_sockets[i].connected = false;
	}

	// Clients can only join before game starts
	uint32 bytes_received, bytes_read, bytes_written;

	uint16 numPlayers = 0;
	std::vector<std::string> names = { "", "", "", "" };

	listen(sock.handle, 4); // Listen for connection requests

	// Set of socket descriptors
	fd_set readfds;
	int activity;
	int max_sd = sock.handle;
	uint8 existingBytes = 0;

    // Accept clients and receive input, wait for game to start
	while (true)
	{
		//Clear the socket set  
		FD_ZERO(&readfds);

		//add listening socket to set  
		FD_SET(sock.handle, &readfds);

		//add client sockets to set  
		for (uint16 i = 0; i < c_max_clients; i++)
		{
			//if valid socket descriptor then add to read list  
			if (client_sockets[i].connected) {
				FD_SET(client_sockets[i].socket.handle, &readfds);

				//highest file descriptor number, need it for the select function  
				if (client_sockets[i].socket.handle > max_sd) {
					max_sd = client_sockets[i].socket.handle;
				}
			}
		}

		
		//wait for an activity on one of the sockets , timeout is NULL, so wait indefinitely  
		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

		if ((activity < 0) && (errno != EINTR))
		{
			printf("select error");
		}
		
		//If something happened on the master socket, then its an incoming connection  
		if (FD_ISSET(sock.handle, &readfds))
		{
			Net::Socket tempSocket;
			sockaddr_in tempAddress;
			if (socket_accept(&sock, &tempSocket, &tempAddress)) {

				printf("Client_Message::Connect from %u\n", tempAddress);
				uint16 slot = (uint16)-1;
				for (uint16 i = 0; i < c_max_clients; ++i)
				{
					if (!client_sockets[i].connected)
					{
						slot = i;
						break;
					}
				}

				if (slot != (uint16)-1)
				{
					printf("Client will be assigned to slot %hu\n", slot);
					client_sockets[slot].socket = tempSocket;
					client_sockets[slot].address = tempAddress;
					client_sockets[slot].connected = true;

					writeBuffer[0] = 162;
					// Send validation key
					if (!send(client_sockets[slot].socket.handle, (const char*)writeBuffer, 1, 0))
					{
						printf("Send failed: %d\n", WSAGetLastError());
						closesocket(client_sockets[slot].socket.handle);
						client_sockets[slot].connected = false;
						numPlayers--;
					}
				}
				else
				{
					closesocket(tempSocket.handle);
					printf("Could not find a slot for player\n");
				}

				// If max players have joined, start game loop
				if (numPlayers == c_max_clients) {
					goto startGame;
				}
			}
		}

		//else its some IO operation on some other socket 
		for (uint16 i = 0; i < c_max_clients; i++)
		{
			if (client_sockets[i].connected) {

				if (FD_ISSET(client_sockets[i].socket.handle, &readfds))
				{
					Net::socket_receive(&client_sockets[i].socket, buffer + existingBytes, c_socket_buffer_size - existingBytes, &bytes_received);
					if (bytes_received == 0) { // Client connection closed
						printf("Client %d connection closing\n", i);
						closesocket(client_sockets[i].socket.handle);
						client_sockets[i].connected = false;;
						names[i] = "";
						numPlayers--;
					}
					else if (bytes_received == SOCKET_ERROR) {
						int error = WSAGetLastError();
						if (error != WSAEWOULDBLOCK)
						{
							printf("recvfrom returned SOCKET_ERROR, WSAGetLastError() %d\n", error);
							closesocket(client_sockets[i].socket.handle);
							client_sockets[i].connected = false;;
							names[i] = "";
							numPlayers--;
						}
					}
					else {
						if (bytes_received + existingBytes < buffer[0]) { // Full packet not received yet
							existingBytes = existingBytes + buffer[0];
							continue;
						}

						while (bytes_received + existingBytes >= buffer[0]) {
							existingBytes = bytes_received + existingBytes - buffer[0];
							bytes_received = 0;
							switch (buffer[1])
							{
							case (uint8)Client_Message::Join:
							{
								printf("Client_Message::Join from %u\n", client_sockets[i].address);

								// Get player name from packet sent
								bytes_read = 2;
								std::string inputName;
								char inputChar;

								while (bytes_read < buffer[0]) {
									memcpy(&inputChar, &buffer[bytes_read], 1);
									bytes_read += 1;
									inputName = inputName + inputChar;
								}

								writeBuffer[0] = (uint8)Server_Message::Join_Result;
								if (!std::count(names.begin(), names.end(), inputName))
								{
									printf("Sending slot number to client %hu\n", i);
									writeBuffer[1] = 1;
									memcpy(&writeBuffer[2], &i, 2);

									if (Net::socket_send(&client_sockets[i].socket, writeBuffer, 3))
									{
										//time_since_heard_from_clients[slot] = 0.0f;
										names[i] = inputName;
										numPlayers++;
									}
									else
									{
										printf("Disconnecting client - send failed: %d\n", WSAGetLastError());
										closesocket(client_sockets[i].socket.handle);
										client_sockets[i].connected = false;
									}
								}
								else {
									printf("Name is already taken\n");
									writeBuffer[1] = 0;
									writeBuffer[2] = 1;

									if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, 3))
									{
										printf("Disconnecting client - send failed: %d\n", WSAGetLastError());
										closesocket(client_sockets[i].socket.handle);
										client_sockets[i].connected = false;
									}
								}
							}
							break;

							case (uint8)Client_Message::Leave:
							{
								closesocket(client_sockets[i].socket.handle);
								client_sockets[i].connected = false;
								printf("Client_Message::Leave from %hu(%u)\n", i, client_sockets[i].address);
								names[i] = "";
								numPlayers--;
							}
							break;

							// If one of clients presses start game button
							case (uint8)Client_Message::StartGame:
							{
								goto startGame;
							}
							break;
							}

							if (existingBytes > 0) {
								uint8 processedBytes = buffer[0];
								memmove(buffer, buffer + processedBytes, existingBytes);
							}
						}

						// If max players have joined, start game loop
						if (numPlayers == c_max_clients) {
							goto startGame;
						}
					}

					// Send list of players in server
					writeBuffer[0] = (uint8)Server_Message::PlayerList;
					bytes_written = 1;
					// Signify where name ends in string of characters
					std::string breakPoint = "@";
					for (uint16 i = 0; i < c_max_clients; ++i)
					{
						if (client_sockets[i].connected)
						{
							if (names[i] != "") {
								memcpy(&writeBuffer[bytes_written], names[i].c_str(), strlen(names[i].c_str()));
								bytes_written += strlen(names[i].c_str());

								memcpy(&writeBuffer[bytes_written], breakPoint.c_str(), strlen(breakPoint.c_str()));
								bytes_written += strlen(breakPoint.c_str());
							}
						}

					}
					for (uint16 i = 0; i < c_max_clients; ++i)
					{
						if (client_sockets[i].connected && names[i] != "")
						{
							if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
							{
								printf("Send failed: %d\n", WSAGetLastError());
								closesocket(client_sockets[i].socket.handle);
								client_sockets[i].connected = false;
								names[i] = "";
								numPlayers--;
							}
						}
					}
				}
			}
		}
	}

startGame:

	closesocket(sock.handle); // Stop accepting connection requests

	for (uint16 i = 0; i < c_max_clients; i++) { // Disconnect clients who never inputted a name
		if (client_sockets[i].connected) {
			if (names[i] == "") {
				closesocket(client_sockets[i].socket.handle);
				client_sockets[i].connected = false;
			}
		}
	}

	// Set up board and Chance/Community Chest cards
	Board board;

	// Get slot numbers of clients
	std::vector<uint16> slotNums;
	for (uint16 i = 0; i < c_max_clients; i++) {
		if (client_sockets[i].connected) {
			slotNums.push_back(i);
		}
	}

	// Create players and set names and gamepieces
	board.createPlayers(names, slotNums);

	// Tell players that game has started, send slot numbers, names, and gamepieces
	writeBuffer[0] = (int8)Server_Message::StartGame;
	bytes_written = 1;
	std::string breakPoint = "@"; // Signify end of character string

	for (uint16 i = 0; i < c_max_clients; ++i)
	{
		if (client_sockets[i].connected)
		{
			memcpy(&writeBuffer[bytes_written], &i, sizeof(i));
			bytes_written += sizeof(i);

			memcpy(&writeBuffer[bytes_written], board.players[i].getPlayerName().c_str(), strlen(board.players[i].getPlayerName().c_str()));
			bytes_written += strlen(board.players[i].getPlayerName().c_str());

			memcpy(&writeBuffer[bytes_written], breakPoint.c_str(), strlen(breakPoint.c_str()));
			bytes_written += strlen(breakPoint.c_str());

			memcpy(&writeBuffer[bytes_written], board.players[i].getGamePiece().c_str(), strlen(board.players[i].getGamePiece().c_str()));
			bytes_written += strlen(board.players[i].getGamePiece().c_str());

			memcpy(&writeBuffer[bytes_written], breakPoint.c_str(), strlen(breakPoint.c_str()));
			bytes_written += strlen(breakPoint.c_str());
		}
	}
	for (uint16 i = 0; i < c_max_clients; ++i)
	{
		if (client_sockets[i].connected)
		{
			if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
			{
				printf("Send failed: %d\n", WSAGetLastError());
				closesocket(client_sockets[i].socket.handle);
				client_sockets[i].connected = false;
				numPlayers--;
			}
		}
	}

	// Wait 100ms to make sure clients are set up
	Sleep(100);

	uint16 bankrupt = 0;
	uint16 turn = 0;
	uint16 diceRoll;
	uint16 playerAmountPay = 0;
	bool amountPayIsRent = false;
	bool endTurn = true;
	bytes_received = 0;
	existingBytes = 0;

	// Play until there is only one player not bankrupt
	while (bankrupt < numPlayers - 1) {
		
		if (endTurn) {
			// Next player's turn
			if (turn == numPlayers - 1) {
				turn = 0;
			}
			else {
				turn++;
			}
			// Skip if player is bankrupt or empty slot
			while (board.players[turn].getIsBankrupt() == true || client_sockets[turn].connected == false) {
				if (turn == numPlayers - 1) {
					turn = 0;
				}
				else {
					turn++;
				}
			}

			endTurn = false;

			std::string printText = "It is " + board.players[turn].getPlayerName() + "'s turn! (" + std::to_string(turn + 1) + ")\n";
			printf(printText.c_str());

			// Roll dice
			srand(time(NULL)); // initialize random seed
			diceRoll = rand() % 11 + 2;

			printText = "Dice roll: " + std::to_string(diceRoll) + "\n";
			printf(printText.c_str());

			// Tell every player it is a new player's turn and what their dice roll was
			writeBuffer[0] = (uint8)Server_Message::PlayerTurn;
			bytes_written = 1;

			memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
			bytes_written += sizeof(turn);

			memcpy(&writeBuffer[bytes_written], &diceRoll, sizeof(diceRoll));
			bytes_written += sizeof(diceRoll);

			for (uint16 i = 0; i < c_max_clients; ++i)
			{
				if (client_sockets[i].connected)
				{
					if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
					{
						printf("Send failed: %d\n", WSAGetLastError());
					}
				}
			}

			// Move player
			uint16 passedGo = board.players[turn].movePlayer(diceRoll);

		firstLandedOn:

			uint16 currentLocation = board.players[turn].getLocation(); // Get location index
			std::string posType = board.positions[currentLocation].getPosType();

			printText = "Player current location index: " + std::to_string(currentLocation) + "\n";
			printf(printText.c_str());

			// Tell players where they landed
			writeBuffer[0] = (uint8)Server_Message::LandedOn;
			bytes_written = 1;

			memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
			bytes_written += sizeof(turn);

			memcpy(&writeBuffer[bytes_written], &currentLocation, sizeof(currentLocation));
			bytes_written += sizeof(currentLocation);

			memcpy(&writeBuffer[bytes_written], &passedGo, sizeof(passedGo));
			bytes_written += sizeof(passedGo);

			passedGo = 0;

			for (uint16 i = 0; i < c_max_clients; ++i)
			{
				if (client_sockets[i].connected)
				{
					if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
					{
						printf("Send failed: %d\n", WSAGetLastError());
					}
				}
			}

			if (posType == "GO") {
				board.players[turn].receiveMoney(200);
				std::cout << "Pass GO, collected $200." << "\n";

				uint16 amountReceived = 200;
				// Tell players about money received
				writeBuffer[0] = (uint8)Server_Message::ReceivedMoney;
				bytes_written = 1;

				memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
				bytes_written += sizeof(turn);

				memcpy(&writeBuffer[bytes_written], &amountReceived, sizeof(amountReceived));
				bytes_written += sizeof(amountReceived);

				for (uint16 i = 0; i < c_max_clients; ++i)
				{
					if (client_sockets[i].connected)
					{
						if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
						{
							printf("Sendto failed: %d\n", WSAGetLastError());
						}
					}
				}
			}

			else if (posType == "Tax") {
				// Check if player has enough money
				if (board.positions[currentLocation].getTax() > board.players[turn].getBalance()) {
					printf("Not enough money to pay!");

					if (!board.hasProperties(turn)) {
						board.bankrupt(turn);
						bankrupt++;
						endTurn = true;

						// Tell players about player bankrupt
						writeBuffer[0] = (uint8)Server_Message::PlayerBankrupt;
						bytes_written = 1;
						memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
						bytes_written += sizeof(turn);

						for (uint16 i = 0; i < c_max_clients; ++i)
						{
							if (client_sockets[i].connected)
							{
								if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
								{
									printf("Sendto failed: %d\n", WSAGetLastError());
								}
							}
						}

					}

					else {
						playerAmountPay = board.positions[currentLocation].getTax();
						// Tell player they do not have enough money to pay
						writeBuffer[0] = (uint8)Server_Message::NoMoney;
						bytes_written = 1;

						if (!Net::socket_send(&client_sockets[turn].socket, writeBuffer, bytes_written))
						{
							printf("Sendto failed: %d\n", WSAGetLastError());
						}
					}
				}
				else {
					board.players[turn].payMoney(board.positions[currentLocation].getTax());
					std::cout << "Paid $" << board.positions[currentLocation].getTax() << " in tax.\n";

					uint16 moneyPaid = board.positions[currentLocation].getTax();
					// Tell players about money paid
					writeBuffer[0] = (uint8)Server_Message::PaidMoney;
					bytes_written = 1;

					memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
					bytes_written += sizeof(turn);

					memcpy(&writeBuffer[bytes_written], &moneyPaid, sizeof(moneyPaid));
					bytes_written += sizeof(moneyPaid);

					for (uint16 i = 0; i < c_max_clients; ++i)
					{
						if (client_sockets[i].connected)
						{
							if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
							{
								printf("Sendto failed: %d\n", WSAGetLastError());
							}
						}
					}
				}
			}

			else if (posType == "Jail" || posType == "Free Parking") {
				std::cout << "Visiting " << posType << "\n";
			}

			else if (posType == "Go To Jail") {
				std::cout << "GO TO JAIL, pay $50.\n";
				board.players[turn].movePlayer(0, 10);
				currentLocation = 10; // Jail
				std::cout << "Player current location index: " << std::to_string(10) << "\n";

				if (50 > board.players[turn].getBalance()) {
					printf("Not enough money to pay!");

					if (!board.hasProperties(turn)) {
						board.bankrupt(turn);
						bankrupt++;
						endTurn = true;

						// Tell players about player bankrupt
						writeBuffer[0] = (uint8)Server_Message::PlayerBankrupt;
						bytes_written = 1;
						memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
						bytes_written += sizeof(turn);

						for (uint16 i = 0; i < c_max_clients; ++i)
						{
							if (client_sockets[i].connected)
							{
								if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
								{
									printf("Sendto failed: %d\n", WSAGetLastError());
								}
							}
						}

					}

					else {
						playerAmountPay = 50;
						// Tell player they do not have enough money to pay
						writeBuffer[0] = (uint8)Server_Message::NoMoney;
						bytes_written = 1;

						if (!Net::socket_send(&client_sockets[turn].socket, writeBuffer, bytes_written))
						{
							printf("Send failed: %d\n", WSAGetLastError());
						}
					}
				}
				else {
					board.players[turn].payMoney(50);

					uint16 moneyPaid = 50;
					// Tell players about money paid
					writeBuffer[0] = (uint8)Server_Message::PaidMoney;
					bytes_written = 1;

					memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
					bytes_written += sizeof(turn);

					memcpy(&writeBuffer[bytes_written], &moneyPaid, sizeof(moneyPaid));
					bytes_written += sizeof(moneyPaid);

					for (uint16 i = 0; i < c_max_clients; ++i)
					{
						if (client_sockets[i].connected)
						{
							if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
							{
								printf("Send failed: %d\n", WSAGetLastError());
							}
						}
					}
				}
			}

			else if (posType == "Chance") {
				// Draw random card
				srand(time(NULL)); // initialize random seed
				int cardNum = rand() % 11;

				std::cout << board.chanceCards[cardNum].getDescription() << "\n";

				// Tell players about drawn card
				writeBuffer[0] = (int8)Server_Message::ChanceCard;
				int32 bytes_written = 1;

				memcpy(&writeBuffer[bytes_written], &cardNum, sizeof(cardNum));
				bytes_written += sizeof(cardNum);

				for (uint16 i = 0; i < c_max_clients; ++i)
				{
					if (client_sockets[i].connected)
					{
						if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
						{
							printf("Sendto failed: %d\n", WSAGetLastError());
						}
					}
				}

				// Check if lose or gain money
				if (board.chanceCards[cardNum].getAmountMoney() > 1 && board.chanceCards[cardNum].getAmountMoney() != 0) {
					board.players[turn].receiveMoney(board.chanceCards[cardNum].getAmountMoney()); // Transfer money

					int amountReceived = board.chanceCards[cardNum].getAmountMoney();
					// Tell players about money received
					writeBuffer[0] = (int8)Server_Message::ReceivedMoney;
					int32 bytes_written = 1;

					memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
					bytes_written += sizeof(turn);

					memcpy(&writeBuffer[bytes_written], &amountReceived, sizeof(amountReceived));
					bytes_written += sizeof(amountReceived);

					for (uint16 i = 0; i < c_max_clients; ++i)
					{
						if (client_sockets[i].connected)
						{
							if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
							{
								printf("Sendto failed: %d\n", WSAGetLastError());
							}
						}
					}
				}

				// If have to pay, check if player has enough money
				else if (board.chanceCards[cardNum].getAmountMoney() < 1 && board.chanceCards[cardNum].getAmountMoney() != 0) {
					if (abs(board.chanceCards[cardNum].getAmountMoney()) > board.players[turn].getBalance()) {
						printf("Not enough money to pay!");

						if (!board.hasProperties(turn)) {
							board.bankrupt(turn);
							bankrupt++;
							endTurn = true;

							// Tell players about player bankrupt
							writeBuffer[0] = (int8)Server_Message::PlayerBankrupt;
							bytes_written = 1;
							memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
							bytes_written += sizeof(turn);

							for (uint16 i = 0; i < c_max_clients; ++i)
							{
								if (client_sockets[i].connected)
								{
									if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
									{
										printf("Sendto failed: %d\n", WSAGetLastError());
									}
								}
							}

						}

						else {
							playerAmountPay = abs(board.chanceCards[cardNum].getAmountMoney());
							// Tell player they do not have enough money to pay
							writeBuffer[0] = (int8)Server_Message::NoMoney;
							bytes_written = 1;

							if (!Net::socket_send(&client_sockets[turn].socket, writeBuffer, bytes_written))
							{
								printf("Send failed: %d\n", WSAGetLastError());
							}
						}
					}
					else {
						int moneyPaid = abs(board.chanceCards[cardNum].getAmountMoney());
						board.players[turn].payMoney(moneyPaid);

						// Tell players about money received
						writeBuffer[0] = (int8)Server_Message::PaidMoney;
						bytes_written = 1;

						memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
						bytes_written += sizeof(turn);

						memcpy(&writeBuffer[bytes_written], &moneyPaid, sizeof(moneyPaid));
						bytes_written += sizeof(moneyPaid);

						for (uint16 i = 0; i < c_max_clients; ++i)
						{
							if (client_sockets[i].connected)
							{
								if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
								{
									printf("Send failed: %d\n", WSAGetLastError());
								}
							}
						}
					}
				}
				// Move player, if applicable
				if (board.chanceCards[cardNum].getMoveTo() != -1) {
					board.players[turn].movePlayer(0, board.chanceCards[cardNum].getMoveTo());
					goto firstLandedOn;
				}
			}

			else if (posType == "Community Chest") {
				// Draw random card
				srand(time(NULL)); // initialize random seed
				uint16 cardNum = rand() % 15;

				std::cout << board.communityChestCards[cardNum].getDescription() << "\n";

				writeBuffer[0] = (uint8)Server_Message::CommunityChestCard;
				bytes_written = 1;

				memcpy(&writeBuffer[bytes_written], &cardNum, sizeof(cardNum));
				bytes_written += sizeof(cardNum);

				for (uint16 i = 0; i < c_max_clients; ++i)
				{
					if (client_sockets[i].connected)
					{
						if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
						{
							printf("Sendto failed: %d\n", WSAGetLastError());
						}
					}
				}

				// Check if lose or gain money
				if (board.communityChestCards[cardNum].getAmountMoney() > 1 && board.communityChestCards[cardNum].getAmountMoney() != 0) {
					uint16 amountReceived = board.communityChestCards[cardNum].getAmountMoney();
					board.players[turn].receiveMoney(amountReceived); // Transfer money

					// Tell players about money received
					writeBuffer[0] = (uint8)Server_Message::ReceivedMoney;
					bytes_written = 1;

					memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
					bytes_written += sizeof(turn);

					memcpy(&writeBuffer[bytes_written], &amountReceived, sizeof(amountReceived));
					bytes_written += sizeof(amountReceived);

					for (uint16 i = 0; i < c_max_clients; ++i)
					{
						if (client_sockets[i].connected)
						{
							if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
							{
								printf("Send failed: %d\n", WSAGetLastError());
							}
						}
					}
				}

				// If have to pay, check if player has enough money
				else if (board.communityChestCards[cardNum].getAmountMoney() < 1 && board.communityChestCards[cardNum].getAmountMoney() != 0) {
					if (abs(board.communityChestCards[cardNum].getAmountMoney()) > board.players[turn].getBalance()) {
						printf("Not enough money to pay!");

						if (!board.hasProperties(turn)) {
							board.bankrupt(turn);
							bankrupt++;
							endTurn = true;

							// Tell players about player bankrupt
							writeBuffer[0] = (uint8)Server_Message::PlayerBankrupt;
							bytes_written = 1;
							memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
							bytes_written += sizeof(turn);

							for (uint16 i = 0; i < c_max_clients; ++i)
							{
								if (client_sockets[i].connected)
								{
									if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
									{
										printf("Sendto failed: %d\n", WSAGetLastError());
									}
								}
							}

						}

						else {
							playerAmountPay = abs(board.communityChestCards[cardNum].getAmountMoney());
							// Tell player they do not have enough money to pay
							writeBuffer[0] = (uint8)Server_Message::NoMoney;
							bytes_written = 1;

							if (!Net::socket_send(&client_sockets[turn].socket, writeBuffer, bytes_written))
							{
								printf("Sendto failed: %d\n", WSAGetLastError());
							}
						}
					}
					else {
						uint16 moneyPaid = abs(board.communityChestCards[cardNum].getAmountMoney());
						board.players[turn].payMoney(moneyPaid);

						// Tell players about money paid
						writeBuffer[0] = (uint8)Server_Message::PaidMoney;
						bytes_written = 1;

						memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
						bytes_written += sizeof(turn);

						memcpy(&writeBuffer[bytes_written], &moneyPaid, sizeof(moneyPaid));
						bytes_written += sizeof(moneyPaid);

						for (uint16 i = 0; i < c_max_clients; ++i)
						{
							if (client_sockets[i].connected)
							{
								if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
								{
									printf("Sendto failed: %d\n", WSAGetLastError());
								}
							}
						}
					}
				}
				// Move player, if applicable
				if (board.communityChestCards[cardNum].getMoveTo() != -1) {
					board.players[turn].movePlayer(0, board.communityChestCards[cardNum].getMoveTo());
					goto firstLandedOn;
				}
			}

			else if (posType == "Property" || posType == "Railroad" || posType == "Utility") {
				// Unowned
				if (board.positions[currentLocation].getOwner() == -1) {

					uint16 buyPropertyOption = 0;

					// Enough money to buy property
					if (board.positions[currentLocation].getPrice() <= board.players[turn].getBalance()) {
						std::cout << "Buy property option";
						buyPropertyOption = 1;

					}
					else {
						std::cout << "Not enough money to buy this property.\n";
					}

					// Tell player if they have the option to buy property
					writeBuffer[0] = (uint8)Server_Message::BuyPropertyOption;
					bytes_written = 1;

					memcpy(&writeBuffer[bytes_written], &buyPropertyOption, sizeof(buyPropertyOption));
					bytes_written += sizeof(buyPropertyOption);

					if (!Net::socket_send(&client_sockets[turn].socket, writeBuffer, bytes_written))
					{
						printf("Sendto failed: %d\n", WSAGetLastError());
					}
				}
				// Owned
				else {
					// By self
					if (board.positions[currentLocation].getOwner() == turn) {

						uint16 buyHouseOption = 0;

						// If is property
						if (posType == "Property") {

							// Check if max houses have been built
							if (board.positions[currentLocation].getHouses() < 5) {

								// Check if player has enough money to buy a house
								if (board.positions[currentLocation].getHousePrice() <= board.players[turn].getBalance()) {
									std::cout << "Build house option";
									buyHouseOption = 1;
								}
								else {
									std::cout << "Not enough money to build a house.\n";
									buyHouseOption = 2;
								}
							}
							else {
								std::cout << "Already built the maximum number of houses.\n";
								buyHouseOption = 3;
							}
						}
						else {
							std::cout << "Already owned.\n";
						}

						// Tell player if they have the option to buy a house
						writeBuffer[0] = (uint8)Server_Message::BuyHouseOption;
						bytes_written = 1;

						memcpy(&writeBuffer[bytes_written], &buyHouseOption, sizeof(buyHouseOption));
						bytes_written += sizeof(buyHouseOption);

						if (!Net::socket_send(&client_sockets[turn].socket, writeBuffer, bytes_written))
						{
							printf("Sendto failed: %d\n", WSAGetLastError());
						}
					}
					// By someone else
					else {
						if (diceRoll == -1 && posType == "Utility") {
							std::cout << "Rolling dice to calculate rent amount.";
							srand(time(NULL)); // initialize random seed
							diceRoll = rand() % 11 + 2;
							std::cout << "Rolled a " << diceRoll << "\n";

							writeBuffer[0] = (uint8)Server_Message::DiceRoll;
							bytes_written = 1;

							memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
							bytes_written += sizeof(turn);

							memcpy(&writeBuffer[bytes_written], &diceRoll, sizeof(diceRoll));
							bytes_written += sizeof(diceRoll);

							for (uint16 i = 0; i < c_max_clients; ++i)
							{
								if (client_sockets[i].connected)
								{
									if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
									{
										printf("Sendto failed: %d\n", WSAGetLastError());
									}
								}
							}
						}
						uint16 rent = board.calculateRent(currentLocation, diceRoll);
						if (rent > board.players[turn].getBalance()) {
							printf("Not enough money to pay!");

							if (!board.hasProperties(turn)) {
								board.bankrupt(turn);
								bankrupt++;
								endTurn = true;

								// Tell players about player bankrupt
								writeBuffer[0] = (uint8)Server_Message::PlayerBankrupt;
								bytes_written = 1;
								memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
								bytes_written += sizeof(turn);

								for (uint16 i = 0; i < c_max_clients; ++i)
								{
									if (client_sockets[i].connected)
									{
										if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
										{
											printf("Sendto failed: %d\n", WSAGetLastError());
										}
									}
								}

							}

							else {
								playerAmountPay = rent;
								amountPayIsRent = true;
								// Tell player they do not have enough money to pay
								writeBuffer[0] = (uint8)Server_Message::NoMoney;
								bytes_written = 1;

								if (!Net::socket_send(&client_sockets[turn].socket, writeBuffer, bytes_written))
								{
									printf("Sendto failed: %d\n", WSAGetLastError());
								}
							}
						}
						else {
							uint16 propertyOwner = board.positions[currentLocation].getOwner();
							board.payRent(turn, propertyOwner, rent);
							std::cout << "Paid $" << rent << " of rent.\n";

							// Tell players about rent transfer
							writeBuffer[0] = (uint8)Server_Message::RentTransfer;
							bytes_written = 1;

							memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
							bytes_written += sizeof(turn);

							memcpy(&writeBuffer[bytes_written], &propertyOwner, sizeof(propertyOwner));
							bytes_written += sizeof(propertyOwner);

							memcpy(&writeBuffer[bytes_written], &rent, sizeof(rent));
							bytes_written += sizeof(rent);

							for (uint16 i = 0; i < c_max_clients; ++i)
							{
								if (client_sockets[i].connected)
								{
									if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
									{
										printf("Sendto failed: %d\n", WSAGetLastError());
									}
								}
							}
						}
					}
				}
			}
		}
		
		//Clear the socket set  
		FD_ZERO(&readfds);
		max_sd = 0; // Reset value

		//Add client sockets to set  
		for (uint16 i = 0; i < c_max_clients; i++)
		{
			//if valid socket descriptor then add to read list  
			if (client_sockets[i].connected) {
				FD_SET(client_sockets[i].socket.handle, &readfds);

				//highest file descriptor number, need it for the select function  
				if (client_sockets[i].socket.handle > max_sd) {
					max_sd = client_sockets[i].socket.handle;
				}
			}
		}

		//wait for an activity on one of the sockets , timeout is NULL, so wait indefinitely  
		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

		if ((activity < 0) && (errno != EINTR))
		{
			printf("select error");
		}

		//If something happened on client socket 
		for (uint16 slot = 0; slot < c_max_clients; slot++)
		{
			if (client_sockets[slot].connected) {

				if (FD_ISSET(client_sockets[slot].socket.handle, &readfds))
				{
					Net::socket_receive(&client_sockets[slot].socket, buffer + existingBytes, c_socket_buffer_size - existingBytes, &bytes_received);
					if (bytes_received == 0) { // Client connection closed
						printf("Client closed connection from %hu(%u)\n", slot, client_sockets[slot].address);
						closesocket(client_sockets[slot].socket.handle);
						client_sockets[slot].connected = false;

						// Set player as bankrupt
						board.bankrupt(slot);
						bankrupt++;
						if (slot == turn) {
							endTurn = true;
						}

						// Tell players that they have left
						buffer[0] = (uint8)Server_Message::PlayerLeft;
						bytes_written = 1;
						memcpy(&buffer[bytes_written], &slot, sizeof(slot));
						bytes_written += sizeof(slot);

						for (uint16 i = 0; i < c_max_clients; ++i)
						{
							if (client_sockets[i].connected)
							{
								if (!Net::socket_send(&client_sockets[i].socket, buffer, bytes_written))
								{
									printf("Send failed: %d\n", WSAGetLastError());
								}
							}
						}
					}

					else if (bytes_received == SOCKET_ERROR) {
						int error = WSAGetLastError();
						if (error != WSAEWOULDBLOCK)
						{
							printf("recvfrom returned SOCKET_ERROR, WSAGetLastError() %d\n", error);
							closesocket(client_sockets[slot].socket.handle);
							client_sockets[slot].connected = false;

							// Set player as bankrupt
							board.bankrupt(slot);
							bankrupt++;
							if (slot == turn) {
								endTurn = true;
							}

							// Tell players that they have left
							buffer[0] = (uint8)Server_Message::PlayerLeft;
							bytes_written = 1;
							memcpy(&buffer[bytes_written], &slot, sizeof(slot));
							bytes_written += sizeof(slot);

							for (uint16 i = 0; i < c_max_clients; ++i)
							{
								if (client_sockets[i].connected)
								{
									if (!Net::socket_send(&client_sockets[i].socket, buffer, bytes_written))
									{
										printf("Send failed: %d\n", WSAGetLastError());
									}
								}
							}
						}
					}

					else {
						if (bytes_received + existingBytes < buffer[0]) { // Full packet not received yet
							existingBytes = existingBytes + buffer[0];
							continue;
						}

						while (bytes_received + existingBytes >= buffer[0]) {
							existingBytes = bytes_received + existingBytes - buffer[0];
							bytes_received = 0;

							switch (buffer[1])
							{
							case (uint8)Client_Message::Leave:
							{
								printf("Client_Message::Leave from %hu(%u)\n", slot, client_sockets[slot].address);

								// Set player as bankrupt
								client_sockets[slot].connected = false;
								board.bankrupt(slot);
								bankrupt++;
								if (slot == turn) {
									endTurn = true;
								}

								// Tell players that they have left
								writeBuffer[0] = (uint8)Server_Message::PlayerLeft;
								bytes_written = 1;
								memcpy(&writeBuffer[bytes_written], &slot, sizeof(slot));
								bytes_written += sizeof(slot);

								for (uint16 i = 0; i < c_max_clients; ++i)
								{
									if (client_sockets[i].connected)
									{
										if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
										{
											printf("Send failed: %d\n", WSAGetLastError());
										}
									}
								}
							}
							break;

							case (uint8)Client_Message::SellProperty:
							{
								uint16 propertyNum;
								bytes_read = 2;
								memcpy(&propertyNum, &buffer[bytes_read], sizeof(propertyNum));

								uint16 soldFor = board.sellProperty(slot, propertyNum);

								// Tell players about sold property
								writeBuffer[0] = (uint8)Server_Message::SoldProperty;
								bytes_written = 1;

								memcpy(&writeBuffer[bytes_written], &slot, sizeof(slot));
								bytes_written += sizeof(slot);

								memcpy(&writeBuffer[bytes_written], &propertyNum, sizeof(propertyNum));
								bytes_written += sizeof(propertyNum);

								memcpy(&writeBuffer[bytes_written], &soldFor, sizeof(soldFor));
								bytes_written += sizeof(soldFor);

								for (uint16 i = 0; i < c_max_clients; ++i)
								{
									if (client_sockets[i].connected)
									{
										if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
										{
											printf("Sendto failed: %d\n", WSAGetLastError());
										}
									}
								}
							}
							break;

							case (uint8)Client_Message::BuyProperty:
							{

								if (slot == turn && board.positions[board.players[slot].getLocation()].getPrice() <= board.players[slot].getBalance()) {

									uint16 boughtProperty = board.players[slot].getLocation();

									board.buyProperty(slot, boughtProperty);

									// Tell players about bought property
									writeBuffer[0] = (uint8)Server_Message::BoughtProperty;
									bytes_written = 1;

									memcpy(&writeBuffer[bytes_written], &slot, sizeof(slot));
									bytes_written += sizeof(slot);

									memcpy(&writeBuffer[bytes_written], &boughtProperty, sizeof(boughtProperty));
									bytes_written += sizeof(boughtProperty);

									for (uint16 i = 0; i < c_max_clients; ++i)
									{
										if (client_sockets[i].connected)
										{
											if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
											{
												printf("Sendto failed: %d\n", WSAGetLastError());
											}
										}
									}
								}
								else {
									printf("Cannot buy property - not player's turn/not enough money");
								}
							}
							break;

							case (uint8)Client_Message::BuildHouse:
							{

								if (slot == turn && board.positions[board.players[slot].getLocation()].getOwner() == slot && board.positions[board.players[slot].getLocation()].getHouses() < 5 && board.positions[board.players[slot].getLocation()].getHousePrice() <= board.players[slot].getBalance()) {

									uint16 propertyIndex = board.players[slot].getLocation();

									board.buyHouse(slot, propertyIndex);

									//time_since_heard_from_clients[slot] = 0.0f;

									// Tell players about bought property
									writeBuffer[0] = (uint8)Server_Message::BuiltHouse;
									bytes_written = 1;

									memcpy(&writeBuffer[bytes_written], &slot, sizeof(slot));
									bytes_written += sizeof(slot);

									memcpy(&writeBuffer[bytes_written], &propertyIndex, sizeof(propertyIndex));
									bytes_written += sizeof(propertyIndex);

									for (uint16 i = 0; i < c_max_clients; ++i)
									{
										if (client_sockets[i].connected)
										{
											if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
											{
												printf("Sendto failed: %d\n", WSAGetLastError());
											}
										}
									}
								}
								else {
									printf("Cannot build house - not player's turn/not enough money/max houses/not owned by player");
								}
							}
							break;

							case (uint8)Client_Message::NoMoneyDone:
							{
								if (slot == turn) {

									if (playerAmountPay > board.players[turn].getBalance()) {
										printf("Still not enough money to pay!");

										if (!board.hasProperties(turn)) {
											board.bankrupt(turn);
											bankrupt++;
											endTurn = true;

											// Reset amount pay
											playerAmountPay = 0;
											amountPayIsRent = false;

											// Tell players about player bankrupt
											writeBuffer[0] = (uint8)Server_Message::PlayerBankrupt;
											bytes_written = 1;
											memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
											bytes_written += sizeof(turn);

											for (uint16 i = 0; i < c_max_clients; ++i)
											{
												if (client_sockets[i].connected)
												{
													if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
													{
														printf("Sendto failed: %d\n", WSAGetLastError());
													}
												}
											}
										}

										else {

											// Tell player they do not have enough money to pay
											writeBuffer[0] = (uint8)Server_Message::NoMoney;
											bytes_written = 1;

											if (!Net::socket_send(&client_sockets[turn].socket, writeBuffer, bytes_written))
											{
												printf("Sendto failed: %d\n", WSAGetLastError());
											}
										}

									}
									else {
										if (amountPayIsRent) {
											uint16 propertyOwner = board.positions[board.players[turn].getLocation()].getOwner();
											board.payRent(turn, propertyOwner, playerAmountPay);
											amountPayIsRent = false;

											// Tell players about rent transfer
											writeBuffer[0] = (uint8)Server_Message::RentTransfer;
											bytes_written = 1;

											memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
											bytes_written += sizeof(turn);

											memcpy(&writeBuffer[bytes_written], &propertyOwner, sizeof(propertyOwner));
											bytes_written += sizeof(propertyOwner);

											memcpy(&writeBuffer[bytes_written], &playerAmountPay, sizeof(playerAmountPay));
											bytes_written += sizeof(playerAmountPay);

											for (uint16 i = 0; i < c_max_clients; ++i)
											{
												if (client_sockets[i].connected)
												{
													if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
													{
														printf("Sendto failed: %d\n", WSAGetLastError());
													}
												}
											}

										}
										else {
											board.players[turn].payMoney(playerAmountPay);
											std::cout << "Paid $" << std::to_string(playerAmountPay);

											// Tell players about amount paid
											writeBuffer[0] = (uint8)Server_Message::PaidMoney;
											bytes_written = 1;

											memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
											bytes_written += sizeof(turn);

											memcpy(&writeBuffer[bytes_written], &playerAmountPay, sizeof(playerAmountPay));
											bytes_written += sizeof(playerAmountPay);

											for (uint16 i = 0; i < c_max_clients; ++i)
											{
												if (client_sockets[i].connected)
												{
													if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
													{
														printf("Sendto failed: %d\n", WSAGetLastError());
													}
												}
											}
										}
										playerAmountPay = 0;
									}
									//time_since_heard_from_clients[slot] = 0.0f;
								}
								else {
									printf("Not player's turn");
								}
							}
							break;

							case (uint8)Client_Message::GiveUp:
							{
								if (slot == turn) {

									board.bankrupt(slot);
									bankrupt++;
									endTurn = true;

									// Tell players about bankrupt player
									writeBuffer[0] = (uint8)Server_Message::PlayerBankrupt;
									bytes_written = 1;
									memcpy(&writeBuffer[bytes_written], &slot, sizeof(slot));
									bytes_written += sizeof(slot);

									for (uint16 i = 0; i < c_max_clients; ++i)
									{
										if (client_sockets[i].connected)
										{
											if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
											{
												printf("Sendto failed: %d\n", WSAGetLastError());
											}
										}
									}
									//time_since_heard_from_clients[slot] = 0.0f;
								}
								else {
									printf("Not player's turn");
								}
							}
							break;

							case (uint8)Client_Message::EndTurn:
							{
								if (slot == turn) {
									endTurn = true;
									//time_since_heard_from_clients[slot] = 0.0f;
								}
								else {
									printf("Not player's turn");
								}
							}
							break;
							}

							if (endTurn) {
								// Next player's turn
								if (turn == numPlayers - 1) {
									turn = 0;
								}
								else {
									turn++;
								}
								// Skip if player is bankrupt or empty slot
								while (board.players[turn].getIsBankrupt() == true || client_sockets[turn].connected == false) {
									if (turn == numPlayers - 1) {
										turn = 0;
									}
									else {
										turn++;
									}
								}

								endTurn = false;

								std::string printText = "It is " + board.players[turn].getPlayerName() + "'s turn! (" + std::to_string(turn + 1) + ")\n";
								printf(printText.c_str());

								// Roll dice
								srand(time(NULL)); // initialize random seed
								diceRoll = rand() % 11 + 2;

								printText = "Dice roll: " + std::to_string(diceRoll) + "\n";
								printf(printText.c_str());

								// Tell every player it is a new player's turn and what their dice roll was
								writeBuffer[0] = (uint8)Server_Message::PlayerTurn;
								bytes_written = 1;

								memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
								bytes_written += sizeof(turn);

								memcpy(&writeBuffer[bytes_written], &diceRoll, sizeof(diceRoll));
								bytes_written += sizeof(diceRoll);

								for (uint16 i = 0; i < c_max_clients; ++i)
								{
									if (client_sockets[i].connected)
									{
										if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
										{
											printf("Send failed: %d\n", WSAGetLastError());
										}
									}
								}

								// Move player
								uint16 passedGo = board.players[turn].movePlayer(diceRoll);

							landedOn:

								uint16 currentLocation = board.players[turn].getLocation(); // Get location index
								std::string posType = board.positions[currentLocation].getPosType();

								printText = "Player current location index: " + std::to_string(currentLocation) + "\n";
								printf(printText.c_str());

								// Tell players where they landed
								writeBuffer[0] = (uint8)Server_Message::LandedOn;
								bytes_written = 1;

								memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
								bytes_written += sizeof(turn);

								memcpy(&writeBuffer[bytes_written], &currentLocation, sizeof(currentLocation));
								bytes_written += sizeof(currentLocation);

								memcpy(&writeBuffer[bytes_written], &passedGo, sizeof(passedGo));
								bytes_written += sizeof(passedGo);

								for (uint16 i = 0; i < c_max_clients; ++i)
								{
									if (client_sockets[i].connected)
									{
										if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
										{
											printf("Send failed: %d\n", WSAGetLastError());
										}
									}
								}

								passedGo = 0;

								if (posType == "GO") {
									board.players[turn].receiveMoney(200);
									std::cout << "Pass GO, collected $200." << "\n";

									uint16 amountReceived = 200;
									// Tell players about money received
									writeBuffer[0] = (uint8)Server_Message::ReceivedMoney;
									bytes_written = 1;

									memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
									bytes_written += sizeof(turn);

									memcpy(&writeBuffer[bytes_written], &amountReceived, sizeof(amountReceived));
									bytes_written += sizeof(amountReceived);

									for (uint16 i = 0; i < c_max_clients; ++i)
									{
										if (client_sockets[i].connected)
										{
											if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
											{
												printf("Sendto failed: %d\n", WSAGetLastError());
											}
										}
									}
								}

								else if (posType == "Tax") {
									// Check if player has enough money
									if (board.positions[currentLocation].getTax() > board.players[turn].getBalance()) {
										printf("Not enough money to pay!");

										if (!board.hasProperties(turn)) {
											board.bankrupt(turn);
											bankrupt++;
											endTurn = true;

											// Tell players about player bankrupt
											writeBuffer[0] = (uint8)Server_Message::PlayerBankrupt;
											bytes_written = 1;
											memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
											bytes_written += sizeof(turn);

											for (uint16 i = 0; i < c_max_clients; ++i)
											{
												if (client_sockets[i].connected)
												{
													if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
													{
														printf("Sendto failed: %d\n", WSAGetLastError());
													}
												}
											}

										}

										else {
											playerAmountPay = board.positions[currentLocation].getTax();
											// Tell player they do not have enough money to pay
											writeBuffer[0] = (uint8)Server_Message::NoMoney;
											bytes_written = 1;

											if (!Net::socket_send(&client_sockets[turn].socket, writeBuffer, bytes_written))
											{
												printf("Sendto failed: %d\n", WSAGetLastError());
											}
										}
									}
									else {
										board.players[turn].payMoney(board.positions[currentLocation].getTax());
										std::cout << "Paid $" << board.positions[currentLocation].getTax() << " in tax.\n";

										uint16 moneyPaid = board.positions[currentLocation].getTax();
										// Tell players about money paid
										writeBuffer[0] = (uint8)Server_Message::PaidMoney;
										bytes_written = 1;

										memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
										bytes_written += sizeof(turn);

										memcpy(&writeBuffer[bytes_written], &moneyPaid, sizeof(moneyPaid));
										bytes_written += sizeof(moneyPaid);

										for (uint16 i = 0; i < c_max_clients; ++i)
										{
											if (client_sockets[i].connected)
											{
												if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
												{
													printf("Sendto failed: %d\n", WSAGetLastError());
												}
											}
										}
									}
								}

								else if (posType == "Jail" || posType == "Free Parking") {
									std::cout << "Visiting " << posType << "\n";
								}

								else if (posType == "Go To Jail") {
									std::cout << "GO TO JAIL, pay $50.\n";
									board.players[turn].movePlayer(0, 10);
									currentLocation = 10; // Jail
									std::cout << "Player current location index: " << std::to_string(10) << "\n";

									if (50 > board.players[turn].getBalance()) {
										printf("Not enough money to pay!");

										if (!board.hasProperties(turn)) {
											board.bankrupt(turn);
											bankrupt++;
											endTurn = true;

											// Tell players about player bankrupt
											writeBuffer[0] = (uint8)Server_Message::PlayerBankrupt;
											bytes_written = 1;
											memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
											bytes_written += sizeof(turn);

											for (uint16 i = 0; i < c_max_clients; ++i)
											{
												if (client_sockets[i].connected)
												{
													if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
													{
														printf("Sendto failed: %d\n", WSAGetLastError());
													}
												}
											}

										}

										else {
											playerAmountPay = 50;
											// Tell player they do not have enough money to pay
											writeBuffer[0] = (uint8)Server_Message::NoMoney;
											bytes_written = 1;

											if (!Net::socket_send(&client_sockets[turn].socket, writeBuffer, bytes_written))
											{
												printf("Send failed: %d\n", WSAGetLastError());
											}
										}
									}
									else {
										board.players[turn].payMoney(50);

										uint16 moneyPaid = 50;
										// Tell players about money paid
										writeBuffer[0] = (uint8)Server_Message::PaidMoney;
										bytes_written = 1;

										memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
										bytes_written += sizeof(turn);

										memcpy(&writeBuffer[bytes_written], &moneyPaid, sizeof(moneyPaid));
										bytes_written += sizeof(moneyPaid);

										for (uint16 i = 0; i < c_max_clients; ++i)
										{
											if (client_sockets[i].connected)
											{
												if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
												{
													printf("Send failed: %d\n", WSAGetLastError());
												}
											}
										}
									}
								}

								else if (posType == "Chance") {
									// Draw random card
									srand(time(NULL)); // initialize random seed
									int cardNum = rand() % 11;

									std::cout << board.chanceCards[cardNum].getDescription() << "\n";

									// Tell players about drawn card
									writeBuffer[0] = (int8)Server_Message::ChanceCard;
									int32 bytes_written = 1;

									memcpy(&writeBuffer[bytes_written], &cardNum, sizeof(cardNum));
									bytes_written += sizeof(cardNum);

									for (uint16 i = 0; i < c_max_clients; ++i)
									{
										if (client_sockets[i].connected)
										{
											if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
											{
												printf("Sendto failed: %d\n", WSAGetLastError());
											}
										}
									}

									// Check if lose or gain money
									if (board.chanceCards[cardNum].getAmountMoney() > 1 && board.chanceCards[cardNum].getAmountMoney() != 0) {
										board.players[turn].receiveMoney(board.chanceCards[cardNum].getAmountMoney()); // Transfer money

										int amountReceived = board.chanceCards[cardNum].getAmountMoney();
										// Tell players about money received
										writeBuffer[0] = (int8)Server_Message::ReceivedMoney;
										int32 bytes_written = 1;

										memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
										bytes_written += sizeof(turn);

										memcpy(&writeBuffer[bytes_written], &amountReceived, sizeof(amountReceived));
										bytes_written += sizeof(amountReceived);

										for (uint16 i = 0; i < c_max_clients; ++i)
										{
											if (client_sockets[i].connected)
											{
												if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
												{
													printf("Sendto failed: %d\n", WSAGetLastError());
												}
											}
										}
									}

									// If have to pay, check if player has enough money
									else if (board.chanceCards[cardNum].getAmountMoney() < 1 && board.chanceCards[cardNum].getAmountMoney() != 0) {
										if (abs(board.chanceCards[cardNum].getAmountMoney()) > board.players[turn].getBalance()) {
											printf("Not enough money to pay!");

											if (!board.hasProperties(turn)) {
												board.bankrupt(turn);
												bankrupt++;
												endTurn = true;

												// Tell players about player bankrupt
												writeBuffer[0] = (int8)Server_Message::PlayerBankrupt;
												bytes_written = 1;
												memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
												bytes_written += sizeof(turn);

												for (uint16 i = 0; i < c_max_clients; ++i)
												{
													if (client_sockets[i].connected)
													{
														if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
														{
															printf("Sendto failed: %d\n", WSAGetLastError());
														}
													}
												}

											}

											else {
												playerAmountPay = abs(board.chanceCards[cardNum].getAmountMoney());
												// Tell player they do not have enough money to pay
												writeBuffer[0] = (int8)Server_Message::NoMoney;
												bytes_written = 1;

												if (!Net::socket_send(&client_sockets[turn].socket, writeBuffer, bytes_written))
												{
													printf("Send failed: %d\n", WSAGetLastError());
												}
											}
										}
										else {
											int moneyPaid = abs(board.chanceCards[cardNum].getAmountMoney());
											board.players[turn].payMoney(moneyPaid);

											// Tell players about money received
											writeBuffer[0] = (int8)Server_Message::PaidMoney;
											bytes_written = 1;

											memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
											bytes_written += sizeof(turn);

											memcpy(&writeBuffer[bytes_written], &moneyPaid, sizeof(moneyPaid));
											bytes_written += sizeof(moneyPaid);

											for (uint16 i = 0; i < c_max_clients; ++i)
											{
												if (client_sockets[i].connected)
												{
													if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
													{
														printf("Send failed: %d\n", WSAGetLastError());
													}
												}
											}
										}
									}
									// Move player, if applicable
									if (board.chanceCards[cardNum].getMoveTo() != -1) {
										board.players[turn].movePlayer(0, board.chanceCards[cardNum].getMoveTo());
										goto landedOn;
									}
								}

								else if (posType == "Community Chest") {
									// Draw random card
									srand(time(NULL)); // initialize random seed
									uint16 cardNum = rand() % 15;

									std::cout << board.communityChestCards[cardNum].getDescription() << "\n";

									writeBuffer[0] = (uint8)Server_Message::CommunityChestCard;
									bytes_written = 1;

									memcpy(&writeBuffer[bytes_written], &cardNum, sizeof(cardNum));
									bytes_written += sizeof(cardNum);

									for (uint16 i = 0; i < c_max_clients; ++i)
									{
										if (client_sockets[i].connected)
										{
											if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
											{
												printf("Sendto failed: %d\n", WSAGetLastError());
											}
										}
									}

									// Check if lose or gain money
									if (board.communityChestCards[cardNum].getAmountMoney() > 1 && board.communityChestCards[cardNum].getAmountMoney() != 0) {
										uint16 amountReceived = board.communityChestCards[cardNum].getAmountMoney();
										board.players[turn].receiveMoney(amountReceived); // Transfer money

										// Tell players about money received
										writeBuffer[0] = (uint8)Server_Message::ReceivedMoney;
										bytes_written = 1;

										memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
										bytes_written += sizeof(turn);

										memcpy(&writeBuffer[bytes_written], &amountReceived, sizeof(amountReceived));
										bytes_written += sizeof(amountReceived);

										for (uint16 i = 0; i < c_max_clients; ++i)
										{
											if (client_sockets[i].connected)
											{
												if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
												{
													printf("Send failed: %d\n", WSAGetLastError());
												}
											}
										}
									}

									// If have to pay, check if player has enough money
									else if (board.communityChestCards[cardNum].getAmountMoney() < 1 && board.communityChestCards[cardNum].getAmountMoney() != 0) {
										if (abs(board.communityChestCards[cardNum].getAmountMoney()) > board.players[turn].getBalance()) {
											printf("Not enough money to pay!");

											if (!board.hasProperties(turn)) {
												board.bankrupt(turn);
												bankrupt++;
												endTurn = true;

												// Tell players about player bankrupt
												writeBuffer[0] = (uint8)Server_Message::PlayerBankrupt;
												bytes_written = 1;
												memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
												bytes_written += sizeof(turn);

												for (uint16 i = 0; i < c_max_clients; ++i)
												{
													if (client_sockets[i].connected)
													{
														if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
														{
															printf("Sendto failed: %d\n", WSAGetLastError());
														}
													}
												}

											}

											else {
												playerAmountPay = abs(board.communityChestCards[cardNum].getAmountMoney());
												// Tell player they do not have enough money to pay
												writeBuffer[0] = (uint8)Server_Message::NoMoney;
												bytes_written = 1;

												if (!Net::socket_send(&client_sockets[turn].socket, writeBuffer, bytes_written))
												{
													printf("Sendto failed: %d\n", WSAGetLastError());
												}
											}
										}
										else {
											uint16 moneyPaid = abs(board.communityChestCards[cardNum].getAmountMoney());
											board.players[turn].payMoney(moneyPaid);

											// Tell players about money paid
											writeBuffer[0] = (uint8)Server_Message::PaidMoney;
											bytes_written = 1;

											memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
											bytes_written += sizeof(turn);

											memcpy(&writeBuffer[bytes_written], &moneyPaid, sizeof(moneyPaid));
											bytes_written += sizeof(moneyPaid);

											for (uint16 i = 0; i < c_max_clients; ++i)
											{
												if (client_sockets[i].connected)
												{
													if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
													{
														printf("Sendto failed: %d\n", WSAGetLastError());
													}
												}
											}
										}
									}
									// Move player, if applicable
									if (board.communityChestCards[cardNum].getMoveTo() != -1) {
										board.players[turn].movePlayer(0, board.communityChestCards[cardNum].getMoveTo());
										goto landedOn;
									}
								}

								else if (posType == "Property" || posType == "Railroad" || posType == "Utility") {
									// Unowned
									if (board.positions[currentLocation].getOwner() == -1) {

										uint16 buyPropertyOption = 0;

										// Enough money to buy property
										if (board.positions[currentLocation].getPrice() <= board.players[turn].getBalance()) {
											std::cout << "Buy property option";
											buyPropertyOption = 1;

										}
										else {
											std::cout << "Not enough money to buy this property.\n";
										}

										// Tell player if they have the option to buy property
										writeBuffer[0] = (uint8)Server_Message::BuyPropertyOption;
										bytes_written = 1;

										memcpy(&writeBuffer[bytes_written], &buyPropertyOption, sizeof(buyPropertyOption));
										bytes_written += sizeof(buyPropertyOption);

										if (!Net::socket_send(&client_sockets[turn].socket, writeBuffer, bytes_written))
										{
											printf("Sendto failed: %d\n", WSAGetLastError());
										}
									}
									// Owned
									else {
										// By self
										if (board.positions[currentLocation].getOwner() == turn) {

											uint16 buyHouseOption = 0;

											// If is property
											if (posType == "Property") {

												// Check if max houses have been built
												if (board.positions[currentLocation].getHouses() < 5) {

													// Check if player has enough money to buy a house
													if (board.positions[currentLocation].getHousePrice() <= board.players[turn].getBalance()) {
														std::cout << "Build house option";
														buyHouseOption = 1;
													}
													else {
														std::cout << "Not enough money to build a house.\n";
														buyHouseOption = 2;
													}
												}
												else {
													std::cout << "Already built the maximum number of houses.\n";
													buyHouseOption = 3;
												}
											}
											else {
												std::cout << "Already owned.\n";
											}

											// Tell player if they have the option to buy a house
											writeBuffer[0] = (uint8)Server_Message::BuyHouseOption;
											bytes_written = 1;

											memcpy(&writeBuffer[bytes_written], &buyHouseOption, sizeof(buyHouseOption));
											bytes_written += sizeof(buyHouseOption);

											if (!Net::socket_send(&client_sockets[turn].socket, writeBuffer, bytes_written))
											{
												printf("Sendto failed: %d\n", WSAGetLastError());
											}
										}
										// By someone else
										else {
											if (diceRoll == -1 && posType == "Utility") {
												std::cout << "Rolling dice to calculate rent amount.";
												srand(time(NULL)); // initialize random seed
												diceRoll = rand() % 11 + 2;
												std::cout << "Rolled a " << diceRoll << "\n";

												writeBuffer[0] = (uint8)Server_Message::DiceRoll;
												bytes_written = 1;

												memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
												bytes_written += sizeof(turn);

												memcpy(&writeBuffer[bytes_written], &diceRoll, sizeof(diceRoll));
												bytes_written += sizeof(diceRoll);

												for (uint16 i = 0; i < c_max_clients; ++i)
												{
													if (client_sockets[i].connected)
													{
														if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
														{
															printf("Sendto failed: %d\n", WSAGetLastError());
														}
													}
												}
											}
											uint16 rent = board.calculateRent(currentLocation, diceRoll);
											if (rent > board.players[turn].getBalance()) {
												printf("Not enough money to pay!");

												if (!board.hasProperties(turn)) {
													board.bankrupt(turn);
													bankrupt++;
													endTurn = true;

													// Tell players about player bankrupt
													writeBuffer[0] = (uint8)Server_Message::PlayerBankrupt;
													bytes_written = 1;
													memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
													bytes_written += sizeof(turn);

													for (uint16 i = 0; i < c_max_clients; ++i)
													{
														if (client_sockets[i].connected)
														{
															if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
															{
																printf("Sendto failed: %d\n", WSAGetLastError());
															}
														}
													}

												}

												else {
													playerAmountPay = rent;
													amountPayIsRent = true;
													// Tell player they do not have enough money to pay
													writeBuffer[0] = (uint8)Server_Message::NoMoney;
													bytes_written = 1;

													if (!Net::socket_send(&client_sockets[turn].socket, writeBuffer, bytes_written))
													{
														printf("Sendto failed: %d\n", WSAGetLastError());
													}
												}
											}
											else {
												uint16 propertyOwner = board.positions[currentLocation].getOwner();
												board.payRent(turn, propertyOwner, rent);
												std::cout << "Paid $" << rent << " of rent.\n";

												// Tell players about rent transfer
												writeBuffer[0] = (uint8)Server_Message::RentTransfer;
												bytes_written = 1;

												memcpy(&writeBuffer[bytes_written], &turn, sizeof(turn));
												bytes_written += sizeof(turn);

												memcpy(&writeBuffer[bytes_written], &propertyOwner, sizeof(propertyOwner));
												bytes_written += sizeof(propertyOwner);

												memcpy(&writeBuffer[bytes_written], &rent, sizeof(rent));
												bytes_written += sizeof(rent);

												for (uint16 i = 0; i < c_max_clients; ++i)
												{
													if (client_sockets[i].connected)
													{
														if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
														{
															printf("Sendto failed: %d\n", WSAGetLastError());
														}
													}
												}
											}
										}
									}
								}
							}

							if (existingBytes > 0) {
								uint8 processedBytes = buffer[0];
								memmove(buffer, buffer + processedBytes, existingBytes);
							}
						}
					}
				}
			}
		}
	}

	// Determine winner
	uint16 winner = turn;
	for (uint16 i = 0; i < numPlayers; i++) {
		if (board.players[i].getIsBankrupt() == false) {
			winner = i;
			break;
		}
	}

	std::cout << "The winner is " << board.players[winner].getPlayerName() << "! (Player " << winner+1 << ")\n";
	
	// Send winner to all players
	writeBuffer[0] = (uint8)Server_Message::Winner;
	bytes_written = 1;

	memcpy(&writeBuffer[bytes_written], &winner, sizeof(winner));
	bytes_written += sizeof(winner);

	for (uint16 i = 0; i < c_max_clients; ++i)
	{
		if (client_sockets[i].connected)
		{
			if (!Net::socket_send(&client_sockets[i].socket, writeBuffer, bytes_written))
			{
				printf("Sendto failed: %d\n", WSAGetLastError());
			}
		}
	}

	// Close sockets
	for (uint16 i = 0; i < c_max_clients; ++i)
	{
		if (client_sockets[i].connected)
		{
			closesocket(client_sockets[i].socket.handle);
		}
	}

	WSACleanup();
	return 0;
}