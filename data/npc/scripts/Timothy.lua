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
	
	if(msgcontains(msg, "research notes")) then
		if(getPlayerStorageValue(cid, 30) == 1) then
			npcHandler:say("Oh, you are the contact person of the academy? Here are the notes that contain everything I have found out so far. ...", cid)
			npcHandler:say("This city is absolutely fascinating, I tell you! If there hadn't been all this trouble and chaos in the past, this city would certainly be the greatest centre of knowledge in the world. ...", cid)
			npcHandler:say("Oh, by the way, speaking about all the trouble here reminds me of Palimuth, a friend of mine. He is a native who was quite helpful in gathering all these information. ...", cid)
			npcHandler:say("I'd like to pay him back for his kindness by sending him some experienced helper that assists him in his effort to restore some order in this city. Maybe you are interested in this job?", cid)
			talkState[talkUser] = 1
		end
	elseif(msgcontains(msg, "yes")) then
		if(talkState[talkUser] == 1) then
			setPlayerStorageValue(cid, 30, 2)
			npcHandler:say("Excellent! You will find Palimuth near the entrance of the city centre. Just ask him if you can assist him in a few missions. ", cid)
			doPlayerAddItem(cid, 10090, 1)
			talkState[talkUser] = 0
		end
	end
	return true
end
 
npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:addModule(FocusModule:new())