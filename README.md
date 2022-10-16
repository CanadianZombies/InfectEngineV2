[![Stars](https://img.shields.io/github/stars/canadianzombies/InfectEngineV2.svg?style=plastic)](https://github.com/canadianzombies/InfectEngineV2/stargazers)
[![Issues](https://img.shields.io/github/issues/canadianzombies/infectInfectEngineV2edcity?style=plastic)](https://github.com/canadianzombies/InfectEngineV2/issues)
[![Size](https://img.shields.io/github/repo-size/canadianzombies/InfectEngineV2.svg?style=plastic)](https://github.com/canadianzombies/InfectEngineV2)
[![Discord](https://img.shields.io/discord/234145231359049729?style=plastic)](https://discord.gg/bCsV7km9PE)
[![Code style](https://img.shields.io/badge/code%20style-black-000000.svg?style=plastic)](https://github.com/psf/black)
[![Donate PayPal](https://img.shields.io/badge/donate-paypal-blue.svg?style=plastic)](https://www.paypal.me/simmydizzle)

Infected City
-------------

Currently Achived goals will be marked with a *

Utilizing ROM24B6 w/Ivans OLC 1.81 as a base

	- CME and InfectEngine V1 source is easily ported to ROM
	- ROM has several systems completed that both CME and InfectEngineV1 did not
	- ROM requires us to change license to match Diku (downside)
	- ROM has a lot of support
	- It won't look like ROM for long, but will work like it

Using Infect Engine V2 (codename IEV2)
	- InfectEngine V2 is a colab engine of CombatMUD Engine and InfectEngine V1
	- InfectEngine V2 is now being redesigned to utilize ROM as a base.

Use Language System to store command/skill responses in data file
	for processing.  For ease of changing/development.
	- Default messages such as "You gain %d experience" will be stored in datafile format
	- for quick/easy online editing/saving.

Use of two races, with 3 sub-races as choices at level 30

Use of three classes with 2 subclasses per class at level 15

*Staff use flags not levels

*Staff commands are granted not gained by level
	- Use of system from InfectEngineV1 most likely unless we feel otherwise

Hybrid C/C++

Corpses will turn into Zombies (unless they were already undead)

	-- CORPSE_NPC_ZOMBIE type to be added
	-- CORPSE_PC will not turn into zombies
	-- CORPSE_NPC will turn into zombies after a few ticks
		--  Desecrate the corpse to prevent this from happening
		--  Desecration of corpses while stopping the turn into undead leaves you open
		    to attack for a bit.

Skills Cooldown

	-- Ticks till they cool down for re-use
	-- passive skills effect global cooldown
	-- if too many skills are on cooldown you cannot use any skills


Staff skills execute via / command to separate them from the masses


Levels gained from leveler MOB, upon reaching level 15/30 respectively
	players will be put into new STATES to select their new traits
	based off their class/race decisions (these will unlock other skills/spells)


Total of 60 playable levels

*Experience pool system (EXP gained all the time and trickles in)

Other Systems to add:

	* New log system from IEV1
	* Format system from IEV1
	* Twitter System from IEV1 (Soon to be ported)
	Account System from IEV1
	Leaderboards from CombatMUD
	HotReboot System from IEV1
	* KaViR's Protocol System
	InterMUD3 System from IEV1
	* Users command from IEV1 (Because we like it)
	* Event System from IEV1
		* -- EXP System from IEV1
		* -- Twitter integration
	* Ivans OLC for Rom24
	* Kavirs Protocol Snippet (Huge time saver/life saver)
	* Random Item Generator
	* New Materials System
	* Item Requirements
	* Repop Percentages (NPC and Item)
	Various Snippets to expediate the development cycle.


Possible: SLB2 Integration once all other work done is (previously used in InfectEngineV1)

Possible: MySQL Integration once all core functionality is done

Possible: WebServer system from IEV1 (once all is sorted)

Races			Sub Race at level 30

	Human

			Infected

			Mutated

			Survivor

	Revanant

			Specter

			Bogeyman

			Eidolon


Classes			Sub Classes at level 15 (Classes are now known as Archetypes)

	Stalker

			Hunter

			Thief

	Soldier

			Assaulter

			Defender

	Sorcerer

			Archmage

			Shaman

