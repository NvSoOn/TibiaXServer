function onSay(cid, words, param, channel)
	local strings = {""}
	local i = 1
	local position = 1
	local added = false
	if(isLiveStreamOn(cid)) then
		if(setLiveStreamDescription(cid, param)) then
			doPlayerSendTextMessage(cid, MESSAGE_STATUS_CONSOLE_BLUE, "Your Live Stream description has been set successful")
		else
			doPlayerSendTextMessage(cid, MESSAGE_STATUS_CONSOLE_BLUE, "Unknown problem, while setting Live Stream description, contact administrator")
		end
	else
		doPlayerSendTextMessage(cid, MESSAGE_STATUS_CONSOLE_BLUE, "You currently don't have any Live Stream running")
	end
	return true
end
