function onSay(cid, words, param)
	local player = Player(cid)
	if player:getGroup():getAccess() and param ~= "" then
		local split = param:split(",")
		player:teleportTo(Position(split[1], split[2], split[3]))
	else
		local pos = player:getPosition()
		player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "Your current position is: " .. pos.x .. ", " .. pos.y .. ", " .. pos.z .. ".")
	end
	return false
end
