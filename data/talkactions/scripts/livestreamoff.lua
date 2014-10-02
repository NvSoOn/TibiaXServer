function onSay(cid, words, param, channel)
	local isTurnedOff = setLiveStream(cid, false)
	if(isTurnedOff) then
		doPlayerSendTextMessage(cid, MESSAGE_STATUS_CONSOLE_BLUE, "Your Live Stream has been turned off")
	else
		if(not isLiveStreamOn(cid)) then
			doPlayerSendTextMessage(cid, MESSAGE_STATUS_CONSOLE_BLUE, "You currently don't have any Live Stream running")
		else
			doPlayerSendTextMessage(cid, MESSAGE_STATUS_CONSOLE_BLUE, "Unknown problem, while setting Live Stream on, contact administrator")
		end
	end
	return true
end