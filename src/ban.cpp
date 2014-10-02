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

#include "ban.h"
#include "database.h"
#include "tools.h"

bool Ban::acceptConnection(uint32_t clientip)
{
	std::lock_guard<std::recursive_mutex> lockClass(lock);

	uint64_t currentTime = OTSYS_TIME();

	auto it = ipConnectMap.find(clientip);
	if (it == ipConnectMap.end()) {
		ConnectBlock cb;
		cb.lastAttempt = currentTime;
		cb.blockTime = 0;
		cb.count = 1;
		ipConnectMap[clientip] = cb;
		return true;
	}

	ConnectBlock& connectBlock = it->second;
	if (connectBlock.blockTime > currentTime) {
		connectBlock.blockTime += 250;
		return false;
	}

	int64_t timeDiff = currentTime - connectBlock.lastAttempt;
	connectBlock.lastAttempt = currentTime;
	if (timeDiff <= 5000) {
		if (++connectBlock.count > 5) {
			connectBlock.count = 0;
			if (timeDiff <= 500) {
				connectBlock.blockTime = currentTime + 3000;
				return false;
			}
		}
	} else {
		connectBlock.count = 1;
	}
	return true;
}

bool IOBan::isAccountBanned(uint32_t accountId, BanInfo& banInfo)
{
	Database* db = Database::getInstance();

	std::ostringstream query;
	query << "SELECT `reason`, `expires_at`, `banned_at`, `banned_by`, (SELECT `name` FROM `players` WHERE `id` = `banned_by`) AS `name` FROM `account_bans` WHERE `account_id` = " << accountId;

	DBResult* result = db->storeQuery(query.str());
	if (!result) {
		return false;
	}

	int64_t expiresAt = result->getNumber<int64_t>("expires_at");
	if (expiresAt != 0 && time(nullptr) > expiresAt) {
		// Move the ban to history if it has expired
		query.str("");
		query << "INSERT INTO `account_ban_history` (`account_id`, `reason`, `banned_at`, `expired_at`, `banned_by`) VALUES (" << accountId << ',' << db->escapeString(result->getDataString("reason")) << ',' << result->getDataInt("banned_at") << ',' << expiresAt << ',' << result->getDataInt("banned_by") << ')';
		db->executeQuery(query.str());

		query.str("");
		query << "DELETE FROM `account_bans` WHERE `account_id` = " << accountId;
		db->executeQuery(query.str());

		db->freeResult(result);
		return false;
	}

	banInfo.expiresAt = expiresAt;
	banInfo.reason = result->getDataString("reason");
	banInfo.bannedBy = result->getDataString("name");
	db->freeResult(result);
	return true;
}

bool IOBan::isIpBanned(uint32_t clientip, BanInfo& banInfo)
{
	if (clientip == 0) {
		return false;
	}

	Database* db = Database::getInstance();

	std::ostringstream query;
	query << "SELECT `reason`, `expires_at`, (SELECT `name` FROM `players` WHERE `id` = `banned_by`) AS `name` FROM `ip_bans` WHERE `ip` = " << clientip;

	DBResult* result = db->storeQuery(query.str());
	if (!result) {
		return false;
	}

	int64_t expiresAt = result->getNumber<int64_t>("expires_at");
	if (expiresAt != 0 && time(nullptr) > expiresAt) {
		query << "DELETE FROM `ip_bans` WHERE `ip` = " << clientip;
		db->executeQuery(query.str());

		db->freeResult(result);
		return false;
	}

	banInfo.expiresAt = expiresAt;
	banInfo.reason = result->getDataString("reason");
	banInfo.bannedBy = result->getDataString("name");
	db->freeResult(result);
	return true;
}

bool IOBan::isPlayerNamelocked(uint32_t playerId)
{
	Database* db = Database::getInstance();

	std::ostringstream query;
	query << "SELECT 1 FROM `player_namelocks` WHERE `player_id` = " << playerId;

	DBResult* result = db->storeQuery(query.str());
	if (!result) {
		return false;
	}

	db->freeResult(result);
	return true;
}
