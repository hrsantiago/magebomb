-- About:

	Demonstration video: http://www.youtube.com/watch?v=y9JFRNBYQKg
	Tutorial video: https://www.youtube.com/watch?v=33vcKFkBU3A

	This program was made by Baxnie, use and abuse. (17/08/2008)
	Email/MSN: promagebomb@hotmail.com
	Works on Tibia 8.0 - 8.1 - 8.11 - 8.2 - 8.21 - 8.22

	REMEMBER TO CONFIGURE THE FILES "config.lua" AND "characters.xml" BEFORE USING.

-- Features:

	When leader attacks someone, all mages will attack together.
	If a mage receives a PM, he will send it to leader.
	Automatic heal, utamo vita.
	Follow target, melee attack, wears target's outfit.
	No more advertisement.

-- Characters.xml

	- account
	The account where to login player. Do not use more than one player per account.

	- password
	Be careful because "LOL" is different than "lol" your password is case sensitive.

	- name
	Put the name of character, like password, it's case sensitive too.

	- utamovita
	Values are "YES" or "NO". Knights won't cast utamo vita. If you want it, buy some energy ring.

	- rune
	Rune that the mage will attack. Values are "SD","GFB","HMM","LMM","EXP" and "NO" if you do not want to use runes.

	- meleeattack
	When you attack a target, mage will attack melee too.

	- followtarget
	When you attack a target, mage will follow target.

	- wearoutfit
	When you attack a target, mage will wear the same outfit as target. It wont wear looktype and addons.

	- heal
	When mage's hitpoints are below x percent, it will use an UH rune. To disable, set value to "0".

-- Commands

	If you are leader, you can use following commands:
	%wearoutfit, %randomoutfit, %follow on/off, %say text, %move north/south/east/west/northeast/northwest/southeast/southwest, %drink hp/mp/shp/smp/gmp/ghp

	- %wearoutfit
	Mages will wear your outfit color.

	- %randomoutfit
	Mages will wear a random outfit.

	- %follow on/off
	Mages will start following leader, if it doesnt follow, set off and on again.

	- %say text
	Mages will say "text", you can use it to cast spells or just say something.

	- %move north/south/east/west/northeast/northwest/southeast/southwest
	Mages will move in the specified direction.

	- %drink hp/mp/shp/smp/gmp/ghp
	Mages will drink the specified fluid.

	- To make only one mage execute the specified command, send a private message to him.

