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

	if(msgcontains(msg, "dress pattern")) then
		if(getPlayerStorageValue(cid, 250) == 15) then
			if(talkState[talkUser] < 1) then
				npcHandler:say("DRESS FLATTEN? WHO WANTS ME TO FLATTEN A DRESS? ", cid)
				talkState[talkUser] = 1
			elseif(talkState[talkUser] == 1) then
				npcHandler:say("A PRESS LANTERN? NEVER HEARD ABOUT IT! ", cid)
				talkState[talkUser] = 2
			elseif(talkState[talkUser] == 2) then
				npcHandler:say("CHESS? I DONT PLAY CHESS! ", cid)
				talkState[talkUser] = 3
			elseif(talkState[talkUser] == 3) then
				npcHandler:say("A PATTERN IN THIS MESS?? HEY DON'T INSULT MY MACHINEHALL! ", cid)
				talkState[talkUser] = 4
			elseif(talkState[talkUser] == 4) then
				npcHandler:say("AH YES! I WORKED ON THE DRESS PATTERN FOR THOSE UNIFORMS. STAINLESS TROUSERES, STEAM DRIVEN BOOTS! ANOTHERMARVEL TO BEHOLD! I'LL SENT A COPY TO KEVIN IMEDIATELY! ", cid)
				setPlayerStorageValue(cid, 250, 16)
				talkState[talkUser] = 0
			end
		end
	end
	return true
end
 
npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:addModule(FocusModule:new())