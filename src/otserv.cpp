/**
 * The TibiaX Server - a free and open-source MMORPG server emulator
 * Copyright (C) 2014  TibiaX <http://tibiax.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "otpch.h"

#include "server.h"

#include "game.h"

#include "iologindata.h"
#include "iomarket.h"

#ifndef _WIN32
#include <csignal> // for sigemptyset()
#endif

#include "monsters.h"
#include "outfit.h"
#include "vocation.h"
#include "configmanager.h"
#include "scriptmanager.h"
#include "tools.h"
#include "rsa.h"
#include "protocolgame.h"
#include "protocolold.h"
#include "protocollogin.h"
#include "protocolstatus.h"
#include "house.h"
#include "databasemanager.h"
#include "scheduler.h"

Dispatcher* g_dispatcher = new Dispatcher;
Scheduler* g_scheduler = new Scheduler;

Game g_game;
ConfigManager g_config;
Monsters g_monsters;
Vocations g_vocations;
RSA g_RSA;

std::mutex g_loaderLock;
std::condition_variable g_loaderSignal;
std::unique_lock<std::mutex> g_loaderUniqueLock(g_loaderLock);

void startupErrorMessage(const std::string& errorStr)
{
	std::cout << "> ERROR: " << errorStr << std::endl;
	g_loaderSignal.notify_all();
}

void mainLoader(int argc, char* argv[], ServiceManager* servicer);

void badAllocationHandler()
{
	// Use functions that only use stack allocation
	puts("Asignacion de la memoria, el servidor fallo. Disminuya el tama¤o de su mapa o compile en modo de 64 bits.");
	getchar();
	exit(-1);
}

int main(int argc, char* argv[])
{
	// Setup bad allocation handler
	std::set_new_handler(badAllocationHandler);

#ifndef _WIN32
	// ignore sigpipe...
	struct sigaction sigh;
	sigh.sa_handler = SIG_IGN;
	sigh.sa_flags = 0;
	sigemptyset(&sigh.sa_mask);
	sigaction(SIGPIPE, &sigh, nullptr);
#endif

	ServiceManager servicer;

	g_dispatcher->start();
	g_scheduler->start();

	g_dispatcher->addTask(createTask(std::bind(mainLoader, argc, argv, &servicer)));

	g_loaderSignal.wait(g_loaderUniqueLock);

	if (servicer.is_running()) {
		std::cout << ">> " << g_config.getString(ConfigManager::SERVER_NAME) << " Server En Linea!" << std::endl << std::endl;
		servicer.run();
		g_scheduler->join();
		g_dispatcher->join();
	} else {
		std::cout << ">> No hay servicios en ejecucion. El servidor no esta en linea." << std::endl;
		g_scheduler->stop();
		g_dispatcher->stop();
		g_dispatcher->addTask(createTask([](){
			g_scheduler->shutdown();
			g_dispatcher->shutdown();
		}));
		g_scheduler->join();
		g_dispatcher->join();
	}
	return 0;
}

void mainLoader(int argc, char* argv[], ServiceManager* services)
{
	//dispatcher thread
	g_game.setGameState(GAME_STATE_STARTUP);

	srand((unsigned int)OTSYS_TIME());
#ifdef _WIN32
	SetConsoleTitle(STATUS_SERVER_NAME);
#endif
	std::cout << STATUS_SERVER_NAME << " - Version " << STATUS_SERVER_VERSION << std::endl;
	std::cout << "Compilado en " << __DATE__ << ' ' << __TIME__ << " para la arquitectura ";

#if defined(__amd64__) || defined(_M_X64)
	std::cout << "x64" << std::endl;
#elif defined(__i386__) || defined(_M_IX86) || defined(_X86_)
	std::cout << "x86" << std::endl;
#elif defined(__arm__)
	std::cout << "ARM" << std::endl;
#elif defined(__mips__)
	std::cout << "MIPS" << std::endl;
#else
	std::cout << "unk" << std::endl;
#endif
	std::cout << std::endl;

	std::cout << "Un servidor desarrollado por " << STATUS_SERVER_DEVELOPERS << std::endl;
	std::cout << "Visita nuestro foro para actualizaciones, soporte y recursos: http://tibiax.org/." << std::endl;
	std::cout << std::endl;

	// read global config
	std::cout << ">> Cargando config" << std::endl;
	if (!g_config.load()) {
		startupErrorMessage("No se puede cargar config.lua!");
		return;
	}

#ifdef _WIN32
	std::string defaultPriority = asLowerCaseString(g_config.getString(ConfigManager::DEFAULT_PRIORITY));
	if (defaultPriority == "realtime") {
		SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	} else if (defaultPriority == "high") {
		SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
	} else if (defaultPriority == "higher") {
		SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
	}
#endif

	//set RSA key
	const char* p("14299623962416399520070177382898895550795403345466153217470516082934737582776038882967213386204600674145392845853859217990626450972452084065728686565928113");
	const char* q("7630979195970404721891201847792002125535401292779123937207447574596692788513647179235335529307251350570728407373705564708871762033017096809910315212884101");
	g_RSA.setKey(p, q);

	std::cout << ">> Estableciendo conexion con la base de datos..." << std::flush;

	Database* db = Database::getInstance();
	if (!db->connect()) {
		startupErrorMessage("No se pudo conectar a la base de datos.");
		return;
	}

	std::cout << " MySQL " << Database::getClientVersion() << std::endl;

	// run database manager
	std::cout << ">> Ejecutando el gestor de bases de datos" << std::endl;

	if (!DatabaseManager::isDatabaseSetup()) {
		startupErrorMessage("La base de datos que ha especificado en config.lua esta vacia, por favor importar el schema.sql a su base de datos.");
		return;
	}

	DatabaseManager::updateDatabase();

	if (g_config.getBoolean(ConfigManager::OPTIMIZE_DATABASE) && !DatabaseManager::optimizeTables()) {
		std::cout << "> No hay tablas que optimizar." << std::endl;
	}

	//load vocations
	std::cout << ">> Cargando vocations" << std::endl;
	if (!g_vocations.loadFromXml()) {
		startupErrorMessage("No se puede cargar vocations!");
		return;
	}

	// load item data
	std::cout << ">> Cargando items" << std::endl;
	if (Item::items.loadFromOtb("data/items/items.otb")) {
		startupErrorMessage("No se puede cargar items (OTB)!");
		return;
	}

	if (!Item::items.loadFromXml()) {
		startupErrorMessage("No se puede cargar items (XML)!");
		return;
	}

	std::cout << ">> Cargando script systems" << std::endl;
	if (!ScriptingManager::getInstance()->loadScriptSystems()) {
		startupErrorMessage("No se puede cargar script systems");
		return;
	}

	std::cout << ">> Cargando monsters" << std::endl;
	if (!g_monsters.loadFromXml()) {
		startupErrorMessage("No se puede cargar monsters!");
		return;
	}

	std::cout << ">> Cargando outfits" << std::endl;
	Outfits* outfits = Outfits::getInstance();
	if (!outfits->loadFromXml()) {
		startupErrorMessage("No se puede cargar outfits!");
		return;
	}

	std::cout << ">> Chequeando el tipo de mundo... " << std::flush;
	std::string worldType = asLowerCaseString(g_config.getString(ConfigManager::WORLD_TYPE));
	if (worldType == "pvp") {
		g_game.setWorldType(WORLD_TYPE_PVP);
	} else if (worldType == "no-pvp") {
		g_game.setWorldType(WORLD_TYPE_NO_PVP);
	} else if (worldType == "pvp-enforced") {
		g_game.setWorldType(WORLD_TYPE_PVP_ENFORCED);
	} else {
		std::cout << std::endl;

		std::ostringstream ss;
		ss << "> ERROR: Tipo desconocido de mundo: " << g_config.getString(ConfigManager::WORLD_TYPE) << ", los tipos de mundos que son validos: pvp, no-pvp y pvp-enforced.";
		startupErrorMessage(ss.str());
		return;
	}
	std::cout << asUpperCaseString(worldType) << std::endl;

	std::cout << ">> Cargando mapa" << std::endl;
	if (!g_game.loadMainMap(g_config.getString(ConfigManager::MAP_NAME))) {
		startupErrorMessage("Error al cargar el mapa");
		return;
	}

	std::cout << ">> Inicializando los estados del juego" << std::endl;
	g_game.setGameState(GAME_STATE_INIT);

	// Game client protocols
	services->add<ProtocolGame>(g_config.getNumber(ConfigManager::GAME_PORT));
	services->add<ProtocolLogin>(g_config.getNumber(ConfigManager::LOGIN_PORT));

	// OT protocols
	services->add<ProtocolStatus>(g_config.getNumber(ConfigManager::STATUS_PORT));

	// Legacy protocols
	services->add<ProtocolOldLogin>(g_config.getNumber(ConfigManager::LOGIN_PORT));
	services->add<ProtocolOldGame>(g_config.getNumber(ConfigManager::LOGIN_PORT));

	Houses::getInstance().payHouses();
	g_game.checkExpiredMarketOffers();
	IOMarket::getInstance()->updateStatistics();

	std::cout << ">> Todos los modulos cargados, el servidor se prepara..." << std::endl;

#ifndef WIN32
	if (getuid() == 0 || geteuid() == 0) {
		std::cout << "> Advertencia: " << STATUS_SERVER_NAME << " ha sido ejecutado como usuario root, por favor considere ejecutar como un usuario normal." << std::endl;
	}
#endif

	g_game.start(services);
	g_game.setGameState(GAME_STATE_NORMAL);
	g_loaderSignal.notify_all();
}
