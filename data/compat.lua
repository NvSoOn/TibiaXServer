function pushThing(thing)
	local t = {uid = 0, itemid = 0, type = 0, actionid = 0}
	if thing ~= nil then
		if thing:isItem() then
			t.uid = thing:getUniqueId()
			t.itemid = thing:getId()
			if ItemType(t.itemid):hasSubType() then
				t.type = thing:getSubType()
			end
			t.actionid = thing:getActionId()
		elseif thing:isCreature() then
			t.uid = thing:getId()
			t.itemid = 1
			if thing:isPlayer() then
				t.type = 1
			elseif thing:isMonster() then
				t.type = 2
			else
				t.type = 3
			end
		end
	end
	return t
end

function isCreature(cid) return Creature(cid) ~= nil end
function isPlayer(cid) return Player(cid) ~= nil end
function isMonster(cid) return Monster(cid) ~= nil end
function isNpc(cid) return Npc(cid) ~= nil end
function isItem(uid) return Item(uid) ~= nil end
function isContainer(uid) return Container(uid) ~= nil end

function getCreatureName(cid) local c = Creature(cid) return c ~= nil and c:getName() or false end
function getCreatureHealth(cid) local c = Creature(cid) return c ~= nil and c:getHealth() or false end
function getCreatureMaxHealth(cid) local c = Creature(cid) return c ~= nil and c:getMaxHealth() or false end
function getCreaturePosition(cid) local c = Creature(cid) return c ~= nil and c:getPosition() or false end
function getCreatureOutfit(cid) local c = Creature(cid) return c ~= nil and c:getOutfit() or false end

function getCreatureTarget(cid)
	local c = Creature(cid)
	if c ~= nil then
		local target = c:getTarget()
		return target ~= nil and target:getId() or 0
	end
	return false
end

function getCreatureMaster(cid)
	local c = Creature(cid)
	if c ~= nil then
		local master = c:getMaster()
		return master ~= nil and master:getId() or c:getId()
	end
	return false
end

getCreaturePos = getCreaturePosition

function doCreatureAddHealth(cid, health) local c = Creature(cid) return c ~= nil and c:addHealth(health) or false end
function doRemoveCreature(cid) local c = Creature(cid) return c ~= nil and c:remove() or false end
function doCreatureSetLookDir(cid, direction) local c = Creature(cid) return c ~= nil and c:setDirection(direction) or false end
function doCreatureSay(cid, text, type, ...) local c = Creature(cid) return c ~= nil and c:say(text, type, ...) or false end
function doCreatureChangeOutfit(cid, outfit) local c = Creature(cid) return c ~= nil and c:setOutfit(outfit) or false end

doSetCreatureDirection = doCreatureSetLookDir

function registerCreatureEvent(cid, name) local c = Creature(cid) return c ~= nil and c:registerEvent(name) or false end
function unregisterCreatureEvent(cid, name) local c = Creature(cid) return c ~= nil and c:unregisterEvent(name) or false end

function getPlayerByName(name) local p = Player(name) return p ~= nil and p:getId() or false end
function getIPByPlayerName(name) local p = Player(name) return p ~= nil and p:getIp() or false end
function getPlayerGUID(cid) local p = Player(cid) return p ~= nil and p:getGuid() or false end
function getPlayerIp(cid) local p = Player(cid) return p ~= nil and p:getIp() or false end
function getPlayerAccountType(cid) local p = Player(cid) return p ~= nil and p:getAccountType() or false end
function getPlayerLastLoginSaved(cid) local p = Player(cid) return p ~= nil and p:getLastLoginSaved() or false end
function getPlayerName(cid) local p = Player(cid) return p ~= nil and p:getName() or false end
function getPlayerFreeCap(cid) local p = Player(cid) return p ~= nil and p:getFreeCapacity() or false end
function getPlayerPosition(cid) local p = Player(cid) return p ~= nil and p:getPosition() or false end
function getPlayerMagLevel(cid) local p = Player(cid) return p ~= nil and p:getMagicLevel() or false end
function getPlayerAccess(cid)
	local player = Player(cid)
	if player == nil then
		return false
	end
	return player:getGroup():getAccess() and 1 or 0
end
function getPlayerSkill(cid, skillId) local p = Player(cid) return p ~= nil and p:getSkillLevel(skillId) or false end
function getPlayerMana(cid) local p = Player(cid) return p ~= nil and p:getMana() or false end
function getPlayerMaxMana(cid) local p = Player(cid) return p ~= nil and p:getMaxMana() or false end
function getPlayerLevel(cid) local p = Player(cid) return p ~= nil and p:getLevel() or false end
function getPlayerTown(cid) local p = Player(cid) return p ~= nil and p:getTown():getId() or false end
function getPlayerVocation(cid) local p = Player(cid) return p ~= nil and p:getVocation():getId() or false end
function getPlayerSoul(cid) local p = Player(cid) return p ~= nil and p:getSoul() or false end
function getPlayerSex(cid) local p = Player(cid) return p ~= nil and p:getSex() or false end
function getPlayerStorageValue(cid, key) local p = Player(cid) return p ~= nil and p:getStorageValue(key) or false end
function getPlayerBalance(cid) local p = Player(cid) return p ~= nil and p:getBankBalance() or false end
function getPlayerMoney(cid) local p = Player(cid) return p ~= nil and p:getMoney() or false end
function getPlayerGroupId(cid) local p = Player(cid) return p ~= nil and p:getGroup():getId() or false end
function getPlayerLookDir(cid) local p = Player(cid) return p ~= nil and p:getDirection() or false end
function getPlayerLight(cid) local p = Player(cid) return p ~= nil and p:getLight() or false end
function getPlayerDepotItems(cid, depotId) local p = Player(cid) return p ~= nil and p:getDepotItems(depotId) or false end
function getPlayerSkullType(cid) local p = Player(cid) return p ~= nil and p:getSkull() or false end
function getPlayerLossPercent(cid) local p = Player(cid) return p ~= nil and p:getDeathPenalty() or false end
function getPlayerMount(cid, mountId) local p = Player(cid) return p ~= nil and p:hasMount(mountId) or false end
function getPlayerPremiumDays(cid) local p = Player(cid) return p ~= nil and p:getPremiumDays() or false end
function getPlayerBlessing(cid, blessing) local p = Player(cid) return p ~= nil and p:hasBlessing(blessing) or false end
function getPlayerGuildId(cid)
	local player = Player(cid)
	if player == nil then
		return false
	end

	local guild = player:getGuild()
	if guild == nil then
		return false
	end
	return guild:getId()
end
function getPlayerGuildLevel(cid) local p = Player(cid) return p ~= nil and p:getGuildLevel() or false end
function getPlayerGuildName(cid)
	local player = Player(cid)
	if player == nil then
		return false
	end

	local guild = player:getGuild()
	if guild == nil then
		return false
	end
	return guild:getName()
end
function getPlayerGuildRank(cid)
	local player = Player(cid)
	if player == nil then
		return false
	end

	local guild = player:getGuild()
	if guild == nil then
		return false
	end

	local rank = guild:getRankByLevel(player:getGuildLevel())
	return rank ~= nil and rank.name or false
end
function getPlayerGuildNick(cid) local p = Player(cid) return p ~= nil and p:getGuildNick() or false end
function getPlayerMasterPos(cid) local p = Player(cid) return p ~= nil and p:getTown():getTemplePosition() or false end
function getPlayerItemCount(cid, itemId, ...) local p = Player(cid) return p ~= nil and p:getItemCount(itemId, ...) or false end
function getPlayerSlotItem(cid, slot)
	local player = Player(cid)
	if player == nil then
		return pushThing(nil)
	end
	return pushThing(player:getSlotItem(slot))
end
function getPlayerItemById(cid, deepSearch, itemId, ...)
	local player = Player(cid)
	if player == nil then
		return pushThing(nil)
	end
	return pushThing(player:getItemById(itemId, deepSearch, ...))
end
function getPlayerFood(cid)
	local player = Player(cid)
	if player == nil then
		return false
	end
	local c = player:getCondition(CONDITION_REGENERATION, CONDITIONID_DEFAULT) return c ~= nil and math.floor(c:getTicks() / 1000) or 0
end
function canPlayerLearnInstantSpell(cid, name) local p = Player(cid) return p ~= nil and p:canLearnSpell(name) or false end
function getPlayerLearnedInstantSpell(cid, name) local p = Player(cid) return p ~= nil and p:hasLearnedSpell(name) or false end
function isPlayerGhost(cid) local p = Player(cid) return p ~= nil and p:isGhost() or false end
function isPlayerPzLocked(cid) local p = Player(cid) return p ~= nil and p:isPzLocked() or false end
function getPlayersByIPAddress(ip, mask)
	if mask == nil then mask = 0xFFFFFFFF end
	local masked = bit.band(ip, mask)
	local result = {}
	for _, player in ipairs(Game.getPlayers()) do
		if bit.band(player:getIp(), mask) == masked then
			result[#result + 1] = player:getId()
		end
	end
	return result
end
function getOnlinePlayers()
	local result = {}
	for _, player in ipairs(Game.getPlayers()) do
		result[#result + 1] = player:getName()
	end
	return result
end
function getPlayersByAccountNumber(accountNumber)
	local result = {}
	for _, player in ipairs(Game.getPlayers()) do
		if player:getAccountId() == accountNumber then
			result[#result + 1] = player:getId()
		end
	end
	return result
end
function getPlayerGUIDByName(name)
	local player = Player(name)
	if player ~= nil then
		return player:getGuid()
	end

	local resultId = db.storeQuery("SELECT `id` FROM `players` WHERE `name` = " .. db.escapeString(name))
	if resultId ~= false then
		local guid = result.getDataInt(resultId, "id")
		result.free(resultId)
		return guid
	end
	return 0
end
function getAccountNumberByPlayerName(name)
	local player = Player(name)
	if player ~= nil then
		return player:getAccountId()
	end

	local resultId = db.storeQuery("SELECT `account_id` FROM `players` WHERE `name` = " .. db.escapeString(name))
	if resultId ~= false then
		local accountId = result.getDataInt(resultId, "account_id")
		result.free(resultId)
		return accountId
	end
	return 0
end

getPlayerAccountBalance = getPlayerBalance
getIpByName = getIPByPlayerName

function setPlayerStorageValue(cid, key, value) local p = Player(cid) return p ~= nil and p:setStorageValue(key, value) or false end
function doPlayerSetBalance(cid, balance) local p = Player(cid) return p ~= nil and p:setBankBalance(balance) or false end
function doPlayerAddMoney(cid, money) local p = Player(cid) return p ~= nil and p:addMoney(money) or false end
function doPlayerRemoveMoney(cid, money) local p = Player(cid) return p ~= nil and p:removeMoney(money) or false end
function doPlayerAddSoul(cid, soul) local p = Player(cid) return p ~= nil and p:addSoul(soul) or false end
function doPlayerSetVocation(cid, vocation) local p = Player(cid) return p ~= nil and p:setVocation(Vocation(vocation)) or false end
function doPlayerSetTown(cid, town) local p = Player(cid) return p ~= nil and p:setTown(Town(town)) or false end
function setPlayerGroupId(cid, groupId) local p = Player(cid) return p ~= nil and p:setGroup(Group(groupId)) or false end
function doPlayerSetSex(cid, sex) local p = Player(cid) return p ~= nil and p:setSex(sex) or false end
function doShowTextDialog(cid, itemId, text) local p = Player(cid) return p ~= nil and p:showTextDialog(itemId, text) or false end
function doPlayerAddItemEx(cid, uid, ...) local p = Player(cid) return p ~= nil and p:addItemEx(Item(uid), ...) or false end
function doPlayerAddPremiumDays(cid, days) local p = Player(cid) return p ~= nil and p:addPremiumDays(days) or false end
function doPlayerRemovePremiumDays(cid, days) local p = Player(cid) return p ~= nil and p:removePremiumDays(days) or false end
function doPlayerAddBlessing(cid, blessing) local p = Player(cid) return p ~= nil and p:addBlessing(blessing) or false end
function doPlayerAddOutfit(cid, lookType, addons) local p = Player(cid) return p ~= nil and p:addOutfitAddon(lookType, addons) or false end
function doPlayerRemOutfit(cid, lookType, addons)
	local player = Player(cid)
	if player == nil then
		return false
	end
	if addons == 255 then
		return player:removeOutfit(lookType)
	else
		return player:removeOutfitAddon(lookType, addons)
	end
end
function canPlayerWearOutfit(cid, lookType, addons) local p = Player(cid) return p ~= nil and p:hasOutfit(lookType, addons) or false end
function doPlayerAddMount(cid, mountId) local p = Player(cid) return p ~= nil and p:addMount(mountId) or false end
function doPlayerRemoveMount(cid, mountId) local p = Player(cid) return p ~= nil and p:removeMount(mountId) or false end
function doPlayerSendCancel(cid, text) local p = Player(cid) return p ~= nil and p:sendCancelMessage(text) or false end
function doPlayerFeed(cid, food) local p = Player(cid) return p ~= nil and p:feed(food) or false end
function playerLearnInstantSpell(cid, name) local p = Player(cid) return p ~= nil and p:learnSpell(name) or false end
function doPlayerPopupFYI(cid, message) local p = Player(cid) return p ~= nil and p:popupFYI(message) or false end
function doSendTutorial(cid, tutorialId) local p = Player(cid) return p ~= nil and p:sendTutorial(tutorialId) or false end
function doAddMapMark(cid, pos, type, description) local p = Player(cid) return p ~= nil and p:addMapMark(pos, type, description or "") or false end
function doPlayerSendTextMessage(cid, type, text, ...) local p = Player(cid) return p ~= nil and p:sendTextMessage(type, text, ...) or false end
function doSendAnimatedText() debugPrint("Deprecated function.") return true end
function doPlayerAddExp(cid, exp, useMult, ...)
	local player = Player(cid)
	if player == nil then
		return false
	end

	if useMult then
		exp = exp * Game.getExperienceStage(player:getLevel())
	end
	return player:addExperience(exp, ...)
end
function doPlayerAddManaSpent(cid, mana) local p = Player(cid) return p ~= nil and p:addManaSpent(mana * configManager.getNumber(configKeys.RATE_MAGIC)) or false end
function doPlayerAddSkillTry(cid, skillid, n) local p = Player(cid) return p ~= nil and p:addSkillTries(skillid, n * configManager.getNumber(configKeys.RATE_SKILL)) or false end
function doPlayerAddMana(cid, mana, ...) local p = Player(cid) return p ~= nil and p:addMana(mana, ...) or false end

doPlayerSendDefaultCancel = doPlayerSendCancel

function getMonsterTargetList(cid)
	local monster = Monster(cid)
	if monster == nil then
		return false
	end

	local result = {}
	for _, creature in ipairs(monster:getTargetList()) do
		if monster:isTarget(creature) then
			result[#result + 1] = creature:getId()
		end
	end
	return result
end
function getMonsterFriendList(cid)
	local monster = Monster(cid)
	if monster == nil then
		return false
	end

	local z = monster:getPosition().z

	local result = {}
	for _, creature in ipairs(monster:getFriendList()) do
		if not creature:isRemoved() and creature:getPosition().z == z then
			result[#result + 1] = creature:getId()
		end
	end
	return result
end
function doSetMonsterTarget(cid, target)
	local monster = Monster(cid)
	if monster == nil then
		return false
	end

	if monster:getMaster() ~= nil then
		return true
	end

	local target = Creature(cid)
	if target == nil then
		return false
	end

	monster:selectTarget(target)
	return true
end
function doMonsterChangeTarget(cid)
	local monster = Monster(cid)
	if monster == nil then
		return false
	end

	if monster:getMaster() ~= nil then
		return true
	end

	monster:searchTarget(1)
	return true
end
function doConvinceCreature(cid, target)
	local creature = Creature(cid)
	if creature == nil then
		return false
	end

	local targetCreature = Creature(target)
	if targetCreature == nil then
		return false
	end

	targetCreature:setMaster(creature)
	return true
end

function getTownId(townName) local t = Town(townName) return t ~= nil and t:getId() or false end
function getTownName(townId) local t = Town(townId) return t ~= nil and t:getName() or false end
function getTownTemplePosition(townId) local t = Town(townId) return t ~= nil and t:getTemplePosition() or false end

function doSetItemActionId(uid, actionId) local i = Item(uid) return i ~= nil and i:setActionId(actionId) or false end
function doTransformItem(uid, newItemId, ...) local i = Item(uid) return i ~= nil and i:transform(newItemId, ...) or false end
function doRemoveItem(uid, ...) local i = Item(uid) return i ~= nil and i:remove(...) or false end

function getContainerSize(uid) local c = Container(uid) return c ~= nil and c:getSize() or false end
function getContainerCap(uid) local c = Container(uid) return c ~= nil and c:getCapacity() or false end
function getContainerItem(uid, slot)
	local container = Container(uid)
	if container == nil then
		return pushThing(nil)
	end
	return pushThing(container:getItem(slot))
end

function doAddContainerItemEx(uid, virtualId)
	local container = Container(uid)
	if container == nil then
		return false
	end

	local res = container:addItemEx(Item(virtualId))
	if res == nil then
		return false
	end
	return res
end

function doSendMagicEffect(pos, magicEffect, ...) return Position(pos):sendMagicEffect(magicEffect, ...) end
function doSendDistanceShoot(fromPos, toPos, distanceEffect, ...) return Position(fromPos):sendDistanceEffect(toPos, distanceEffect, ...) end
function isSightClear(fromPos, toPos, floorCheck) return Position(fromPos):isSightClear(toPos, floorCheck) end

function getPromotedVocation(vocationId)
	local vocation = Vocation(vocationId)
	if vocation == nil then
		return 0
	end

	local promotedVocation = vocation:getPromotion()
	if promotedVocation == nil then
		return 0
	end
	return promotedVocation:getId()
end

function getGuildId(guildName)
	local resultId = db.query("SELECT `id` FROM `guilds` WHERE `name` = " .. db.escapeString(guildName))
	if resultId == false then
		return false
	end

	local guildId = result.getDataInt(resultId, "id")
	result.free(resultId)
	return guildId
end

function getHouseName(houseId) local h = House(houseId) return h ~= nil and h:getName() or false end
function getHouseOwner(houseId) local h = House(houseId) return h ~= nil and h:getOwnerGuid() or false end
function getHouseEntry(houseId) local h = House(houseId) return h ~= nil and h:getExitPosition() or false end
function getHouseTown(houseId) local h = House(houseId) if h == nil then return false end local t = h:getTown() return t ~= nil and t:getId() or false end
function getHouseTilesSize(houseId) local h = House(houseId) return h ~= nil and h:getTileCount() or false end

function isItemStackable(itemId) return ItemType(itemId):isStackable() end
function isItemRune(itemId) return ItemType(itemId):isRune() end
function isItemDoor(itemId) return ItemType(itemId):isDoor() end
function isItemContainer(itemId) return ItemType(itemId):isContainer() end
function isItemFluidContainer(itemId) return ItemType(itemId):isFluidContainer() end
function isItemMovable(itemId) return ItemType(itemId):isMovable() end
function isCorpse(uid) local i = Item(uid) return i ~= nil and ItemType(i:getId()):isCorpse() or false end

isItemMoveable = isItemMovable
isMoveable = isMovable

function getItemName(itemId) return ItemType(itemId):getName() end
function getItemWeight(itemId, ...) return ItemType(itemId):getWeight(...) end
function getItemDescriptions(itemId)
	local itemType = ItemType(itemId)
	return {
		name = itemType:getName(),
		plural = itemType:getPluralName(),
		article = itemType:getArticle(),
		description = itemType:getDescription()
	}
end
function getItemIdByName(name)
	local id = ItemType(name):getId()
	if id == 0 then
		return false
	end
	return id
end
function getItemWeightByUID(uid, ...)
	local item = Item(uid)
	if item == nil then
		return false
	end

	local itemType = ItemType(item:getId())
	return itemType:isStackable() and itemType:getWeight(item:getCount(), ...) or itemType:getWeight(1, ...)
end
function getItemRWInfo(uid)
	local item = Item(uid)
	if item == nil then
		return false
	end

	local rwFlags = 0
	local itemType = ItemType(item:getId())
	if itemType:isReadable() then
		rwFlags = bit.bor(rwFlags, 1)
	end

	if itemType:isWritable() then
		rwFlags = bit.bor(rwFlags, 2)
	end
	return rwFlags
end
function getContainerCapById(itemId) return ItemType(itemId):getCapacity() end
function getFluidSourceType(itemId) local it = ItemType(itemId) return it.id ~= 0 and it:getFluidSource() or false end

function doSetItemText(uid, text)
	local item = Item(uid)
	if item == nil then
		return false
	end

	if text ~= "" then
		item:setAttribute(ITEM_ATTRIBUTE_TEXT, text)
	else
		item:removeAttribute(ITEM_ATTRIBUTE_TEXT)
	end
	return true
end
function doSetItemSpecialDescription(uid, desc)
	local item = Item(uid)
	if item == nil then
		return false
	end

	if desc ~= "" then
		item:setAttribute(ITEM_ATTRIBUTE_DESCRIPTION, desc)
	else
		item:removeAttribute(ITEM_ATTRIBUTE_DESCRIPTION)
	end
	return true
end
function doDecayItem(uid) local i = Item(uid) return i ~= nil and i:decay() or false end

function setHouseOwner(id, guid) local h = House(id) return h ~= nil and h:setOwnerGuid(guid) or false end
function getHouseRent(id) local h = House(id) return h ~= nil and h:getRent() or nil end
function getHouseAccessList(id, listId) local h = House(id) return h ~= nil and h:getAccessList(listId) or nil end
function setHouseAccessList(id, listId, listText) local h = House(id) return h ~= nil and h:setAccessList(listId, listText) or false end

function getTilePzInfo(position)
	local t = Tile(position)
	if t == nil then
		return false
	end
	return t:hasFlag(TILESTATE_PROTECTIONZONE)
end

function getTileInfo(position)
	local t = Tile(position)
	if t == nil then
		return false
	end

	local ret = pushThing(t:getGround())
	ret.protection = t:hasFlag(TILESTATE_PROTECTIONZONE)
	ret.nopz = ret.protection
	ret.nologout = t:hasFlag(TILESTATE_NOLOGOUT)
	ret.refresh = t:hasFlag(TILESTATE_REFRESH)
	ret.house = t:hasFlag(TILESTATE_HOUSE)
	ret.bed = t:hasFlag(TILESTATE_BED)
	ret.depot = t:hasFlag(TILESTATE_DEPOT)

	ret.things = t:getThingCount()
	ret.creatures = t:getCreatureCount()
	ret.items = t:getItemCount()
	ret.topItems = t:getTopItemCount()
	ret.downItems = t:getDownItemCount()
	return ret
end

function getTileItemByType(position, itemType)
	local t = Tile(position)
	if t == nil then
		return pushThing(nil)
	end
	return pushThing(t:getItemByType(itemType))
end

function getTileItemById(position, itemId, ...)
	local t = Tile(position)
	if t == nil then
		return pushThing(nil)
	end
	return pushThing(t:getItemById(itemId, ...))
end

function getTileThingByPos(position)
	local t = Tile(position)
	if t == nil then
		if position.stackpos == -1 then
			return -1
		end
		return pushThing(nil)
	end

	if position.stackpos == -1 then
		return t:getThingCount()
	end
	return pushThing(t:getThing(position.stackpos))
end

function getTileThingByTopOrder(position, topOrder)
	local t = Tile(position)
	if t == nil then
		return pushThing(nil)
	end
	return pushThing(t:getItemByTopOrder(topOrder))
end

function getTopCreature(position)
	local t = Tile(position)
	if t == nil then
		return pushThing(nil)
	end
	return pushThing(t:getTopCreature())
end

function queryTileAddThing(thing, position, ...) local t = Tile(position) return t ~= nil and t:queryAdd(thing, ...) or false end

function doTeleportThing(uid, dest, pushMovement)
	if uid >= 0x10000000 then
		local creature = Creature(uid)
		if creature ~= nil then
			return creature:teleportTo(dest, pushMovement or false)
		end
	else
		local item = Item(uid)
		if item ~= nil then
			return item:moveTo(dest)
		end
	end
	return false
end

function getThing(uid)
	return uid >= 0x10000000 and pushThing(Creature(uid)) or pushThing(Item(uid))
end

function getConfigInfo(info)
	if type(info) ~= "string" then
		return nil
	end
	dofile('config.lua')
	return _G[info]
end

function getWorldCreatures(type)
	if type == 0 then
		return Game.getPlayerCount()
	elseif type == 1 then
		return Game.getMonsterCount()
	elseif type == 2 then
		return Game.getNpcCount()
	end
	return Game.getPlayerCount() + Game.getMonsterCount() + Game.getNpcCount()
end

saveData = saveServer

function getGlobalStorageValue(key)
	return Game.getStorageValue(key) or -1
end

function setGlobalStorageValue(key, value)
	Game.setStorageValue(key, value)
	return true
end

function isPlayerInCast(cid) local p = Player(cid) return p ~= nil and p:isInCast() or false end
function doPlayerSetInCast(cid, value) local p = Player(cid) return p ~= nil and p:setInCast(value) or false end
function getPlayerPassword(cid) local p = Player(cid) return p ~= nil and p:getPassword() or false end
function doPlayerSetPassword(cid, value) local p = Player(cid) return p ~= nil and p:setPassword(value) or false end
function getPlayerViewers(cid) local p = Player(cid) return p ~= nil and p:getViewers() or false end
function getPlayerViews(cid, ...) local p = Player(cid) return p ~= nil and p:getViews(...) or false end
