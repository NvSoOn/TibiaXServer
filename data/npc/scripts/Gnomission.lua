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
	if(not npcHandler:isFocused(cid)) then
		return false
	end
	local talkUser = NPCHANDLER_CONVBEHAVIOR == CONVERSATION_DEFAULT and 0 or cid

	if(msgcontains(msg, "warzones")) then
		if(getPlayerStorageValue(cid, 900) == 17) then
			npcHandler:say("There are three warzones. In each warzone you will find fearsome foes. At the end you'll find their mean master. The masters is well protected though. ...", cid)
			npcHandler:say("Make sure to talk to our gnomish agent in there for specifics of its' protection. ...", cid)
			npcHandler:say("Oh, and to be able to enter the second warzone you have to best the first. To enter the third you have to best the second. ...", cid)
			npcHandler:say("And you can enter each one only once every twenty hours. Your normal teleport crystals won't work on these teleporters. You will have to get mission crystals from Gnomally.", cid)
			talkState[talkUser] = 1
		end
	elseif(msgcontains(msg, "job")) then
		if(getPlayerStorageValue(cid, 900) == 17) then
			npcHandler:say("I am responsible for our war missions, to trade with seasoned soldiers and rewarding war heroes. You have to be rank 4 to enter the warzones. ", cid)
			talkState[talkUser] = 2
		end
	elseif(msgcontains(msg, "heroes")) then
		if(talkState[talkUser] == 2) then
			npcHandler:say("You can trade special spoils of war to get a permission to use the war teleporters to the area of the corresponding boss without need of mission crystals. ... ", cid)
			npcHandler:say("Which one would you like to trade: the deathstrike's snippet, gnomevil's hat or the abyssador lash?", cid)
			talkState[talkUser] = 3
		end
	
	elseif(msgcontains(msg, "snippet")) then
		if(talkState[talkUser] == 3) then
			if(getPlayerItemCount(cid, 18430) >= 1) then
				doPlayerRemoveItem(cid, 18430, 1)
				setPlayerStorageValue(cid, 955, 1)
				npcHandler:say("As a war hero you are allowed to use the warzone teleporter one for free! ", cid)
				talkState[talkUser] = 0
			end
		end	
	elseif(msgcontains(msg, "lash")) then
		if(talkState[talkUser] == 3) then
			if(getPlayerItemCount(cid, 18496) >= 1) then
				doPlayerRemoveItem(cid, 18496, 1)
				setPlayerStorageValue(cid, 956, 1)
				npcHandler:say("As a war hero you are allowed to use the warzone teleporter one for free! ", cid)
				talkState[talkUser] = 0
			end
		end	
	elseif(msgcontains(msg, "hat")) then
		if(talkState[talkUser] == 3) then
			if(getPlayerItemCount(cid, 18495) >= 1) then
				doPlayerRemoveItem(cid, 18495, 1)
				setPlayerStorageValue(cid, 957, 1)
				npcHandler:say("As a war hero you are allowed to use the warzone teleporter one for free! ", cid)
				talkState[talkUser] = 0
			end
		end	
		
	elseif(msgcontains(msg, "mission")) then
		if(talkState[talkUser] == 1) then
			npcHandler:say("Fine, I grant you the permission to enter the warzones. Be warned though, this will be not a picnic. Better bring some friends with you. Bringing a lot of them sounds like a good idea. ", cid)
			setPlayerStorageValue(cid, 900, 18)
			setPlayerStorageValue(cid, 954, 1)
			talkState[talkUser] = 0
		end
	end
	return true
end
 
npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:addModule(FocusModule:new())