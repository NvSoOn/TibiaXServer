function onSay(cid, words, param, channel)
	local isTurnedOn = setLiveStream(cid, true)
	if(isTurnedOn) then
		doPlayerSendTextMessage(cid, MESSAGE_STATUS_CONSOLE_BLUE, "Your Live Stream has been turned on")
	else
		if(isLiveStreamOn(cid)) then
			doPlayerSendTextMessage(cid, MESSAGE_STATUS_CONSOLE_BLUE, "You currently have Your Live Stream running")
		else
			doPlayerSendTextMessage(cid, MESSAGE_STATUS_CONSOLE_BLUE, "Unknown problem, while setting Live Stream on, contact administrator")
		end
	end
	return true
end
