local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)

function onCreatureAppear(cid) npcHandler:onCreatureAppear(cid) end
function onCreatureDisappear(cid) npcHandler:onCreatureDisappear(cid) end
function onCreatureSay(cid, type, msg) npcHandler:onCreatureSay(cid, type, msg) end
function onThink() npcHandler:onThink() end

function creatureSayCallback(cid, type, msg)
	local player = Player(cid)
	if not npcHandler:isFocused(cid) then
		return false
	elseif(msgcontains(msg, "mission")) then
		if(player:getStorageValue(1015) == 9) then
			npcHandler:say("Me people wanting peace. No war with others. No war with little men. We few. We weak. Need help. We not wanting make war. No hurt. ", cid)
			player:setStorageValue(1015, 10)
			player:setStorageValue(12133, 2) --Questlog, The New Frontier Quest "Mission 03: Strangers in the Night"
		end
	end
	return true
end
npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:addModule(FocusModule:new())
