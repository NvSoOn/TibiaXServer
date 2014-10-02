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

#ifndef FS_PLAYER_H_4083D3D3A05B4EDE891B31BB720CD06F
#define FS_PLAYER_H_4083D3D3A05B4EDE891B31BB720CD06F

#include "creature.h"
#include "container.h"
#include "cylinder.h"
#include "outfit.h"
#include "enums.h"
#include "vocation.h"
#include "protocolgame.h"
#include "ioguild.h"
#include "party.h"
#include "inbox.h"
#include "depotchest.h"
#include "depotlocker.h"
#include "guild.h"
#include "groups.h"
#include "town.h"
#include "mounts.h"

class House;
class NetworkMessage;
class Weapon;
class ProtocolGame;
class Npc;
class Party;
class SchedulerTask;
class Bed;
class Guild;

enum skillsid_t {
	SKILL_LEVEL = 0,
	SKILL_TRIES = 1,
	SKILL_PERCENT = 2
};

enum playerinfo_t {
	PLAYERINFO_LEVEL,
	PLAYERINFO_LEVELPERCENT,
	PLAYERINFO_HEALTH,
	PLAYERINFO_MAXHEALTH,
	PLAYERINFO_MANA,
	PLAYERINFO_MAXMANA,
	PLAYERINFO_MAGICLEVEL,
	PLAYERINFO_MAGICLEVELPERCENT,
	PLAYERINFO_SOUL,
};

enum freeslot_t {
	SLOT_TYPE_NONE,
	SLOT_TYPE_INVENTORY,
	SLOT_TYPE_CONTAINER
};

enum chaseMode_t {
	CHASEMODE_STANDSTILL,
	CHASEMODE_FOLLOW,
};

enum fightMode_t {
	FIGHTMODE_ATTACK,
	FIGHTMODE_BALANCED,
	FIGHTMODE_DEFENSE
};

enum secureMode_t {
	SECUREMODE_ON,
	SECUREMODE_OFF
};

enum tradestate_t {
	TRADE_NONE,
	TRADE_INITIATED,
	TRADE_ACCEPT,
	TRADE_ACKNOWLEDGE,
	TRADE_TRANSFER
};

struct VIPEntry {
	VIPEntry(uint32_t guid, const std::string& name, const std::string& description, uint32_t icon, bool notify)
		: guid(guid), name(name), description(description), icon(icon), notify(notify) {}

	uint32_t guid;
	std::string name;
	std::string description;
	uint32_t icon;
	bool notify;
};

struct OpenContainer {
	Container* container;
	uint16_t index;
};

struct OutfitEntry {
	OutfitEntry(uint16_t lookType, uint8_t addons) : lookType(lookType), addons(addons) {}

	uint16_t lookType;
	uint8_t addons;
};

typedef std::map<uint32_t, uint32_t> MuteCountMap;
typedef std::vector<uint32_t> GuildWarList;

#define PLAYER_MAX_SPEED 1500
#define PLAYER_MIN_SPEED 10

class Player : public Creature, public Cylinder
{
	public:
#ifdef ENABLE_SERVER_DIAGNOSTIC
		static uint32_t playerCount;
#endif
		Player(ProtocolGame* p);
		virtual ~Player();

		virtual Player* getPlayer() {
			return this;
		}
		virtual const Player* getPlayer() const {
			return this;
		}

		void setID() {
			if (this->id == 0) {
				this->id = playerAutoID++;
			}
		}

		static MuteCountMap muteCountMap;

		virtual const std::string& getName() const {
			return name;
		}
		void setName(const std::string& name) {
			this->name = name;
		}
		virtual const std::string& getNameDescription() const {
			return name;
		}
		virtual std::string getDescription(int32_t lookDistance) const;

		virtual CreatureType_t getType() const {
			return CREATURETYPE_PLAYER;
		}

		uint8_t getCurrentMount() const;
		void setCurrentMount(uint8_t mountId);
		bool isMounted() const {
			return defaultOutfit.lookMount != 0;
		}
		bool toggleMount(bool mount);
		bool tameMount(uint8_t mountId);
		bool untameMount(uint8_t mountId);
		bool hasMount(const Mount* mount) const;
		void dismount();

		void sendFYIBox(const std::string& message) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendFYIBox(message);
				}
			}
		}

		void setGUID(uint32_t _guid) {
			guid = _guid;
		}
		uint32_t getGUID() const {
			return guid;
		}
		virtual bool canSeeInvisibility() const {
			return hasFlag(PlayerFlag_CanSenseInvisibility) || group->access;
		}

		void removeList();
		void addList();
		void kickPlayer(bool displayEffect);

		static uint64_t getExpForLevel(int32_t lv) {
			lv--;
			return ((50ULL * lv * lv * lv) - (150ULL * lv * lv) + (400ULL * lv)) / 3ULL;
		}

		uint16_t getStaminaMinutes() const {
			return staminaMinutes;
		}
		void regenerateStamina(int32_t offlineTime);
		void useStamina();

		bool addOfflineTrainingTries(skills_t skill, int32_t tries);

		void addOfflineTrainingTime(int32_t addTime) {
			offlineTrainingTime = std::min<int32_t>(12 * 3600 * 1000, offlineTrainingTime + addTime);
		}
		void removeOfflineTrainingTime(int32_t removeTime) {
			offlineTrainingTime = std::max<int32_t>(0, offlineTrainingTime - removeTime);
		}
		int32_t getOfflineTrainingTime() const {
			return offlineTrainingTime;
		}

		int32_t getOfflineTrainingSkill() const {
			return offlineTrainingSkill;
		}
		void setOfflineTrainingSkill(int32_t skill) {
			offlineTrainingSkill = skill;
		}

		uint64_t getBankBalance() const {
			return bankBalance;
		}
		void setBankBalance(uint64_t balance) {
			bankBalance = balance;
		}

		Guild* getGuild() const {
			return guild;
		}
		void setGuild(Guild* guild) {
			this->guild = guild;
		}

		int8_t getGuildLevel() const {
			return guildLevel;
		}
		void setGuildLevel(uint8_t newGuildLevel);

		bool isGuildMate(const Player* player) const;

		const std::string& getGuildNick() const {
			return guildNick;
		}
		void setGuildNick(const std::string& nick) {
			guildNick = nick;
		}

		bool isInWar(const Player* player) const;
		bool isInWarList(uint32_t guild_id) const;

		void setLastWalkthroughAttempt(int64_t walkthroughAttempt) {
			lastWalkthroughAttempt = walkthroughAttempt;
		}
		void setLastWalkthroughPosition(const Position& walkthroughPosition) {
			lastWalkthroughPosition = walkthroughPosition;
		}

		bool hasRequestedOutfit() const {
			return requestedOutfit;
		}
		void hasRequestedOutfit(bool newValue) {
			requestedOutfit = newValue;
		}

		Inbox* getInbox() const {
			return inbox;
		}

		uint16_t getClientIcons() const;

		const GuildWarList& getGuildWarList() const {
			return guildWarList;
		}
		void setGuildWarList(const GuildWarList& _guildWarList) {
			guildWarList = _guildWarList;
		}

		Vocation* getVocation() const {
			return vocation;
		}

		OperatingSystem_t getOperatingSystem() const {
			return operatingSystem;
		}
		void setOperatingSystem(OperatingSystem_t clientos) {
			operatingSystem = clientos;
		}

		uint16_t getProtocolVersion() const {
			if (clients.size() == 0) {
				return 0;
			}

			return clients.front()->getVersion();
		}

		secureMode_t getSecureMode() const {
			return secureMode;
		}

		void setParty(Party* _party) {
			party = _party;
		}
		Party* getParty() const {
			return party;
		}
		PartyShields_t getPartyShield(const Player* player) const;
		bool isInviting(const Player* player) const;
		bool isPartner(const Player* player) const;
		void sendPlayerPartyIcons(Player* player);
		bool addPartyInvitation(Party* party);
		bool removePartyInvitation(Party* party);
		void clearPartyInvitations();

		GuildEmblems_t getGuildEmblem(const Player* player) const;

		uint64_t getSpentMana() const {
			return manaSpent;
		}

		bool hasFlag(PlayerFlags value) const {
			return (0 != (group->flags & ((uint64_t)1 << value)));
		}

		BedItem* getBedItem() {
			return bedItem;
		}
		void setBedItem(BedItem* b) {
			bedItem = b;
		}

		void addBlessing(int16_t blessing) {
			blessings |= blessing;
		}
		void removeBlessing(int16_t blessing) {
			blessings &= ~blessing;
		}
		bool hasBlessing(int16_t value) const {
			return (0 != (blessings & ((int16_t)1 << value)));
		}

		bool isFemale() const {
			return sex == PLAYERSEX_FEMALE;
		}
		bool isMale() const {
			return sex == PLAYERSEX_MALE;
		}

		bool isOffline() const {
			return (getID() == 0);
		}
		void disconnect() {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->disconnect();
				}
			}
		}
		uint32_t getIP() const;

		void addContainer(uint8_t cid, Container* container);
		void closeContainer(uint8_t cid);
		void setContainerIndex(uint8_t cid, uint16_t index);

		Container* getContainerByID(uint8_t cid);
		int8_t getContainerID(const Container* container) const;
		uint16_t getContainerIndex(uint8_t cid) const;

		bool canOpenCorpse(uint32_t ownerId) const;

		void addStorageValue(const uint32_t key, const int32_t value, const bool isLogin = false);
		bool getStorageValue(const uint32_t key, int32_t& value) const;
		void genReservedStorageRange();

		void setGroup(Group* newGroup);
		Group* getGroup() const {
			return group;
		}

		void setInMarket(bool value) {
			inMarket = value;
		}
		bool isInMarket() const {
			return inMarket;
		}

		void setLastDepotId(int16_t newId) {
			lastDepotId = newId;
		}
		int16_t getLastDepotId() const {
			return lastDepotId;
		}

		void resetIdleTime() {
			idleTime = 0;
		}
		bool getNoMove() const {
			return mayNotMove;
		}

		bool isInGhostMode() const {
			return ghostMode;
		}
		void switchGhostMode() {
			ghostMode = !ghostMode;
		}

		uint32_t getAccount() const {
			return accountNumber;
		}
		AccountType_t getAccountType() const {
			return accountType;
		}
		uint32_t getLevel() const {
			return level;
		}
		int32_t getMagicLevel() const {
			return getPlayerInfo(PLAYERINFO_MAGICLEVEL);
		}
		uint32_t getBaseMagicLevel() const {
			return magLevel;
		}
		bool isAccessPlayer() const {
			return group->access;
		}
		bool isPremium() const;
		void setPremiumDays(int32_t v);

		uint16_t getHelpers() const;

		bool setVocation(uint32_t vocId);
		uint32_t getVocationId() const {
			return vocation->getId();
		}

		PlayerSex_t getSex() const {
			return sex;
		}
		void setSex(PlayerSex_t);
		int32_t getPlayerInfo(playerinfo_t playerinfo) const;
		uint64_t getExperience() const {
			return experience;
		}

		time_t getLastLoginSaved() const {
			return lastLoginSaved;
		}

		time_t getLastLogout() const {
			return lastLogout;
		}

		const Position& getLoginPosition() const {
			return loginPosition;
		}
		const Position& getTemplePosition() const {
			return town->getTemplePosition();
		}
		Town* getTown() const {
			return town;
		}
		void setTown(Town* _town) {
			town = _town;
		}

		void clearModalWindows();
		bool hasModalWindowOpen(uint32_t modalWindowId) const;
		void onModalWindowHandled(uint32_t modalWindowId);

		virtual bool isPushable() const;
		virtual int32_t getThrowRange() const {
			return 1;
		}
		uint32_t isMuted() const;
		void addMessageBuffer();
		void removeMessageBuffer();

		bool removeItemOfType(uint16_t itemId, uint32_t amount, int32_t subType, bool ignoreEquipped = false) const;

		double getCapacity() const {
			if (hasFlag(PlayerFlag_CannotPickupItem)) {
				return 0.00;
			} else if (hasFlag(PlayerFlag_HasInfiniteCapacity)) {
				return 10000.00;
			} else {
				return capacity;
			}
		}

		double getFreeCapacity() const {
			if (hasFlag(PlayerFlag_CannotPickupItem)) {
				return 0.00;
			} else if (hasFlag(PlayerFlag_HasInfiniteCapacity)) {
				return 10000.00;
			} else {
				return std::max<double>(0.00, capacity - inventoryWeight);
			}
		}

		virtual int32_t getMaxHealth() const {
			return getPlayerInfo(PLAYERINFO_MAXHEALTH);
		}
		virtual int32_t getMaxMana() const {
			return getPlayerInfo(PLAYERINFO_MAXMANA);
		}

		Item* getInventoryItem(slots_t slot) const;

		bool isItemAbilityEnabled(slots_t slot) const {
			return inventoryAbilities[slot];
		}
		void setItemAbility(slots_t slot, bool enabled) {
			inventoryAbilities[slot] = enabled;
		}

		int32_t getBaseSkill(skills_t skill) const {
			return skills[skill][SKILL_LEVEL];
		}

		int32_t getVarSkill(skills_t skill) const {
			return varSkills[skill];
		}
		void setVarSkill(skills_t skill, int32_t modifier) {
			varSkills[skill] += modifier;
		}

		int32_t getVarStats(stats_t stat) const {
			return varStats[stat];
		}
		void setVarStats(stats_t stat, int32_t modifier);
		int32_t getDefaultStats(stats_t stat) const;

		void setConditionSuppressions(uint32_t conditions, bool remove);

		DepotChest* getDepotChest(uint32_t depotId, bool autoCreate);
		DepotLocker* getDepotLocker(uint32_t depotId);
		void onReceiveMail();
		bool isNearDepotBox() const;

		virtual bool canSee(const Position& pos) const;
		virtual bool canSeeCreature(const Creature* creature) const;

		bool canWalkthrough(const Creature* creature) const;
		bool canWalkthroughEx(const Creature* creature) const;

		virtual RaceType_t getRace() const {
			return RACE_BLOOD;
		}

		uint64_t getMoney() const;

		//safe-trade functions
		void setTradeState(tradestate_t state) {
			tradeState = state;
		}
		tradestate_t getTradeState() const {
			return tradeState;
		}
		Item* getTradeItem() {
			return tradeItem;
		}

		//shop functions
		void setShopOwner(Npc* owner, int32_t onBuy, int32_t onSell) {
			shopOwner = owner;
			purchaseCallback = onBuy;
			saleCallback = onSell;
		}

		Npc* getShopOwner(int32_t& onBuy, int32_t& onSell) {
			onBuy = purchaseCallback;
			onSell = saleCallback;
			return shopOwner;
		}

		const Npc* getShopOwner(int32_t& onBuy, int32_t& onSell) const {
			onBuy = purchaseCallback;
			onSell = saleCallback;
			return shopOwner;
		}

		//V.I.P. functions
		void notifyStatusChange(Player* player, VipStatus_t status);
		bool removeVIP(uint32_t guid);
		bool addVIP(uint32_t guid, const std::string& name, VipStatus_t status);
		bool addVIPInternal(uint32_t guid);
		bool editVIP(uint32_t _guid, const std::string& description, uint32_t icon, bool notify);

		//follow functions
		virtual bool setFollowCreature(Creature* creature, bool fullPathSearch = false);
		virtual void goToFollowCreature();

		//follow events
		virtual void onFollowCreature(const Creature* creature);

		//walk events
		virtual void onWalk(Direction& dir);
		virtual void onWalkAborted();
		virtual void onWalkComplete();

		void stopWalk();
		void openShopWindow(Npc* npc, const std::list<ShopInfo>& shop);
		bool closeShopWindow(bool sendCloseShopWindow = true);
		void updateSaleShopList(uint32_t itemId);
		bool hasShopItemForSale(uint32_t itemId, uint8_t subType) const;

		void setChaseMode(chaseMode_t mode);
		void setFightMode(fightMode_t mode);
		void setSecureMode(secureMode_t mode) {
			secureMode = mode;
		}

		//combat functions
		virtual bool setAttackedCreature(Creature* creature);
		bool isImmune(CombatType_t type) const;
		bool isImmune(ConditionType_t type) const;
		bool hasShield() const;
		virtual bool isAttackable() const;

		virtual void changeHealth(int32_t healthChange, bool sendHealthChange = true);
		virtual void changeMana(int32_t manaChange);
		void changeSoul(int32_t soulChange);

		bool isPzLocked() const {
			return pzLocked;
		}
		virtual BlockType_t blockHit(Creature* attacker, CombatType_t combatType, int32_t& damage,
		                             bool checkDefense = false, bool checkArmor = false);
		virtual void doAttacking(uint32_t interval);
		virtual bool hasExtraSwing() {
			return lastAttack > 0 && ((OTSYS_TIME() - lastAttack) >= getAttackSpeed());
		}
		int32_t getShootRange() const {
			return shootRange;
		}

		int32_t getSkill(skills_t skilltype, skillsid_t skillinfo) const;
		bool getAddAttackSkill() const {
			return addAttackSkillPoint;
		}
		BlockType_t getLastAttackBlockType() const {
			return lastAttackBlockType;
		}

		Item* getWeapon(bool ignoreAmmo = false);
		virtual WeaponType_t getWeaponType();
		int32_t getWeaponSkill(const Item* item) const;
		void getShieldAndWeapon(const Item*& shield, const Item*& weapon) const;

		virtual void drainHealth(Creature* attacker, int32_t damage);
		virtual void drainMana(Creature* attacker, int32_t manaLoss);
		void addManaSpent(uint64_t amount);
		void addSkillAdvance(skills_t skill, uint32_t count);

		virtual int32_t getArmor() const;
		virtual int32_t getDefense() const;
		virtual float getAttackFactor() const;
		virtual float getDefenseFactor() const;

		void addInFightTicks(bool pzlock = false);

		virtual uint64_t getGainedExperience(Creature* attacker) const;

		//combat event functions
		virtual void onAddCondition(ConditionType_t type);
		virtual void onAddCombatCondition(ConditionType_t type);
		virtual void onEndCondition(ConditionType_t type);
		virtual void onCombatRemoveCondition(const Creature* attacker, Condition* condition);
		virtual void onAttackedCreature(Creature* target);
		virtual void onAttacked();
		virtual void onAttackedCreatureDrainHealth(Creature* target, int32_t points);
		virtual void onTargetCreatureGainHealth(Creature* target, int32_t points);
		virtual bool onKilledCreature(Creature* target, bool lastHit = true);
		virtual void onGainExperience(uint64_t gainExp, Creature* target);
		virtual void onGainSharedExperience(uint64_t gainExp);
		virtual void onAttackedCreatureBlockHit(Creature* target, BlockType_t blockType);
		virtual void onBlockHit(BlockType_t blockType);
		virtual void onChangeZone(ZoneType_t zone);
		virtual void onAttackedCreatureChangeZone(ZoneType_t zone);
		virtual void onIdleStatus();
		virtual void onPlacedCreature();
		virtual void onRemovedCreature();

		virtual void getCreatureLight(LightInfo& light) const;

		Skulls_t getSkull() const;
		Skulls_t getSkullClient(const Player* player) const;
		int64_t getSkullTicks() const { return skullTicks; }
		void setSkullTicks(int64_t ticks) { skullTicks = ticks; }

		bool hasAttacked(const Player* attacked) const;
		void addAttacked(const Player* attacked);
		void clearAttacked();
		void addUnjustifiedDead(const Player* attacked);
		void setSkull(Skulls_t newSkull) {
			skull = newSkull;
		}
		void sendCreatureSkull(const Creature* creature) const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendCreatureSkull(creature);
				}
			}
		}
		void checkSkullTicks(int32_t ticks);

		bool canWear(uint32_t lookType, uint8_t addons) const;
		void addOutfit(uint16_t lookType, uint8_t addons);
		bool removeOutfit(uint16_t lookType);
		bool removeOutfitAddon(uint16_t lookType, uint8_t addons);
		bool getOutfitAddons(const Outfit& outfit, uint8_t& addons) const;

		bool canLogout();

		//tile
		//send methods
		void sendAddTileItem(const Tile* tile, const Position& pos, const Item* item) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendAddTileItem(tile, pos, tile->getClientIndexOfThing(this, item), item);
				}
			}
		}
		void sendUpdateTileItem(const Tile* tile, const Position& pos, const Item* olditem, const Item* newitem) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendUpdateTileItem(tile, pos, tile->getClientIndexOfThing(this, newitem), newitem);
				}
			}
		}
		void sendRemoveTileItem(const Tile* tile, const Position& pos, uint32_t stackpos, const Item* item) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendRemoveTileItem(tile, pos, stackpos);
				}
			}
		}
		void sendUpdateTile(const Tile* tile, const Position& pos) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendUpdateTile(tile, pos);
				}
			}
		}

		void sendChannelMessage(const std::string& author, const std::string& text, SpeakClasses type, uint16_t channel) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendChannelMessage(author, text, type, channel);
				}
			}
		}
		void sendChannelEvent(uint16_t channelId, const std::string& playerName, ChannelEvent_t channelEvent) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendChannelEvent(channelId, playerName, channelEvent);
				}
			}
		}
		void sendCreatureAppear(const Creature* creature, const Position& pos, bool isLogin) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendAddCreature(creature, pos, creature->getTile()->getClientIndexOfThing(this, creature), isLogin);
				}
			}
		}
		void sendCreatureDisappear(const Creature* creature, uint32_t stackpos) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendRemoveCreature(creature, creature->getPosition(), stackpos);
				}
			}
		}
		void sendCreatureMove(const Creature* creature, const Tile* newTile, const Position& newPos,
		                      const Tile* oldTile, const Position& oldPos, uint32_t oldStackPos, bool teleport) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendMoveCreature(creature, newTile, newPos, newTile->getClientIndexOfThing(this, creature), oldTile, oldPos, oldStackPos, teleport);
				}
			}
		}
		void sendCreatureTurn(const Creature* creature) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendCreatureTurn(creature, creature->getTile()->getClientIndexOfThing(this, creature));
				}
			}
		}
		void sendCreatureSay(const Creature* creature, SpeakClasses type, const std::string& text, Position* pos = nullptr) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendCreatureSay(creature, type, text, pos);
				}
			}
		}
		void sendCreatureSquare(const Creature* creature, SquareColor_t color) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendCreatureSquare(creature, color);
				}
			}
		}
		void sendCreatureChangeOutfit(const Creature* creature, const Outfit_t& outfit) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendCreatureOutfit(creature, outfit);
				}
			}
		}
		void sendCreatureChangeVisible(const Creature* creature, bool visible) {
			if (clients.size() > 0) {
				if (creature->getPlayer()) {
					if (visible) {
						for (auto& client : clients) {
							client->sendCreatureOutfit(creature, creature->getCurrentOutfit());
						}
					} else {
						static Outfit_t outfit;
						for (auto& client : clients) {
							client->sendCreatureOutfit(creature, outfit);
						}
					}
				} else {
					if (canSeeInvisibility()) {
						for (auto& client : clients) {
							client->sendCreatureOutfit(creature, creature->getCurrentOutfit());
						}
					} else {
						if (visible) {
							for (auto& client : clients) {
								client->sendAddCreature(creature, creature->getPosition(), creature->getTile()->getClientIndexOfThing(this, creature), false);
							}
						} else {
							for (auto& client : clients) {
								client->sendRemoveCreature(creature, creature->getPosition(), creature->getTile()->getClientIndexOfThing(this, creature));
							}
						}
					}
				}
			}
		}
		void sendCreatureLight(const Creature* creature) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendCreatureLight(creature);
				}
			}
		}
		void sendCreatureWalkthrough(const Creature* creature, bool walkthrough) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendCreatureWalkthrough(creature, walkthrough);
				}
			}
		}
		void sendCreatureShield(const Creature* creature) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendCreatureShield(creature);
				}
			}
		}
		void sendCreatureType(uint32_t creatureId, uint8_t creatureType) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendCreatureType(creatureId, creatureType);
				}
			}
		}
		void sendCreatureHelpers(uint32_t creatureId, uint16_t helpers) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendCreatureHelpers(creatureId, helpers);
				}
			}
		}
		void sendSpellCooldown(uint8_t spellId, uint32_t time) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendSpellCooldown(spellId, time);
				}
			}
		}
		void sendSpellGroupCooldown(SpellGroup_t groupId, uint32_t time) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendSpellGroupCooldown(groupId, time);
				}
			}
		}

		void sendDamageMessage(MessageClasses mclass, const std::string& message, const Position& pos,
		                       uint32_t primaryDamage = 0, TextColor_t primaryColor = TEXTCOLOR_NONE,
		                       uint32_t secondaryDamage = 0, TextColor_t secondaryColor = TEXTCOLOR_NONE) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendDamageMessage(mclass, message, pos, primaryDamage, primaryColor, secondaryDamage, secondaryColor);
				}
			}
		}
		void sendHealMessage(MessageClasses mclass, const std::string& message, const Position& pos, uint32_t heal, TextColor_t color) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendHealMessage(mclass, message, pos, heal, color);
				}
			}
		}
		void sendExperienceMessage(MessageClasses mclass, const std::string& message, const Position& pos, uint32_t exp, TextColor_t color) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendExperienceMessage(mclass, message, pos, exp, color);
				}
			}
		}
		void sendModalWindow(const ModalWindow& modalWindow);

		//container
		void sendAddContainerItem(const Container* container, const Item* item);
		void sendUpdateContainerItem(const Container* container, uint16_t slot, const Item* oldItem, const Item* newItem);
		void sendRemoveContainerItem(const Container* container, uint16_t slot);
		void sendContainer(uint8_t cid, const Container* container, bool hasParent, uint16_t firstIndex) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendContainer(cid, container, hasParent, firstIndex);
				}
			}
		}

		//inventory
		void sendInventoryItem(slots_t slot, const Item* item) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendInventoryItem(slot, item);
				}
			}
		}

		//event methods
		virtual void onUpdateTileItem(const Tile* tile, const Position& pos, const Item* oldItem,
		                              const ItemType& oldType, const Item* newItem, const ItemType& newType);
		virtual void onRemoveTileItem(const Tile* tile, const Position& pos, const ItemType& iType,
		                              const Item* item);

		virtual void onCreatureAppear(const Creature* creature, bool isLogin);
		virtual void onCreatureDisappear(const Creature* creature, uint32_t stackpos, bool isLogout);
		virtual void onCreatureMove(const Creature* creature, const Tile* newTile, const Position& newPos,
		                            const Tile* oldTile, const Position& oldPos, bool teleport);

		virtual void onAttackedCreatureDisappear(bool isLogout);
		virtual void onFollowCreatureDisappear(bool isLogout);

		//container
		void onAddContainerItem(const Container* container, const Item* item);
		void onUpdateContainerItem(const Container* container, uint16_t slot,
		                           const Item* oldItem, const ItemType& oldType, const Item* newItem, const ItemType& newType);
		void onRemoveContainerItem(const Container* container, uint16_t slot, const Item* item);

		void onCloseContainer(const Container* container);
		void onSendContainer(const Container* container);
		void autoCloseContainers(const Container* container);

		//inventory
		void onAddInventoryItem(slots_t slot, Item* item);
		void onUpdateInventoryItem(slots_t slot, Item* oldItem, const ItemType& oldType,
		                           Item* newItem, const ItemType& newType);
		void onRemoveInventoryItem(slots_t slot, Item* item);

		void sendCancel(const std::string& msg) const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendTextMessage(MSG_STATUS_SMALL, msg);
				}
			}
		}
		void sendCancelMessage(ReturnValue message) const;
		void sendCancelTarget() const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendCancelTarget();
				}
			}
		}
		void sendCancelWalk() const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendCancelWalk();
				}
			}
		}
		void sendChangeSpeed(const Creature* creature, uint32_t newSpeed) const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendChangeSpeed(creature, newSpeed);
				}
			}
		}
		void sendCreatureHealth(const Creature* creature) const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendCreatureHealth(creature);
				}
			}
		}
		void sendDistanceShoot(const Position& from, const Position& to, unsigned char type) const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendDistanceShoot(from, to, type);
				}
			}
		}
		void sendHouseWindow(House* house, uint32_t listId) const;
		void sendCreatePrivateChannel(uint16_t channelId, const std::string& channelName) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendCreatePrivateChannel(channelId, channelName);
				}
			}
		}
		void sendClosePrivate(uint16_t channelId);
		void sendIcons() const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendIcons(getClientIcons());
				}
			}
		}
		void sendMagicEffect(const Position& pos, uint8_t type) const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendMagicEffect(pos, type);
				}
			}
		}
		void sendPing();
		void sendPingBack() const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendPingBack();
				}
			}
		}
		void sendStats();
		void sendBasicData() const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendBasicData();
				}
			}
		}
		void sendSkills() const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendSkills();
				}
			}
		}
		void sendTextMessage(MessageClasses mclass, const std::string& message, Position* pos = nullptr, uint32_t value = 0, TextColor_t color = TEXTCOLOR_NONE) const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendTextMessage(mclass, message, pos, value, color);
				}
			}
		}
		void sendTextMessage(const TextMessage& message) const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendTextMessage(message);
				}
			}
		}
		void sendReLoginWindow(uint8_t unfairFightReduction) const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendReLoginWindow(unfairFightReduction);
				}
			}
		}
		void sendTextWindow(Item* item, uint16_t maxlen, bool canWrite) const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendTextWindow(windowTextId, item, maxlen, canWrite);
				}
			}
		}
		void sendTextWindow(uint32_t itemId, const std::string& text) const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendTextWindow(windowTextId, itemId, text);
				}
			}
		}
		void sendToChannel(const Creature* creature, SpeakClasses type, const std::string& text, uint16_t channelId) const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendToChannel(creature, type, text, channelId);
				}
			}
		}
		void sendShop(Npc* npc) const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendShop(npc, shopItemList);
				}
			}
		}
		void sendSaleItemList() const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendSaleItemList(shopItemList);
				}
			}
		}
		void sendCloseShop() const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendCloseShop();
				}
			}
		}
		void sendMarketEnter(uint32_t depotId) const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendMarketEnter(depotId);
				}
			}
		}
		void sendMarketLeave() {
			inMarket = false;
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendMarketLeave();
				}
			}
		}
		void sendMarketBrowseItem(uint16_t itemId, const MarketOfferList& buyOffers, const MarketOfferList& sellOffers) const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendMarketBrowseItem(itemId, buyOffers, sellOffers);
				}
			}
		}
		void sendMarketBrowseOwnOffers(const MarketOfferList& buyOffers, const MarketOfferList& sellOffers) const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendMarketBrowseOwnOffers(buyOffers, sellOffers);
				}
			}
		}
		void sendMarketBrowseOwnHistory(const HistoryMarketOfferList& buyOffers, const HistoryMarketOfferList& sellOffers) const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendMarketBrowseOwnHistory(buyOffers, sellOffers);
				}
			}
		}
		void sendMarketDetail(uint16_t itemId) const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendMarketDetail(itemId);
				}
			}
		}
		void sendMarketAcceptOffer(const MarketOfferEx& offer) const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendMarketAcceptOffer(offer);
				}
			}
		}
		void sendMarketCancelOffer(const MarketOfferEx& offer) const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendMarketCancelOffer(offer);
				}
			}
		}
		void sendTradeItemRequest(const Player* player, const Item* item, bool ack) const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendTradeItemRequest(player, item, ack);
				}
			}
		}
		void sendTradeClose() const {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendCloseTrade();
				}
			}
		}
		void sendWorldLight(const LightInfo& lightInfo) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendWorldLight(lightInfo);
				}
			}
		}
		void sendChannelsDialog() {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendChannelsDialog();
				}
			}
		}
		void sendOpenPrivateChannel(const std::string& receiver) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendOpenPrivateChannel(receiver);
				}
			}
		}
		void sendOutfitWindow() {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendOutfitWindow();
				}
			}
		}
		void sendCloseContainer(uint8_t cid) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendCloseContainer(cid);
				}
			}
		}

		void sendChannel(uint16_t channelId, const std::string& channelName, const UsersMap* channelUsers, const InvitedMap* invitedUsers) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendChannel(channelId, channelName, channelUsers, invitedUsers);
				}
			}
		}
		void sendTutorial(uint8_t tutorialId) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendTutorial(tutorialId);
				}
			}
		}
		void sendAddMarker(const Position& pos, uint8_t markType, const std::string& desc) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendAddMarker(pos, markType, desc);
				}
			}
		}
		void sendQuestLog() {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendQuestLog();
				}
			}
		}
		void sendQuestLine(const Quest* quest) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendQuestLine(quest);
				}
			}
		}
		void sendEnterWorld() {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->sendEnterWorld();
				}
			}
		}
		void sendNetworkMessage(const NetworkMessage& message) {
			if (clients.size() > 0) {
				for (auto& client : clients) {
					client->writeToOutputBuffer(message);
				}
			}
		}

		void receivePing() {
			lastPong = OTSYS_TIME();
		}

		virtual void onThink(uint32_t interval);
		virtual void onAttacking(uint32_t interval);

		virtual void postAddNotification(Thing* thing, const Cylinder* oldParent, int32_t index, cylinderlink_t link = LINK_OWNER);
		virtual void postRemoveNotification(Thing* thing, const Cylinder* newParent, int32_t index, bool isCompleteRemoval, cylinderlink_t link = LINK_OWNER);

		void setNextAction(int64_t time) {
			if (time > nextAction) {
				nextAction = time;
			}
		}
		bool canDoAction() const {
			return nextAction <= OTSYS_TIME();
		}
		uint32_t getNextActionTime() const;

		Item* getWriteItem(uint32_t& _windowTextId, uint16_t& _maxWriteLen);
		void setWriteItem(Item* item, uint16_t _maxWriteLen = 0);

		House* getEditHouse(uint32_t& _windowTextId, uint32_t& _listId);
		void setEditHouse(House* house, uint32_t listId = 0);

		void learnInstantSpell(const std::string& name);
		void forgetInstantSpell(const std::string& name);
		bool hasLearnedInstantSpell(const std::string& name) const;

		bool isInCast() const {
			return inCast;
		}
		void setInCast(bool value);
		const std::string& getPassword() const {
			return password;
		}
		void setPassword(const std::string& value) {
			password = value;
		}

		uint8_t getViewers() const {
			return static_cast<uint8_t>(clients.size());
		}
		uint32_t getViews(bool increase = false) {
			if (increase) {
				views++;
			}

			return views;
		}

	protected:
		void checkTradeState(const Item* item);
		bool hasCapacity(const Item* item, uint32_t count) const;

		void gainExperience(uint64_t exp);
		void addExperience(uint64_t exp, bool sendText = false, bool applyStaminaChange = false);

		void updateInventoryWeight();
		void postUpdateGoods(uint32_t itemId);

		void setNextWalkActionTask(SchedulerTask* task);
		void setNextWalkTask(SchedulerTask* task);
		void setNextActionTask(SchedulerTask* task);

		void death(Creature* _lastHitCreature);
		virtual bool dropCorpse(Creature* _lastHitCreature, Creature* mostDamageCreature, bool lastHitUnjustified, bool mostDamageUnjustified);
		virtual Item* getCorpse(Creature* _lastHitCreature, Creature* mostDamageCreature);

		//cylinder implementations
		virtual ReturnValue __queryAdd(int32_t index, const Thing* thing, uint32_t count,
		                               uint32_t flags, Creature* actor = nullptr) const;
		virtual ReturnValue __queryMaxCount(int32_t index, const Thing* thing, uint32_t count, uint32_t& maxQueryCount,
		                                    uint32_t flags) const;
		virtual ReturnValue __queryRemove(const Thing* thing, uint32_t count, uint32_t flags) const;
		virtual Cylinder* __queryDestination(int32_t& index, const Thing* thing, Item** destItem,
		                                     uint32_t& flags);

		virtual void __addThing(Thing* thing);
		virtual void __addThing(int32_t index, Thing* thing);

		virtual void __updateThing(Thing* thing, uint16_t itemId, uint32_t count);
		virtual void __replaceThing(uint32_t index, Thing* thing);

		virtual void __removeThing(Thing* thing, uint32_t count);

		virtual int32_t __getIndexOfThing(const Thing* thing) const;
		virtual int32_t __getFirstIndex() const;
		virtual int32_t __getLastIndex() const;
		virtual uint32_t __getItemTypeCount(uint16_t itemId, int32_t subType = -1) const;
		virtual std::map<uint32_t, uint32_t>& __getAllItemTypeCount(std::map<uint32_t, uint32_t>& countMap) const;
		virtual Thing* __getThing(uint32_t index) const;

		virtual void __internalAddThing(Thing* thing);
		virtual void __internalAddThing(uint32_t index, Thing* thing);

		std::unordered_set<uint32_t> attackedSet;
		std::unordered_set<uint32_t> VIPList;

		std::map<uint8_t, OpenContainer> openContainers;
		std::map<uint32_t, DepotLocker*> depotLockerMap;
		std::map<uint32_t, DepotChest*> depotChests;
		std::map<uint32_t, int32_t> storageMap;

		std::vector<OutfitEntry> outfits;

		std::list<Party*> invitePartyList;
		std::list<uint32_t> modalWindows;
		std::list<ShopInfo> shopItemList;
		std::list<std::string> learnedInstantSpellList;
		std::list<ProtocolGame*> clients;
		ConditionList storedConditionList;
		GuildWarList guildWarList;

		std::string name;
		std::string guildNick;
		std::string password;

		LightInfo itemsLight;
		Position loginPosition;
		Position lastWalkthroughPosition;

		time_t lastLoginSaved;
		time_t lastLogout;
		time_t nextUseStaminaTime;

		double inventoryWeight;
		double capacity;

		uint64_t experience;
		uint64_t manaSpent;
		uint64_t lastAttack;
		uint64_t bankBalance;
		int64_t lastFailedFollow;
		int64_t skullTicks;
		int64_t lastQuestlogUpdate;
		int64_t lastWalkthroughAttempt;
		int64_t lastToggleMount;
		int64_t lastPing;
		int64_t lastPong;
		int64_t nextAction;

		BedItem* bedItem;
		Guild* guild;
		Group* group;
		Inbox* inbox;
		Item* tradeItem;
		Item* inventory[SLOT_LAST];
		Item* writeItem;
		House* editHouse;
		Npc* shopOwner;
		Party* party;
		Player* tradePartner;
		SchedulerTask* walkTask;
		Town* town;
		Vocation* vocation;

		uint32_t damageImmunities;
		uint32_t conditionImmunities;
		uint32_t conditionSuppressions;
		uint32_t level;
		uint32_t levelPercent;
		uint32_t magLevel;
		uint32_t magLevelPercent;
		uint32_t actionTaskEvent;
		uint32_t nextStepEvent;
		uint32_t walkTaskEvent;
		uint32_t MessageBufferTicks;
		uint32_t lastIP;
		uint32_t accountNumber;
		uint32_t guid;
		uint32_t windowTextId;
		uint32_t editListId;
		uint32_t maxDepotItems;
		uint32_t maxVipEntries;
		uint32_t skills[SKILL_LAST + 1][3];
		uint32_t views;
		int32_t varSkills[SKILL_LAST + 1];
		int32_t varStats[STAT_LAST + 1];
		int32_t purchaseCallback;
		int32_t saleCallback;
		int32_t MessageBufferCount;
		int32_t premiumDays;
		int32_t soul;
		int32_t soulMax;
		int32_t bloodHitCount;
		int32_t shieldBlockCount;
		int32_t offlineTrainingSkill;
		int32_t offlineTrainingTime;
		int32_t idleTime;
		int32_t shootRange;

		AccountType_t accountType;
		PlayerSex_t sex;
		Skulls_t skull;
		OperatingSystem_t operatingSystem;
		chaseMode_t chaseMode;
		fightMode_t fightMode;
		secureMode_t secureMode;
		BlockType_t lastAttackBlockType;
		tradestate_t tradeState;

		uint16_t lastStatsTrainingTime;
		uint16_t staminaMinutes;
		uint16_t maxWriteLen;
		int16_t lastDepotId;
		int16_t blessings;

		uint8_t guildLevel;

		bool mayNotMove;
		bool requestedOutfit;
		bool inMarket;
		bool ghostMode;
		bool pzLocked;
		bool isConnecting;
		bool addAttackSkillPoint;
		bool inventoryAbilities[SLOT_LAST];
		bool inCast;

		static uint32_t playerAutoID;

		void updateItemsLight(bool internal = false);
		virtual int32_t getStepSpeed() const {
			if (getSpeed() > PLAYER_MAX_SPEED) {
				return PLAYER_MAX_SPEED;
			} else if (getSpeed() < PLAYER_MIN_SPEED) {
				return PLAYER_MIN_SPEED;
			}

			return getSpeed();
		}
		void updateBaseSpeed() {
			if (!hasFlag(PlayerFlag_SetMaxSpeed)) {
				baseSpeed = vocation->getBaseSpeed() + (2 * (level - 1));
			} else {
				baseSpeed = PLAYER_MAX_SPEED;
			}
		}

		bool isPromoted() const;

		uint32_t getAttackSpeed() const {
			return vocation->getAttackSpeed();
		}

		uint16_t getDropPercent() const;

		static uint32_t getPercentLevel(uint64_t count, uint64_t nextLevelCount);
		double getLostPercent() const;
		virtual uint64_t getLostExperience() const {
			return skillLoss ? uint64_t(experience * getLostPercent()) : 0;
		}
		virtual void dropLoot(Container* corpse, Creature* _lastHitCreature);
		virtual uint32_t getDamageImmunities() const {
			return damageImmunities;
		}
		virtual uint32_t getConditionImmunities() const {
			return conditionImmunities;
		}
		virtual uint32_t getConditionSuppressions() const {
			return conditionSuppressions;
		}
		virtual uint16_t getLookCorpse() const;
		virtual void getPathSearchParams(const Creature* creature, FindPathParams& fpp) const;

		friend class Game;
		friend class Npc;
		friend class LuaScriptInterface;
		friend class Map;
		friend class Actions;
		friend class IOLoginData;
		friend class ProtocolGame;
};

#endif
