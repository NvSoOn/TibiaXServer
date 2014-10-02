function onSay(cid, words, param)
	local housePrice = configManager.getNumber(configKeys.HOUSE_PRICE)
	if housePrice == -1 then
		return true
	end

	local player = Player(cid)
	if player:getPremiumDays() <= 0 then
		player:sendCancelMessage("You need a premium account.")
		return false
	end

	local position = player:getPosition()
	position:getNextPosition(player:getDirection())

	local house = House(getTileHouseInfo(position))
	if house == nil then
		player:sendCancelMessage("You have to be looking at the door of the house you would like to buy.")
		return false
	end

	if house:getOwnerGuid() > 0 then
		player:sendCancelMessage("This house already has an owner.")
		return false
	end

	if getHouseByPlayerGUID(player:getGuid()) then
		player:sendCancelMessage("You are already the owner of a house.")
		return false
	end

	local price = house:getTileCount() * housePrice
	if not player:removeMoney(price) then
		player:sendCancelMessage("You do not have enough money.")
		return false
	end

	house:setOwnerGuid(player:getGuid())
	player:sendTextMessage(MESSAGE_INFO_DESCR, "You have successfully bought this house, be sure to have the money for the rent in the bank.")
	return false
end
