/**
 * \file player.h
 * \brief Player implementation
 *
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 * Copyright (c) 2011 elly+angband@leptoquark.net. See COPYING.
 * Copyright (c) 2015 Nick McConnell
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

#ifndef PLAYER_H
#define PLAYER_H

#include "guid.h"
#include "obj-properties.h"
#include "object.h"
#include "option.h"

/**
 * Indexes of the player stats (hard-coded by savefiles).
 */
enum {
	#define STAT(a) STAT_##a,
	#include "list-stats.h"
	#undef STAT

	STAT_MAX
};

/**
 * Player race and class flags
 */
enum
{
	#define PF(a) PF_##a,
	#include "list-player-flags.h"
	#undef PF
	PF_MAX
};

#define PF_SIZE                FLAG_SIZE(PF_MAX)

#define pf_has(f, flag)        flag_has_dbg(f, PF_SIZE, flag, #f, #flag)
#define pf_next(f, flag)       flag_next(f, PF_SIZE, flag)
#define pf_count(f)            flag_count(f, PF_SIZE)
#define pf_is_empty(f)         flag_is_empty(f, PF_SIZE)
#define pf_is_full(f)          flag_is_full(f, PF_SIZE)
#define pf_is_inter(f1, f2)    flag_is_inter(f1, f2, PF_SIZE)
#define pf_is_subset(f1, f2)   flag_is_subset(f1, f2, PF_SIZE)
#define pf_is_equal(f1, f2)    flag_is_equal(f1, f2, PF_SIZE)
#define pf_on(f, flag)         flag_on_dbg(f, PF_SIZE, flag, #f, #flag)
#define pf_off(f, flag)        flag_off(f, PF_SIZE, flag)
#define pf_wipe(f)             flag_wipe(f, PF_SIZE)
#define pf_setall(f)           flag_setall(f, PF_SIZE)
#define pf_negate(f)           flag_negate(f, PF_SIZE)
#define pf_copy(f1, f2)        flag_copy(f1, f2, PF_SIZE)
#define pf_union(f1, f2)       flag_union(f1, f2, PF_SIZE)
#define pf_inter(f1, f2)       flag_inter(f1, f2, PF_SIZE)
#define pf_diff(f1, f2)        flag_diff(f1, f2, PF_SIZE)

/**
 * The range of possible indexes into tables based upon stats.
 * Currently things range from 3 to 18/220 = 40.
 */
#define STAT_RANGE 38

/**
 * Player constants
 */
#define PY_MAX_EXP		99999999L	/* Maximum exp */
#define PY_KNOW_LEVEL	30			/* Level to know all runes */
#define PY_MAX_LEVEL	50			/* Maximum level */

/**
 * Flags for player.spell_flags[]
 */
#define PY_SPELL_LEARNED    0x01 	/* Spell has been learned */
#define PY_SPELL_WORKED     0x02 	/* Spell has been successfully tried */
#define PY_SPELL_FORGOTTEN  0x04 	/* Spell has been forgotten */

#define BTH_PLUS_ADJ    	3 		/* Adjust BTH per plus-to-hit */

/**
 * Ways in which players can be marked as cheaters
 */
#define NOSCORE_WIZARD		0x0002
#define NOSCORE_DEBUG		0x0008
#define NOSCORE_JUMPING     0x0010

/**
 * Terrain that the player has a chance of digging through
 */
enum {
	DIGGING_RUBBLE = 0,
	DIGGING_MAGMA,
	DIGGING_QUARTZ,
	DIGGING_GRANITE,
	DIGGING_DOORS,

	DIGGING_MAX
};

/**
 * Skill indexes
 */
enum {
	SKILL_DISARM_PHYS,		/* Disarming - physical */
	SKILL_DISARM_MAGIC,		/* Disarming - magical */
	SKILL_DEVICE,			/* Magic Devices */
	SKILL_SAVE,				/* Saving throw */
	SKILL_SEARCH,			/* Searching ability */
	SKILL_STEALTH,			/* Stealth factor */
	SKILL_TO_HIT_MELEE,		/* To hit (normal) */
	SKILL_TO_HIT_BOW,		/* To hit (shooting) */
	SKILL_TO_HIT_THROW,		/* To hit (throwing) */
	SKILL_DIGGING,			/* Digging */

	SKILL_MAX
};

/**
 * Types of quest
 */
enum quest_type {
	QUEST_MONSTER = 0,
	QUEST_UNIQUE,
	QUEST_PLACE,
	QUEST_FINAL,
	QUEST_MAX
};

/**
 * Structure for quest locations per map
 */
struct quest_place {
	struct level_map *map;
	int place;
	bool block;
	struct quest_place *next;
};

/**
 * Structure for quest artifacts
 */
struct quest_artifact {
	int index;
	int chance;
	struct quest_artifact *next;
};

/**
 * Structure for the quests
 */
struct quest {
	struct quest *next;
	char *name;
	int type;
	struct quest_place *place;	/* Place */
	struct monster_race *race;	/* Monster race */
	struct quest_artifact *arts;/* Artifacts dropped by quest unique */
	int cur_num;				/* Number killed/found */
	int max_num;				/* Number required */
	bool complete;
};

/**
 * A single equipment slot
 */
struct equip_slot {
	struct equip_slot *next;

	uint16_t type;
	char *name;
	struct object *obj;
};

/**
 * A player 'body'
 */
struct player_body {
	struct player_body *next;

	char *name;
	uint16_t count;
	struct equip_slot *slots;
};

struct player_race;

/**
 * Player race list structure to record likes and dislikes
 */
struct player_race_list {
	struct player_race_list *next;
	const char *name;
	struct player_race *race;
	int rel;
};

/**
 * Player race info
 */
struct player_race {
	struct player_race *next;
	const char *name;
	struct player_race_list *dislikes;

	unsigned int ridx;

	int r_mhp;		/**< Hit-dice modifier */
    int difficulty;	/**< Race difficulty factor */
    int start_lev;	/**< Race starting level */
    char *hometown;	/**< Race starting town */

	int b_age;		/**< Base age */
	int m_age;		/**< Mod age */

	int base_hgt;	/**< Base height */
	int mod_hgt;	/**< Mod height */
	int base_wgt;	/**< Base weight */
	int mod_wgt;	/**< Mod weight */

	int infra;		/**< Infra-vision range */

	int body;		/**< Race body */

	int r_adj[STAT_MAX];		/**< Stat bonuses */

	int r_skills[SKILL_MAX];	/**< Skills */

	bitflag flags[OF_SIZE];		/**< Racial (object) flags */
	bitflag pflags[PF_SIZE];	/**< Racial (player) flags */

	struct history_chart *history;

	struct element_info el_info[ELEM_MAX]; /**< Resists */
};

/**
 * Blow names for shapechanged players
 */
struct player_blow {
	struct player_blow *next;
	char *name;
};

/**
 * Player shapechange shape info
 */
struct player_shape {
	struct player_shape *next;
	const char *name;

	int sidx;

	int to_a;				/**< Plusses to AC */
	int to_h;				/**< Plusses to hit */
	int to_d;				/**< Plusses to damage */

	int skills[SKILL_MAX];  /**< Skills */
	bitflag flags[OF_SIZE];		/**< Shape (object) flags */
	bitflag pflags[PF_SIZE];	/**< Shape (player) flags */
	int16_t modifiers[OBJ_MOD_MAX];	/**< Stat and other modifiers*/
	const char *breath_name;
	struct activation *breath;
	struct element_info el_info[ELEM_MAX]; /**< Resists */

	struct effect *effect;	/**< Effect on taking this shape (effects.c) */

	struct player_blow *blows;
	int num_blows;
};

/**
 * Items the player starts with.  Used in player_class and specified in
 * class.txt.
 */
struct start_item {
	int tval;	/**< General object type (see TV_ macros) */
	int sval;	/**< Object sub-type  */
	int min;	/**< Minimum starting amount */
	int max;	/**< Maximum starting amount */
	char *ego;	/**< Ego type if any */
	int *eopts;     /**< Indices (zero terminated array) for birth options which can exclude item */
	struct start_item *next;
};

/**
 * Structure for magic realms
 */
struct magic_realm {
	struct magic_realm *next;
	char *code;
	char *name;
	int stat;
	char *verb;
	char *spell_noun;
	char *book_noun;
};

/**
 * A structure to hold class-dependent information on spells.
 */
struct class_spell {
	char *name;
	char *text;

	struct effect *effect;	/**< The spell's effect */
	const struct magic_realm *realm;	/**< The magic realm of this spell */

	int sidx;				/**< The index of this spell for this class */
	int bidx;				/**< The index into the player's books array */
	int slevel;				/**< Required level (to learn) */
	int smana;				/**< Required mana (to cast) */
	int sfail;				/**< Base chance of failure */
	int sexp;				/**< Encoded experience bonus */
};

/**
 * A structure to hold class-dependent information on spell books.
 */
struct class_book {
	int tval;							/**< Item type of the book */
	int sval;							/**< Item sub-type for book */
	bool dungeon;						/**< Whether this is a dungeon book */
	int num_spells;						/**< Number of spells in this book */
	const struct magic_realm *realm;	/**< The magic realm of this book */
	struct class_spell *spells;			/**< Spells in the book*/
};

/**
 * Information about class magic knowledge
 */
struct class_magic {
	int spell_first;			/**< Level of first spell */
	int spell_weight;			/**< Max armor weight to avoid mana penalties */
	int num_books;				/**< Number of spellbooks */
	struct class_book *books;	/**< Details of spellbooks */
	int total_spells;			/**< Number of spells for this class */
};

/**
 * Player class info
 */
struct player_class {
	struct player_class *next;
	const char *name;
	unsigned int cidx;

	const char *title[10];		/**< Titles */

	int c_adj[STAT_MAX];		/**< Stat modifier */

	int c_skills[SKILL_MAX];	/**< Class skills */
	int x_skills[SKILL_MAX];	/**< Extra skills */

	int c_mhp;					/**< Hit-dice adjustment */

	bitflag flags[OF_SIZE];		/**< (Object) flags */
	bitflag pflags[PF_SIZE];	/**< (Player) flags */
	bitflag specialties[PF_SIZE];	/**< Specialty abilities for this class */

	int max_attacks;			/**< Maximum possible attacks */
	int min_weight;				/**< Minimum weapon weight for calculations */
	int att_multiply;			/**< Multiplier for attack calculations */

	struct start_item *start_items; /**< Starting inventory */

	struct class_magic magic;	/**< Magic spells */
};

/**
 * Info for player abilities
 */
struct player_ability {
	struct player_ability *next;
	uint16_t index;			/* PF_*, OF_* or element index */
	char *type;			/* Ability type */
	char *name;			/* Ability short description */
	char *desc;			/* Ability description */
	int group;			/* Ability group (set locally when viewing) */
	int value;			/* Resistance value for elements */
};

/**
 * Histories are a graph of charts; each chart contains a set of individual
 * entries for that chart, and each entry contains a text description and a
 * successor chart to move history generation to.
 * For example:
 * 	chart 1 {
 * 		entry {
 * 			desc "You are the illegitimate and unacknowledged child";
 * 			next 2;
 * 		};
 * 		entry {
 * 			desc "You are the illegitimate but acknowledged child";
 * 			next 2;
 * 		};
 * 		entry {
 * 			desc "You are one of several children";
 * 			next 3;
 * 		};
 * 	};
 *
 * History generation works by walking the graph from the starting chart for
 * each race, picking a random entry (with weighted probability) each time.
 */
struct history_entry {
	struct history_entry *next;
	struct history_chart *succ;
	int isucc;
	int roll;
	char *text;
};

struct history_chart {
	struct history_chart *next;
	struct history_entry *entries;
	unsigned int idx;
};

/**
 * Player history information
 *
 * See player-history.c/.h
 */
struct player_history {
	struct history_info *entries;	/**< List of entries */
	size_t next;					/**< First unused entry */
	size_t length;					/**< Current length */
};

/**
 * All the variable state that changes when you put on/take off equipment.
 * Player flags are not currently variable, but useful here so monsters can
 * learn them.
 */
struct player_state {
	int stat_add[STAT_MAX];	/**< Equipment stat bonuses */
	int stat_ind[STAT_MAX];	/**< Indexes into stat tables */
	int stat_use[STAT_MAX];	/**< Current modified stats */
	int stat_top[STAT_MAX];	/**< Maximal modified stats */

	int skills[SKILL_MAX];		/**< Skills */

	int speed;			/**< Current speed */

	int num_blows;		/**< Number of blows x100 */
	int num_shots;		/**< Number of shots x10 */
	int num_moves;		/**< Number of extra movement actions */

	int ammo_mult;		/**< Ammo multiplier */
	int ammo_tval;		/**< Ammo variety */

	int ac;				/**< Base ac */
	int dam_red;		/**< Damage reduction */
	int perc_dam_red;	/**< Percentage damage reduction */
	int to_a;			/**< Bonus to ac */
	int to_h;			/**< Bonus to hit */
	int to_d;			/**< Bonus to dam */

	int see_infra;		/**< Infravision range */

	int cur_light;		/**< Radius of light (if any) */

	int evasion_chance;	/**< Evasion chance (if any) */

    bool shield_on_back;/**< Player carrying a shield on his back. -LM- */
	bool heavy_wield;	/**< Heavy weapon */
	bool heavy_shoot;	/**< Heavy shooter */
	bool bless_wield;	/**< Blessed (or blunt) weapon */

	bool cumber_armor;	/**< Mana draining armor */

	bitflag flags[OF_SIZE];					/**< Status flags from race and items */
	bitflag pflags[PF_SIZE];				/**< Player intrinsic flags */
	struct element_info el_info[ELEM_MAX];	/**< Resists from race and items */
};

#define player_has(p, flag)       (pf_has(p->state.pflags, (flag)))

/**
 * Temporary, derived, player-related variables used during play but not saved
 *
 * XXX Some of these probably should go to the UI
 */
struct player_upkeep {
	bool playing;			/* True if player is playing */
	bool autosave;			/* True if autosave is pending */
	bool generate_level;	/* True if level needs regenerating */
	bool only_partial;		/* True if only partial updates are needed */
	bool dropping;			/* True if auto-drop is in progress */
	bool free_energy;		/* True if this turn is using no energy */

	int energy_use;			/* Energy use this turn */
	int new_spells;			/* Number of spells available */
	int new_specialties;	/* Number of specialties available */

	struct monster *health_who;			/* Health bar trackee */
	struct monster_race *monster_race;	/* Monster race trackee */
	struct object *object;				/* Object trackee */
	struct object_kind *object_kind;	/* Object kind trackee */

	uint32_t notice;		/* Bit flags for pending actions such as
							 * reordering inventory, ignoring, etc. */
	uint32_t update;		/* Bit flags for recalculations needed
							 * such as HP, or visible area */
	uint32_t redraw;		/* Bit flags for things that /have/ changed,
							 * and just need to be redrawn by the UI,
							 * such as HP, Speed, etc.*/

	int command_wrk;		/* Used by the UI to decide whether
							 * to start off showing equipment or
							 * inventory listings when offering
							 * a choice.  See obj-ui.c */

	bool light_level;		/* Level is to be lit on creation */
	bool arena_level;		/* Current level is an arena */
    int path_coord;			/**< Records where the exit path was */
	int create_stair;		/* Create defined stair on next level */

	int resting;			/* Resting counter */

	int running;				/* Running counter */
	bool running_withpathfind;	/* Are we using the pathfinder ? */
	bool running_firststep;		/* Is this our first step running? */

	struct object **quiver;	/* Quiver objects */
	struct object **inven;	/* Inventory objects */
	int total_weight;		/* Total weight being carried */
	int inven_cnt;			/* Number of items in inventory */
	int equip_cnt;			/* Number of items in equipment */
	int quiver_cnt;			/* Number of items in the quiver */
	int recharge_pow;		/* Power of recharge effect */
};

/**
 * Most of the "player" information goes here.
 *
 * This stucture gives us a large collection of player variables.
 *
 * This entire structure is wiped when a new character is born.
 *
 * This structure is more or less laid out so that the information
 * which must be saved in the savefile precedes all the information
 * which can be recomputed as needed.
 */
struct player {
	const struct player_race *race;
	const struct player_class *class;

	struct loc grid;	/* Player location */
	struct loc old_grid;/* Player location before leaving for an arena */

	uint8_t hitdie;		/* Hit dice (sides) */

	int16_t age;		/* Characters age */
	int16_t ht;		/* Height */
	int16_t wt;		/* Weight */

	int32_t au;		/* Current Gold */

	int16_t max_depth;	/* Max depth */
    int16_t home;		/* Home town */
	int16_t depth;		/* Cur depth */

    int16_t recall[4];	/* Recall points */
    int16_t recall_pt;	/* Which recall point is active */
    int16_t place;		/* Current place */
    int16_t last_place;/* Previous place */

	int16_t max_lev;	/* Max level */
	int16_t lev;		/* Cur level */

	int32_t max_exp;	/* Max experience */
	int32_t exp;		/* Cur experience */
	uint16_t exp_frac;	/* Cur exp frac (times 2^16) */

	int16_t mhp;		/* Max hit pts */
	int16_t chp;		/* Cur hit pts */
	uint16_t chp_frac;	/* Cur hit frac (times 2^16) */

	int16_t msp;		/* Max mana pts */
	int16_t csp;		/* Cur mana pts */
	uint16_t csp_frac;	/* Cur mana frac (times 2^16) */

	int16_t stat_max[STAT_MAX];	/* Current "maximal" stat values */
	int16_t stat_cur[STAT_MAX];	/* Current "natural" stat values */
	int16_t stat_map[STAT_MAX];	/* Tracks remapped stats from temp stat swap */

	int16_t *timed;				/* Timed effects */

	int16_t word_recall;			/* Word of recall counter */
	int16_t deep_descent;			/* Deep Descent counter */

	int16_t energy;				/* Current energy */
	uint32_t total_energy;			/* Total energy used (including resting) */
	uint32_t resting_turn;			/* Number of player turns spent resting */

	int16_t food;				/* Current nutrition */

	uint8_t unignoring;			/* Unignoring */

	uint8_t *spell_flags;			/* Spell flags */
	uint8_t *spell_order;			/* Spell order */
	bitflag specialties[PF_SIZE];		/* Learned specialty abilities */
    int16_t speed_boost;		/* Short term speed boost (Fury, Phasewalk) */
    int16_t heighten_power;		/* Magic Intensity boost (Heightn Magic) */
	uint8_t num_traps;				/* Number of current monster traps set */
    uint8_t themed_level; 			/* Player in a themed level?  Which one? */
    uint16_t themed_level_appeared;	/* Which themed levels already appeared */

	char full_name[PLAYER_NAME_LEN];	/* Full name */
	char died_from[80];					/* Cause of death */
	char *history;						/* Player history */
	uint16_t total_winner;			/* Total winner */

	uint16_t noscore;			/* Cheating flags */

	bool is_dead;				/* Player is dead */

	bool wizard;				/* Player is in wizard mode */

	int16_t player_hp[PY_MAX_LEVEL];	/* HP gained per level */

	/* Saved values for quickstart */
	int32_t au_birth;			/* Birth gold when option birth_money is false */
	int16_t stat_birth[STAT_MAX];		/* Birth "natural" stat values */
	int16_t ht_birth;			/* Birth Height */
	int16_t wt_birth;			/* Birth Weight */

	struct player_options opts;			/* Player options */
	struct player_history hist;			/* Player history (see player-history.c) */

	struct player_body body;			/* Equipment slots available */
	struct player_shape *shape;			/* Current player shape */

	struct object *gear;				/* Real gear */
	struct object *gear_k;				/* Known gear */

	struct object *obj_k;				/* Object knowledge ("runes") */
	struct chunk *cave;					/* Known version of current level */

	struct player_state state;			/* Calculatable state */
	struct player_state known_state;	/* What the player can know of the above */
	struct player_upkeep *upkeep;		/* Temporary player-related values */
};


/**
 * ------------------------------------------------------------------------
 * Externs
 * ------------------------------------------------------------------------ */

extern struct player_body *bodies;
extern struct player_race *races;
extern struct player_shape *shapes;
extern struct player_class *classes;
extern struct player_ability *player_abilities;
extern struct magic_realm *realms;

extern const int32_t player_exp[PY_MAX_LEVEL];
extern struct player *player;

/* player-class.c */
struct player_class *player_id2class(guid id);

/* player.c */
int stat_name_to_idx(const char *name);
const char *stat_idx_to_name(int type);
const struct magic_realm *lookup_realm(const char *code);
bool player_stat_inc(struct player *p, int stat);
bool player_stat_dec(struct player *p, int stat, bool permanent);
void player_exp_gain(struct player *p, int32_t amount);
void player_exp_lose(struct player *p, int32_t amount, bool permanent);
void player_flags(struct player *p, bitflag f[OF_SIZE]);
void player_flags_timed(struct player *p, bitflag f[OF_SIZE]);
uint8_t player_hp_attr(struct player *p);
uint8_t player_sp_attr(struct player *p);
bool player_restore_mana(struct player *p, int amt);
size_t player_random_name(char *buf, size_t buflen);
void player_safe_name(char *safe, size_t safelen, const char *name, bool strip_suffix);
void player_cleanup_members(struct player *p);

/* player-race.c */
struct player_race *player_id2race(guid id);
struct player_race *player_race_from_name(const char *name);

#endif /* !PLAYER_H */
