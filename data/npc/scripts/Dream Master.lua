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

	if(msgcontains(msg, "join")) then
		if(getPlayerStorageValue(cid, 67) < 1 and getPlayerStorageValue(cid, 66) < 1) then
			npcHandler:say("The Nightmare Knights are almost extinct now, and as far as I know I am the only teacher that is left. But you might beright and its time to accept new disciples ... ", cid)
			npcHandler:say("After all you have passed the Dream Challenge to reach this place, which used to be the process of initiation in the past... ", cid)
			npcHandler:say("So I ask you: do you wish to become a member of the ancient order of the Nightmare Knights, " .. getCreatureName(cid) .."?", cid)
			talkState[talkUser] = 1
		end
	elseif(msgcontains(msg, "advancement")) then
		if(getPlayerStorageValue(cid, 66) == 1) then
			npcHandler:say("So you want to advance to a {Initiate} rank? Did you bring 500 demonic essences with you?", cid)
			talkState[talkUser] = 3
		elseif(getPlayerStorageValue(cid, 66) == 2) then
			npcHandler:say("So you want to advance to a {Dreamer} rank? Did you bring 1000 demonic essences with you?", cid)
			talkState[talkUser] = 4
		elseif(getPlayerStorageValue(cid, 66) == 3) then
			npcHandler:say("So you want to advance to a {Lord Protector} rank? Did you bring 1500 demonic essences with you?", cid)
			talkState[talkUser] = 5
		end
	elseif(msgcontains(msg, "yes")) then
		if(talkState[talkUser] == 1) then
			npcHandler:say("Please know that your decision is irrevocable. You will abandon the opportunity to join any order whose doctrine is incontrast to our own ...", cid)
			npcHandler:say("Do you still want to join our order?", cid)
			talkState[talkUser] = 2
		elseif(talkState[talkUser] == 2) then
			npcHandler:say("So I welcome you as the latest member of the order of the Nightmare Knights. You entered this place as a stranger, butyou will leave this place as a friend ... ", cid)
			npcHandler:say("You can always ask me about your current rank and about the privileges the ranks grant to those who hold them. ", cid)
			setPlayerStorageValue(cid, 66, 1)
			talkState[talkUser] = 0
		elseif(talkState[talkUser] == 3) then
			if(getPlayerItemCount(cid, 6500) >= 500) then
				doPlayerRemoveItem(cid, 6500, 500)
				doSendMagicEffect(getPlayerPosition(cid), CONST_ME_MAGIC_BLUE)
				setPlayerStorageValue(cid, 66, 2)
				npcHandler:say("You advanced to {Initiate} rank! You are now able to use teleports of second floor of Knightwatch Tower.", cid)
			else
				npcHandler:say("Come back when you gather all essences.", cid)
			end
			talkState[talkUser] = 0
		elseif(talkState[talkUser] == 4) then
			if(getPlayerItemCount(cid, 6500) >= 1000) then
				doPlayerRemoveItem(cid, 6500, 1000)
				doSendMagicEffect(getPlayerPosition(cid), CONST_ME_MAGIC_BLUE)
				setPlayerStorageValue(cid, 66, 3)
				doPlayerAddItem(cid, 6391, 1)
				npcHandler:say("You advanced to {Dreamer} rank!", cid)
			else
				npcHandler:say("Come back when you gather all essences.", cid)
			end
			talkState[talkUser] = 0
		elseif(talkState[talkUser] == 5) then
			if(getPlayerItemCount(cid, 6500) >= 1500) then
				doPlayerRemoveItem(cid, 6500, 1500)
				doSendMagicEffect(getPlayerPosition(cid), CONST_ME_MAGIC_BLUE)
				setPlayerStorageValue(cid, 66, 4)
				npcHandler:say("You advanced to {Lord Protector} rank! You are now able to use teleports of fourth floor of Knightwatch Tower and to create addon scrolls.", cid)
			else
				npcHandler:say("Come back when you gather all essences.", cid)
			end
			talkState[talkUser] = 0
		end
	end
	return true
end
 
npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:addModule(FocusModule:new())