function onSay(cid, words, param, channel)
	local strings = {""}
	local i = 1
	local position = 1
	local added = false
	if(isLiveStreamOn(cid)) then
		for _, pid in ipairs(getLiveStreamSpectacors(cid)) do
			if(i > (position * 7)) then
				strings[position] = strings[position] .. ","
				position = position + 1
				strings[position] = ""
			else
				strings[position] = i == 1 and "" or strings[position] .. ", "
			end
			strings[position] = strings[position] .. "Spectacor" .. " (" .. getLiveStreamSpectacorName(pid) .. ")"
			i = i + 1
		end
		local streamDescription = getLiveStreamDescription(cid)
		if(streamDescription == "") then
			streamDescription = "<NON SET>"
		end
		doPlayerSendTextMessage(cid, MESSAGE_STATUS_CONSOLE_BLUE, "Stream description: " .. streamDescription)
		doPlayerSendTextMessage(cid, MESSAGE_STATUS_CONSOLE_BLUE, "Spectacors online: " .. (i - 1))
		if(i > 1) then
			for i, str in ipairs(strings) do
				if(str:sub(str:len()) ~= ",") then
					str = str .. "."
				end
				doPlayerSendTextMessage(cid, MESSAGE_STATUS_CONSOLE_BLUE, str)
			end
		end
	else
		doPlayerSendTextMessage(cid, MESSAGE_STATUS_CONSOLE_BLUE, "You currently don't have any Live Stream running")
	end
	return true
end
