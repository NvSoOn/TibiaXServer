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

#ifndef FS_WEAPONS_H_69D1993478AA42948E24C0B90B8F5BF5
#define FS_WEAPONS_H_69D1993478AA42948E24C0B90B8F5BF5

#include "luascript.h"
#include "player.h"
#include "baseevents.h"
#include "combat.h"
#include "const.h"

class Weapon;
class WeaponMelee;
class WeaponDistance;
class WeaponWand;

class Weapons : public BaseEvents
{
	public:
		Weapons();
		virtual ~Weapons();

		void loadDefaults();
		const Weapon* getWeapon(const Item* item) const;

		static int32_t getMaxMeleeDamage(int32_t attackSkill, int32_t attackValue);
		static int32_t getMaxWeaponDamage(uint32_t level, int32_t attackSkill, int32_t attackValue, float attackFactor);

	protected:
		virtual void clear();
		virtual LuaScriptInterface& getScriptInterface();
		virtual std::string getScriptBaseName();
		virtual Event* getEvent(const std::string& nodeName);
		virtual bool registerEvent(Event* event, const pugi::xml_node& node);

		std::map<uint32_t, Weapon*> weapons;

		LuaScriptInterface m_scriptInterface;
};

class Weapon : public Event
{
	public:
		Weapon(LuaScriptInterface* _interface);
		virtual ~Weapon();

		virtual bool configureEvent(const pugi::xml_node& node);
		virtual bool loadFunction(const std::string& functionName);
		virtual bool configureWeapon(const ItemType& it);
		virtual bool interruptSwing() const {
			return false;
		}

		virtual int32_t playerWeaponCheck(Player* player, Creature* target) const;
		static bool useFist(Player* player, Creature* target);
		virtual bool useWeapon(Player* player, Item* item, Creature* target) const;

		virtual int32_t getWeaponDamage(const Player* player, const Creature* target, const Item* item, bool maxDamage = false) const = 0;
		virtual int32_t getElementDamage(const Player* player, const Creature* target, const Item* item) const = 0;
		virtual CombatType_t getElementType() const = 0;

		uint16_t getID() const {
			return id;
		}

		uint32_t getReqLevel() const {
			return level;
		}
		int32_t getReqMagLv() const {
			return magLevel;
		}
		bool isPremium() const {
			return premium;
		}
		bool isWieldedUnproperly() const {
			return wieldUnproperly;
		}

	protected:
		virtual std::string getScriptEventName();

		bool executeUseWeapon(Player* player, const LuaVariant& var) const;
		bool internalUseWeapon(Player* player, Item* item, Creature* target, int32_t damageModifier) const;
		bool internalUseWeapon(Player* player, Item* item, Tile* tile) const;

		virtual void onUsedWeapon(Player* player, Item* item, Tile* destTile) const;
		virtual void onUsedAmmo(Player* player, Item* item, Tile* destTile) const;
		virtual bool getSkillType(const Player* player, const Item* item, skills_t& skill, uint32_t& skillpoint) const {
			return false;
		}

		int32_t getManaCost(const Player* player) const;

		uint16_t id;
		bool enabled;
		bool premium;
		bool wieldUnproperly;
		int32_t level;
		int32_t magLevel;
		int32_t mana;
		int32_t manaPercent;
		int32_t soul;
		int32_t range;
		AmmoAction_t ammoAction;
		CombatParams params;

	private:
		std::map<int32_t, bool> vocWeaponMap;
		friend class Combat;
};

class WeaponMelee : public Weapon
{
	public:
		WeaponMelee(LuaScriptInterface* _interface);
		virtual ~WeaponMelee() {}

		virtual bool configureEvent(const pugi::xml_node& node);
		virtual bool configureWeapon(const ItemType& it);

		virtual bool useWeapon(Player* player, Item* item, Creature* target) const;

		int32_t getWeaponDamage(const Player* player, const Creature* target, const Item* item, bool maxDamage = false) const;
		int32_t getElementDamage(const Player* player, const Creature* target, const Item* item) const;
		CombatType_t getElementType() const { return elementType; }

	protected:
		virtual void onUsedWeapon(Player* player, Item* item, Tile* destTile) const;
		virtual void onUsedAmmo(Player* player, Item* item, Tile* destTile) const;
		virtual bool getSkillType(const Player* player, const Item* item, skills_t& skill, uint32_t& skillpoint) const;

		CombatType_t elementType;
		int16_t elementDamage;
};

class WeaponDistance : public Weapon
{
	public:
		WeaponDistance(LuaScriptInterface* _interface);
		virtual ~WeaponDistance() {}

		virtual bool configureEvent(const pugi::xml_node& node);
		virtual bool configureWeapon(const ItemType& it);
		virtual bool interruptSwing() const {
			return true;
		}

		virtual int32_t playerWeaponCheck(Player* player, Creature* target) const;
		virtual bool useWeapon(Player* player, Item* item, Creature* target) const;

		int32_t getWeaponDamage(const Player* player, const Creature* target, const Item* item, bool maxDamage = false) const;
		int32_t getElementDamage(const Player* player, const Creature* target, const Item* item) const;
		CombatType_t getElementType() const { return elementType; }

	protected:
		virtual void onUsedWeapon(Player* player, Item* item, Tile* destTile) const;
		virtual void onUsedAmmo(Player* player, Item* item, Tile* destTile) const;
		virtual bool getSkillType(const Player* player, const Item* item, skills_t& skill, uint32_t& skillpoint) const;

		int32_t hitChance;
		int32_t maxHitChance;
		int32_t breakChance;
		int32_t ammuAttackValue;

		CombatType_t elementType;
		int16_t elementDamage;
};

class WeaponWand : public Weapon
{
	public:
		WeaponWand(LuaScriptInterface* _interface);
		virtual ~WeaponWand() {}

		virtual bool configureEvent(const pugi::xml_node& node);
		virtual bool configureWeapon(const ItemType& it);

		int32_t getWeaponDamage(const Player* player, const Creature* target, const Item* item, bool maxDamage = false) const;
		int32_t getElementDamage(const Player* player, const Creature* target, const Item* item) const { return 0; }
		CombatType_t getElementType() const { return COMBAT_NONE; }

	protected:
		virtual bool getSkillType(const Player* player, const Item* item, skills_t& skill, uint32_t& skillpoint) const {
			return false;
		}

		int32_t minChange;
		int32_t maxChange;
};

#endif
