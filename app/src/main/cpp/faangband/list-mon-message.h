/**
 * \file list-mon-message.h
 * \brief List of monster message types
 *
 * id           - the message constant name
 * msg          - MSG_ type for the printed message
 * omit_subject - true to omit a monster name beforehand (see mon-msg.c)
 * text         - the message text
 */

/* id						MSG_GENERIC,	text */
MON_MSG(NONE,				MSG_GENERIC,	false,	"[is|are] hurt.")
MON_MSG(DIE,				MSG_KILL,		false,	"die[s].")
MON_MSG(DESTROYED,			MSG_KILL,		false,	"[is|are] destroyed.")
MON_MSG(RESIST_A_LOT,		MSG_GENERIC,	false,	"resist[s] a lot.")
MON_MSG(HIT_HARD,			MSG_GENERIC,	false,	"[is|are] hit hard.")
MON_MSG(RESIST,				MSG_GENERIC,	false,	"resist[s].")
MON_MSG(IMMUNE,				MSG_GENERIC,	false,	"[is|are] immune.")
MON_MSG(RESIST_SOMEWHAT,	MSG_GENERIC,	false,	"resist[s] somewhat.")
MON_MSG(UNAFFECTED,			MSG_GENERIC,	false,	"[is|are] unaffected!")
MON_MSG(SPAWN,				MSG_GENERIC,	false,	"spawn[s]!")
MON_MSG(HEALTHIER,			MSG_GENERIC,	false,	"look[s] healthier.")
MON_MSG(FALL_ASLEEP,		MSG_GENERIC,	false,	"fall[s] asleep!")
MON_MSG(WAKES_UP,			MSG_GENERIC,	false,	"wake[s] up.")
MON_MSG(CRINGE_LIGHT,		MSG_GENERIC,	false,	"cringe[s] from the light!")
MON_MSG(SHRIVEL_LIGHT,		MSG_KILL,		false,	"shrivel[s] away in the light!")
MON_MSG(DISMAY_LIGHT,		MSG_KILL,		false,	"[is|are] dismayed by the Light!")
MON_MSG(LOSE_SKIN,			MSG_GENERIC,	false,	"lose[s] some skin!")
MON_MSG(DISSOLVE,			MSG_KILL,		false,	"dissolve[s]!")
MON_MSG(CATCH_FIRE,			MSG_GENERIC,	false,	"catch[es] fire!")
MON_MSG(BADLY_FROZEN,		MSG_GENERIC,	false,	"[is|are] badly frozen.")
MON_MSG(STRUCK_ELEC,		MSG_GENERIC,	false,	"[is|are] struck by lightning!")
MON_MSG(SHUDDER,			MSG_GENERIC,	false,	"shudder[s].")
MON_MSG(CHANGE,				MSG_GENERIC,	false,	"change[s]!")
MON_MSG(DISAPPEAR,			MSG_GENERIC,	false,	"disappear[s]!")
MON_MSG(MORE_DAZED,			MSG_GENERIC,	false,	"[is|are] even more stunned.")
MON_MSG(DAZED,				MSG_GENERIC,	false,	"[is|are] stunned.")
MON_MSG(NOT_DAZED,			MSG_GENERIC,	false,	"[is|are] no longer stunned.")
MON_MSG(MORE_CONFUSED,		MSG_GENERIC,	false,	"look[s] more confused.")
MON_MSG(CONFUSED,			MSG_GENERIC,	false,	"look[s] confused.")
MON_MSG(NOT_CONFUSED,		MSG_GENERIC,	false,	"[is|are] no longer confused.")
MON_MSG(MORE_SLOWED,		MSG_GENERIC,	false,	"look[s] more slowed.")
MON_MSG(SLOWED,				MSG_GENERIC,	false,	"look[s] slowed.")
MON_MSG(NOT_SLOWED	,		MSG_GENERIC,	false,	"speed[s] up.")
MON_MSG(MORE_HASTED,		MSG_GENERIC,	false,	"look[s] even faster!")
MON_MSG(HASTED,				MSG_GENERIC,	false,	"start[s] moving faster.")
MON_MSG(NOT_HASTED,			MSG_GENERIC,	false,	"slow[s] down.")
MON_MSG(MORE_AFRAID,		MSG_GENERIC,	false,	"look[s] more terrified!")
MON_MSG(FLEE_IN_TERROR,		MSG_FLEE,		false,	"flee[s] in terror!")
MON_MSG(NOT_AFRAID,			MSG_GENERIC,	false,	"[is|are] no longer afraid.")
MON_MSG(HELD,				MSG_GENERIC,    false,  "[is|are] frozen to the spot!")
MON_MSG(NOT_HELD,			MSG_GENERIC,    false,  "can move again.")
MON_MSG(DISEN,				MSG_GENERIC,    false,  "seem[s] less magical!")
MON_MSG(NOT_DISEN,			MSG_GENERIC,    false,  "seem[s] magical again.")
MON_MSG(COMMAND,			MSG_GENERIC,    false,  "falls under your spell!")
MON_MSG(NOT_COMMAND,		MSG_GENERIC,    false,  "is no longer under your control.")
MON_MSG(SHAPE_FAIL,			MSG_GENERIC,    false,  "shimmers for a moment.")
MON_MSG(MORIA_DEATH,		MSG_KILL,		true,	"You hear [a|several] scream[|s] of agony!")
MON_MSG(DISINTEGRATES,		MSG_KILL,		false,	"disintegrate[s]!")
MON_MSG(FREEZE_SHATTER,		MSG_KILL,		false,	"freeze[s] and shatter[s]!")
MON_MSG(MANA_DRAIN,			MSG_GENERIC,	false,	"lose[s] some mana!")
MON_MSG(BRIEF_PUZZLE,		MSG_GENERIC,	false,	"look[s] briefly puzzled.")
MON_MSG(MAINTAIN_SHAPE,		MSG_GENERIC,	false,	"maintain[s] the same shape.")
MON_MSG(SAVE_HELD,			MSG_GENERIC,	false,	"fight[s] off your spell.")
MON_MSG(UNHARMED,			MSG_GENERIC,	false,	"[is|are] unharmed.")
MON_MSG(APPEAR,			MSG_GENERIC,	false,	"appear[s]!")
/* Dummy messages for monster pain - we use edit file info instead. */
MON_MSG(95,					MSG_GENERIC,	false,	"")
MON_MSG(75,					MSG_GENERIC,	false,	"")
MON_MSG(50,					MSG_GENERIC,	false,	"")
MON_MSG(35,					MSG_GENERIC,	false,	"")
MON_MSG(20,					MSG_GENERIC,	false,	"")
MON_MSG(10,					MSG_GENERIC,	false,	"")
MON_MSG(0,					MSG_GENERIC,	false,	"")
MON_MSG(MAX,				MSG_GENERIC,	false,	"")
