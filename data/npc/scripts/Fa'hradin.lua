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
	local talkUser = NPCHANDLER_CONVBEHAVIOR == CONVERSATION_DEFAULT and 0 or cid
	
	-- GREET
	if(msg == "DJANNI'HAH") and (not npcHandler:isFocused(cid)) then
		if(getPlayerStorageValue(cid, Factions) > 0) then
			npcHandler:addFocus(cid)
			if(getPlayerStorageValue(cid, GreenDjinn.MissionStart) < 1 or not BlueOrGreen) then
				npcHandler:say("Aaaah... what have we here. A human - interesting. And such an ugly specimen, too... All right, human " .. getCreatureName(cid) .. ". How can I help you?", cid)
				npcHandler:addFocus(cid)
			end
		end
	end
	-- GREET
	if(not npcHandler:isFocused(cid)) then
		return false
	end

	if(msgcontains(msg, "mission")) then
		if(getPlayerStorageValue(cid, BlueDjinn.MissionStart+1) == 3 and getPlayerStorageValue(cid, BlueDjinn.MissionStart+2) < 1) then
			npcHandler:say({"I have heard some good things about you from Bo'ques. But I don't know. ...","Well, all right. I do have a job for you. ...","In order to stay informed about our enemy's doings, we have managed to plant a spy in Mal'ouquah. ...","He has kept the Efreet and Malor under surveillance for quite some time. ...","But unfortunately, I have lost contact with him months ago. ...","I do not fear for his safety because his cover is foolproof, but I cannot contact him either. This is where you come in. ...","I need you to infiltrate Mal'ouqhah, contact our man there and get his latest spyreport. The password is {PIEDPIPER}. Remember it well! ..."," I do not have to add that this is a dangerous mission, do I? If you are discovered expect to be attacked! So goodluck, human!"}, cid, 0, 1, 3500)
			setPlayerStorageValue(cid, BlueDjinn.MissionStart+2, 1)
		elseif(getPlayerStorageValue(cid, BlueDjinn.MissionStart+2) == 2) then
			npcHandler:say("Did you already retrieve the spyreport?", cid)
			talkState[talkUser] = 1
		end
	elseif(msgcontains(msg, "yes")) then
		if(talkState[talkUser] == 1) then
			if(doPlayerRemoveItem(cid, 2345, 1)) then
				npcHandler:say({"You really have made it? You have the report? How come you did not get slaughtered? I must say I'm impressed. Your race will never cease to surprise me. ...","Well, let's see. ...","I think I need to talk to Gabel about this. I am sure he will know what to do. Perhaps you should have aword with him, too."}, cid, 0, 1, 3000)
				talkState[talkUser] = 0
				setPlayerStorageValue(cid, BlueDjinn.MissionStart+2, 3)
			end
		end
	end
	if (msgcontains(msg, "bye") or msgcontains(msg, "farewell")) then
		npcHandler:say("Farewell, human. I will always remember you. Unless I forget you, of course.", cid)
		talkState[talkUser] = 0
		npcHandler:releaseFocus(cid)
	end
	return true
end
 
npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)