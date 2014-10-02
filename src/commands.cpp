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

#include <fstream>

#include "commands.h"
#include "player.h"
#include "npc.h"
#include "monsters.h"
#include "game.h"
#include "actions.h"
#include "house.h"
#include "iologindata.h"
#include "tools.h"
#include "configmanager.h"
#include "spells.h"
#include "talkaction.h"
#include "movement.h"
#include "spells.h"
#include "weapons.h"
#include "raids.h"
#include "quests.h"
#include "mounts.h"
#include "globalevent.h"
#include "monster.h"
#include "scheduler.h"
#include "events.h"
#ifdef ENABLE_SERVER_DIAGNOSTIC
#include "outputmessage.h"
#include "connection.h"
#include "protocolstatus.h"
#include "protocollogin.h"
#endif

#include "pugicast.h"

extern ConfigManager g_config;
extern Actions* g_actions;
extern Monsters g_monsters;
extern TalkActions* g_talkActions;
extern MoveEvents* g_moveEvents;
extern Spells* g_spells;
extern Weapons* g_weapons;
extern Game g_game;
extern CreatureEvents* g_creatureEvents;
extern GlobalEvents* g_globalEvents;
extern Events* g_events;

s_defcommands Commands::defined_commands[] = {
	//admin commands
	{"/reload", &Commands::reloadInfo},
	{"/hide", &Commands::hide},
	{"/raid", &Commands::forceRaid},
	{"/ghost", &Commands::ghost},
	{"/clean", &Commands::clean},
	{"/mccheck", &Commands::multiClientCheck},
	{"/serverdiag", &Commands::serverDiag},

	// player commands - TODO: make them talkactions
	{"!sellhouse", &Commands::sellHouse}
};

Commands::Commands()
{
	loaded = false;

	//setup command map
	for (uint32_t i = 0; i < sizeof(defined_commands) / sizeof(defined_commands[0]); i++) {
		Command* cmd = new Command;
		cmd->loadedGroupId = false;
		cmd->loadedAccountType = false;
		cmd->loadedLogging = false;
		cmd->logged = true;
		cmd->groupId = 1;
		cmd->f = defined_commands[i].f;
		std::string key = defined_commands[i].name;
		commandMap[key] = cmd;
	}
}

Commands::~Commands()
{
	for (const auto& it : commandMap) {
		delete it.second;
	}
}

bool Commands::loadFromXml()
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file("data/XML/commands.xml");
	if (!result) {
		std::cout << "[Error - Commands::loadFromXml] Failed to load data/XML/commands.xml: " << result.description() << std::endl;
		return false;
	}

	loaded = true;

	for (pugi::xml_node commandNode = doc.child("commands").first_child(); commandNode; commandNode = commandNode.next_sibling()) {
		pugi::xml_attribute cmdAttribute = commandNode.attribute("cmd");
		if (!cmdAttribute) {
			std::cout << "[Warning - Commands::loadFromXml] Missing cmd" << std::endl;
			continue;
		}

		auto it = commandMap.find(cmdAttribute.as_string());
		if (it == commandMap.end()) {
			std::cout << "[Warning - Commands::loadFromXml] Unknown command " << cmdAttribute.as_string() << std::endl;
			continue;
		}

		Command* command = it->second;

		pugi::xml_attribute groupAttribute = commandNode.attribute("group");
		if (groupAttribute) {
			if (!command->loadedGroupId) {
				command->groupId = pugi::cast<uint32_t>(groupAttribute.value());
				command->loadedGroupId = true;
			} else {
				std::cout << "[Notice - Commands::loadFromXml] Duplicate command: " << it->first << std::endl;
			}
		}

		pugi::xml_attribute acctypeAttribute = commandNode.attribute("acctype");
		if (acctypeAttribute) {
			if (!command->loadedAccountType) {
				command->accountType = (AccountType_t)pugi::cast<uint32_t>(acctypeAttribute.value());
				command->loadedAccountType = true;
			} else {
				std::cout << "[Notice - Commands::loadFromXml] Duplicate command: " << it->first << std::endl;
			}
		}

		pugi::xml_attribute logAttribute = commandNode.attribute("log");
		if (logAttribute) {
			if (!command->loadedLogging) {
				command->logged = booleanString(logAttribute.as_string());
				command->loadedLogging = true;
			} else {
				std::cout << "[Notice - Commands::loadFromXml] Duplicate log tag for: " << it->first << std::endl;
			}
		}
	}

	for (const auto& it : commandMap) {
		Command* command = it.second;
		if (!command->loadedGroupId) {
			std::cout << "[Warning - Commands::loadFromXml] Missing group id for command " << it.first << std::endl;
		}

		if (!command->loadedAccountType) {
			std::cout << "[Warning - Commands::loadFromXml] Missing acctype level for command " << it.first << std::endl;
		}

		if (!command->loadedLogging) {
			std::cout << "[Warning - Commands::loadFromXml] Missing log command " << it.first << std::endl;
		}

		g_game.addCommandTag(it.first.front());
	}
	return loaded;
}

bool Commands::reload()
{
	loaded = false;

	for (const auto& it : commandMap) {
		Command* command = it.second;
		command->groupId = 1;
		command->accountType = ACCOUNT_TYPE_GOD;
		command->loadedGroupId = false;
		command->loadedAccountType = false;
		command->logged = true;
		command->loadedLogging = false;
	}

	g_game.resetCommandTag();
	return loadFromXml();
}

bool Commands::exeCommand(Player& player, const std::string& cmd)
{
	std::string str_command;
	std::string str_param;

	std::string::size_type loc = cmd.find(' ', 0);
	if (loc != std::string::npos) {
		str_command = std::string(cmd, 0, loc);
		str_param = std::string(cmd, (loc + 1), cmd.size() - loc - 1);
	} else {
		str_command = cmd;
	}

	//find command
	auto it = commandMap.find(str_command);
	if (it == commandMap.end()) {
		return false;
	}

	Command* command = it->second;
	if (command->groupId > player.getGroup()->id || command->accountType > player.getAccountType()) {
		if (player.getGroup()->access) {
			player.sendTextMessage(MSG_STATUS_SMALL, "You can not execute this command.");
		}

		return false;
	}

	//execute command
	CommandFunc cfunc = command->f;
	(this->*cfunc)(player, str_param);

	if (command->logged) {
		player.sendTextMessage(MSG_STATUS_CONSOLE_RED, cmd);

		std::ostringstream logFile;
		logFile << "data/logs/" << player.getName() << " commands.log";
		std::ofstream out(logFile.str(), std::ios::app);
		if (out.is_open()) {
			time_t ticks = time(nullptr);
			const tm* now = localtime(&ticks);
			char buf[32];
			strftime(buf, sizeof(buf), "%d/%m/%Y %H:%M", now);

			out << '[' << buf << "] " << cmd << std::endl;
			out.close();
		}
	}
	return true;
}

void Commands::reloadInfo(Player& player, const std::string& param)
{
	std::string tmpParam = asLowerCaseString(param);
	if (tmpParam == "action" || tmpParam == "actions") {
		g_actions->reload();
		player.sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "Reloaded actions.");
	} else if (tmpParam == "config" || tmpParam == "configuration") {
		g_config.reload();
		player.sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "Reloaded config.");
	} else if (tmpParam == "command" || tmpParam == "commands") {
		reload();
		player.sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "Reloaded commands.");
	} else if (tmpParam == "creaturescript" || tmpParam == "creaturescripts") {
		g_creatureEvents->reload();
		player.sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "Reloaded creature scripts.");
	} else if (tmpParam == "monster" || tmpParam == "monsters") {
		g_monsters.reload();
		player.sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "Reloaded monsters.");
	} else if (tmpParam == "move" || tmpParam == "movement" || tmpParam == "movements"
	           || tmpParam == "moveevents" || tmpParam == "moveevent") {
		g_moveEvents->reload();
		player.sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "Reloaded movements.");
	} else if (tmpParam == "npc" || tmpParam == "npcs") {
		Npcs::reload();
		player.sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "Reloaded npcs.");
	} else if (tmpParam == "raid" || tmpParam == "raids") {
		Raids::getInstance()->reload();
		Raids::getInstance()->startup();
		player.sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "Reloaded raids.");
	} else if (tmpParam == "spell" || tmpParam == "spells") {
		g_spells->reload();
		g_monsters.reload();
		player.sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "Reloaded spells.");
	} else if (tmpParam == "talk" || tmpParam == "talkaction" || tmpParam == "talkactions") {
		g_talkActions->reload();
		player.sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "Reloaded talk actions.");
	} else if (tmpParam == "items") {
		Item::items.reload();
		player.sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "Reloaded items.");
	} else if (tmpParam == "weapon" || tmpParam == "weapons") {
		g_weapons->reload();
		g_weapons->loadDefaults();
		player.sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "Reloaded weapons.");
	} else if (tmpParam == "quest" || tmpParam == "quests") {
		Quests::getInstance()->reload();
		player.sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "Reloaded quests.");
	} else if (tmpParam == "mount" || tmpParam == "mounts") {
		Mounts::getInstance()->reload();
		player.sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "Reloaded mounts.");
	} else if (tmpParam == "globalevents" || tmpParam == "globalevent") {
		g_globalEvents->reload();
		player.sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "Reloaded globalevents.");
	} else if (tmpParam == "events") {
		g_events->load();
		player.sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "Reloaded events.");
	} else {
		player.sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "Reload type not found.");
	}
}

void Commands::sellHouse(Player& player, const std::string& param)
{
	Player* tradePartner = g_game.getPlayerByName(param);
	if (!tradePartner || tradePartner == &player) {
		player.sendCancel("Trade player not found.");
		return;
	}

	if (!Position::areInRange<2, 2, 0>(tradePartner->getPosition(), player.getPosition())) {
		player.sendCancel("Trade player is too far away.");
		return;
	}

	if (!tradePartner->isPremium()) {
		player.sendCancel("Trade player does not have a premium account.");
		return;
	}

	HouseTile* houseTile = dynamic_cast<HouseTile*>(player.getTile());
	if (!houseTile) {
		player.sendCancel("You must stand in your house to initiate the trade.");
		return;
	}

	House* house = houseTile->getHouse();
	if (!house || house->getOwner() != player.getGUID()) {
		player.sendCancel("You don't own this house.");
		return;
	}

	if (Houses::getInstance().getHouseByPlayerId(tradePartner->getGUID())) {
		player.sendCancel("Trade player already owns a house.");
		return;
	}

	if (IOLoginData::hasBiddedOnHouse(tradePartner->getGUID())) {
		player.sendCancel("Trade player is currently the highest bidder of an auctioned house.");
		return;
	}

	Item* transferItem = house->getTransferItem();
	if (!transferItem) {
		player.sendCancel("You can not trade this house.");
		return;
	}

	transferItem->getParent()->setParent(&player);

	if (!g_game.internalStartTrade(&player, tradePartner, transferItem)) {
		house->resetTransferItem();
	}
}

void Commands::hide(Player& player, const std::string& param)
{
	player.setHiddenHealth(!player.isHealthHidden());
	g_game.addCreatureHealth(&player);
}

void Commands::forceRaid(Player& player, const std::string& param)
{
	Raid* raid = Raids::getInstance()->getRaidByName(param);
	if (!raid || !raid->isLoaded()) {
		player.sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "No such raid exists.");
		return;
	}

	if (Raids::getInstance()->getRunning()) {
		player.sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "Another raid is already being executed.");
		return;
	}

	Raids::getInstance()->setRunning(raid);

	RaidEvent* event = raid->getNextRaidEvent();
	if (!event) {
		player.sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "The raid does not contain any data.");
		return;
	}

	raid->setState(RAIDSTATE_EXECUTING);

	uint32_t ticks = event->getDelay();
	if (ticks > 0) {
		g_scheduler->addEvent(createSchedulerTask(ticks,
		                     std::bind(&Raid::executeRaidEvent, raid, event)));
	} else {
		g_dispatcher->addTask(createTask(
		                         std::bind(&Raid::executeRaidEvent, raid, event)));
	}

	player.sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "Raid started.");
}

void Commands::clean(Player& player, const std::string& param)
{
	uint32_t count = g_game.getMap()->clean();
	if (count != 1) {
		std::ostringstream ss;
		ss << "Cleaned " << count << " items from the map.";
		g_game.broadcastMessage(ss.str(), MSG_STATUS_WARNING);
	} else {
		g_game.broadcastMessage("Cleaned 1 item from the map.", MSG_STATUS_WARNING);
	}
}

void Commands::serverDiag(Player& player, const std::string& param)
{
#ifdef ENABLE_SERVER_DIAGNOSTIC
	std::ostringstream text;
	text << "Server diagonostic:\n";
	text << "World:\n";
	text << "Player: " << g_game.getPlayersOnline() << " (" << Player::playerCount << ")\n";
	text << "Npc: " << g_game.getNpcsOnline() << " (" << Npc::npcCount << ")\n";
	text << "Monster: " << g_game.getMonstersOnline() << " (" << Monster::monsterCount << ")\n";

	text << "\nProtocols:\n";
	text << "--------------------\n";
	text << "ProtocolGame: " << ProtocolGame::protocolGameCount << "\n";
	text << "ProtocolLogin: " << ProtocolLogin::protocolLoginCount << "\n";
	text << "ProtocolStatus: " << ProtocolStatus::protocolStatusCount << "\n\n";

	text << "\nConnections:\n";
	text << "--------------------\n";
	text << "Active connections: " << Connection::connectionCount << "\n";
	text << "Total message pool: " << OutputMessagePool::getInstance()->getTotalMessageCount() << "\n";
	text << "Auto message pool: " << OutputMessagePool::getInstance()->getAutoMessageCount() << "\n";
	text << "Free message pool: " << OutputMessagePool::getInstance()->getAvailableMessageCount() << "\n";

	text << "\nLibraries:\n";
	text << "--------------------\n";
	text << "asio: " << BOOST_ASIO_VERSION << "\n";
	text << "lua: " << LUA_VERSION << "\n";
	text << "pugixml: " << PUGIXML_VERSION << "\n";

	player.sendTextMessage(MSG_STATUS_CONSOLE_BLUE, text.str().c_str());
#else
	player.sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "This command requires the server to be compiled with the ENABLE_SERVER_DIAGNOSTIC flag.");
#endif
}

void Commands::ghost(Player& player, const std::string& param)
{
	player.switchGhostMode();

	Player* playerPtr = &player;

	SpectatorVec list;
	g_game.getSpectators(list, player.getPosition(), true, true);
	for (Creature* spectator : list) {
		Player* tmpPlayer = spectator->getPlayer();
		if (tmpPlayer != playerPtr && !tmpPlayer->isAccessPlayer()) {
			if (player.isInGhostMode()) {
				tmpPlayer->sendCreatureDisappear(playerPtr, player.getTile()->getClientIndexOfThing(tmpPlayer, playerPtr));
			} else {
				tmpPlayer->sendCreatureAppear(playerPtr, player.getPosition(), true);
			}
		} else {
			tmpPlayer->sendCreatureChangeVisible(playerPtr, !player.isInGhostMode());
		}
	}

	if (player.isInGhostMode()) {
		for (const auto& it : g_game.getPlayers()) {
			if (!it.second->isAccessPlayer()) {
				it.second->notifyStatusChange(playerPtr, VIPSTATUS_OFFLINE);
			}
		}

		IOLoginData::updateOnlineStatus(player.getGUID(), false);
		player.sendTextMessage(MSG_INFO_DESCR, "You are now invisible.");
		g_game.addMagicEffect(list, player.getPosition(), NM_ME_YALAHARIGHOST);
	} else {
		for (const auto& it : g_game.getPlayers()) {
			if (!it.second->isAccessPlayer()) {
				it.second->notifyStatusChange(playerPtr, VIPSTATUS_ONLINE);
			}
		}

		IOLoginData::updateOnlineStatus(player.getGUID(), true);
		player.sendTextMessage(MSG_INFO_DESCR, "You are visible again.");
		Position pos = player.getPosition();
		pos.x += 1;
		g_game.addMagicEffect(list, pos, NM_ME_SMOKE);
	}
}

void Commands::multiClientCheck(Player& player, const std::string& param)
{
	player.sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "Multiclient Check List:");
	std::map<uint32_t, std::vector<Player*>> ipMap;

	for (const auto& it : g_game.getPlayers()) {
		Player* tmpPlayer = it.second;
		if (tmpPlayer->getIP() == 0) {
			continue;
		}

		ipMap[tmpPlayer->getIP()].push_back(tmpPlayer);
	}

	for (const auto& it : ipMap) {
		if (it.second.size() < 2) {
			continue;
		}

		Player* tmpPlayer = it.second.front();
		std::ostringstream ss;
		ss << convertIPToString(it.first) << ": " << tmpPlayer->getName() << " [" << tmpPlayer->getLevel() << ']';

		for (std::vector<Player*>::size_type i = 1, size = it.second.size(); i < size; ++i) {
			tmpPlayer = it.second[i];
			ss << ", " << tmpPlayer->getName() << " [" << tmpPlayer->getLevel() << ']';
		}

		ss << '.';
		player.sendTextMessage(MSG_STATUS_CONSOLE_BLUE, ss.str());
	}
}
