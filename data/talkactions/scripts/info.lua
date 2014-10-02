local function convertIPToString(ip)
	return string.format("%d.%d.%d.%d",
		bit.band(ip, 0xFF),
		bit.band(bit.rshift(ip, 8), 0xFF),
		bit.band(bit.rshift(ip, 16), 0xFF),
		bit.rshift(ip, 24)
	)
end

function onSay(cid, words, param)
	local player = Player(cid)	
	if not player:getGroup():getAccess() then
		return true
	end

	local target = Player(param)
	if target == nil then
		player:sendCancelMessage("Player not found.")
		return false
	end

	if target:getAccountType() > player:getAccountType() then
		player:sendCancelMessage("You can not get info about this player.")
		return false
	end

	local targetIp = target:getIp()
	player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "Name: " .. target:getName())
	player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "Access: " .. (target:getGroup():getAccess() and "1" or "0"))
	player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "Level: " .. target:getLevel())
	player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "Magic Level: " .. target:getMagicLevel())
	player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "Speed: " .. getCreatureSpeed(player:getId()))
	player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "Position: " .. string.format("(%0.5d / %0.5d / %0.3d)", target:getPosition().x, target:getPosition().y, target:getPosition().z))
	player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "IP: " .. convertIPToString(targetIp))

	local players = {}
	for _, tmpPlayer in ipairs(Game.getPlayers()) do
		if tmpPlayer:getIp() == targetIp and tmpPlayer ~= target then
			players[#players + 1] = tmpPlayer:getName() .. " [" .. tmpPlayer:getLevel() .. "]"
		end
	end

	if #players > 0 then
		player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "Other players on same IP: " .. table.concat(players, ", ") .. ".")
	end
	return false
end
