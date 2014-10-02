local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)

function onCreatureAppear(cid) npcHandler:onCreatureAppear(cid) end
function onCreatureDisappear(cid) npcHandler:onCreatureDisappear(cid) end
function onCreatureSay(cid, type, msg) npcHandler:onCreatureSay(cid, type, msg) end
function onThink() npcHandler:onThink() end

function creatureSayCallback(cid, type, msg)
	if(not npcHandler:isFocused(cid)) then
		return false
	end
	local player = Player(cid)
	
	if player:getLevel() < 30 then --calculate prize for blessing
		blessprize = 2000*5*1.1
	elseif player:getLevel() >= 30 and player:getLevel() <= 120 then
		blessprize = 200*(player:getLevel()-20)*5*1.1
	elseif player:getLevel() > 120 then
		blessprize = 20000*5*1.1
	end
	
	if(msgcontains(msg, "inquisitor")) then
		npcHandler:say("The churches of the gods entrusted me with the enormous and responsible task to lead the inquisition. I leave the field work to inquisitors who I recruit from fitting people that cross my way.", cid)
	elseif(msgcontains(msg, "join")) then
		if(player:getStorageValue(200) < 1) then
			npcHandler:say("Do you want to join the inquisition?", cid)
			npcHandler.topic[cid] = 2
		end
	elseif(msgcontains(msg, "blessing") or msgcontains(msg, "bless")) then
		if(player:getStorageValue(200) == 25) then --if quest is done
			npcHandler:say("Do you want to receive the blessing of the inquisition - which means all five available blessings - for "..blessprize.." gold?", cid)
			npcHandler.topic[cid] = 7
		else
			npcHandler:say("You cannot get this blessing unless you have completed The Inquisition Quest.", cid)
			npcHandler.topic[cid] = 0
		end
	elseif(msgcontains(msg, "mission") or msgcontains(msg, "report")) then
		if(player:getStorageValue(200) == 1) then
			npcHandler:say({"Let's see if you are worthy. Take an inquisitor's field guide from the box in the back room. ...","Follow the instructions in the guide to talk to the Thaian guards that protect the walls and gates of the city and test their loyalty. Then report to me about your {mission}."}, cid, 0, 1, 3000)
			npcHandler.topic[cid] = 0
			player:setStorageValue(200, 2)
			player:setStorageValue(12110, 2) -- The Inquisition Questlog- "Ask for Mission"
			player:setStorageValue(12111, 1) -- The Inquisition Questlog- "Mission 1: Interrogation"
		elseif(player:getStorageValue(200) == 2) then
			npcHandler:say("Your current mission is to investigate the reliability of certain guards. Are you done with that mission? ", cid)
			npcHandler.topic[cid] = 3
		elseif(player:getStorageValue(200) == 3) then
			npcHandler:say({"Listen, we have information about a heretic coven that hides in a mountain called the Big Old One. The witches reach this cursed place on flying brooms and think they are safe there. ...","I've arranged a flying carpet that will bring you to their hideout. Travel to Femor Hills and tell the carpet pilot the codeword 'eclipse' ...","He'll bring you to your destination. At their meeting place, you'll find a cauldron in which they cook some forbidden brew ...","Use this vial of holy water to destroy the brew. Also steal their grimoire and bring it to me."}, cid, 0, 1, 3500)
			npcHandler.topic[cid] = 0
			player:setStorageValue(200, 4)
			player:setStorageValue(12112, 1) -- The Inquisition Questlog- "Mission 2: Eclipse"
			player:addItem(7494, 1)
		elseif(player:getStorageValue(200) == 5) then
			if (player:removeItem(8702, 1)) then
				npcHandler:say({"I think it's time to truly test your abilities. One of our allies has requested assistance. I think you are just the right person to help him ...","Storkus is an old and grumpy dwarf who works as a vampire hunter since many, many decades. He's quite successful but even hehas his limits. ...","So occasionally, we send him help. In return he trains and tests our recruits. It's an advantageous agreement for both sides ...","You'll find him in his cave at the mountain outside of Kazordoon. He'll tell you about your next mission."}, cid, 0, 1, 3000)
				npcHandler.topic[cid] = 0
				player:setStorageValue(200, 6)
				player:setStorageValue(12112, 3) -- The Inquisition Questlog- "Mission 2: Eclipse"
				player:setStorageValue(12113, 1) -- The Inquisition Questlog- "Mission 3: Vampire Hunt"
			else
				npcHandler:say("You need bring me the witches' grimoire.", cid)
			end
		elseif(player:getStorageValue(200) > 5 and player:getStorageValue(200) < 11) then
			npcHandler:say("Your current mission is to help the vampire hunter Storkus. Are you done with that mission? ", cid)
			npcHandler.topic[cid] = 4
		elseif(player:getStorageValue(200) == 11) then
			npcHandler:say({"We've got a report about an abandoned and haunted house in Liberty Bay. I want you to examine this house. It's the only ruin in Liberty Bay so you should have no trouble finding it. ...","There's an evil being somewhere. I assume that it will be easier to find the right spot at night. Use this vial of holy water on that spot to drive out the evil being."}, cid, 0, 1, 3500)
			player:setStorageValue(200, 12)
			player:setStorageValue(12114, 1) -- The Inquisition Questlog- "Mission 4: The Haunted Ruin"
			player:addItem(7494, 1)
			npcHandler.topic[cid] = 0
		elseif(player:getStorageValue(200) == 12 or player:getStorageValue(200) == 13) then
			npcHandler:say("Your current mission is to exorcise an evil being from a house in Liberty Bay. Are you done with that mission? ", cid)
			npcHandler.topic[cid] = 5
		elseif(player:getStorageValue(200) == 14) then
			npcHandler:say({"You've handled heretics, witches, vampires and ghosts. Now be prepared to face the most evil creatures we are fighting - demons. Your new task is extremely simple, though far from easy. ...","Go and slay demonic creatures wherever you find them. Bring me 20 of their essences as a proof of your accomplishments."}, cid, 0, 1, 3000)
			npcHandler.topic[cid] = 0
			player:setStorageValue(200, 15)
			player:setStorageValue(12115, 1) -- The Inquisition Questlog- "Mission 5: Essential Gathering"
		elseif(player:getStorageValue(200) == 15) then
			if(player:getItemCount(6500) >= 20) then
				npcHandler:say({"You're indeed a dedicated protector of the true believers. Don't stop now. Kill as many of these creatures as you can. ...","I also have a reward for your great efforts. Talk to me about your {demon hunter outfit} anytime from now on. Afterwards, let's talk about the next mission that's awaiting you."}, cid, 0, 1, 3000)
				player:setStorageValue(200, 16)
				player:setStorageValue(12115, 2) -- The Inquisition Questlog- "Mission 5: Essential Gathering"
				player:removeItem(6500, 20)
			else
				npcHandler:say("You need 20 of them.", cid)
			end
			npcHandler.topic[cid] = 0
		elseif(player:getStorageValue(200) == 17) then
			npcHandler:say({"We've got information about something very dangerous going on on the isle of Edron. The demons are preparing something there ...","Something that is a threat to all of us. Our investigators were able to acquire vital information before some of them were slain by a demon named Ungreez. ...","It'll be your task to take revenge and to kill that demon. You'll find him in the depths of Edron. Good luck."}, cid, 0, 1, 3200)
			npcHandler.topic[cid] = 0
			player:setStorageValue(200, 18)
			player:setStorageValue(12116, 1) -- The Inquisition Questlog- "Mission 6: The Demon Ungreez"
		elseif(player:getStorageValue(200) == 19) then
			npcHandler:say({"So the beast is finally dead! Thank the gods. At least some things work out in our favour ...","Our other operatives were not that lucky, though. But you will learn more about that in your next {mission}."}, cid, 0, 1, 3000)
			npcHandler.topic[cid] = 0
			player:setStorageValue(200, 20)
			player:setStorageValue(12116, 3) -- The Inquisition Questlog- "Mission 6: The Demon Ungreez"
			player:addOutfitAddon(288, 1)
			player:addOutfitAddon(289, 1)
			player:getPosition():sendMagicEffect(CONST_ME_MAGIC_BLUE)
		elseif(player:getStorageValue(200) == 20) then
			npcHandler:say("Destroy the shadow nexus using this vial of holy water and kill all demon lords.", cid)
			npcHandler.topic[cid] = 0
			player:setStorageValue(200, 21)
			player:setStorageValue(12117, 1) -- The Inquisition Questlog- "Mission 7: The Shadow Nexus"
			player:addItem(7494, 1)
		elseif(player:getStorageValue(200) == 21 or player:getStorageValue(200) == 22) then
			npcHandler:say("Your current mission is to destroy the shadow nexus in the Demon Forge. Are you done with that mission?", cid)
			npcHandler.topic[cid] = 6
		end
	elseif(msgcontains(msg, "yes")) then
		if(npcHandler.topic[cid] == 2) then
			npcHandler:say("So be it. Now you are a member of the inquisition. You might ask me for a {mission} to raise in my esteem.", cid)
			npcHandler.topic[cid] = 0
			player:setStorageValue(200, 1)
			player:setStorageValue(12110, 1) -- The Inquisition Questlog- "Ask for Mission"
		elseif(npcHandler.topic[cid] == 3) then
			if(player:getStorageValue(201) == 1 and player:getStorageValue(202) == 1 and player:getStorageValue(203) == 1 and player:getStorageValue(204) == 1 and player:getStorageValue(205) == 1) then
				npcHandler:say({"Indeed, this is exactly what my other sources told me. Of course I knew the outcome of this investigation in advance. This was just a test. ...","Well, now that you've proven yourself as useful, you can ask me for another mission. Let's see if you can handle some field duty, too."}, cid, 0, 1, 3000)
				npcHandler.topic[cid] = 0
				player:setStorageValue(200, 3)
				player:setStorageValue(12111, 7) -- The Inquisition Questlog- "Mission 1: Interrogation"
			else
				npcHandler:say("You haven't done your mission yet.", cid)
				npcHandler.topic[cid] = 0
			end
		elseif(npcHandler.topic[cid] == 4) then
			if(player:getStorageValue(200) == 10) then
				npcHandler:say("Good, you've returned. Your skill in practical matters seems to be useful. If you're ready for a further mission, just ask. ", cid)
				player:setStorageValue(200, 11)
				player:setStorageValue(12113, 6) -- The Inquisition Questlog- "Mission 3: Vampire Hunt"
			else
				npcHandler:say("You haven't done your mission with {Storkus} yet.", cid)
			end
			npcHandler.topic[cid] = 0
		elseif(npcHandler.topic[cid] == 5) then
			if(player:getStorageValue(200) == 13) then
				npcHandler:say("Well, this was an easy task, but your next mission will be much more challenging. ", cid)
				player:setStorageValue(200, 14)
				player:setStorageValue(12114, 3) -- The Inquisition Questlog- "Mission 4: The Haunted Ruin"
			else
				npcHandler:say("You haven't done your mission with {Storkus} yet.", cid)
			end
			npcHandler.topic[cid] = 0
		elseif(npcHandler.topic[cid] == 6) then
			if (player:getStorageValue(200) == 22) then
				npcHandler:say({"Incredible! You're a true defender of faith! I grant you the title of a High Inquisitor for your noble deeds. From now on you can obtain the blessing of the inquisition which makes the pilgrimage of ashes obsolete ...","The blessing of the inquisition will bestow upon you all available blessings for the price of 60000 gold. Also, don't forget to ask me about your {outfit} to receive the final addon as demon hunter."}, cid, 0, 1, 4000)
				player:setStorageValue(200, 23)
				player:setStorageValue(12117, 3) -- The Inquisition Questlog- "Mission 7: The Shadow Nexus"
				npcHandler.topic[cid] = 0
			else
				npcHandler:say("Come back when you have destroyed the shadow nexus.", cid)
			end
			npcHandler.topic[cid] = 0
		elseif(npcHandler.topic[cid] == 7) then
			if player:hasBlessing(1) or player:hasBlessing(2) or player:hasBlessing(3) or player:hasBlessing(4) or player:hasBlessing(5) then
				npcHandler:say("You already have been blessed!", cid)
			else
				if player:removeMoney(blessprize) then
					npcHandler:say("You have been blessed by all of five gods!, " .. player:getName() .. ".", cid)
					player:addBlessing(1)
					player:addBlessing(2)
					player:addBlessing(3)
					player:addBlessing(4)
					player:addBlessing(5)
					player:getPosition():sendMagicEffect(CONST_ME_HOLYAREA)
					npcHandler.topic[cid] = 0
				else
					npcHandler:say("Come back when you have enough money.", cid)
				end
			end
			npcHandler.topic[cid] = 0
		end
	elseif(msgcontains(msg, "no")) then
		if(npcHandler.topic[cid] > 0) then
			npcHandler:say("Then no.", cid)
			npcHandler.topic[cid] = 0
		end
	elseif(msgcontains(msg, "outfit")) then
		if(player:getStorageValue(200) == 16) then
			npcHandler:say("Here is your demon hunter outfit. You deserve it. Unlock more addons by completing more missions. ", cid)
			npcHandler.topic[cid] = 0
			player:setStorageValue(200, 17)
			player:setStorageValue(12115, 3) -- The Inquisition Questlog- "Mission 5: Essential Gathering"
			player:addOutfit(288, 0)
			player:addOutfit(289, 0)
			player:getPosition():sendMagicEffect(CONST_ME_MAGIC_BLUE)
		elseif(player:getStorageValue(200) == 23) then
			npcHandler:say("Here is the final addon for your demon hunter outfit. Congratulations! ", cid)
			npcHandler.topic[cid] = 0
			player:setStorageValue(200, 24)
			player:setStorageValue(12117, 4) -- The Inquisition Questlog- "Mission 7: The Shadow Nexus"
			player:addOutfitAddon(288, 2)
			player:addOutfitAddon(289, 2)
			player:getPosition():sendMagicEffect(CONST_ME_MAGIC_BLUE)
		end
	elseif msgcontains(msg, 'name') then
		npcHandler:say('I\'m Henricus, the Lord Inquisitor.', cid)
		npcHandler.topic[cid] = 0
	elseif msgcontains(msg, 'job') then
		npcHandler:say('By edict of the churches I\'m the Lord Inquisitor.', cid)
		npcHandler.topic[cid] = 0
	elseif msgcontains(msg, 'believer') then
		npcHandler:say('Belive on the gods and they will show you the path.', cid)
		npcHandler.topic[cid] = 0
	elseif msgcontains(msg, 'inquisitor') then
		npcHandler:say('The churches of the gods entrusted me with the enormous and responsible task to lead the inquisition. I leave the field work to inquisitors who I recruit from fitting people that cross my way.', cid)
		npcHandler.topic[cid] = 0
	elseif msgcontains(msg, 'church') then
		npcHandler:say('The churches of the gods united to fight heresy and dark magic. They are the shield of the true believers, while the inquisition is the sword that fights all enemies of virtuousness.', cid)
		npcHandler.topic[cid] = 0
	elseif msgcontains(msg, 'gods') then
		npcHandler:say('We owe to the gods of good our creation and continuing existence. If it weren\'t for them, we would surely fall prey to the minions of the vile and dark gods.', cid)
		npcHandler.topic[cid] = 0
	elseif msgcontains(msg, 'dark') then
		npcHandler:say('The dark powers are always present. If a human shows only the slightest weakness, they try to corrupt him and to lure him into their service. ...', cid)
		npcHandler:say('We must be constantly aware of evil that comes in many disguises.', cid, 3000)
		npcHandler.topic[cid] = 0
	elseif msgcontains(msg, 'king') then
		npcHandler:say('The Thaian kings are crowned by a representative of the churches. This means they reign in the name of the gods of good and are part of the godly plan for humanity. ...', cid)
		npcHandler:say('As nominal head of the church of Banor, the kings aren\'t only worldly but also spiritual authorities. ...', cid, 4000)
		npcHandler:say('The kings fund the inquisition and sometimes provide manpower in matters of utmost importance. The inquisition, in return, protects the realm from heretics and individuals that aim to undermine the holy reign of the kings.', cid, 7000)
		npcHandler.topic[cid] = 0
	elseif msgcontains(msg, 'banor') then
		npcHandler:say('In the past, the order of Banor was the only order of knighthood in existence. In the course of time,  the order concentrated more and more on spiritual matters rather than on worldly ones. ...', cid)
		npcHandler:say('Nowadays, the order of Banor sanctions new orders and offers spiritual guidance to the fighters of good.', cid, 4000)
		npcHandler.topic[cid] = 0
	elseif msgcontains(msg, 'fardos') then
		npcHandler:say('The priests of Fardos are often mystics who have secluded themselves from worldly matters. Others provide guidance and healing to people in need in the temples.', cid)
		npcHandler.topic[cid] = 0
	elseif msgcontains(msg, 'uman') then
		npcHandler:say('The church of Uman oversees the education of the masses as well as the doings of the sorcerer and druid guilds. It decides which lines of research are in accordance with the will of Uman and which are not. ...', cid)
		npcHandler:say('Concerned, the inquisition watches the attempts of these guilds to become more and more independent and to make own decisions. ...', cid, 4000)
		npcHandler:say('Unfortunately, the sorcerer guild has become dangerously influential and so the hands of our priests are tied due to political matters ...', cid, 7000)
		npcHandler:say('The druids lately claim that they are serving Crunor\'s will and not Uman\'s. Such heresy could only become possible with the independence of Carlin from the Thaian kingdom. ...', cid, 10000)
		npcHandler:say('The spiritual centre of the druids switched to Carlin where they have much influence and cannot be supervised by the inquisition.', cid, 13500)
		npcHandler.topic[cid] = 0
	elseif msgcontains(msg, 'crunor') then
		npcHandler:say('The church of Crunor works closely together with the druid guild. This makes a cooperation sometimes difficult.', cid)
		npcHandler.topic[cid] = 0
	elseif msgcontains(msg, 'zathroth') then
		npcHandler:say('We can see his evil influence almost everywhere. Keep your eyes open or the dark one will lead you on the wrong way and destroy you.', cid)
		npcHandler.topic[cid] = 0
	elseif msgcontains(msg, 'fafnar') then
		npcHandler:say('Fafnar is mostly worshipped by the peasants and farmers in rural areas. ...', cid)
		npcHandler:say('The inquisition has a close eye on these activities. Simply people tend to mix local superstitions with the teachings of the gods. This again may lead to heretical subcults.', cid, 2500)
		npcHandler.topic[cid] = 0
	elseif msgcontains(msg, 'carlin') then
		npcHandler:say('Carlin is a city of sin and heresy. After the reunion of Carlin with the kingdom, the inquisition will have much work to purify the city and its inhabitants.', cid)
		npcHandler.topic[cid] = 0
	elseif msgcontains(msg, 'demon') then
		npcHandler:say('Demons exist in many different shapes and levels of power. In general, they are servants of the dark gods and command great powers of destruction.', cid)
		npcHandler.topic[cid] = 0
	elseif msgcontains(msg, 'darashia') then
		npcHandler:say('Darashia is a godless town full of mislead fools. One day, it will surely share the fate of its sister town Drefia.', cid)
		npcHandler.topic[cid] = 0
	elseif msgcontains(msg, 'drefia') then
		npcHandler:say('Drefia used to be a city of sin and heresy, just like Carlin nowadays. One day, the gods decided to destroy this town and to erase all evil there.', cid)
		npcHandler.topic[cid] = 0
	elseif msgcontains(msg, 'venore') then
		npcHandler:say('Venore is somewhat difficult to handle. The merchants have a close eye on our activities in their city and our authority is limited there. However, we will use all of our influence to prevent a second Carlin.', cid)
		npcHandler.topic[cid] = 0
	elseif msgcontains(msg, 'edron') then
		npcHandler:say('Edron illustrates perfectly why the inquisition is needed and why we need more funds and manpower. ...', cid)
		npcHandler:say('Our agents were on their way to investigate certain occurrences there when some faithless knights fled to some unholy ruins. ...', cid, 3000)
		npcHandler:say('We were unable to wipe them out and the local order of knighthood was of little help. ...', cid, 6000)
		npcHandler:say('It\'s almost sure that something dangerous is going on there, so we have to continue our efforts.', cid, 9000)
		npcHandler.topic[cid] = 0
	elseif msgcontains(msg, 'ankrahmun') then
		npcHandler:say('Even though they claim differently, this city is in the firm grip of Zathroth and his evil minions. Their whole twisted religion is a mockery of the teachings of our gods ...', cid)
		npcHandler:say('As soon as we have gathered the strength, we should crush this city once and for all.', cid, 4000)
		npcHandler.topic[cid] = 0
	elseif msgcontains(msg, 'elves') or msgcontains(msg, 'ab\'dendriel') then
		npcHandler:say('Those elves are hardly any more civilised than orcs. They can become a threat to mankind at any time.', cid)
		npcHandler.topic[cid] = 0
	elseif msgcontains(msg, 'dwarf') or msgcontains(msg, 'kazordoon') then
		npcHandler:say('The dwarfs are allied with Thais but follow their own obscure religion. Although dwarfs keep mostly to themselves, we have to observe this alliance closely.', cid)
		npcHandler.topic[cid] = 0
	elseif msgcontains(msg, 'druid') then
		npcHandler:say('The druids here still follow the old rules. Sadly, the druids of Carlin have left the right path in the last years.', cid)
		npcHandler.topic[cid] = 0
	elseif msgcontains(msg, 'sorcerer') then
		npcHandler:say('Those who wield great power have to resist great temptations. We have the burden to eliminate all those who give in to the temptations.', cid)
		npcHandler.topic[cid] = 0
	elseif msgcontains(msg, 'knight') then
		npcHandler:say('Nowadays, most knights seem to have forgotten the noble cause to which all knights were bound in the past. Only a few have remained pious, serve the gods and follow their teachings.', cid)
		npcHandler.topic[cid] = 0
	elseif msgcontains(msg, 'paladin') then
		npcHandler:say('It\'s a shame that only a few paladins still use their abilities to further the cause of the gods of good. Too many paladins have become selfish and greedy.', cid)
		npcHandler.topic[cid] = 0
	end
	return true
end

npcHandler:setMessage(MESSAGE_GREET, "Greetings, fellow {believer} |PLAYERNAME|!")
npcHandler:setMessage(MESSAGE_FAREWELL, "Always be on guard, |PLAYERNAME|!")
npcHandler:setMessage(MESSAGE_WALKAWAY, "This ungraceful haste is most suspicious!") 
npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:addModule(FocusModule:new())
