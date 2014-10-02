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

#include "protocolold.h"
#include "outputmessage.h"
#include "connection.h"

#include "rsa.h"
#include "game.h"

extern Game g_game;

#ifdef ENABLE_SERVER_DIAGNOSTIC
uint32_t ProtocolOld::protocolOldCount = 0;
#endif

void ProtocolOld::disconnectClient(uint8_t error, const char* message)
{
	OutputMessage_ptr output = OutputMessagePool::getInstance()->getOutputMessage(this, false);
	if (output) {
		output->AddByte(error);
		output->AddString(message);
		OutputMessagePool::getInstance()->send(output);
	}

	getConnection()->closeConnection();
}

bool ProtocolOld::parseFirstPacket(NetworkMessage& msg)
{
	if (g_game.getGameState() == GAME_STATE_SHUTDOWN) {
		getConnection()->closeConnection();
		return false;
	}

	/*uint16_t clientOS =*/ msg.get<uint16_t>();
	uint16_t version = msg.get<uint16_t>();
	msg.SkipBytes(12);

	if (version <= 760) {
		disconnectClient(0x0A, "Only clients with protocol " CLIENT_VERSION_STR " allowed!");
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

	if (version <= 822) {
		disableChecksum();
	}

	disconnectClient(0x0A, "Only clients with protocol " CLIENT_VERSION_STR " allowed!");
	return false;
}

void ProtocolOld::onRecvFirstMessage(NetworkMessage& msg)
{
	parseFirstPacket(msg);
}
