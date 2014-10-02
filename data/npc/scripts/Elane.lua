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
	if msgcontains(msg, "addon") or msgcontains(msg, "outfit") then
		if player:getStorageValue(12055) < 1 then
			npcHandler:say("Oh, my winged tiara? Those are traditionally awarded after having completed a difficult {task} for our guild, only to female aspirants though. Male warriors will receive a hooded cloak.", player)
			talkState[talkUser] = 1
		end
	elseif msgcontains(msg, "Task") then
		if talkState[talkUser] == 1 then
			npcHandler:say("So you are saying that you would like to prove that you deserve to wear such a hooded cloak?", player)
			talkState[talkUser] = 2
		end
	elseif msgcontains(msg, "crossbow") then
		if player:getStorageValue(12055) == 1 then
			npcHandler:say("I'm so excited! Have you really found my crossbow?", player)
			talkState[talkUser] = 4
		end
	elseif msgcontains(msg, "leather") then
		if player:getStorageValue(12055) == 2 then
			npcHandler:say("Did you bring me 100 pieces of lizard leather and 100 pieces of red dragon leather?", player)
			talkState[talkUser] = 5
		end
	elseif msgcontains(msg, "chicken wing") then
		if player:getStorageValue(12055) == 3 then
			npcHandler:say("Were you able to get hold of 5 enchanted chicken wings?", player)
			talkState[talkUser] = 6
		end
	elseif msgcontains(msg, "steel") then
		if player:getStorageValue(12055) == 4 then
			npcHandler:say("Ah, have you brought one piece of royal steel, draconian steel and hell steel each?", player)
			talkState[talkUser] = 7
		end
	elseif msgcontains(msg, "Sniper Gloves") then
		if player:getStorageValue(12056) < 1 then
			npcHandler:say("You found sniper gloves?! Incredible! Listen, if you give them to me, I will grant you the right to wear the sniper gloves accessory. How about it?", player)
			talkState[talkUser] = 8
		end
	elseif msgcontains(msg, "yes") then
		if talkState[talkUser] == 2 then	
			npcHandler:say({"Alright, I will give you a chance. Pay close attention to what I'm going to tell you now. ...",
							"Recently, one of our members moved to Liberty Bay out of nowhere, talking about some strange cult. That is not the problem, but he took my favourite crossbow with him. ...",
							"Please find my crossbow. It has my name engraved on it and is very special to me. ...",
							"Secondly, we need a lot of leather for new quivers. 100 pieces of lizard leather and 100 pieces of red dragon leather should suffice. ...",
							"Third, since we are giving out tiaras, we are always in need of enchanted chicken wings. Please bring me 5, that would help us tremendously. ...",
							"Lastly, for our arrow heads we need a lot of steel. Best would be one piece of royal steel, one piece of draconian steel and one piece of hell steel. ...",
							"Did you understand everything I told you and are willing to handle this task?"}, player, 0, 1, 4000)
			talkState[talkUser] = 3
		elseif talkState[talkUser] == 3 then	
			npcHandler:say("That's the spirit! I hope you will find my crossbow, " .. player:getName() .. "!", player)
			player:setStorageValue(12055, 1)
			player:setStorageValue(12010, 1) --this for default start of Outfit and Addon Quests
			talkState[talkUser] = 0
		elseif talkState[talkUser] == 4 then
			if player:getItemCount( 5947) >= 1 then
				npcHandler:say("Yeah! I could kiss you right here and there! Besides, you're a handsome one. <giggles> Please bring me 100 pieces of lizard leather and 100 pieces of red dragon leather now!", player)
				player:removeItem(5947, 1)
				player:setStorageValue(12055, 2)
				talkState[talkUser] = 0	
			else
				npcHandler:say("You don't have it...", player)
			end
		elseif talkState[talkUser] == 5 then
			if player:getItemCount( 5876) >= 100 and player:getItemCount( 5948) >= 100  then
				npcHandler:say("Good work, " .. player:getName() .. "! That is enough leather for a lot of sturdy quivers. Now, please bring me 5 enchanted chicken wings.", player)
				player:removeItem(5876, 100)
				player:removeItem(5948, 100)
				player:setStorageValue(12055, 3)
				talkState[talkUser] = 0	
			else
				npcHandler:say("You don't have it...", player)
			end
		elseif talkState[talkUser] == 6 then
			if player:getItemCount( 5891) >= 5 then
				npcHandler:say("Great! Now we can create a few more Tiaras. If only they weren't that expensive... Well anyway, please obtain one piece of royal steel, draconian steel and hell steel each.", player)
				player:removeItem(5891, 5)
				player:setStorageValue(12055, 4)
				talkState[talkUser] = 0	
			else
				npcHandler:say("You don't have it...", player)
			end
		elseif talkState[talkUser] == 7 then
			if player:getItemCount( 5887) >= 1 and player:getItemCount( 5888) >= 1 and player:getItemCount( 5889) >= 1  then
				npcHandler:say("Wow, I'm impressed, " .. player:getName() .. ". Your really are a valuable member of our paladin guild. I shall grant you your reward now. Wear it proudly!", player)
				player:removeItem(5887, 1)
				player:removeItem(5888, 1)
				player:removeItem(5889, 1)
				player:setStorageValue(12055, 5)
				player:addOutfitAddon(129, 1)
				player:addOutfitAddon(137, 1)
				player:getPosition():sendMagicEffect(CONST_ME_MAGIC_BLUE)
				talkState[talkUser] = 0	
			else
				npcHandler:say("You don't have it...", player)
			end
		elseif talkState[talkUser] == 8 then
			if player:getItemCount( 5875) >= 1 then
				npcHandler:say("Great! I hereby grant you the right to wear the sniper gloves as accessory. Congratulations!", player)
				player:removeItem(5875, 1)
				player:setStorageValue(12056, 1)
				player:addOutfitAddon(129, 2)
				player:addOutfitAddon(137, 2)
				player:getPosition():sendMagicEffect(CONST_ME_MAGIC_BLUE)
				talkState[talkUser] = 0				
			else
				npcHandler:say("You don't have it...", player)
			end
		end
	elseif msgcontains(msg, "no") then
		if talkState[talkUser] > 1 then
			npcHandler:say("Then no.", player)
			talkState[talkUser] = 0
		end
	return true
	end
end

npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:addModule(FocusModule:new())