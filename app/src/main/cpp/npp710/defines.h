#ifndef INCLUDED_DEFINES_H
#define INCLUDED_DEFINES_H

/* File: defines.h */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *                    Jeff Greene, Diego Gonzalez
 *
 * This work is free software; you can redistribute it and/or modify it
 * under the terms of either:
 *
 * a) the GNU General Public License as published by the Free Software
 *    Foundation, version 2, or
 *
 * b) the "Angband licence":
 *    This software may be copied and distributed for educational, research,
 *    and not for profit purposes provided that this copyright and statement
 *    are included in all such copies.  Other copyrights may also apply.
 */


/*
 * Do not edit this file unless you know *exactly* what you are doing.
 *
 * Some of the values in this file were chosen to preserve game balance,
 * while others are hard-coded based on the format of old save-files, the
 * definition of arrays in various places, mathematical properties, fast
 * computation, storage limits, or the format of external text files.
 *
 * Changing some of these values will induce crashes or memory errors or
 * savefile mis-reads.  Most of the comments in this file are meant as
 * reminders, not complete descriptions, and even a complete knowledge
 * of the source may not be sufficient to fully understand the effects
 * of changing certain definitions.
 *
 * Lastly, note that the code does not always use the symbolic constants
 * below, and sometimes uses various hard-coded values that may not even
 * be defined in this file, but which may be related to definitions here.
 * This is of course bad programming practice, but nobody is perfect.
 *
 *
 * You have been warned.
 */


/*
 * Name of the version/variant
 */
#define VERSION_NAME "NPPAngband"
#define SAVEFILE_NAME  "VNLA"

#define VERSION_MODE_NAME (game_mode == GAME_NPPMORIA ? "NPPMoria" : "NPPAngband")

/*
 * Current version string
 */
#define VERSION_STRING	"7.1.0"


/*
 * Current version numbers
 */
#define VERSION_MAJOR	7
#define VERSION_MINOR	1
#define VERSION_PATCH	0
#define VERSION_EXTRA	0


/*
 * Oldest version number that can still be imported
 */
#define OLD_VERSION_MAJOR	7
#define OLD_VERSION_MINOR	0
#define OLD_VERSION_PATCH	0

/* Various game modes */
#define GAME_NPPANGBAND		1
#define GAME_NPPMORIA		2


/*
 * Number of grids in each block (vertically)
 * Probably hard-coded to 11, see "generate.c"
 */
#define BLOCK_HGT	11

/*
 * Number of grids in each block (horizontally)
 * Probably hard-coded to 11, see "generate.c"
 */
#define BLOCK_WID	11

/*
 * Number of grids in each panel (vertically)
 * Must be a multiple of BLOCK_HGT
 */
#define PANEL_HGT	11

/*since dungeon generation uses PANEL wid to generate dungeons,
 *the width there can't be dependent on bigtile mode.
 */

#define PANEL_WID_FIXED	33
/*
 * Number of grids in each panel (horizontally)
 * Must be a multiple of BLOCK_WID
 */
#define PANEL_WID	(use_bigtile ? 16 : PANEL_WID_FIXED)


#define ROW_MAP			1
#define COL_MAP			13

/*
 * Number of grids in each screen (vertically)
 * Must be a multiple of PANEL_HGT (at least 2x)
 */
#define SCREEN_HGT	(Term->hgt - ROW_MAP - 1)

/*
 * Number of grids in each screen (horizontally)
 * Must be a multiple of PANEL_WID (at least 2x)
 */
#define SCREEN_WID	((Term->wid - COL_MAP - 1) / (use_bigtile ? 2 : 1))

/*
 * The last row of the main window
 */
#define TERM_LAST_ROW (Term->hgt - 1)


/*
 * Number of grids in each dungeon (horizontally)
 * Must be a multiple of PANEL_HGT
 * Must be less or equal to 256
 */
#define MAX_DUNGEON_HGT		66

/*
 * Number of grids in each dungeon (vertically)
 * Must be a multiple of PANEL_WID
 * Must be less or equal to 256
 */
#define MAX_DUNGEON_WID		198

/*
 * Total number of grids in the dungeon
 */
#define MAX_DUNGEON_AREA	(MAX_DUNGEON_HGT * MAX_DUNGEON_WID)


/*
 * Number of rooms to attempt to generate in dungeon.
 *
 */
#define DUN_ROOMS	50

/*
 * Maximum amount of Angband windows.
 */
#define ANGBAND_TERM_MAX 8


/* Sidebar types */
#define SIDEBAR_RACE		0
#define SIDEBAR_TITLE		1
#define SIDEBAR_CLASS		2
#define SIDEBAR_LEVEL		3
#define SIDEBAR_XP			4
#define SIDEBAR_GOLD		5
#define SIDEBAR_EQUIPPY		6
#define SIDEBAR_STAT_MIN	7
#define SIDEBAR_STAT_MAX	8
#define SIDEBAR_AC			9
#define SIDEBAR_HP			10
#define SIDEBAR_MANA		11
#define SIDEBAR_MON_MIN		12
#define SIDEBAR_MON_MAX		13
#define SIDEBAR_SPEED		14
#define SIDEBAR_DEPTH		15
#define SIDEBAR_QUEST		16
#define SIDEBAR_FEELING		17

#define SIDEBAR_MAX_TYPES	18

#define SIDEBAR_MONSTER_MAX	25


/*
 * Mouse click region names
 */
#define MOUSE_NULL			SIDEBAR_MAX_TYPES + 0
#define MOUSE_MAP			SIDEBAR_MAX_TYPES + 1
#define MOUSE_MESSAGE		SIDEBAR_MAX_TYPES + 2
#define MOUSE_STATUS_BAR	SIDEBAR_MAX_TYPES + 3

/* Mouse click */
#define DEFINED_XFF	'\xff'


/*
 * Maximum number of player "sex" types (see "table.c", etc)
 */
#define MAX_SEXES		2

/*
 * Player sex constants (hard-coded by save-files, arrays, etc)
 */
#define SEX_FEMALE		0
#define SEX_MALE		1


/*
 * Maximum number of high scores in the high score file
 */
#define MAX_HISCORES	100


/*
 * Maximum dungeon level.  The player can never reach this level
 * in the dungeon, and this value is used for various calculations
 * involving object and monster creation.  In Angband, it must be at least 100.
 * In Moria, it must be above 50
 * Setting it below 128 may prevent the creation of some objects.
 */
#define MAX_DEPTH_ALL	128
#define MAX_DEPTH  		((game_mode == GAME_NPPMORIA) ? 64: 128)
#define MORIA_QUEST_DEPTH	50


/*
 * Maximum size of the "view" array (see "cave.c")
 * Note that the "view radius" will NEVER exceed 20, and even if the "view"
 * was octagonal, we would never require more than 1520 entries in the array.
 */
#define VIEW_MAX 1536

/*
 * Maximum size of the "temp" array (see "cave.c")
 * Note that we must be as large as "VIEW_MAX" for proper functioning
 * of the "update_view()" function, and we must also be as large as the
 * largest illuminatable room, but no room is larger than 800 grids.  We
 * must also be large enough to allow "good enough" use as a circular queue,
 * to calculate monster flow, but note that the flow code is "paranoid".
 */
#define TEMP_MAX 1536

/*
 * Maximum size of the "dyna_g" array.
 */
#define DYNA_MAX 1536

/* Flags for the "dynamic_grid_type" data type */
#define DF1_OCCUPIED 0x01	/* The entry is being used */
#define DF1_NEW_BORN 0x02	/* The dynamic feature was created recently */


#define ENERGY_TO_MOVE		200
#define BASE_ENERGY_MOVE	100
#define BASE_ENERGY_FLYING	BASE_ENERGY_MOVE


/*
 * OPTION: Maximum number of macros (see "util.c")
 * Default: assume at most 256 macros are used
 */
#define MACRO_MAX	256

/*
 * OPTION: Maximum number of "quarks" (see "util.c")
 * Default: assume at most 512 different inscriptions are used
 */
#define QUARK_MAX	512

/*
 * OPTION: Maximum number of autoinscriptions(see "object1.c")
 */
#define AUTOINSCRIPTIONS_MAX 216

/*
 * OPTION: Maximum number of messages to remember (see "util.c")
 * Default: assume maximal memorization of 2048 total messages
 */
#define MESSAGE_MAX	2048

/*
 * OPTION: Maximum space for the message text buffer (see "util.c")
 * Default: assume that each of the 2048 messages is repeated an
 * average of three times, and has an average length of 48
 */
#define MESSAGE_BUF	32768


/*
 * Maximum value storable in a "u16b" (hard-coded)
 */
#define MAX_SHORT_UNSIGNED	65535


/*
 * Store constants
 */
#define STORE_INVEN_MAX	24		/* Max number of discrete objs in inven */
#define STORE_SHUFFLE	25		/* 1/Chance (per day) of an owner changing */
#define STORE_TURNS		1000	/* Number of turns between turnovers */
#define STORE_TURNOVER_NPPANGBAND	12		/* Normal shop turnover, per day */
#define STORE_MIN_KEEP_NPPANGBAND	8		/* Min slots to "always" keep full */
#define STORE_MAX_KEEP_NPPANGBAND	20		/* Max slots to "always" keep full */
#define STORE_TURNOVER_NPPMORIA		9		/* Normal shop turnover, per day */
#define STORE_MIN_KEEP_NPPMORIA		10		/* Min slots to "always" keep full */
#define STORE_MAX_KEEP_NPPMORIA		18		/* Max slots to "always" keep full */

/*
 * Misc constants
 */
#define TOWN_DAWN		10000	/* Number of turns from dawn to dawn XXX */
#define BREAK_GLYPH		400		/* Rune of protection resistance */
#define BTH_PLUS_ADJ	3		/* Adjust BTH per plus-to-hit */
#define MON_MULT_ADJ	8		/* High value slows multiplication */
#define QUEST_TURNS		1200	/* Number of turns between quest failure checks */
#define MON_DRAIN_LIFE	2		/* Percent of player exp drained per hit */
#define USE_DEVICE		3		/* x> Harder devices x< Easier devices */
#define INTEREST_OFFSET	10		/* Amount of levels OOD before a history is interesting */


/*
 * Percentage of maximum noise you can make just walking around, given a
 * stealth of zero.
 */
#define WAKEUP_ADJ      20


/*
 * More maximum values
 */
#define MAX_SIGHT	20	/* Maximum view distance */
#define MAX_RANGE	20	/* Maximum range (spells, etc) */


/*
 * There is a 1/160 chance per round of creating a new monster
 */
#define MAX_M_ALLOC_CHANCE	160

/*
 * Normal levels get at least 14 monsters
 */
#define MIN_M_ALLOC_LEVEL	14

/*
 * The town starts out with 4 residents during the day
 */
#define MIN_M_ALLOC_TD		4

/*
 * The town starts out with 8 residents during the night
 */
#define MIN_M_ALLOC_TN		8


/*
 * A monster can only "multiply" (reproduce) if there are fewer than 100
 * monsters on the level capable of such spontaneous reproduction.  This
 * is a hack which prevents the "mon_list[]" array from exploding due to
 * reproducing monsters.  Messy, but necessary.
 */
#define MAX_REPRO	100


/*
 * A "stack" of items is limited to less than 100 items (hard-coded).
 */
#define MAX_STACK_SIZE	100

/*
 * An item's pval (for charges, amount of gold, etc) is limited to s16b
 */
#define MAX_PVAL	 32767
#define MIN_PVAL	-32768


/*
 * Maximum number of objects allowed in a single dungeon grid.
 *
 * The main-screen has a minimum size of 24 rows, so we can always
 * display 23 objects + 1 header line.
 */
#define MAX_FLOOR_STACK			23


/*
 * Spell types used by project(), and related functions.
 */
#define GF_XXX1			1
#define GF_ARROW		2
#define GF_MISSILE		3
#define GF_MANA			4
#define GF_HOLY_ORB		5
#define GF_LIGHT_WEAK	6
#define GF_DARK_WEAK	7
#define GF_WATER		8
#define GF_PLASMA		9
#define GF_METEOR		10
#define GF_ICE			11
#define GF_GRAVITY		12
#define GF_INERTIA		13
#define GF_FORCE		14
#define GF_TIME			15
#define GF_ACID			16
#define GF_ELEC			17
#define GF_FIRE			18
#define GF_COLD			19
#define GF_POIS			20
#define GF_STATIC		21
#define GF_LIGHT		22
#define GF_DARK			23
#define GF_EXTINGUISH	24
#define GF_CONFUSION	25
#define GF_SOUND		26
#define GF_SHARD		27
#define GF_NEXUS		28
#define GF_NETHER		29
#define GF_CHAOS		30
#define GF_DISENCHANT	31
#define GF_CLEAR_AIR	32
#define GF_KILL_WALL	33
#define GF_KILL_DOOR	34
#define GF_KILL_TRAP	35
#define GF_MAKE_WALL	36
#define GF_MAKE_DOOR	37
#define GF_MAKE_TRAP	38
#define GF_STERILIZE	39
#define GF_SAND			40
#define GF_AWAY_UNDEAD	41
#define GF_AWAY_EVIL	42
#define GF_AWAY_ALL		43
#define GF_TURN_UNDEAD	44
#define GF_TURN_EVIL	45
#define GF_TURN_ALL		46
#define GF_DISP_UNDEAD	47
#define GF_DISP_EVIL	48
#define GF_DISP_ALL		49
#define GF_MAKE_WARY	50

#define GF_OLD_CLONE	51
#define GF_OLD_POLY		52
#define GF_OLD_HEAL		53
#define GF_OLD_SPEED	54
#define GF_OLD_SLOW		55
#define GF_OLD_CONF		56
#define GF_OLD_SLEEP	57
#define GF_LIFE_DRAIN	58
#define GF_SPORE		59
#define GF_MASS_IDENTIFY 60
#define GF_BRIDGE		61
#define GF_LAVA			62
#define GF_BWATER		63
#define GF_BMUD			64
#define GF_LOCK_DOOR	92

#define GF_FEATURE		95
#define GF_VAPOUR		96
#define GF_STEAM		97
#define GF_SMOKE		98
#define GF_FOG			99
#define GF_ELEC_BURST  100


/*
 * Some constants for the "learn" code.  These generalized from the
 * old DRS constants.
 */
#define LRN_FREE_SAVE	14
#define LRN_MANA		15
#define LRN_ACID		16
#define LRN_ELEC		17
#define LRN_FIRE		18
#define LRN_COLD		19
#define LRN_POIS		20
#define LRN_FEAR_SAVE	21
#define LRN_LIGHT		22
#define LRN_DARK		23
#define LRN_BLIND		24
#define LRN_CONFU		25
#define LRN_SOUND		26
#define LRN_SHARD		27
#define LRN_NEXUS		28
#define LRN_NETHR		29
#define LRN_CHAOS		30
#define LRN_DISEN		31
#define LRN_SAVE		39
#define LRN_ARCH		40
#define LRN_PARCH		41
#define LRN_ICE			42
#define LRN_PLAS		43
#define LRN_SOUND2		44 /* attacks which aren't resisted, but res sound prevent stun */
#define LRN_STORM		45
#define LRN_WATER		46
#define LRN_NEXUS_SAVE	47 /* Both resist Nexus and Saves apply */
#define LRN_BLIND_SAVE	48 /* Both resist Blind and Saves apply */
#define LRN_CONFU_SAVE	49 /* Both resist Confusion and Saves apply */
#define LRN_DARK_SAVE	50
#define LRN_HOLY_SAVE	51
#define LRN_LAVA  		62

#define LRN_MAX			63


/*
 * Keymap modes.
 */
enum
{
	KEYMAP_MODE_ORIG = 0,
	KEYMAP_MODE_ROGUE,

	KEYMAP_MODES		/* Total */
};


/*** Function flags ***/

#define PROJECT_NO			0
#define PROJECT_NOT_CLEAR	1
#define PROJECT_CLEAR		2


/*
 * Bit flags for the "project()", the "projectable()", and/or the
 * "project_path()" functions.
 *
 *   BEAM:  Work as a beam weapon (affect every grid passed through)
 *   ARC:   Act as an arc spell (a portion of a caster-centered ball)
 *   STAR:  Act as a starburst - a randomized ball
 *   BOOM:  Explode in some way
 *   WALL:  Affect one layer of any wall, even if not passable
 *   PASS:  Ignore walls entirely
 *   GRID:  Affect each grid in the "blast area" in some way
 *   ITEM:  Affect each object in the "blast area" in some way
 *   KILL:  Affect each monster in the "blast area" in some way
 *   PLAY:  Explicitly affect the player
 *   EFFECT Set Effects based on this projection
 *   HIDE:  Hack -- disable graphical effects of projection
 *   STOP:  Stop as soon as we hit a monster
 *   JUMP:  Jump directly to the target location
 *   THRU:  Continue "through" the target (used for projectiles)
 *   CHCK:  Note occupied grids, but do not stop at them
 *   ORTH:  Projection cannot move diagonally (used for wall spells)
 */

/* Projection types */
#define PROJECT_NONE		0x00000000
#define PROJECT_BEAM		0x00000001
#define PROJECT_ARC			0x00000002
#define PROJECT_STAR		0x00000004
#define PROJECT_ROCK		0x00000008 /* A boulder is being thrown, use rock graphic (affects visuals only) */
#define PROJECT_SHOT		0x00000010 /* A rock is being thrown/fired, use shot graphic (affects visuals only) */
#define PROJECT_AMMO		0x00000020 /* A bolt/arrow is being thrown/fired, use shot graphic (affects visuals only) */

/* What projections do */
#define PROJECT_BOOM		0x00000040
#define PROJECT_WALL		0x00000080
#define PROJECT_PASS		0x00000100  /*Ignore walls*/
#define PROJECT_ROOM		0x00000200  /* Create a room as you kill_wall */
#define PROJECT_SAME		0x00000400  /* Don't damage similar monsters */

/* What projections affect */
#define PROJECT_GRID		0x00000800
#define PROJECT_ITEM		0x00001000
#define PROJECT_KILL		0x00002000 /* Hurt the monsters*/
#define PROJECT_PLAY		0x00004000 /* Hurt the player*/
#define PROJECT_EFCT		0x00008000 /* Use Effects*/
#define PROJECT_CLOUD		0x00010000 /* Always set the effect regardless of damage*/
#define PROJECT_NO_EFCT		0x00020000 /* Cancel effects */

/* Graphics */
#define PROJECT_HIDE		0x00040000
#define PROJECT_NO_REDRAW	0x00080000
#define PROJECT_XXX9		0x00100000

/* How projections travel */
#define PROJECT_STOP		0x00200000
#define PROJECT_JUMP		0x00400000
#define PROJECT_THRU		0x00800000
#define PROJECT_CHCK		0x01000000
#define PROJECT_ORTH		0x02000000 /*(unused)*/
#define PROJECT_XX10		0x04000000

/* Projection blockage indicators */
#define PATH_G_FULL			0
#define PATH_G_BLCK			1
#define PATH_G_WALL			2
#define PATH_G_NONE			100

#define PATH_SIZE			512

/*Who caused the projection? */
#define SOURCE_PLAYER			-1	/*player is the source of projection*/
#define SOURCE_TRAP				-2	/*Trap*/
#define SOURCE_EFFECT			-3	/*Effect*/
#define SOURCE_OTHER			-4	/*Terrain, something other than player or monster*/
#define SOURCE_MONSTER_START	 0	/*Greater than 0 monster is the source*/


/*Mode whether to describe traps/spells or set off/cast them*/
#define MODE_DESCRIBE	1
#define MODE_ACTION		2
#define MODE_FLAGS		3


/*
 * An arc with a width (in degrees) less than this value will lose less
 * power over distance.
 */
#define ARC_STANDARD_WIDTH     90


/*
 * Bit flags for the "enchant()" function
 */
#define ENCH_TOHIT	0x01
#define ENCH_TODAM	0x02
#define ENCH_TOAC	0x04


/*
 * Bit flags for the "target_set" function
 *
 *	KILL: Target monsters
 *	LOOK: Describe grid fully
 *	XTRA: Currently unused flag
 *	GRID: Select from all grids
 *	PROBE: - Terrain can be targeted as well as a monster (for probing)
 */
#define TARGET_KILL		0x01
#define TARGET_LOOK		0x02
#define TARGET_XTRA		0x04
#define TARGET_GRID		0x08
#define TARGET_QUIET	0x08
#define TARGET_TRAP		0x20
#define TARGET_PROBE	0x40


#define LEV_THEME_HEAD				11
#define LEV_THEME_CREEPING_COIN		0	/*creeping coins*/
#define LEV_THEME_ORC				1	/*orc*/
#define LEV_THEME_TROLL				2	/*troll*/
#define LEV_THEME_OGRE				3	/*ogre*/
#define LEV_THEME_HOUND				4	/*hound*/
#define LEV_THEME_GIANT				5	/*giant*/
#define LEV_THEME_DRAGON_YOUNG		6	/*young dragon*/
#define LEV_THEME_DRAGON_ACID		7	/*acid dragon*/
#define LEV_THEME_DRAGON_FIRE		8	/*fire dragon*/
#define LEV_THEME_DRAGON_ELEC		9	/*electric dragon*/
#define LEV_THEME_DRAGON_COLD		10	/*cold dragon*/
#define LEV_THEME_DRAGON_POIS		11	/*poison dragon*/
#define LEV_THEME_DRAGON_CHROMATIC	12	/*chromatic dragon*/
#define LEV_THEME_DRAGON_MISC		13	/*dragon*/
#define LEV_THEME_DRAGON_ANCIENT	14	/*ancient dragon*/
#define LEV_THEME_JELLY				15	/*jelly*/
#define LEV_THEME_ORC_NAGA_YEEK_KOBOLD	16 /*kobold, yeek, orc, and naga*/
#define LEV_THEME_ANIMAL			17	/*animal*/
#define LEV_THEME_HUMANOID			18	/*humanoid*/
#define LEV_THEME_DEMON_MINOR		19	/*minor demon*/
#define LEV_THEME_DEMON_ALL			20	/*demon*/
#define LEV_THEME_DEMON_MAJOR		21	/*major demon*/
#define LEV_THEME_CAVE_DWELLER		22	/*cave dweller*/
#define LEV_THEME_UNDEAD			23	/*undead*/
#define LEV_THEME_DRAGON_ELEMENTAL	24	/*Elemental Dragons*/
#define LEV_THEME_VALAR_SERVANTS	25	/* Servants of the Valar*/
#define LEV_THEME_TAIL				26

#define PIT_LEVEL_BOOST				6
#define NEST_LEVEL_BOOST			5
#define PIT_NEST_QUEST_BOOST		8

#define THEMED_LEVEL_NO_QUEST_BOOST	7
#define THEMED_LEVEL_QUEST_BOOST	10



/*** Option Definitions ***/

/*
 * Option indexes (offsets)
 *
 * These values are hard-coded by savefiles (and various pieces of code).
 */
#define OPT_USER_INTERFACE			0
#define OPT_DISTURBANCE				16
#define OPT_GAME_PLAY				32
#define OPT_EFFICIENCY				48
#define OPT_BIRTH					128
#define OPT_CHEAT					160
#define OPT_ADULT					192
#define OPT_SCORE					224
#define OPT_NONE					255
#define OPT_MAX						256


/*
 * Option indexes (normal)
 *
 * These values are hard-coded by savefiles.
 */
#define OPT_rogue_like_commands		0
#define OPT_quick_messages			1
#define OPT_use_sound				2
#define OPT_carry_query_flag		3
#define OPT_use_old_target			4
#define OPT_always_pickup			5
#define OPT_floor_query_flag		6
/* xxx */
#define OPT_stack_force_notes		8
#define OPT_stack_force_costs		9
#define OPT_expand_inscribe			10
#define OPT_disturb_detect			11
/* xxx */
/* xxx */
#define OPT_ring_bell				14
#define OPT_show_flavors			15
/* xxx */
/* xxx */
/* xxx */
/* xxx */
#define OPT_disturb_move			20
#define OPT_disturb_near			21
/* xxx */
#define OPT_disturb_state			23
/* xxx */
/* xxx */
/* xxx OPT_alert_hitpoint */
/* xxx OPT_alert_failure */
#define OPT_verify_destroy			28
/* xxx */
/* xxx */
/* xxx */
/* xxx */
#define OPT_auto_scum				33
#define OPT_allow_themed_levels		34
/* xxx */
/* xxx */
/* xxx */
#define OPT_view_perma_grids		38
#define OPT_view_torch_grids		39
/* xxx */
/* xxx */
/* xxx */
/* xxx */
#define OPT_smart_cheat				47
/* xxx */
/* xxx */
/* xxx */
/* xxx */
#define OPT_flush_failure			52
#define OPT_flush_disturb			53
/* xxx */
/* xxx */
/* xxx */
/* xxx */
/* xxx */
#define OPT_hilight_player			59
#define OPT_view_yellow_light		60
#define OPT_view_bright_light		61
#define OPT_view_granite_light		62
#define OPT_view_special_light		63
#define OPT_easy_open 				64
#define OPT_easy_alter 				65
/* xxx */
#define OPT_show_piles				67
#define OPT_center_player			68
#define OPT_animate_flicker			69
#define OPT_xchars_to_file			70
#define OPT_auto_more				71
/* xxx */
#define OPT_auto_display_lists		73
#define OPT_hp_changes_color  		74
#define OPT_verify_leave_quest		75
#define OPT_mark_squelch_items		76
#define OPT_mouse_movement			77
#define OPT_mouse_buttons			78
#define OPT_notify_recharge			79
/* xxx */
/* xxx */
/* xxx xxx */
/* xxx xxx */						/*(OPT_BIRTH+0)*/
#define OPT_birth_no_selling		(OPT_BIRTH+1)
#define OPT_birth_maximize			(OPT_BIRTH+2)
#define OPT_birth_preserve			(OPT_BIRTH+3)
#define OPT_birth_ironman			(OPT_BIRTH+4)
#define OPT_birth_no_stores			(OPT_BIRTH+5)
#define OPT_birth_no_artifacts		(OPT_BIRTH+6)
#define OPT_birth_rand_artifacts	(OPT_BIRTH+7)
#define OPT_birth_no_stacking		(OPT_BIRTH+8)
#define OPT_birth_take_notes		(OPT_BIRTH+9)
#define OPT_birth_force_small_lev	(OPT_BIRTH+10)
#define OPT_birth_connected_stairs	(OPT_BIRTH+11)
#define OPT_birth_no_quests			(OPT_BIRTH+12)
#define OPT_birth_no_player_ghosts	(OPT_BIRTH+13)
#define OPT_birth_no_store_services	(OPT_BIRTH+14)
#define OPT_birth_no_xtra_artifacts	(OPT_BIRTH+15)
#define OPT_birth_money				(OPT_BIRTH+16)
#define OPT_birth_simple_dungeons	(OPT_BIRTH+17)
#define OPT_birth_swap_weapons		(OPT_BIRTH+18)

/* xxx xxx */
#define OPT_cheat_peek				(OPT_CHEAT+0)
#define OPT_cheat_hear				(OPT_CHEAT+1)
#define OPT_cheat_room				(OPT_CHEAT+2)
#define OPT_cheat_xtra				(OPT_CHEAT+3)
#define OPT_cheat_know				(OPT_CHEAT+4)
#define OPT_cheat_live				(OPT_CHEAT+5)
/* xxx xxx */
/* xxx xxx */       				/*(OPT_ADULT+0)*/
#define OPT_adult_no_selling		(OPT_ADULT+1)
#define OPT_adult_maximize			(OPT_ADULT+2)
#define OPT_adult_preserve			(OPT_ADULT+3)
#define OPT_adult_ironman			(OPT_ADULT+4)
#define OPT_adult_no_stores			(OPT_ADULT+5)
#define OPT_adult_no_artifacts		(OPT_ADULT+6)
#define OPT_adult_rand_artifacts	(OPT_ADULT+7)
#define OPT_adult_no_stacking		(OPT_ADULT+8)
#define OPT_adult_take_notes		(OPT_ADULT+9)
#define OPT_adult_force_small_lev	(OPT_ADULT+10)
#define OPT_adult_connected_stairs	(OPT_ADULT+11)
#define OPT_adult_no_quests			(OPT_ADULT+12)
#define OPT_adult_no_player_ghosts	(OPT_ADULT+13)
#define OPT_adult_no_store_services	(OPT_ADULT+14)
#define OPT_adult_no_xtra_artifacts	(OPT_ADULT+15)
#define OPT_adult_birth_money		(OPT_ADULT+16)
#define OPT_adult_simple_dungeons	(OPT_ADULT+17)
#define OPT_adult_swap_weapons		(OPT_ADULT+18)
/* xxx xxx */
#define OPT_score_peek				(OPT_SCORE+0)
#define OPT_score_hear				(OPT_SCORE+1)
#define OPT_score_room				(OPT_SCORE+2)
#define OPT_score_xtra				(OPT_SCORE+3)
#define OPT_score_know				(OPT_SCORE+4)
#define OPT_score_live				(OPT_SCORE+5)
/* xxx xxx */


/*
 * Hack -- Option symbols
 */
#define rogue_like_commands		op_ptr->opt[OPT_rogue_like_commands]
#define quick_messages			op_ptr->opt[OPT_quick_messages]
#define use_sound				op_ptr->opt[OPT_use_sound]
#define carry_query_flag		op_ptr->opt[OPT_carry_query_flag]
#define use_old_target			op_ptr->opt[OPT_use_old_target]
#define always_pickup			op_ptr->opt[OPT_always_pickup]
#define floor_query_flag		op_ptr->opt[OPT_floor_query_flag]
/* xxx */
#define stack_force_notes		op_ptr->opt[OPT_stack_force_notes]
#define stack_force_costs		op_ptr->opt[OPT_stack_force_costs]
#define expand_inscribe			op_ptr->opt[OPT_expand_inscribe]
/* xxx */
/* xxx */
/* xxx */
#define ring_bell				op_ptr->opt[OPT_ring_bell]
#define show_flavors			op_ptr->opt[OPT_show_flavors]
/* xxx */
/* xxx */
/* xxx */
/* xxx */
#define disturb_move			op_ptr->opt[OPT_disturb_move]
#define disturb_near			op_ptr->opt[OPT_disturb_near]
/* xxx */
#define disturb_state			op_ptr->opt[OPT_disturb_state]
/* xxx */
/* xxx */
#define disturb_detect			op_ptr->opt[OPT_disturb_detect]
/* xxx alert_failure */
#define verify_destroy			op_ptr->opt[OPT_verify_destroy]
/* xxx */
/* xxx */
/* xxx */
/* xxx */
#define auto_scum				op_ptr->opt[OPT_auto_scum]
#define allow_themed_levels		op_ptr->opt[OPT_allow_themed_levels]
/* xxx */
/* xxx */
/* xxx */
/* xxx */
#define view_perma_grids		op_ptr->opt[OPT_view_perma_grids]
#define view_torch_grids		op_ptr->opt[OPT_view_torch_grids]
/* xxx */
/* xxx */
/* xxx */
/* xxx */
#define smart_cheat				op_ptr->opt[OPT_smart_cheat]
/* xxx */
/* xxx */
/* xxx */
/* xxx */
#define flush_failure			op_ptr->opt[OPT_flush_failure]
#define flush_disturb			op_ptr->opt[OPT_flush_disturb]
/* xxx */
/* xxx */
/* xxx */
/* xxx */
/* xxx */
#define hilight_player			op_ptr->opt[OPT_hilight_player]
#define view_yellow_light		op_ptr->opt[OPT_view_yellow_light]
#define view_bright_light		op_ptr->opt[OPT_view_bright_light]
#define view_granite_light		op_ptr->opt[OPT_view_granite_light]
#define view_special_light		op_ptr->opt[OPT_view_special_light]
#define easy_open				op_ptr->opt[OPT_easy_open]
#define easy_alter				op_ptr->opt[OPT_easy_alter]
/* xxx */
#define show_piles				op_ptr->opt[OPT_show_piles]
#define center_player			op_ptr->opt[OPT_center_player]
#define animate_flicker 		op_ptr->opt[OPT_animate_flicker]
#define xchars_to_file			op_ptr->opt[OPT_xchars_to_file]
#define auto_more				op_ptr->opt[OPT_auto_more]
/* xxx */
#define auto_display_lists		op_ptr->opt[OPT_auto_display_lists]
/* xxx */
/* xxx */
/* xxx */
/* xxx */
#define birth_no_selling		op_ptr->opt[OPT_birth_no_selling]
#define birth_maximize			op_ptr->opt[OPT_birth_maximize]
#define birth_preserve			op_ptr->opt[OPT_birth_preserve]
#define birth_ironman			op_ptr->opt[OPT_birth_ironman]
#define birth_no_stores			op_ptr->opt[OPT_birth_no_stores]
#define birth_no_artifacts		op_ptr->opt[OPT_birth_no_artifacts]
#define birth_rand_artifacts	op_ptr->opt[OPT_birth_rand_artifacts]
#define birth_no_stacking		op_ptr->opt[OPT_birth_no_stacking]
#define birth_take_notes		op_ptr->opt[OPT_birth_take_notes]
#define	birth_force_small_lev	op_ptr->opt[OPT_birth_force_small_lev]
#define	birth_connected_stairs	op_ptr->opt[OPT_birth_connected_stairs]
#define birth_no_quests			op_ptr->opt[OPT_birth_no_quests]
#define birth_no_player_ghosts	op_ptr->opt[OPT_birth_no_player_ghosts]
#define birth_no_store_services	op_ptr->opt[OPT_birth_no_store_services]
#define birth_no_xtra_artifacts	op_ptr->opt[OPT_birth_no_xtra_artifacts]
#define birth_money				op_ptr->opt[OPT_birth_money]
#define birth_simple_dungeons	op_ptr->opt[OPT_birth_simple_dungeons]
#define birth_swap_weapons		op_ptr->opt[OPT_birth_swap_weapons]

/* xxx xxx */
#define cheat_peek				op_ptr->opt[OPT_cheat_peek]
#define cheat_hear				op_ptr->opt[OPT_cheat_hear]
#define cheat_room				op_ptr->opt[OPT_cheat_room]
#define cheat_xtra				op_ptr->opt[OPT_cheat_xtra]
#define cheat_know				op_ptr->opt[OPT_cheat_know]
#define cheat_live				op_ptr->opt[OPT_cheat_live]
/* xxx xxx */
/* xxx xxx */
#define adult_no_selling		op_ptr->opt[OPT_adult_no_selling]
#define adult_maximize			op_ptr->opt[OPT_adult_maximize]
#define adult_preserve			op_ptr->opt[OPT_adult_preserve]
#define adult_ironman			op_ptr->opt[OPT_adult_ironman]
#define adult_no_stores			op_ptr->opt[OPT_adult_no_stores]
#define adult_no_artifacts		op_ptr->opt[OPT_adult_no_artifacts]
#define adult_rand_artifacts	op_ptr->opt[OPT_adult_rand_artifacts]
#define adult_no_stacking		op_ptr->opt[OPT_adult_no_stacking]
#define adult_take_notes		op_ptr->opt[OPT_adult_take_notes]
#define	adult_force_small_lev	op_ptr->opt[OPT_adult_force_small_lev]
#define	adult_connected_stairs	op_ptr->opt[OPT_adult_connected_stairs]
#define adult_no_quests			op_ptr->opt[OPT_adult_no_quests]
#define adult_no_player_ghosts	op_ptr->opt[OPT_adult_no_player_ghosts]
#define adult_no_store_services	op_ptr->opt[OPT_adult_no_store_services]
#define adult_no_xtra_artifacts	op_ptr->opt[OPT_adult_no_xtra_artifacts]
#define adult_money				op_ptr->opt[OPT_adult_money]
#define adult_simple_dungeons	op_ptr->opt[OPT_adult_simple_dungeons]
#define adult_swap_weapons		op_ptr->opt[OPT_adult_swap_weapons]

#define hp_changes_color		op_ptr->opt[OPT_hp_changes_color]
#define verify_leave_quest		op_ptr->opt[OPT_verify_leave_quest]
#define mark_squelch_items		op_ptr->opt[OPT_mark_squelch_items]
#define mouse_movement			op_ptr->opt[OPT_mouse_movement]
#define mouse_buttons			op_ptr->opt[OPT_mouse_buttons]
#define notify_recharge			op_ptr->opt[OPT_notify_recharge]

#define score_peek				op_ptr->opt[OPT_score_peek]
#define score_hear				op_ptr->opt[OPT_score_hear]
#define score_room				op_ptr->opt[OPT_score_room]
#define score_xtra				op_ptr->opt[OPT_score_xtra]
#define score_know				op_ptr->opt[OPT_score_know]
#define score_live				op_ptr->opt[OPT_score_live]
/* xxx xxx */


/*
 * Information for "do_cmd_options()".
 */
#define OPT_PAGE_MAX			5
#define OPT_PAGE_PER			20



/*** Macro Definitions ***/


/*
 * Hack -- The main "screen"
 */
#define term_screen	(angband_term[0])

/*
 * Convert an "attr"/"char" pair into a "pict" (P)
 */
#define PICT(A,C) \
	((((u16b)(A)) << 8) | ((byte)(C)))

/*
 * Convert a "pict" (P) into an "attr" (A)
 */
#define PICT_A(P) \
	((byte)((P) >> 8))

/*
 * Convert a "pict" (P) into an "char" (C)
 */
#define PICT_C(P) \
	((char)((byte)(P)))


/*
 * Convert a "location" (Y,X) into a "grid" (G)
 */
#define GRID(Y,X) \
	(256 * (Y) + (X))

/*
 * Convert a "grid" (G) into a "location" (Y)
 */
#define GRID_Y(G) \
	((int)((G) / 256U))

/*
 * Convert a "grid" (G) into a "location" (X)
 */
#define GRID_X(G) \
	((int)((G) % 256U))


/*
 * Determines if a map location is "meaningful"
 */
#define in_bounds(Y,X) \
	(((unsigned)(Y) < (unsigned)(p_ptr->cur_map_hgt)) && \
	 ((unsigned)(X) < (unsigned)(p_ptr->cur_map_wid)))

/*
 * Determines if a map location is fully inside the outer walls
 * This is more than twice as expensive as "in_bounds()", but
 * often we need to exclude the outer walls from calculations.
 */
#define in_bounds_fully(Y,X) \
	(((Y) > 0) && ((Y) < p_ptr->cur_map_hgt-1) && \
	 ((X) > 0) && ((X) < p_ptr->cur_map_wid-1))


/*
 * Determines if a map location is currently "on screen"
 * Note that "panel_contains(Y,X)" always implies "in_bounds(Y,X)".
 * Pre-storing this into a cave_info flag would be nice.  XXX XXX
 */
#define panel_contains(Y,X) \
	(((unsigned)((Y) - Term->offset_y) < (unsigned)(SCREEN_HGT)) && \
	 ((unsigned)((X) - Term->offset_x) < (unsigned)(SCREEN_WID)))


/*
 * Determine if a "legal" grid is within "los" of the player
 *
 * Note the use of comparison to zero to force a "boolean" result
 */
#define player_has_los_bold(Y,X) \
	((cave_info[Y][X] & (CAVE_VIEW)) != 0)

/*
 * Determine if the player has a clear enough head to observe things
 */

#define player_can_observe() \
	((!p_ptr->timed[TMD_BLIND]) && (!p_ptr->timed[TMD_CONFUSED]) && (!p_ptr->timed[TMD_IMAGE]) && (!p_ptr->timed[TMD_PARALYZED]))


/*
 * Determine if a "legal" grid can be "seen" by the player
 *
 * Note the use of comparison to zero to force a "boolean" result
 */
#define player_can_see_bold(Y,X) \
	((cave_info[Y][X] & (CAVE_SEEN)) != 0)

/*
 * Determine if a "legal" grid is within "line of fire" of the player
 *
 * Note the use of comparison to zero to force a "boolean" result
 */
#define player_can_fire_bold(Y,X) \
	((cave_info[Y][X] & (CAVE_FIRE)) != 0)


/*
 * Convert a "key event" into a "location" (Y)
 */
#define KEY_GRID_Y(K) \
	((int) ((K.mousey - ROW_MAP) + Term->offset_y))

/*
 * Convert a "key event" into a "location" (X)
 */
#define KEY_GRID_X(K) \
	((int) (((K.mousex - COL_MAP) / (use_bigtile ? 2 : 1)) + Term->offset_x))


/*
 * Maximum number of macro trigger names
 */
#define MAX_MACRO_TRIGGER	200
#define MAX_MACRO_MOD		12


/*
 * Available graphic modes
 */
#define GRAPHICS_NONE			0
#define GRAPHICS_ORIGINAL		1
#define GRAPHICS_ADAM_BOLT		2
#define GRAPHICS_DAVID_GERVAIS	3
#define GRAPHICS_PSEUDO			4


/*
 * List of commands that will be auto-repeated
 *
 * ToDo: This string should be user-configurable.

 */
#define AUTO_REPEAT_COMMANDS "TBDoc+"


/*
 * Artifact activation index
 */
#define ACT_ILLUMINATION        0
#define ACT_MAGIC_MAP           1
#define ACT_CLAIRVOYANCE        2
#define ACT_PROT_EVIL           3
#define ACT_DISP_EVIL           4
#define ACT_HEAL1               5
#define ACT_HEAL2               6
#define ACT_CURE_WOUNDS         7
#define ACT_HASTE1              8
#define ACT_HASTE2              9
#define ACT_FIRE1               10
#define ACT_FIRE2               11
#define ACT_FIRE3               12
#define ACT_FROST1              13
#define ACT_FROST2              14
#define ACT_FROST3              15
#define ACT_FROST4              16
#define ACT_FROST5              17
#define ACT_ACID1               18
#define ACT_RECHARGE1           19
#define ACT_SLEEP               20
#define ACT_LIGHTNING_BOLT      21
#define ACT_ELEC2               22
#define ACT_BANISHMENT          23
#define ACT_MASS_BANISHMENT     24
#define ACT_IDENTIFY            25
#define ACT_DRAIN_LIFE1         26
#define ACT_DRAIN_LIFE2         27
#define ACT_BIZZARE             28
#define ACT_STAR_BALL           29
#define ACT_RAGE_BLESS_RESIST   30
#define ACT_PHASE               31
#define ACT_TRAP_DOOR_DEST      32
#define ACT_DETECT              33
#define ACT_RESIST              34
#define ACT_TELEPORT            35
#define ACT_RESTORE_LIFE        36
#define ACT_MISSILE             37
#define ACT_ARROW               38
#define ACT_REM_FEAR_POIS       39
#define ACT_STINKING_CLOUD      40
#define ACT_STONE_TO_MUD        41
#define ACT_TELE_AWAY           42
#define ACT_WOR                 43
#define ACT_CONFUSE             44
#define ACT_PROBE               45
#define ACT_FIREBRAND           46
#define ACT_STARLIGHT           47
#define ACT_MANA_BOLT           48
#define ACT_BERSERKER           49
#define ACT_RES_ACID			50
#define ACT_RES_ELEC			51
#define ACT_RES_FIRE			52
#define ACT_RES_COLD			53
#define ACT_RES_POIS			54

#define ACT_MAX					55


/*
 * HACK - define if the source contains the cleanup_angband() function.
 */
#define HAS_CLEANUP


/*
 * Given an array, determine how many elements are in the array.
 */
#define N_ELEMENTS(a) (sizeof(a) / sizeof((a)[0]))


/*
 * There is a 1/40 (2.5%) chance of inflating the requested monster_level
 * during the creation of a monsters (see "get_mon_num()" in "monster.c").
 * Lower values yield harder monsters more often.
 */
#define NASTY_MON	(auto_scum ? 30 : 40)	/* 1/chance of inflated monster level */

/*
 * Special note used to mark the end of the notes section in the savefile
 */
#define NOTES_MARK "@@@@@@@ No more notes @@@@@@@"


/*
 * Get the index of a shaded color in angband_color_table.
 * base_color must be between 0 and 15. These are the TERM_* constants.
 * shade must be between 0 and 7 (8 shades supported, 0 is the base color).
 * 16 shades could be supported but the values returned are in the range 0-127
 * to avoid clashes with graphic or big_tile modes (bit 0x80 is reserved)
 * All shades will look like the base color in 16 color ports.
 */
#define MAKE_EXTENDED_COLOR(base_color, shade) \
((((shade) << 4) | ((base_color) & 0x0F)) & 0x7F)

/*
 * Get the base color of a given extended color (shade).
 * Values returned are in the range 0-15 (TERM_*). See above.
 */
#define GET_BASE_COLOR(ext_color) ((ext_color) & 0x0F)

/*
 * Get the shade number of a given extended color.
 * Values returned are in the range 0-7. See above.
 */
#define GET_SHADE(ext_color) (((ext_color) >> 4) & 0x07)


/*
 * Maximum number of colours, and number of "basic" Angband colours
 */
#define MAX_COLORS			128
#define MAX_COLOR_USED		(TERM_EARTH_YELLOW + 1)

/* For the colors to tiles table */
#define TILE_BALL_INFO	0
#define TILE_BOLT_INFO	1

/*
 * Number of base colors. These are the TERM_* constants
 */

#define MAX_BASE_COLORS	16
#define BASIC_COLORS	MAX_BASE_COLORS

/*
 * Number of shades, including the shade 0 (base colors)
 */
#define MAX_SHADES		8


/*
 * These are the return values of squelch_itemp()
 */
#define SQUELCH_FAILED	-1
#define SQUELCH_NO		0
#define SQUELCH_YES		1


/* Utilitary macros for the grid queue type */

/* These two return the coordinates of the grid at the front of the queue */
/* THE QUEUE MUST CONTAIN AT LEAST ONE ELEMENT */
#define GRID_QUEUE_Y(Q) ((Q)->data[(Q)->head].y)
#define GRID_QUEUE_X(Q) ((Q)->data[(Q)->head].x)

/* Returns TRUE if the given queue is empty */
#define GRID_QUEUE_EMPTY(Q) ((Q)->head == (Q)->tail)

/* Returns TRUE if the given queue is full */
#define GRID_QUEUE_FULL(Q) ((((Q)->tail + 1) % (Q)->max_size) == (Q)->head)

/*Square a number*/
#define GET_SQUARE(X) 	((X) * (X))


/* Level types */
#define DUNGEON_TYPE_DEFAULT		1	/* Vanilla-like levels */
#define DUNGEON_TYPE_TOWN			2
#define DUNGEON_TYPE_THEMED_LEVEL	3
#define DUNGEON_TYPE_WILDERNESS		4
#define DUNGEON_TYPE_ARENA			5
#define DUNGEON_TYPE_LABYRINTH		6
#define DUNGEON_TYPE_GREATER_VAULT	7

/** Constants for the various patterns of pits */
#define MAX_PIT_PATTERNS	3
#define PIT_WIDTH			19
#define PIT_HEIGHT			5


/*
 * Return TRUE if the player activated debug mode
 */
#define DEBUG_MODE_ACTIVATED ((p_ptr->noscore & 0x0008) != 0)

/*
 * Return TRUE if the player can be disturbed (resting, running, etc.)
 * If stop_search is TRUE we check if the player is searching unusual things
 */
#define CHECK_DISTURB(stop_search) \
(p_ptr->command_rep || p_ptr->running || p_ptr->resting || ((stop_search) && p_ptr->searching))

/*
 * Minimum values for panel change offsets
 */
#define MIN_PANEL_CHANGE_OFFSET_Y 2
#define MIN_PANEL_CHANGE_OFFSET_X 4


/*
 * Hack -- The quest indicator timer is compacted along with a bit that indicates
 * whether the current quest was successfully completed or not.
 * The value of the bit is 0 if the quest was failed.
 */
#define QUEST_INDICATOR_COMPLETE_BIT 0x8000


/* A savefile must have at least this value inserted at the end of the extension
 * management section */
#define END_EXTENSIONS 438

/* You have to store one of these before every variable in a extension */
#define EXTENSION_TYPE_S32B 1
#define EXTENSION_TYPE_U32B 2
#define EXTENSION_TYPE_S16B 3
#define EXTENSION_TYPE_U16B 4
#define EXTENSION_TYPE_STRING 5
#define EXTENSION_TYPE_BYTE 6
/* Every extension must end with this */
#define EXTENSION_TYPE_END 7

/* player_type.noscore flags */
#define NOSCORE_WIZARD		0x0002
#define NOSCORE_DEBUG		0x0008


#define SCAN_INSTANT	((u32b) -1)
#define SCAN_OFF		0
#define SCAN_MACRO		45


#define MAX_ITEMLIST	1024


#endif /* INCLUDED_DEFINES_H */
