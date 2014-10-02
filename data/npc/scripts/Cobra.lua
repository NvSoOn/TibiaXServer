﻿local keywordHandler = KeywordHandler:new()
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

function greetCallback(cid)
	local player = Player(cid)
	if player:getCondition(CONDITION_POISON) then
		player:sendTextMessage(MESSAGE_STATUS_WARNING, "Venture the path of decay!")
		player:getPosition():sendMagicEffect(CONST_ME_TELEPORT)
		player:teleportTo(Position({x = 33396, y = 32836, z = 14}))
		player:getPosition():sendMagicEffect(CONST_ME_TELEPORT)
		return false
	else
		npcHandler:say("Begone! Hissssss! You bear not the mark of the cobra!", player)
		return false
	end
	return true
end



npcHandler:setCallback(CALLBACK_GREET, greetCallback)
npcHandler:addModule(FocusModule:new())
