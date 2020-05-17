#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include "InfoWindow.h"
#include "JoinServerWindow.h"
#include "PlayerChoiceWindow.h"
#include "PasswordWindow.h"

#include "Player.h"
#include "Card.h"
#include "Position.h"

#include "WinSock.h"
#include "WinSock.cpp"
#include "Net.cpp"
#include <vector>
#include <thread>
#include <mutex>

uint16 thisPlayerIndex;
bool startGameClicked, buyPropertyClicked, sellPropertyClicked, buildHouseClicked, noMoneyDone, endTurnClicked, leaveGameClicked = false;
uint16 sellPropertyIndex = 0;

void startGameWinClosed(uint16 result);

Timing_Info timing_info = timing_info_create();

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	
	// init winsock
	if (!Net::init())
	{
		InfoWindow failedWindow("Net::init failed");
		failedWindow.exec();
		return 0;
	}
	Net::Socket c_sock;
	if (!Net::socket_create(&c_sock))
	{
		InfoWindow failedWindow("Create socket failed");
		failedWindow.exec();
		return 0;
	}

	std::string portNum;

	PasswordWindow portWin; // Get client to input port number to connect
	if (portWin.exec()) {
		portNum = portWin.portEdit->text().toStdString();
	}
	else {
		closesocket(c_sock.handle);
		return 0;
	}

	// Check if input is valid
	if (std::stoi(portNum) <= 1024 || std::stoi(portNum) > 65535) { // Check if port number is valid
		InfoWindow failedWindow("Invalid port");
		failedWindow.exec();
		return 0;
	}
	
	struct addrinfo* serveraddr;
	int result = getaddrinfo("0.tcp.ngrok.io", portNum.c_str(), NULL, &serveraddr);
	if (result != 0) {
		std::wstring error = gai_strerror(result);
		InfoWindow failedWindow("Error from getaddrinfo: " + std::to_string(result));
		failedWindow.exec();
		return 1;
	}

	struct sockaddr_in* server_addr = (struct sockaddr_in*) serveraddr->ai_addr;
	//sockaddr_in sockAddr = *server_addr;
	
	//Net::IP_Endpoint server_endpoint = Net::ip_endpoint_create(127, 0, 0, 1, c_port);
	Net::IP_Endpoint server_endpoint = Net::ip_endpoint_create(192, 168, 0, 109, c_port);
	sockaddr_in sockAddr = ip_endpoint_to_sockaddr_in(&server_endpoint);

	if (!Net::socket_connect(&c_sock, sockAddr)) {
		closesocket(c_sock.handle);
		InfoWindow failedWindow("Cannot connect to server: WSAGetLastError() " + std::to_string(WSAGetLastError()));
		failedWindow.exec();
		return 0;
	}

	int rerror, index;

	// Wait for connect
	WSAEVENT socketEvent0 = CreateEvent(NULL, TRUE, FALSE, NULL);
	WSAEventSelect(c_sock.handle, socketEvent0, FD_CONNECT | FD_CLOSE);

	// Timeout of 4 seconds
	rerror = WSAWaitForMultipleEvents(1, &socketEvent0, FALSE, 4000, FALSE);
	if (rerror == WSA_WAIT_TIMEOUT)
	{
		WSACloseEvent(socketEvent0);
		closesocket(c_sock.handle);
		InfoWindow failedWindow("Cannot connect to server");
		failedWindow.exec();
		return 0;
	}

	index = rerror - WSA_WAIT_EVENT_0; // Get the smallest index of a triggered event
	if (rerror != WSA_WAIT_TIMEOUT && rerror != WSA_WAIT_FAILED)
	{
		WSANETWORKEVENTS networkEvents;
		// Returns a list of the events that occurred
		rerror = WSAEnumNetworkEvents(c_sock.handle, socketEvent0, &networkEvents);
		if (rerror == SOCKET_ERROR)
		{
			WSACloseEvent(socketEvent0);
			closesocket(c_sock.handle);
			InfoWindow failedWindow("WSAEnumNetworkEvents Error WSAGetLastError(): " + std::to_string(WSAGetLastError()));
			failedWindow.exec();
			return 0;
		}

		if (networkEvents.lNetworkEvents & FD_CONNECT)
		{
			WSACloseEvent(socketEvent0);
			goto validateServer;
		}

		if (networkEvents.lNetworkEvents & FD_CLOSE)
		{
			closesocket(c_sock.handle);
			WSACloseEvent(socketEvent0);
			InfoWindow failedWindow("Server closed connection");
			failedWindow.exec();
			return 0;
		}
	}

validateServer :

	uint32 bytes_read, bytes_received, bytes_written;
	uint8 buffer[c_socket_buffer_size];
	uint8 writeBuffer[c_socket_buffer_size];

	// Wait for server to send key to validate right connection
	WSAEVENT socketEvent01 = CreateEvent(NULL, TRUE, FALSE, NULL);
	WSAEventSelect(c_sock.handle, socketEvent01, FD_READ | FD_CLOSE);

	// Timeout of 4 seconds
	rerror = WSAWaitForMultipleEvents(1, &socketEvent01, FALSE, 4000, FALSE);
	if (rerror == WSA_WAIT_TIMEOUT)
	{
		WSACloseEvent(socketEvent0);
		closesocket(c_sock.handle);
		InfoWindow failedWindow("Cannot connect to server");
		failedWindow.exec();
		return 0;
	}

	index = rerror - WSA_WAIT_EVENT_0; // Get the smallest index of a triggered event
	if (rerror != WSA_WAIT_TIMEOUT && rerror != WSA_WAIT_FAILED)
	{
		WSANETWORKEVENTS networkEvents;
		// Returns a list of the events that occurred
		rerror = WSAEnumNetworkEvents(c_sock.handle, socketEvent01, &networkEvents);
		if (rerror == SOCKET_ERROR)
		{
			WSACloseEvent(socketEvent01);
			closesocket(c_sock.handle);
			InfoWindow failedWindow("WSAEnumNetworkEvents Error WSAGetLastError(): " + std::to_string(WSAGetLastError()));
			failedWindow.exec();
			return 0;
		}

		if (networkEvents.lNetworkEvents & FD_READ)
		{
			Net::socket_receive(&c_sock, buffer, c_socket_buffer_size, &bytes_received);
			if (bytes_received == 0) { // Server connection closed
				closesocket(c_sock.handle);
				WSACloseEvent(socketEvent01);
				InfoWindow failedWindow("Cannot connect to server");
				failedWindow.exec();
				return 0;
			}
			else if (bytes_received == SOCKET_ERROR) {
				int error = WSAGetLastError();
				closesocket(c_sock.handle);
				WSACloseEvent(socketEvent01);
				InfoWindow failedWindow("recvfrom returned SOCKET_ERROR, WSAGetLastError(): " + std::to_string(error));
				failedWindow.exec();
				return 0;
			}
			else {
				if (buffer[0] == 162) { // Check validation key
					goto joinServer;
				}
				else {
					closesocket(c_sock.handle);
					WSACloseEvent(socketEvent01);
					InfoWindow failedWindow("Cannot connect to server");
					failedWindow.exec();
					return 0;
				}
			}
		}

		if (networkEvents.lNetworkEvents & FD_CLOSE)
		{
			closesocket(c_sock.handle);
			WSACloseEvent(socketEvent01);
			InfoWindow failedWindow("Cannot connect to server");
			failedWindow.exec();
			return 0;
		}
	}


joinServer :

	uint16 slot = 0xFFFF;
	uint16 numPlayers = 0;

	// Get client to input name in join server window
	std::string name;

	JoinServerWindow joinServerWin;
	if (joinServerWin.exec()) {
		name = joinServerWin.nameEdit->text().toStdString();
	}
	else {
		closesocket(c_sock.handle);
		return 0;
	}

	writeBuffer[0] = (uint8)Client_Message::Join;
	bytes_written = 1;

	startGameClicked = false;
	leaveGameClicked = false;

	memcpy(&writeBuffer[bytes_written], name.c_str(), strlen(name.c_str()));
	bytes_written += strlen(name.c_str());

	if (!Net::socket_send(&c_sock, writeBuffer, bytes_written))
	{
		closesocket(c_sock.handle);
		InfoWindow failedWindow("Join message failed to send: " + std::to_string(WSAGetLastError()));
		failedWindow.exec();
		return 0;
	}

	// Wait for game to start
	PlayerChoiceWindow startGameWin("Players in server: ", "Start Game", "Leave");
	startGameWin.open();
	startGameWin.disableStartButton();

	bool startGame = false;

	std::vector<uint16> slotNums;
	std::vector<std::string> names;
	std::vector<std::string> gamepieces;

	QObject::connect(&startGameWin, &QDialog::finished, startGameWinClosed);

	uint16 existingBytes = 0;

	while (!startGame) {

		WSAEVENT socketEvent1 = CreateEvent(NULL, TRUE, FALSE, NULL);
		WSAEventSelect(c_sock.handle, socketEvent1, FD_READ | FD_CLOSE);

		// Wait 10ms for events in the event array ( in this case just 1 event )
		rerror = WSAWaitForMultipleEvents(1, &socketEvent1, FALSE, 10, FALSE);
		if (rerror == WSA_WAIT_TIMEOUT)
		{
			qApp->processEvents();

			if (numPlayers < 2) {
				startGameWin.disableStartButton();
			}
			else {
				startGameWin.enableStartButton();
			}

			if (startGameClicked) {
				writeBuffer[0] = (uint8)Client_Message::StartGame;
				bytes_written = 1;

				if (!Net::socket_send(&c_sock, writeBuffer, bytes_written))
				{
					closesocket(c_sock.handle);
					WSACloseEvent(socketEvent1);
					InfoWindow failedWindow("Message failed to send");
					failedWindow.exec();
					return 0;
				}
			}
			else if (leaveGameClicked) {
				writeBuffer[0] = (uint8)Client_Message::Leave;
				bytes_written = 1;
				Net::socket_send(&c_sock, writeBuffer, bytes_written);
				WSACloseEvent(socketEvent1);
				return 0;
			}

			continue;
		}

		index = rerror - WSA_WAIT_EVENT_0; // Get the smallest index of a triggered event
		if (rerror != WSA_WAIT_TIMEOUT && rerror != WSA_WAIT_FAILED)
		{
			WSANETWORKEVENTS networkEvents;
			// Returns a list of the events that occurred
			rerror = WSAEnumNetworkEvents(c_sock.handle, socketEvent1, &networkEvents);
			if (rerror == SOCKET_ERROR)
			{
				printf("WSAEnumNetworkEvents Error %d\n", WSAGetLastError());
				continue;
			}

			rerror = 0;

			if (networkEvents.lNetworkEvents & FD_READ)
			{
				Net::socket_receive(&c_sock, buffer + existingBytes, c_socket_buffer_size - existingBytes, &bytes_received);
				
				if (bytes_received == 0) { // Server connection closed
					closesocket(c_sock.handle);
					WSACloseEvent(socketEvent1);
					InfoWindow failedWindow("Server closed connection");
					failedWindow.exec();
					return 0;
				}
				else if (bytes_received == SOCKET_ERROR) {
					int error = WSAGetLastError();
					closesocket(c_sock.handle);
					WSACloseEvent(socketEvent1);
					InfoWindow failedWindow("recvfrom returned SOCKET_ERROR, WSAGetLastError(): " + std::to_string(error));
					failedWindow.exec();
					return 0;
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
						case (uint8)Server_Message::Join_Result:
						{
							if (buffer[2])
							{
								slot = buffer[3];
								thisPlayerIndex = slot;
							}
							else
							{
								if (buffer[3]) {
									WSACloseEvent(socketEvent1);
									InfoWindow nameTakenWindow("Name is already taken");
									nameTakenWindow.exec();
									startGameWin.close();
									goto joinServer;
								}
								else {
									closesocket(c_sock.handle);
									WSACloseEvent(socketEvent1);
									InfoWindow failedWindow("Rejected by server");
									failedWindow.exec();
									return 0;
								}
							}
						}
						break;

						case (uint8)Server_Message::StartGame:
						{
							bytes_read = 2;
							char* breakSignal = "@";
							while (bytes_read < buffer[0])
							{
								uint16 slotNum;
								std::string inputName;
								std::string gamepiece;
								char inputChar;

								memcpy(&slotNum, &buffer[bytes_read], sizeof(slotNum));
								bytes_read += sizeof(slotNum);
								slotNums.push_back(slotNum);

								while (true) {
									memcpy(&inputChar, &buffer[bytes_read], 1);
									bytes_read += 1;
									if (inputChar == *breakSignal) {
										names.push_back(inputName);
										break;
									}
									else {
										inputName = inputName + inputChar;
									}
								}

								while (true) {
									memcpy(&inputChar, &buffer[bytes_read], 1);
									bytes_read += 1;
									if (inputChar == *breakSignal) {
										gamepieces.push_back(gamepiece);
										break;
									}
									else {
										gamepiece = gamepiece + inputChar;
									}
								}

							}
							WSACloseEvent(socketEvent1);
							startGame = true;
						}
						break;

						case (uint8)Server_Message::PlayerList:
						{
							bytes_read = 2;
							std::vector<std::string> names;
							char* breakSignal = "@";
							while (bytes_read < buffer[0])
							{
								std::string name;
								char inputChar;

								while (bytes_read <= buffer[0]) {
									memcpy(&inputChar, &buffer[bytes_read], 1);
									bytes_read += 1;
									if (inputChar == *breakSignal) {
										names.push_back(name);
										break;
									}
									else {
										name = name + inputChar;
									}
								}
							}
							std::string nameText = "Players in server:\n";
							for (uint16 i = 0; i < names.size(); i++) {
								nameText = nameText + names[i] + "\n";
							}
							startGameWin.setText(nameText);
							numPlayers = names.size();
						}
						break;
						}

						if (existingBytes > 0) {
							uint16 processedBytes = buffer[0];
							memmove(buffer, buffer + processedBytes, existingBytes);
						}
					}
				}
			}

			if (networkEvents.lNetworkEvents & FD_CLOSE)
			{
				closesocket(c_sock.handle);
				WSACloseEvent(socketEvent1);
				InfoWindow failedWindow("Server closed connection");
				failedWindow.exec();
				return 0;
			}
		}
	}

	startGameWin.close();
	leaveGameClicked = false;

	// Set up board, cards, and players
	MainWindow mainWin;
	mainWin.createPlayers(names, gamepieces, slotNums);
	mainWin.show();
	mainWin.disableButton("buyProperty");
	mainWin.disableButton("sellProperty");
	mainWin.disableButton("buildHouse");
	mainWin.disableButton("endTurn");
	mainWin.updateText("Your gamepiece is the " + mainWin.players[thisPlayerIndex].getGamePiece());

	// main loop
	uint16 turn = 0;
	bool g_is_running = 1;
	bytes_received = 0;
	existingBytes = 0;

	while (g_is_running) {

		
		WSAEVENT socketEvent2 = CreateEvent(NULL, TRUE, FALSE, NULL);
		WSAEventSelect(c_sock.handle, socketEvent2, FD_READ | FD_CLOSE);

		// Wait 10ms for events in the event array ( in this case just 1 event )
		rerror = WSAWaitForMultipleEvents(1, &socketEvent2, FALSE, 10, FALSE);
		if (rerror == WSA_WAIT_TIMEOUT)
		{
			qApp->processEvents();

			if (mainWin.isVisible() == false) {
				g_is_running = 0;
				break;
			}

			// Enable sell property button if player owns properties, else set as disabled
			bool ownProperties = false;
			for (uint16 i = 0; i < 40; i++) {
				if (mainWin.players[thisPlayerIndex].getOwnedProperty(i)) {
					ownProperties = true;
					break;
				}
			}

			if (ownProperties) {
				mainWin.enableButton("sellProperty");
			}
			else {
				mainWin.disableButton("sellProperty");
			}

			if (buyPropertyClicked) {
				PlayerChoiceWindow sureBuyProperty("Are you sure you want to buy " + mainWin.positions[mainWin.players[thisPlayerIndex].getLocation()].getName() + "?");
				if (sureBuyProperty.exec()) {
					writeBuffer[0] = (uint8)Client_Message::BuyProperty;
					bytes_written = 1;

					if (!Net::socket_send(&c_sock, writeBuffer, bytes_written))
					{
						closesocket(c_sock.handle);
						InfoWindow failedWindow("Connection interrupted");
						failedWindow.exec();
						WSACloseEvent(socketEvent2);
						return 0;
					}
				}

				buyPropertyClicked = false;
			}

			if (sellPropertyClicked) {
				// User has already been asked if they want to sell property
				writeBuffer[0] = (uint8)Client_Message::SellProperty;
				bytes_written = 1;

				memcpy(&writeBuffer[bytes_written], &sellPropertyIndex, sizeof(sellPropertyIndex));
				bytes_written += sizeof(sellPropertyIndex);

				if (!Net::socket_send(&c_sock, writeBuffer, bytes_written))
				{
					closesocket(c_sock.handle);
					WSACloseEvent(socketEvent2);
					InfoWindow failedWindow("Connection interrupted");
					failedWindow.exec();
					return 0;
				}
				sellPropertyClicked = false;
			}

			if (buildHouseClicked) {
				PlayerChoiceWindow sureBuildHouse("Are you sure you want to build a house on " + mainWin.positions[mainWin.players[thisPlayerIndex].getLocation()].getName() + "?");
				if (sureBuildHouse.exec()) {
					writeBuffer[0] = (uint8)Client_Message::BuildHouse;
					bytes_written = 1;

					if (!Net::socket_send(&c_sock, writeBuffer, bytes_written))
					{
						closesocket(c_sock.handle);
						WSACloseEvent(socketEvent2);
						InfoWindow failedWindow("Connection interrupted");
						failedWindow.exec();
						return 0;
					}
				}
				buildHouseClicked = false;
			}

			if (noMoneyDone) {
				writeBuffer[0] = (uint8)Client_Message::NoMoneyDone;
				bytes_written = 1;

				if (!Net::socket_send(&c_sock, writeBuffer, bytes_written))
				{
					closesocket(c_sock.handle);
					WSACloseEvent(socketEvent2);
					InfoWindow failedWindow("Connection interrupted");
					failedWindow.exec();
					return 0;
				}
				noMoneyDone = false;
			}

			if (endTurnClicked) {
				PlayerChoiceWindow sureEndTurn("Are you sure you want to end your turn?");
				if (sureEndTurn.exec()) {
					writeBuffer[0] = (uint8)Client_Message::EndTurn;
					bytes_written = 1;

					if (!Net::socket_send(&c_sock, writeBuffer, bytes_written))
					{
						closesocket(c_sock.handle);
						WSACloseEvent(socketEvent2);
						InfoWindow failedWindow("Connection interrupted");
						failedWindow.exec();
						return 0;
					}
				}
				endTurnClicked = false;
			}

			if (leaveGameClicked) {
				PlayerChoiceWindow sureLeaveGame("Are you sure you want to leave the game?");
				if (sureLeaveGame.exec()) {
					writeBuffer[0] = (uint8)Client_Message::Leave;
					bytes_written = 1;
					Net::socket_send(&c_sock, writeBuffer, bytes_written);
					closesocket(c_sock.handle);
					return 0;
				}
				leaveGameClicked = false;
			}

			buyPropertyClicked, sellPropertyClicked, buildHouseClicked, noMoneyDone, endTurnClicked, leaveGameClicked = false;

			continue;
		}

		index = rerror - WSA_WAIT_EVENT_0; // Get the smallest index of a triggered event
		if (rerror != WSA_WAIT_TIMEOUT && rerror != WSA_WAIT_FAILED)
		{
			WSANETWORKEVENTS networkEvents;
			// Returns a list of the events that occurred
			rerror = WSAEnumNetworkEvents(c_sock.handle, socketEvent2, &networkEvents);
			if (rerror == SOCKET_ERROR)
			{
				printf("WSAEnumNetworkEvents Error %d\n", WSAGetLastError());
				continue;
			}

			rerror = 0;

			if (networkEvents.lNetworkEvents & FD_READ)
			{
				Net::socket_receive(&c_sock, buffer + existingBytes, c_socket_buffer_size - existingBytes, &bytes_received);
				if (bytes_received == 0) { // Server connection closed
					closesocket(c_sock.handle);
					WSACloseEvent(socketEvent2);
					InfoWindow failedWindow("Server closed connection");
					failedWindow.exec();
					return 0;
				}
				else if (bytes_received == SOCKET_ERROR) {
					int error = WSAGetLastError();
					closesocket(c_sock.handle);
					WSACloseEvent(socketEvent2);
					InfoWindow failedWindow("recvfrom returned SOCKET_ERROR, WSAGetLastError(): " + std::to_string(error));
					failedWindow.exec();
					return 0;
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
						case (uint8)Server_Message::SoldProperty:
						{
							bytes_read = 2;
							uint16 playerIndex, propertyNum, soldFor;

							memcpy(&playerIndex, &buffer[bytes_read], sizeof(playerIndex));
							bytes_read += sizeof(playerIndex);

							memcpy(&propertyNum, &buffer[bytes_read], sizeof(propertyNum));
							bytes_read += sizeof(propertyNum);

							memcpy(&soldFor, &buffer[bytes_read], sizeof(soldFor));
							bytes_read += sizeof(soldFor);

							if (playerIndex == thisPlayerIndex) {
								mainWin.sellProperty(propertyNum, soldFor);
								mainWin.updateText("You sold " + mainWin.positions[propertyNum].getName() + " for $" + std::to_string(soldFor));
							}
							else {
								mainWin.positions[propertyNum].reset();
								mainWin.updateText(mainWin.players[playerIndex].getPlayerName() + " sold " + mainWin.positions[propertyNum].getName() + " for $" + std::to_string(soldFor));
							}
						}
						break;

						case (uint8)Server_Message::BoughtProperty:
						{
							bytes_read = 2;
							uint16 playerIndex, propertyNum;

							memcpy(&playerIndex, &buffer[bytes_read], sizeof(playerIndex));
							bytes_read += sizeof(playerIndex);

							memcpy(&propertyNum, &buffer[bytes_read], sizeof(propertyNum));
							bytes_read += sizeof(propertyNum);

							if (playerIndex == thisPlayerIndex) {
								mainWin.disableButton("buyProperty");
								mainWin.buyProperty(propertyNum);
								mainWin.updateText("You bought " + mainWin.positions[propertyNum].getName() + " for $" + std::to_string(mainWin.positions[propertyNum].getPrice()));
							}
							else {
								mainWin.positions[propertyNum].setOwner(playerIndex);
								mainWin.updateText(mainWin.players[playerIndex].getPlayerName() + " bought " + mainWin.positions[propertyNum].getName() + " for $" + std::to_string(mainWin.positions[propertyNum].getPrice()));
							}
						}
						break;

						case (uint8)Server_Message::BuiltHouse:
						{
							bytes_read = 2;
							uint16 playerIndex, propertyNum;

							memcpy(&playerIndex, &buffer[bytes_read], sizeof(playerIndex));
							bytes_read += sizeof(playerIndex);

							memcpy(&propertyNum, &buffer[bytes_read], sizeof(propertyNum));
							bytes_read += sizeof(propertyNum);

							if (playerIndex == thisPlayerIndex) {
								mainWin.disableButton("buildHouse");
								mainWin.buyHouse(propertyNum);
								mainWin.updateText("You built a house on " + mainWin.positions[propertyNum].getName() + " for $" + std::to_string(mainWin.positions[propertyNum].getHousePrice()));
							}
							else {
								mainWin.positions[propertyNum].buildHouse();
								mainWin.updateText(mainWin.players[playerIndex].getPlayerName() + " built a house on " + mainWin.positions[propertyNum].getName() + " for $" + std::to_string(mainWin.positions[propertyNum].getHousePrice()));
							}
						}
						break;

						case (uint8)Server_Message::PaidMoney:
						{
							bytes_read = 2;
							uint16 playerIndex, amountPay;

							memcpy(&playerIndex, &buffer[bytes_read], sizeof(playerIndex));
							bytes_read += sizeof(playerIndex);

							memcpy(&amountPay, &buffer[bytes_read], sizeof(amountPay));
							bytes_read += sizeof(amountPay);

							if (playerIndex == thisPlayerIndex) {
								mainWin.players[thisPlayerIndex].payMoney(amountPay);
								mainWin.updateText("You have paid $" + std::to_string(amountPay));
							}
							else {
								mainWin.updateText(mainWin.players[playerIndex].getPlayerName() + " has paid $" + std::to_string(amountPay));
							}
						}
						break;

						case (uint8)Server_Message::ReceivedMoney:
						{
							bytes_read = 2;
							uint16 playerIndex, amountReceived;

							memcpy(&playerIndex, &buffer[bytes_read], sizeof(playerIndex));
							bytes_read += sizeof(playerIndex);

							memcpy(&amountReceived, &buffer[bytes_read], sizeof(amountReceived));
							bytes_read += sizeof(amountReceived);

							if (playerIndex == thisPlayerIndex) {
								mainWin.players[thisPlayerIndex].receiveMoney(amountReceived);
								mainWin.updateText("You have received $" + std::to_string(amountReceived));
							}
							else {
								mainWin.updateText(mainWin.players[playerIndex].getPlayerName() + " has received $" + std::to_string(amountReceived));
							}
						}
						break;

						case (uint8)Server_Message::RentTransfer:
						{
							bytes_read = 2;
							uint16 playerIndex, ownerIndex, rent;

							memcpy(&playerIndex, &buffer[bytes_read], sizeof(playerIndex));
							bytes_read += sizeof(playerIndex);

							memcpy(&ownerIndex, &buffer[bytes_read], sizeof(ownerIndex));
							bytes_read += sizeof(ownerIndex);

							memcpy(&rent, &buffer[bytes_read], sizeof(rent));
							bytes_read += sizeof(rent);

							if (playerIndex == thisPlayerIndex) {
								mainWin.players[thisPlayerIndex].payMoney(rent);
								mainWin.updateText("You have paid $" + std::to_string(rent) + " in rent to " + mainWin.players[ownerIndex].getPlayerName());
							}
							else if (ownerIndex == thisPlayerIndex) {
								mainWin.players[thisPlayerIndex].receiveMoney(rent);
								mainWin.updateText("You have received $" + std::to_string(rent) + " in rent from " + mainWin.players[playerIndex].getPlayerName());
							}
							else {
								mainWin.updateText(mainWin.players[playerIndex].getPlayerName() + " has paid $" + std::to_string(rent) + " in rent to " + mainWin.players[ownerIndex].getPlayerName());
							}
						}
						break;

						case (uint8)Server_Message::BuyPropertyOption:
						{
							bytes_read = 2;
							uint16 option;

							memcpy(&option, &buffer[bytes_read], sizeof(option));
							bytes_read += sizeof(option);

							switch (option) {
							case 0:
							{
								mainWin.updateText("You do not have enough money to buy this property.");
							}
							break;
							case 1:
							{
								mainWin.enableButton("buyProperty");
							}
							break;
							}

						}
						break;

						case (uint8)Server_Message::BuyHouseOption:
						{
							bytes_read = 2;
							uint16 option;

							memcpy(&option, &buffer[bytes_read], sizeof(option));
							bytes_read += sizeof(option);

							switch (option) {
							case 0:
							{
								mainWin.updateText("You already own this.");
							}
							break;
							case 1:
							{
								mainWin.enableButton("buildHouse");
							}
							break;
							case 2:
							{
								mainWin.updateText("You don't have enough money to build a house.");
							}
							break;
							case 3:
							{
								mainWin.updateText("You have already built the maximum amount of houses.");
							}
							break;
							}
						}
						break;

						case (uint8)Server_Message::PlayerTurn:
						{
							mainWin.disableButton("buyProperty");
							mainWin.disableButton("buildHouse");
							mainWin.disableButton("endTurn");

							bytes_read = 2;
							uint16 playerIndex, diceRoll;

							memcpy(&playerIndex, &buffer[bytes_read], sizeof(playerIndex));
							bytes_read += sizeof(playerIndex);

							memcpy(&diceRoll, &buffer[bytes_read], sizeof(diceRoll));
							bytes_read += sizeof(diceRoll);

							if (playerIndex == thisPlayerIndex) {
								mainWin.enableButton("endTurn");
								InfoWindow turnWindow("Your turn! You rolled " + std::to_string(diceRoll));
								turnWindow.exec();
							}
							else {
								InfoWindow turnWindow(mainWin.players[playerIndex].getPlayerName() + "'s turn! They rolled " + std::to_string(diceRoll));
								turnWindow.exec();
							}
						}
						break;

						case (uint8)Server_Message::DiceRoll:
						{
							bytes_read = 2;
							uint16 playerIndex, diceRoll;

							memcpy(&playerIndex, &buffer[bytes_read], sizeof(playerIndex));
							bytes_read += sizeof(playerIndex);

							memcpy(&diceRoll, &buffer[bytes_read], sizeof(diceRoll));
							bytes_read += sizeof(diceRoll);

							if (playerIndex == thisPlayerIndex) {
								mainWin.updateText("Rolling dice to calculate rent amount...you rolled " + std::to_string(diceRoll));
							}
							else
								mainWin.updateText("Rolling dice to calculate rent amount..." + mainWin.players[playerIndex].getPlayerName() + " rolled " + std::to_string(diceRoll));
						}
						break;

						case (uint8)Server_Message::LandedOn:
						{
							bytes_read = 2;
							uint16 playerIndex, location, passedGo;

							memcpy(&playerIndex, &buffer[bytes_read], sizeof(playerIndex));
							bytes_read += sizeof(playerIndex);

							memcpy(&location, &buffer[bytes_read], sizeof(location));
							bytes_read += sizeof(location);

							memcpy(&passedGo, &buffer[bytes_read], sizeof(passedGo));
							bytes_read += sizeof(passedGo);

							mainWin.moveGamepiece(playerIndex, location); // Update where gamepiece is
							if (playerIndex == thisPlayerIndex) {
								if (passedGo == 1) {
									mainWin.players[thisPlayerIndex].receiveMoney(200);
									mainWin.updateText("You passed GO and collected $200");
								}
								mainWin.players[thisPlayerIndex].movePlayer(0, location);
								mainWin.updateText("You landed on " + mainWin.positions[location].getName());
							}
							else {
								if (passedGo == 1) {
									mainWin.updateText(mainWin.players[playerIndex].getPlayerName() + " passed GO and collected $200");
								}
								mainWin.players[playerIndex].movePlayer(0, location);
								mainWin.updateText(mainWin.players[playerIndex].getPlayerName() + " landed on " + mainWin.positions[location].getName());
							}

							if (location == 30) { // If landed on Go to Jail, move player to jail
								mainWin.moveGamepiece(playerIndex, 10);
								mainWin.players[playerIndex].movePlayer(0, 10);
							}

						}
						break;

						case (uint8)Server_Message::ChanceCard:
						{
							bytes_read = 2;
							uint16 cardNum;

							memcpy(&cardNum, &buffer[bytes_read], sizeof(cardNum));
							bytes_read += sizeof(cardNum);

							mainWin.updateText("Chance card: " + mainWin.chanceCards[cardNum].getDescription());
						}
						break;

						case (uint8)Server_Message::CommunityChestCard:
						{
							bytes_read = 2;
							uint16 cardNum;

							memcpy(&cardNum, &buffer[bytes_read], sizeof(cardNum));
							bytes_read += sizeof(cardNum);

							mainWin.updateText("Community Chest card: " + mainWin.communityChestCards[cardNum].getDescription());
						}
						break;

						case (uint8)Server_Message::NoMoney:
						{
							bool userChoice = mainWin.noMoneyOptions();
							if (userChoice) {
								noMoneyDone = true;
							}
							else {
								buffer[0] = (uint8)Client_Message::GiveUp;
								bytes_written = 1;

								if (!Net::socket_send(&c_sock, buffer, bytes_written))
								{
									closesocket(c_sock.handle);
									WSACloseEvent(socketEvent2);
									InfoWindow failedWindow("Connection interrupted");
									failedWindow.exec();
									return 0;
								}
							}
						}
						break;

						case (uint8)Server_Message::PlayerBankrupt:
						{
							bytes_read = 2;
							uint16 playerIndex;

							memcpy(&playerIndex, &buffer[bytes_read], sizeof(playerIndex));
							bytes_read += sizeof(playerIndex);

							mainWin.bankrupt(playerIndex);

							if (playerIndex == thisPlayerIndex) {
								InfoWindow bankruptWindow("You have gone bankrupt! You are now out of the game.");
								bankruptWindow.exec();
							}
							else {
								InfoWindow bankruptWindow(mainWin.players[playerIndex].getPlayerName() + " has gone bankrupt and is now out of the game! Their properties have been returned to the bank.");
								bankruptWindow.exec();
							}
						}
						break;

						case (uint8)Server_Message::PlayerLeft:
						{
							bytes_read = 2;
							uint16 playerIndex;

							memcpy(&playerIndex, &buffer[bytes_read], sizeof(playerIndex));
							bytes_read += sizeof(playerIndex);

							mainWin.bankrupt(playerIndex);

							InfoWindow playerLeftWindow(mainWin.players[playerIndex].getPlayerName() + " has left the game. Their properties have been returned to the bank");
							playerLeftWindow.exec();
						}
						break;

						case (uint8)Server_Message::Winner:
						{
							bytes_read = 2;
							uint16 playerIndex;

							memcpy(&playerIndex, &buffer[bytes_read], sizeof(playerIndex));
							bytes_read += sizeof(playerIndex);

							InfoWindow winnerWindow("GAME OVER\nThe winner is " + mainWin.players[playerIndex].getPlayerName() + "!");
							winnerWindow.exec();

							writeBuffer[0] = (uint8)Client_Message::Leave;
							bytes_written = 1;
							Net::socket_send(&c_sock, writeBuffer, bytes_written);
							closesocket(c_sock.handle);

							mainWin.close();
							return 0;
						}
						break;
						}

						if (existingBytes > 0) {
							uint8 processedBytes = buffer[0];
							memmove(buffer, buffer + processedBytes, existingBytes);
						}
					}

				}
			}

			if (networkEvents.lNetworkEvents & FD_CLOSE)
			{
				closesocket(c_sock.handle);
				WSACloseEvent(socketEvent2);
				InfoWindow failedWindow("Server closed connection");
				failedWindow.exec();
				return 0;
			}
		}
		
	}

	writeBuffer[0] = (uint8)Client_Message::Leave;
	bytes_written = 1;
	Net::socket_send(&c_sock, writeBuffer, bytes_written);
	closesocket(c_sock.handle);

	mainWin.close();
	return 0;
}

void startGameWinClosed(uint16 result) {
	if (result) {
		startGameClicked = true;
	}
	else {
		leaveGameClicked = true;
	}
}