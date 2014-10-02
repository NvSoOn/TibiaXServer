/**
 * The TibiaX Server - a free and open-source MMORPG server emulator
 * Copyright (C) 2014  TibiaX <http://tibiax.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "otpch.h"

#include "protocollogin.h"

#include "outputmessage.h"
#include "connection.h"
#include "rsa.h"

#include "configmanager.h"
#include "tools.h"
#include "iologindata.h"
#include "ban.h"
#include "game.h"

extern ConfigManager g_config;
extern Game g_game;

#ifdef ENABLE_SERVER_DIAGNOSTIC
uint32_t ProtocolLogin::protocolLoginCount = 0;
#endif

void ProtocolLogin::disconnectClient(uint8_t error, const char* message)
{
	OutputMessage_ptr output = OutputMessagePool::getInstance()->getOutputMessage(this, false);
	if (output) {
		output->AddByte(error);
		output->AddString(message);
		OutputMessagePool::getInstance()->send(output);
	}

	getConnection()->closeConnection();
}

bool ProtocolLogin::parseFirstPacket(NetworkMessage& msg)
{
	if (g_game.getGameState() == GAME_STATE_SHUTDOWN) {
		getConnection()->closeConnection();
		return false;
	}

	uint32_t clientip = getConnection()->getIP();

	/*uint16_t clientos = */
	msg.get<uint16_t>();
	uint16_t version = msg.get<uint16_t>();

	if (version >= 971) {
		msg.SkipBytes(17);
	} else {
		msg.SkipBytes(12);
	}

	/*
	 * Skipped bytes:
	 * 4 bytes: protocolVersion (only 971+)
	 * 12 bytes: dat, spr, pic signatures (4 bytes each)
	 * 1 byte: 0 (only 971+)
	*/

	if (version <= 760) {
		disconnectClient(0x0A, "Only clients with protocol " CLIENT_VERSION_STR " allowed!");
		return false;
	}

	if (!RSA_decrypt(msg)) {
		getConnection()->closeConnection();
		return false;
	}

	uint32_t key[4];
	key[0] = msg.get<uint32_t>();
	key[1] = msg.get<uint32_t>();
	key[2] = msg.get<uint32_t>();
	key[3] = msg.get<uint32_t>();
	enableXTEAEncryption();
	setXTEAKey(key);

	std::string accountName = msg.GetString();
	std::string password = msg.GetString();

	if (version < CLIENT_VERSION_MIN || version > CLIENT_VERSION_MAX) {
		disconnectClient(0x0A, "Only clients with protocol " CLIENT_VERSION_STR " allowed!");
		return false;
	}

	if (g_game.getGameState() == GAME_STATE_STARTUP) {
		disconnectClient(0x0A, "Gameworld is starting up. Please wait.");
		return false;
	}

	if (g_game.getGameState() == GAME_STATE_MAINTAIN) {
		disconnectClient(0x0A, "Gameworld is under maintenance. Please re-connect in a while.");
		return false;
	}

	BanInfo banInfo;
	if (IOBan::isIpBanned(clientip, banInfo)) {
		if (banInfo.reason.empty()) {
			banInfo.reason = "(none)";
		}

		std::ostringstream ss;
		ss << "Your IP has been banned until " << formatDateShort(banInfo.expiresAt) << " by " << banInfo.bannedBy << ".\n\nReason specified:\n" << banInfo.reason;
		disconnectClient(0x0A, ss.str().c_str());
		return false;
	}

	Account account;
	if (!IOLoginData::loginserverAuthentication(accountName, password, account)) {
		disconnectClient(0x0A, "Account name or password is not correct.");
		return false;
	}

	OutputMessage_ptr output = OutputMessagePool::getInstance()->getOutputMessage(this, false);
	if (output) {
		if (!accountName.empty()) {
			//Update premium days
			g_game.updatePremium(account);
		}

		//Add MOTD
		output->AddByte(0x14);

		std::ostringstream ss;
		ss << g_game.getMotdNum() << "\n" << g_config.getString(ConfigManager::MOTD);
		output->AddString(ss.str());

		//Add char list
		output->AddByte(0x64);

		if (!accountName.empty()) {
			output->AddByte(1); // number of worlds

			output->AddByte(0); // world id
			output->AddString(g_config.getString(ConfigManager::SERVER_NAME));
			output->AddString(g_config.getString(ConfigManager::IP));
			output->add<uint16_t>(g_config.getNumber(ConfigManager::GAME_PORT));
			output->AddByte(0);

			output->AddByte((uint8_t)account.charList.size());
			for (const std::string& characterName : account.charList) {
				output->AddByte(0);
				output->AddString(characterName);
			}
		} else {
			auto players = g_game.getPlayersInCast(password);
			uint8_t size = static_cast<uint8_t>(players.size());

			output->AddByte(size);
			for (const auto& player : players) {
				output->AddByte(player->getViewers());
				output->AddString(std::to_string(player->getViewers()) + (player->getViewers() == 1 ? " viewer" : " viewers"));
				output->AddString(g_config.getString(ConfigManager::IP));
				output->add<uint16_t>(g_config.getNumber(ConfigManager::GAME_PORT));
				output->AddByte(0);
			}

			output->AddByte(size);
			for (const auto& player : players) {
				output->AddByte(player->getViewers());
				output->AddString(player->getName());
			}
		}

		//Add premium days
		if (g_config.getBoolean(ConfigManager::FREE_PREMIUM)) {
			output->add<uint16_t>(0xFFFF);    //client displays free premium
		} else {
			output->add<uint16_t>(account.premiumDays);
		}

		OutputMessagePool::getInstance()->send(output);
	}

	getConnection()->closeConnection();
	return true;
}

void ProtocolLogin::onRecvFirstMessage(NetworkMessage& msg)
{
	parseFirstPacket(msg);
}
