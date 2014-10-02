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

#include "party.h"
#include "player.h"
#include "game.h"
#include "configmanager.h"
#include "events.h"

extern Game g_game;
extern ConfigManager g_config;
extern Events* g_events;

Party::Party(Player* _leader)
{
	sharedExpActive = false;
	sharedExpEnabled = false;

	if (_leader) {
		setLeader(_leader);
		_leader->setParty(this);
	}
}

Party::~Party()
{
	//
}

void Party::disband()
{
	if (!g_events->eventPartyOnDisband(this)) {
		return;
	}

	Player* currentLeader = leader;
	setLeader(nullptr);

	currentLeader->setParty(nullptr);
	currentLeader->sendClosePrivate(CHANNEL_PARTY);
	g_game.updatePlayerShield(currentLeader);
	g_game.updatePlayerHelpers(*currentLeader);
	currentLeader->sendCreatureSkull(currentLeader);
	currentLeader->sendTextMessage(MSG_INFO_DESCR, "Your party has been disbanded.");

	for (Player* invitee : inviteList) {
		invitee->removePartyInvitation(this);
		currentLeader->sendCreatureShield(invitee);
	}
	inviteList.clear();

	for (Player* member : memberList) {
		member->setParty(nullptr);
		member->sendClosePrivate(CHANNEL_PARTY);
		member->sendTextMessage(MSG_INFO_DESCR, "Your party has been disbanded.");
	}

	for (Player* member : memberList) {
		g_game.updatePlayerShield(member);

		for (Player* otherMember : memberList) {
			otherMember->sendCreatureSkull(member);
		}

		member->sendCreatureSkull(currentLeader);
		currentLeader->sendCreatureSkull(member);
		g_game.updatePlayerHelpers(*member);
	}
	memberList.clear();
	delete this;
}

bool Party::leaveParty(Player* player)
{
	if (!player) {
		return false;
	}

	if (player->getParty() != this && leader != player) {
		return false;
	}

	if (!g_events->eventPartyOnLeave(this, player)) {
		return false;
	}

	bool missingLeader = false;
	if (leader == player) {
		if (!memberList.empty()) {
			if (memberList.size() == 1 && inviteList.empty()) {
				missingLeader = true;
			} else {
				passPartyLeadership(memberList.front());
			}
		} else {
			missingLeader = true;
		}
	}

	//since we already passed the leadership, we remove the player from the list
	auto it = std::find(memberList.begin(), memberList.end(), player);
	if (it != memberList.end()) {
		memberList.erase(it);
	}

	player->setParty(nullptr);
	player->sendClosePrivate(CHANNEL_PARTY);
	g_game.updatePlayerShield(player);
	g_game.updatePlayerHelpers(*player);

	for (Player* member : memberList) {
		member->sendCreatureSkull(player);
		player->sendPlayerPartyIcons(member);
		g_game.updatePlayerHelpers(*member);
	}

	leader->sendCreatureSkull(player);
	player->sendCreatureSkull(player);
	player->sendPlayerPartyIcons(leader);

	player->sendTextMessage(MSG_INFO_DESCR, "You have left the party.");

	updateSharedExperience();
	clearPlayerPoints(player);

	std::ostringstream ss;
	ss << player->getName() << " has left the party.";
	broadcastPartyMessage(MSG_INFO_DESCR, ss.str());

	if (missingLeader || empty()) {
		disband();
	}

	return true;
}

bool Party::passPartyLeadership(Player* player)
{
	if (!player || leader == player || player->getParty() != this) {
		return false;
	}

	//Remove it before to broadcast the message correctly
	auto it = std::find(memberList.begin(), memberList.end(), player);
	if (it != memberList.end()) {
		memberList.erase(it);
	}

	std::ostringstream ss;
	ss << player->getName() << " is now the leader of the party.";
	broadcastPartyMessage(MSG_INFO_DESCR, ss.str(), true);

	Player* oldLeader = leader;
	setLeader(player);

	memberList.insert(memberList.begin(), oldLeader);

	updateSharedExperience();

	for (Player* member : memberList) {
		member->sendCreatureShield(oldLeader);
		member->sendCreatureShield(leader);
	}

	for (Player* invitee : inviteList) {
		invitee->sendCreatureShield(oldLeader);
		invitee->sendCreatureShield(leader);
	}

	leader->sendCreatureShield(oldLeader);
	leader->sendCreatureShield(leader);

	player->sendTextMessage(MSG_INFO_DESCR, "You are now the leader of the party.");
	return true;
}

bool Party::joinParty(Player& player)
{
	if (!g_events->eventPartyOnJoin(this, &player)) {
		return false;
	}

	auto it = std::find(inviteList.begin(), inviteList.end(), &player);
	if (it == inviteList.end()) {
		return false;
	}

	inviteList.erase(it);

	std::ostringstream ss;
	ss << player.getName() << " has joined the party.";
	broadcastPartyMessage(MSG_INFO_DESCR, ss.str());

	player.setParty(this);

	g_game.updatePlayerShield(&player);

	for (Player* member : memberList) {
		member->sendCreatureSkull(&player);
		player.sendPlayerPartyIcons(member);
	}

	player.sendCreatureSkull(&player);
	leader->sendCreatureSkull(&player);
	player.sendPlayerPartyIcons(leader);

	memberList.push_back(&player);

	g_game.updatePlayerHelpers(player);

	player.removePartyInvitation(this);
	updateSharedExperience();

	const std::string& leaderName = leader->getName();
	ss.str("");
	ss << "You have joined " << leaderName << "'" << (leaderName.back() == 's' ? "" : "s") <<
	   " party. Open the party channel to communicate with your companions.";
	player.sendTextMessage(MSG_INFO_DESCR, ss.str());
	return true;
}

bool Party::removeInvite(Player& player, bool removeFromPlayer/* = true*/)
{
	auto it = std::find(inviteList.begin(), inviteList.end(), &player);
	if (it == inviteList.end()) {
		return false;
	}

	inviteList.erase(it);

	leader->sendCreatureShield(&player);
	player.sendCreatureShield(leader);

	if (removeFromPlayer) {
		player.removePartyInvitation(this);
	}

	if (empty()) {
		disband();
	} else {
		for (Player* member : memberList) {
			g_game.updatePlayerHelpers(*member);
		}

		g_game.updatePlayerHelpers(*leader);
	}

	return true;
}

void Party::revokeInvitation(Player& player)
{
	std::ostringstream ss;
	ss << leader->getName() << " has revoked " << (leader->getSex() == PLAYERSEX_FEMALE ? "her" : "his") << " invitation.";
	player.sendTextMessage(MSG_INFO_DESCR, ss.str());

	ss.str("");
	ss << "Invitation for " << player.getName() << " has been revoked.";
	leader->sendTextMessage(MSG_INFO_DESCR, ss.str());

	removeInvite(player);
}

bool Party::invitePlayer(Player& player)
{
	if (isPlayerInvited(&player)) {
		return false;
	}

	std::ostringstream ss;
	ss << player.getName() << " has been invited.";

	if (memberList.empty() && inviteList.empty()) {
		ss << " Open the party channel to communicate with your members.";
		g_game.updatePlayerShield(leader);
		leader->sendCreatureSkull(leader);
	}

	leader->sendTextMessage(MSG_INFO_DESCR, ss.str());

	inviteList.push_back(&player);

	for (Player* member : memberList) {
		g_game.updatePlayerHelpers(*member);
	}
	g_game.updatePlayerHelpers(*leader);

	leader->sendCreatureShield(&player);
	player.sendCreatureShield(leader);

	player.addPartyInvitation(this);

	ss.str("");
	ss << leader->getName() << " has invited you to " << (leader->getSex() == PLAYERSEX_FEMALE ? "her" : "his") << " party.";
	player.sendTextMessage(MSG_INFO_DESCR, ss.str());
	return true;
}

bool Party::isPlayerInvited(const Player* player) const
{
	return std::find(inviteList.begin(), inviteList.end(), player) != inviteList.end();
}

void Party::updateAllPartyIcons()
{
	for (Player* member : memberList) {
		for (Player* otherMember : memberList) {
			member->sendCreatureShield(otherMember);
		}

		member->sendCreatureShield(leader);
		leader->sendCreatureShield(member);
	}
	leader->sendCreatureShield(leader);
}

void Party::broadcastPartyMessage(MessageClasses msgClass, const std::string& msg, bool sendToInvitations /*= false*/)
{
	for (Player* member : memberList) {
		member->sendTextMessage(msgClass, msg);
	}

	leader->sendTextMessage(msgClass, msg);

	if (sendToInvitations) {
		for (Player* invitee : inviteList) {
			invitee->sendTextMessage(msgClass, msg);
		}
	}
}

void Party::broadcastPartyLoot(const std::string& loot)
{
	leader->sendTextMessage(MSG_INFO_DESCR, loot);

	for (Player* member : memberList) {
		member->sendTextMessage(MSG_INFO_DESCR, loot);
	}
}

void Party::updateSharedExperience()
{
	if (sharedExpActive) {
		bool result = canEnableSharedExperience();

		if (result != sharedExpEnabled) {
			sharedExpEnabled = result;
			updateAllPartyIcons();
		}
	}
}

bool Party::setSharedExperience(Player* player, bool _sharedExpActive)
{
	if (!player || leader != player) {
		return false;
	}

	if (sharedExpActive == _sharedExpActive) {
		return true;
	}

	sharedExpActive = _sharedExpActive;

	if (sharedExpActive) {
		sharedExpEnabled = canEnableSharedExperience();

		if (sharedExpEnabled) {
			leader->sendTextMessage(MSG_INFO_DESCR, "Shared Experience is now active.");
		} else {
			leader->sendTextMessage(MSG_INFO_DESCR, "Shared Experience has been activated, but some members of your party are inactive.");
		}
	} else {
		leader->sendTextMessage(MSG_INFO_DESCR, "Shared Experience has been deactivated.");
	}

	updateAllPartyIcons();
	return true;
}

void Party::shareExperience(uint64_t experience)
{
	uint32_t shareExperience = (uint64_t)std::ceil((((double)experience / (memberList.size() + 1)) + ((double)experience * 0.05)));
	for (Player* member : memberList) {
		member->onGainSharedExperience(shareExperience);
	}
	leader->onGainSharedExperience(shareExperience);
}

bool Party::canUseSharedExperience(const Player* player) const
{
	if (memberList.empty()) {
		return false;
	}

	uint32_t highestLevel = leader->getLevel();
	for (Player* member : memberList) {
		if (member->getLevel() > highestLevel) {
			highestLevel = member->getLevel();
		}
	}

	uint32_t minLevel = (int32_t)std::ceil(((float)(highestLevel) * 2) / 3);
	if (player->getLevel() < minLevel) {
		return false;
	}

	const Position& leaderPos = leader->getPosition();
	const Position& memberPos = player->getPosition();
	if (!Position::areInRange<30, 30, 1>(leaderPos, memberPos)) {
		return false;
	}

	if (!player->hasFlag(PlayerFlag_NotGainInFight)) {
		//check if the player has healed/attacked anything recently
		auto it = pointMap.find(player->getID());
		if (it == pointMap.end()) {
			return false;
		}

		uint64_t timeDiff = OTSYS_TIME() - it->second.ticks;
		if (timeDiff > (uint64_t)g_config.getNumber(ConfigManager::PZ_LOCKED)) {
			return false;
		}
	}
	return true;
}

bool Party::canEnableSharedExperience()
{
	if (!canUseSharedExperience(leader)) {
		return false;
	}

	for (Player* member : memberList) {
		if (!canUseSharedExperience(member)) {
			return false;
		}
	}
	return true;
}

void Party::addPlayerHealedMember(Player* player, uint32_t points)
{
	if (!player->hasFlag(PlayerFlag_NotGainInFight)) {
		if (points > 0) {
			auto it = pointMap.find(player->getID());
			if (it == pointMap.end()) {
				CountBlock_t cb;
				cb.ticks = OTSYS_TIME();
				cb.totalHeal = points;
				cb.totalDamage = 0;
				pointMap[player->getID()] = cb;
			} else {
				it->second.totalHeal += points;
				it->second.ticks = OTSYS_TIME();
			}
			updateSharedExperience();
		}
	}
}

void Party::addPlayerDamageMonster(Player* player, uint32_t points)
{
	if (!player->hasFlag(PlayerFlag_NotGainInFight)) {
		if (points > 0) {
			CountMap::iterator it = pointMap.find(player->getID());
			if (it == pointMap.end()) {
				CountBlock_t cb;
				cb.ticks = OTSYS_TIME();
				cb.totalDamage = points;
				cb.totalHeal = 0;
				pointMap[player->getID()] = cb;
			} else {
				it->second.totalDamage += points;
				it->second.ticks = OTSYS_TIME();
			}
			updateSharedExperience();
		}
	}
}

void Party::clearPlayerPoints(Player* player)
{
	auto it = pointMap.find(player->getID());
	if (it != pointMap.end()) {
		pointMap.erase(it);
		updateSharedExperience();
	}
}

bool Party::canOpenCorpse(uint32_t ownerId) const
{
	if (Player* player = g_game.getPlayerByID(ownerId)) {
		return leader->getID() == ownerId || player->getParty() == this;
	}
	return false;
}
