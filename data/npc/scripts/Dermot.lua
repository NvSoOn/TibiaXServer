local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)
local talkState = {}
 
function onCreatureAppear(cid)
	npcHandler:onCreatureAppear(cid)			
end
function onCreatureDisappear(cid)
	npcHandler:onCreatureDisappear(cid)			
end
function onCreatureSay(cid, type, msg)
	npcHandler:onCreatureSay(cid, type, msg)		
end
function onThink()
	npcHandler:onThink()					
end

local function getPlayerBones(cid)
	return getPlayerItemCount(cid, 2230) + getPlayerItemCount(cid, 2231)
end

local function doPlayerRemoveBones(cid)
	return doPlayerRemoveItem(cid, 2230, getPlayerItemCount(cid, 2230)) and doPlayerRemoveItem(cid, 2231, getPlayerItemCount(cid, 2231))
end

function creatureSayCallback(cid, type, msg)
	if(not npcHandler:isFocused(cid)) then
		return false
	end
	local talkUser = NPCHANDLER_CONVBEHAVIOR == CONVERSATION_DEFAULT and 0 or cid

	if(msgcontains(msg, "present")) then
		if(getPlayerStorageValue(cid, 250) == 11) then
			npcHandler:say("You have a present for me?? Realy?", cid)
			talkState[talkUser] = 1
		end
	elseif(msgcontains(msg, "yes")) then
		if(talkState[talkUser] == 1) then
			if(getPlayerItemCount(cid, 2331) >= 1) then
				doPlayerRemoveItem(cid, 2331, 1)
				npcHandler:say("Thank you very much!", cid)
				setPlayerStorageValue(cid, 250, 12)
				talkState[talkUser] = 0
			end
		end
	end
	return true
end
 
npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:addModule(FocusModule:new())