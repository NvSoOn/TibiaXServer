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

function creatureSayCallback(cid, type, msg)
	if not npcHandler:isFocused(cid) then
		return false
	end
	local talkUser = NPCHANDLER_CONVBEHAVIOR == CONVERSATION_DEFAULT and 0 or cid

	local player = Player(cid)
	if msgcontains(msg, "sniffler") then
		if player:getStorageValue(12001) == 1 then
			npcHandler:say("!", player)
			talkState[talkUser] = 1
		end
	elseif msgcontains(msg, "meat") then
		if talkState[talkUser] == 1 then
			if player:getItemCount(2666) >= 1 then
				player:removeItem(2666, 1)
				npcHandler:say("<munch>", player)
				player:setStorageValue(12001, 2)
				player:setStorageValue(12025, 2) -- Questlog The Ice Islands Quest, Befriending the Musher
				talkState[talkUser] = 0
			end
		end
	end
	return true
end
 
npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:addModule(FocusModule:new())