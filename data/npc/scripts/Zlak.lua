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
	if(not(npcHandler:isFocused(cid))) then
		return false
	end
	local talkUser = NPCHANDLER_CONVBEHAVIOR == CONVERSATION_DEFAULT and 0 or cid
	
	if(msgcontains(msg, "mission")) then
		if(getPlayerStorageValue(cid, 1060) == 22) then
			npcHandler:say("Ze rumour mill iz quite fazt. Ezpecially when zomeone unuzual az you enterz ze zity. Zoon zey will learn zat you have no reazon to be here and our raze will be buzted. ...", cid)
			npcHandler:say("Zo we have to ztrike fazt and quickly. You will have to eliminate zome high ranking key officialz. I zink killing four of zem should be enough. ...", cid)
			npcHandler:say("Ziz will dizrupt ze order in ze zity zignificantly zinze zo much dependz on bureaucracy and ze chain of command. Only chaoz and dizorganization will enable me to help you with ze next ztep in ze plan.", cid)
			setPlayerStorageValue(cid, 1060, 23)
			talkState[talkUser] = 0
		elseif(getPlayerStorageValue(cid, 1060) == 23) then
			npcHandler:say("Chaoz and panic are already zpreading. Your barbaric brutality iz frightening effectively. I could acquire a key zat we need to get you into ze palaze itzelf. But zere are ztill too many guardz and elite zquadz even for you to fight. ...", cid)
			npcHandler:say("I need you to enter ze zity and zlay at leazt zix noblez. Ze otherz will feel zreatened and call guardz to zemzelvez, and ze dragon kingz will accuze each ozer to be rezponzible for zlaying zeir pet noblez. ...", cid)
			npcHandler:say("Zat should enable uz to give you at leazt a chanze to attack ze palaze.", cid)
			setPlayerStorageValue(cid, 1060, 24)
			talkState[talkUser] = 0
		elseif(getPlayerStorageValue(cid, 1060) == 25) then
			npcHandler:say("Word of your deedz iz already zpreading like a wildfire. Zalamon'z plan to unleash zome murderouz beaztz in ze zity workz almozt too well. You are already becoming zome kind of legend with which motherz frighten zeir unruly hatchlingz. ...", cid)
			npcHandler:say("Your next {mizzion} will be a ztrike into ze heart of ze empire.", cid)
			setPlayerStorageValue(cid, 1060, 26)
			talkState[talkUser] = 0
		elseif(getPlayerStorageValue(cid, 1060) == 26) then
			npcHandler:say("Your eagernezz for killing and bloodshed iz frightening, but your next mizzion will zuit your tazte. Wiz ze zity in chaoz and defenzez diverted, ze ztage iz zet for our final ztrike. ...", cid)
			npcHandler:say("A large number of rebelz have arrived undercover in ze zity. Zey will attack ze palaze and zome loyal palaze guardz will let zem in. ...", cid)
			npcHandler:say("Meanwhile, you will take ze old ezcape tunnel zat leadz from ze abandoned bazement in ze norz of ze miniztry to a lift zat endz zomewhere in ze palaze. ...", cid)
			npcHandler:say("When everyzing workz according to ze plan, you will meet Zalamon zomewhere in the underground part of ze palaze.", cid)
			setPlayerStorageValue(cid, 1060, 27)
			talkState[talkUser] = 0
		end
	end
	return true
end
npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:addModule(FocusModule:new())
