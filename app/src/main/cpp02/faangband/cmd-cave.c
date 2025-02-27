/**
 * \file cmd-cave.c
 * \brief Chest and door opening/closing, disarming, running, resting, &c.
 *
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
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

#include "angband.h"
#include "cave.h"
#include "cmd-core.h"
#include "cmds.h"
#include "game-event.h"
#include "game-input.h"
#include "game-world.h"
#include "generate.h"
#include "init.h"
#include "mon-attack.h"
#include "mon-desc.h"
#include "mon-lore.h"
#include "mon-predicate.h"
#include "mon-spell.h"
#include "mon-timed.h"
#include "mon-util.h"
#include "monster.h"
#include "obj-chest.h"
#include "obj-desc.h"
#include "obj-gear.h"
#include "obj-ignore.h"
#include "obj-knowledge.h"
#include "obj-pile.h"
#include "obj-util.h"
#include "player-attack.h"
#include "player-calcs.h"
#include "player-history.h"
#include "player-path.h"
#include "player-timed.h"
#include "player-util.h"
#include "project.h"
#include "store.h"
#include "trap.h"

/**
 * Move house to the current town
 */
void do_cmd_move_house(struct command *cmd)
{
	int i;
	const char *town = locality_name(world->levels[player->place].locality);
	char *prompt = format("Do you really want to move to %s?", town);

	if (!player->depth) {
		/* Already home */
		if (player->place == player->home) {
			msg("You already live here!");
			return;
		}

		/* Check */
		if (!get_check(prompt)) return;

		/* Thralls have no stuff to shift */
		if (player->home) {
			/* Get the current home */
			struct level *hometown = &world->levels[player->home];
			struct town *old_home = NULL, *new_home = NULL;
			struct store *temp;
			for (i = 0; i < world->num_towns; i++) {
				old_home = &world->towns[i];
				if (old_home->index == hometown->index) {
					/* Found it */
					assert(store_is_home(old_home->stores));
					break;
				}
			}
			assert(i < world->num_towns);

			/* Get the new home */
			for (i = 0; i < world->num_towns; i++) {
				new_home = &world->towns[i];
				if (new_home->index == player->place) {
					/* Found it */
					assert(!store_is_home(new_home->stores));
					break;
				}
			}
			assert(i < world->num_towns);

			/* Move */
			temp = old_home->stores;
			old_home->stores = old_home->stores->next;
			temp->next = new_home->stores;
			new_home->stores = temp;
		}

		/* Set the new town */
		player->home = player->place;
		msg("Your home will be here when you return.");

		/* Write message */
		history_add(player, format("Moved house to %s.", town),
					HIST_PLAYER_MOVE);
	} else {
		msg("You can only move to another town!");
	}

	return;
}

/**
 * Get the direction a path is heading
 */
static const char *path_direction(int feat)
{
	if (feat == FEAT_LESS_NORTH) return "north";
	if (feat == FEAT_MORE_NORTH) return "north";
	if (feat == FEAT_LESS_EAST) return "east";
	if (feat == FEAT_MORE_EAST) return "east";
	if (feat == FEAT_LESS_SOUTH) return "south";
	if (feat == FEAT_MORE_SOUTH) return "south";
	if (feat == FEAT_LESS_WEST) return "west";
	if (feat == FEAT_MORE_WEST) return "west";
	if (feat == FEAT_LESS) return "up";
	if (feat == FEAT_MORE) return "down";
	return "";
}

/**
 * Get the return path of a path or stair (sigh)
 */
static int return_path(int feat)
{
	if (feat == FEAT_LESS_NORTH) return FEAT_MORE_SOUTH;
	if (feat == FEAT_MORE_NORTH) return FEAT_LESS_SOUTH;
	if (feat == FEAT_LESS_EAST) return FEAT_MORE_WEST;
	if (feat == FEAT_MORE_EAST) return FEAT_LESS_WEST;
	if (feat == FEAT_LESS_SOUTH) return FEAT_MORE_NORTH;
	if (feat == FEAT_MORE_SOUTH) return FEAT_LESS_NORTH;
	if (feat == FEAT_LESS_WEST) return FEAT_MORE_EAST;
	if (feat == FEAT_MORE_WEST) return FEAT_LESS_EAST;
	if (feat == FEAT_LESS) return FEAT_MORE;
	if (feat == FEAT_MORE) return FEAT_LESS;
	return -1;
}

/**
 * Go up one level
 */
void do_cmd_go_up(struct command *cmd)
{
	int feat = square_feat(cave, player->grid)->fidx;
	int new_place;

	/* Verify stairs */
	if (!(square_isstairs(cave, player->grid) ||
		  square_ispath(cave, player->grid))) {
		msg("I see no path or staircase here.");
		return;
	} else if (square_isdownstairs(cave, player->grid)) {
		if (square_ispath(cave, player->grid)) {
			msg("This is a path to greater danger.");
			return;
		} else {
			msg("This staircase leads down.");
			return;
		}
	}

	/* Make certain the player really wants to leave a themed level. -LM- */
	if (player->themed_level) {
		if (!get_check("This level will never appear again.  Really leave?")) {
			return;
		}
	}

	/* Force descend */
	//if (OPT(player, birth_force_descend)) {
	//	msg("Nothing happens!");
	//	return;
	//}

	new_place = player_get_next_place(player->place, path_direction(feat), 1);

	/* Take a turn */
	player->upkeep->energy_use = z_info->move_energy;

	/* Success */
	if (feat == FEAT_LESS) {
		/* Magical portal for dungeon-only games */
		if (streq(world->name, "Hybrid Dungeon") && (player->depth != 1)) {
			struct level *lev = &world->levels[player->place];
			if (lev->up) {
				struct level *up = level_by_name(world, lev->up);
				if (lev->locality != up->locality) {
					/* Portal */
					msgt(MSG_STAIRS_UP, "You trigger a magic portal.");

					/* Change level */
					player_change_place(player, new_place);
					return;
				}
			} else {
				msg("Nothing happens!");
				return;
			}
		}
		msgt(MSG_STAIRS_UP, "You enter a maze of up staircases.");
	} else {
		msgt(MSG_STAIRS_UP, "You enter a winding path to less danger.");
	}

	/* Create a way back */
	player->upkeep->create_stair = return_path(feat);

	/* Record the non-obvious exit coordinate */
	if ((feat == FEAT_LESS_NORTH) || (feat == FEAT_MORE_NORTH) ||
		(feat == FEAT_LESS_SOUTH) || (feat == FEAT_MORE_SOUTH)) {
		player->upkeep->path_coord = player->grid.x;
	} else if ((feat == FEAT_LESS_EAST) || (feat == FEAT_MORE_EAST) ||
			   (feat == FEAT_LESS_WEST) || (feat == FEAT_MORE_WEST)) {
		player->upkeep->path_coord = player->grid.y;
	}

	/* Change level */
	player_change_place(player, new_place);
}


/**
 * Go down one level
 */
void do_cmd_go_down(struct command *cmd)
{
	int feat = square_feat(cave, player->grid)->fidx;
	int new_place;

	/* Verify stairs */
	if (!(square_isstairs(cave, player->grid) ||
		  square_ispath(cave, player->grid))) {
		msg("I see no path or staircase here.");
		return;
	} else if (square_isupstairs(cave, player->grid)) {
		if (square_ispath(cave, player->grid)) {
			msg("This is a path to less danger.");
			return;
		} else {
			msg("This staircase leads up.");
			return;
		}
	}

	/* Paranoia, no descent from z_info->max_depth - 1 */
	if (player->depth == z_info->max_depth - 1) {
		msg("The dungeon does not appear to extend deeper");
		return;
	}

	/* Make certain the player really wants to leave a themed level. -LM- */
	if (player->themed_level) {
		if (!get_check("This level will never appear again.  Really leave? ")) {
			return;
		}
	}

	new_place = player_get_next_place(player->place, path_direction(feat), 1);

	/* Warn a force_descend player if they're going to a quest level */
	//if (OPT(player, birth_force_descend) && quest_forbid_downstairs(new_place)) {
	//	if (!get_check("Are you sure you want to descend?")) {
	//		return;
	//	}
	//}

	/* Take a turn */
	player->upkeep->energy_use = z_info->move_energy;

	/* Success */
	if (feat == FEAT_MORE) {
		msgt(MSG_STAIRS_DOWN, "You enter a maze of down staircases.");
	} else {
		msgt(MSG_STAIRS_DOWN, "You enter a winding path to greater danger.");
	}

	/* Create a way back */
	player->upkeep->create_stair = return_path(feat);

	/* Record the non-obvious exit coordinate */
	if ((feat == FEAT_LESS_NORTH) || (feat == FEAT_MORE_NORTH) ||
		(feat == FEAT_LESS_SOUTH) || (feat == FEAT_MORE_SOUTH)) {
		player->upkeep->path_coord = player->grid.x;
	} else if ((feat == FEAT_LESS_EAST) || (feat == FEAT_MORE_EAST) ||
			   (feat == FEAT_LESS_WEST) || (feat == FEAT_MORE_WEST)) {
		player->upkeep->path_coord = player->grid.y;
	}

	/* Change level */
	player_change_place(player, new_place);
}



/**
 * Determine if a given grid may be "opened"
 */
static bool do_cmd_open_test(struct loc grid)
{
	/* Must have knowledge */
	if (!square_isknown(cave, grid)) {
		msg("You see nothing there.");
		return false;
	}

	/* Must be a closed door */
	if (!square_iscloseddoor(cave, grid)) {
		msgt(MSG_NOTHING_TO_OPEN, "You see nothing there to open.");
		return false;
	}

	return (true);
}


/**
 * Perform the basic "open" command on doors
 *
 * Assume there is no monster blocking the destination
 *
 * Returns true if repeated commands may continue
 */
static bool do_cmd_open_aux(struct loc grid)
{
	int i, j;
	bool more = false;

	/* Verify legality */
	if (!do_cmd_open_test(grid)) return (false);

	/* Locked door */
	if (square_islockeddoor(cave, grid)) {
		/* Disarm factor */
		i = player->state.skills[SKILL_DISARM_PHYS];

		/* Penalize some conditions */
		if (player->timed[TMD_BLIND] || no_light(player))
			i = i / 10;
		if (player->timed[TMD_CONFUSED] || player->timed[TMD_IMAGE])
			i = i / 10;

		/* Extract the lock power */
		j = square_door_power(cave, grid);

		/* Extract the difficulty XXX XXX XXX */
		j = i - (j * 4);

		/* Always have a small chance of success */
		if (j < 2) j = 2;

		if (randint0(100) < j) {
			/* Message */
			msgt(MSG_LOCKPICK, "You have picked the lock.");

			/* Open the door */
			square_open_door(cave, grid);

			/* Update the visuals */
			square_memorize(cave, grid);
			square_light_spot(cave, grid);
			player->upkeep->update |= (PU_UPDATE_VIEW | PU_MONSTERS);

			/* Experience */
			/* Removed to avoid exploit by repeatedly locking and unlocking */
			/* player_exp_gain(player, 1); */
		} else {
			event_signal(EVENT_INPUT_FLUSH);

			/* Message */
			msgt(MSG_LOCKPICK_FAIL, "You failed to pick the lock.");

			/* We may keep trying */
			more = true;
		}
	} else {
		/* Closed door */
		square_open_door(cave, grid);
		square_memorize(cave, grid);
		square_light_spot(cave, grid);
		player->upkeep->update |= (PU_UPDATE_VIEW | PU_MONSTERS);
		sound(MSG_OPENDOOR);
	}

	/* Result */
	return (more);
}



/**
 * Open a closed/locked/jammed door or a closed/locked chest.
 *
 * Unlocking a locked chest is worth one experience point; since doors are
 * player lockable, there is no experience for unlocking doors.
 */
void do_cmd_open(struct command *cmd)
{
	struct loc grid;
	int dir;
	struct object *obj;
	bool more = false;
	int err;
	struct monster *mon;

	/* Get arguments */
	err = cmd_get_arg_direction(cmd, "direction", &dir);
	if (err || dir == DIR_UNKNOWN) {
		struct loc grid1;
		int n_closed_doors, n_locked_chests;

		n_closed_doors = count_feats(&grid1, square_iscloseddoor, false);
		n_locked_chests = count_chests(&grid1, CHEST_OPENABLE);

		if (n_closed_doors + n_locked_chests == 1) {
			dir = motion_dir(player->grid, grid1);
			cmd_set_arg_direction(cmd, "direction", dir);
		} else if (cmd_get_direction(cmd, "direction", &dir, false)) {
			return;
		}
	}

	/* Get location */
	grid = loc_sum(player->grid, ddgrid[dir]);

	/* Check for chest */
	obj = chest_check(player, grid, CHEST_OPENABLE);

	/* Check for door */
	if (!obj && !do_cmd_open_test(grid)) {
		/* Cancel repeat */
		disturb(player);
		return;
	}

	/* Take a turn */
	player->upkeep->energy_use = z_info->move_energy;

	/* Apply confusion */
	if (player_confuse_dir(player, &dir, false)) {
		/* Get location */
		grid = loc_sum(player->grid, ddgrid[dir]);

		/* Check for chest */
		obj = chest_check(player, grid, CHEST_OPENABLE);
	}

	/* Monster */
	mon = square_monster(cave, grid);
	if (mon) {
		/* Mimics surprise the player */
		if (monster_is_mimicking(mon)) {
			become_aware(cave, mon);

			/* Mimic wakes up and becomes aware*/
			monster_wake(mon, false, 100);
		} else {
			/* Message */
			msg("There is a monster in the way!");

			/* Attack */
			py_attack(player, grid);
		}
	} else if (obj) {
		/* Chest */
		more = do_cmd_open_chest(grid, obj);
	} else {
		/* Door */
		more = do_cmd_open_aux(grid);
	}

	/* Cancel repeat unless we may continue */
	if (!more) disturb(player);
}


/**
 * Determine if a given grid may be "closed"
 */
static bool do_cmd_close_test(struct loc grid)
{
	/* Must have knowledge */
	if (!square_isknown(cave, grid)) {
		/* Message */
		msg("You see nothing there.");

		/* Nope */
		return (false);
	}

 	/* Require open/broken door */
	if (!square_isopendoor(cave, grid) && !square_isbrokendoor(cave, grid)) {
		/* Message */
		msg("You see nothing there to close.");

		/* Nope */
		return (false);
	}

	/* Don't allow if player is in the way. */
	if (square(cave, grid)->mon < 0) {
		/* Message */
		msg("You're standing in that doorway.");

		/* Nope */
		return (false);
	}

	/* Okay */
	return (true);
}


/**
 * Perform the basic "close" command
 *
 * Assume there is no monster blocking the destination
 *
 * Returns true if repeated commands may continue
 */
static bool do_cmd_close_aux(struct loc grid)
{
	bool more = false;

	/* Verify legality */
	if (!do_cmd_close_test(grid)) return (false);

	/* Broken door */
	if (square_isbrokendoor(cave, grid)) {
		msg("The door appears to be broken.");
	} else {
		/* Close door */
		square_close_door(cave, grid);
		square_memorize(cave, grid);
		square_light_spot(cave, grid);
		player->upkeep->update |= (PU_UPDATE_VIEW | PU_MONSTERS);
		sound(MSG_SHUTDOOR);
	}

	/* Result */
	return (more);
}


/**
 * Close an open door.
 */
void do_cmd_close(struct command *cmd)
{
	struct loc grid;
	int dir;
	int err;

	bool more = false;

	/* Get arguments */
	err = cmd_get_arg_direction(cmd, "direction", &dir);
	if (err || dir == DIR_UNKNOWN) {
		struct loc grid1;

		/* Count open doors */
		if (count_feats(&grid1, square_isopendoor, false) == 1) {
			dir = motion_dir(player->grid, grid1);
			cmd_set_arg_direction(cmd, "direction", dir);
		} else if (cmd_get_direction(cmd, "direction", &dir, false)) {
			return;
		}
	}

	/* Get location */
	grid = loc_sum(player->grid, ddgrid[dir]);

	/* Verify legality */
	if (!do_cmd_close_test(grid)) {
		/* Cancel repeat */
		disturb(player);
		return;
	}

	/* Take a turn */
	player->upkeep->energy_use = z_info->move_energy;

	/* Apply confusion */
	if (player_confuse_dir(player, &dir, false)) {
		/* Get location */
		grid = loc_sum(player->grid, ddgrid[dir]);
	}

	/* Monster - alert, then attack */
	if (square(cave, grid)->mon > 0) {
		msg("There is a monster in the way!");
		py_attack(player, grid);
	} else
		/* Door - close it */
		more = do_cmd_close_aux(grid);

	/* Cancel repeat unless told not to */
	if (!more) disturb(player);
}


/**
 * Determine if a given grid may be "tunneled"
 */
static bool do_cmd_tunnel_test(struct loc grid)
{

	/* Must have knowledge */
	if (!square_isknown(cave, grid)) {
		msg("You see nothing there.");
		return (false);
	}

	/* Titanium */
	if (square_isperm(cave, grid)) {
		msg("This seems to be permanent rock.");
		return (false);
	}

	/* Must be a wall/door/etc */
	if (!(square_isdiggable(cave, grid) || square_iscloseddoor(cave, grid))) {
		msg("You see nothing there to tunnel.");
		return (false);
	}

	/* Okay */
	return (true);
}


/**
 * Tunnel through wall.  Assumes valid location.
 *
 * Note that it is impossible to "extend" rooms past their
 * outer walls (which are actually part of the room).
 *
 * Attempting to do so will produce floor grids which are not part
 * of the room, and whose "illumination" status do not change with
 * the rest of the room.
 */
static bool twall(struct loc grid)
{
	/* Paranoia -- Require a wall or door or some such */
	if (!(square_isdiggable(cave, grid) || square_iscloseddoor(cave, grid)))
		return (false);

	/* Sound */
	sound(MSG_DIG);

	/* Forget the wall */
	square_forget(cave, grid);

	/* Remove the feature */
	square_tunnel_wall(cave, grid);

	/* Update the visuals */
	player->upkeep->update |= (PU_UPDATE_VIEW | PU_MONSTERS);

	/* Result */
	return (true);
}


/**
 * Perform the basic "tunnel" command
 *
 * Assumes that no monster is blocking the destination.
 * Uses twall() (above) to do all "terrain feature changing".
 * Returns true if repeated commands may continue.
 */
static bool do_cmd_tunnel_aux(struct loc grid)
{
	bool more = false;
	int digging_chances[DIGGING_MAX], chance;
	bool okay = false;
	bool gold = square_hasgoldvein(cave, grid);
	bool rubble = square_isrubble(cave, grid);
	int weapon_slot = slot_by_name(player, "weapon");
	struct object *current_weapon = slot_object(player, weapon_slot);
	struct object *best_digger = NULL;
	struct player_state local_state;
	struct player_state *used_state = &player->state;
	int oldn = 1;

	/* Verify legality */
	if (!do_cmd_tunnel_test(grid)) return (false);

	/* Find what we're digging with and our chance of success */
	best_digger = player_best_digger(player, false);
	if (best_digger != current_weapon &&
			(!current_weapon || obj_can_takeoff(current_weapon))) {
		/* Use only one without the overhead of gear_obj_for_use(). */
		if (best_digger) {
			oldn = best_digger->number;
			best_digger->number = 1;
		}
		player->body.slots[weapon_slot].obj = best_digger;
		memcpy(&local_state, &player->state, sizeof(local_state));
		calc_bonuses(player, &local_state, false, true);
		used_state = &local_state;
	}
	calc_digging_chances(used_state, digging_chances);

	/* Do we succeed? */
	chance = digging_chances[square_digging(cave, grid) - 1];
	okay = (chance > randint0(1600));

	/* Swap back */
	if (best_digger != current_weapon) {
		if (best_digger) {
			best_digger->number = oldn;
		}
		player->body.slots[weapon_slot].obj = current_weapon;
		calc_bonuses(player, &local_state, false, true);
	}

	/* Success */
	if (okay && twall(grid)) {
		/* Rubble is a special case - could be handled more generally NRM */
		if (rubble) {
			/* Message */
			msg("You have removed the rubble.");

			/* Place an object (except in town) */
			if ((randint0(100) < 10) && player->depth) {
				/* Create a simple object */
				place_object(cave, grid, player->depth, false, false,
							 ORIGIN_RUBBLE, 0);

				/* Observe the new object */
				if (square_object(cave, grid)
						&& !ignore_item_ok(player,
						square_object(cave, grid))
						&& square_isseen(cave, grid)) {
					msg("You have found something!");
				}
			} 
		} else if (gold) {
			/* Found treasure */
			place_gold(cave, grid, player->depth, ORIGIN_FLOOR);
			msg("You have found something!");
		} else {
			msg("You have finished the tunnel.");
		}
		/* On the surface, new terrain may be exposed to the sun. */
		if (cave->depth == 0) expose_to_sun(cave, grid, is_daytime());
		/* Update the visuals. */
		square_memorize(cave, grid);
		square_light_spot(cave, grid);
		player->upkeep->update |= (PU_UPDATE_VIEW | PU_MONSTERS);
	} else if (chance > 0) {
		/* Failure, continue digging */
		if (rubble)
			msg("You dig in the rubble.");
		else
			msg("You tunnel into the %s.",
				square_apparent_name(player->cave, grid));
		more = true;
	} else {
		/* Don't automatically repeat if there's no hope. */
		if (rubble) {
			msg("You dig in the rubble with little effect.");
		} else {
			msg("You chip away futilely at the %s.",
				square_apparent_name(player->cave, grid));
		}
	}

	/* Result */
	return (more);
}


/**
 * Tunnel through "walls" (including rubble and doors, secret or otherwise)
 *
 * Digging is very difficult without a "digger" weapon, but can be
 * accomplished by strong players using heavy weapons.
 */
void do_cmd_tunnel(struct command *cmd)
{
	struct loc grid;
	int dir;
	bool more = false;

	/* Get arguments */
	if (cmd_get_direction(cmd, "direction", &dir, false))
		return;

	/* Get location */
	grid = loc_sum(player->grid, ddgrid[dir]);

	/* Oops */
	if (!do_cmd_tunnel_test(grid)) {
		/* Cancel repeat */
		disturb(player);
		return;
	}

	/* Take a turn */
	player->upkeep->energy_use = z_info->move_energy;

	/* Apply confusion */
	if (player_confuse_dir(player, &dir, false)) {
		/* Get location */
		grid = loc_sum(player->grid, ddgrid[dir]);
	}

	/* Attack any monster we run into */
	if (square(cave, grid)->mon > 0) {
		msg("There is a monster in the way!");
		py_attack(player, grid);
	} else {
		/* Tunnel through walls */
		more = do_cmd_tunnel_aux(grid);
	}

	/* Cancel repetition unless we can continue */
	if (!more) disturb(player);
}

/**
 * Determine if a given grid may be "disarmed"
 */
static bool do_cmd_disarm_test(struct loc grid)
{
	/* Must have knowledge */
	if (!square_isknown(cave, grid)) {
		msg("You see nothing there.");
		return false;
	}

	/* Look for a closed, unlocked door to lock */
	if (square_iscloseddoor(cave, grid) && !square_islockeddoor(cave, grid))
		return true;

	/* Look for a trap */
	if (square_isdisarmabletrap(cave, grid)) {
		return true;
	}
	if (square_ismonstertrap(cave, grid)) {
		return true;
	}
	if (square_iswarded(cave, grid)) {
		return true;
	}
	if (square_isdecoyed(cave, grid)) {
		return true;
	}

	/* Nothing */
	msg("You see nothing there to disarm.");
	return false;
}


/**
 * Perform the command "lock door"
 *
 * Assume there is no monster blocking the destination
 *
 * Returns true if repeated commands may continue
 */
static bool do_cmd_lock_door(struct loc grid)
{
	int i, j, power;
	bool more = false;

	/* Verify legality */
	if (!do_cmd_disarm_test(grid)) return false;

	/* Get the "disarm" factor */
	i = player->state.skills[SKILL_DISARM_PHYS];

	/* Penalize some conditions */
	if (player->timed[TMD_BLIND] || no_light(player))
		i = i / 10;
	if (player->timed[TMD_CONFUSED] || player->timed[TMD_IMAGE])
		i = i / 10;

	/* Calculate lock "power" */
	power = m_bonus(7, player->depth);

	/* Extract the difficulty */
	j = i - power;

	/* Always have a small chance of success */
	if (j < 2) j = 2;

	/* Success */
	if (randint0(100) < j) {
		msg("You lock the door.");
		square_set_door_lock(cave, grid, power);
	}

	/* Failure -- Keep trying */
	else if ((i > 5) && (randint1(i) > 5)) {
		event_signal(EVENT_INPUT_FLUSH);
		msg("You failed to lock the door.");

		/* We may keep trying */
		more = true;
	}
	/* Failure */
	else
		msg("You failed to lock the door.");

	/* Result */
	return more;
}


/**
 * Perform the basic "disarm" command
 *
 * Assume there is no monster blocking the destination
 *
 * Returns true if repeated commands may continue
 */
static bool do_cmd_disarm_aux(struct loc grid)
{
	int skill, power, chance;
    struct trap *trap = square(cave, grid)->trap;
	bool more = false;

	/* Verify legality */
	if (!do_cmd_disarm_test(grid)) return false;

    /* Choose first player or monster trap */
	while (trap) {
		if (trf_has(trap->flags, TRF_TRAP)) {
			break;
		} else if (trf_has(trap->flags, TRF_M_TRAP)) {
			msgt(MSG_DISARM, "You have disarmed your %s.", trap->kind->name);

			/* Trap is gone */
			player->num_traps--;
			square_forget(cave, grid);
			square_destroy_trap(cave, grid);
			return false;
		} else if (trf_has(trap->flags, TRF_GLYPH)) {
			msgt(MSG_DISARM, "You have removed the %s.", trap->kind->name);

			/* Glyph is gone */
			square_forget(cave, grid);
			square_destroy_trap(cave, grid);
			return false;
		}

		trap = trap->next;
	}
	if (!trap)
		return false;

	/* Get the base disarming skill */
	if (trf_has(trap->flags, TRF_MAGICAL))
		skill = player->state.skills[SKILL_DISARM_MAGIC];
	else
		skill = player->state.skills[SKILL_DISARM_PHYS];

	/* Penalize some conditions */
	if (player->timed[TMD_BLIND] ||
			no_light(player) ||
			player->timed[TMD_CONFUSED] ||
			player->timed[TMD_IMAGE])
		skill = skill / 10;

	/* Extract trap power */
	power = cave->depth / 5;

	/* Extract the percentage success */
	chance = skill - power;

	/* Always have a small chance of success */
	if (chance < 2) chance = 2;

	/* Two chances - one to disarm, one not to set the trap off */
	if (randint0(100) < chance) {
		msgt(MSG_DISARM, "You have disarmed the %s.", trap->kind->name);
		player_exp_gain(player, 1 + power);

		/* Trap is gone */
		square_forget(cave, grid);
		square_destroy_trap(cave, grid);
	} else if (randint0(100) < chance) {
		event_signal(EVENT_INPUT_FLUSH);
		msg("You failed to disarm the %s.", trap->kind->name);

		/* Player can try again */
		more = true;
	} else {
		msg("You set off the %s!", trap->kind->name);
		hit_trap(grid, -1);
	}

	/* Result */
	return more;
}


/**
 * Disarms a trap, or a chest
 *
 * Traps must be visible, chests must be known trapped
 */
void do_cmd_disarm(struct command *cmd)
{
	struct loc grid;
	int dir;
	int err;

	struct object *obj;
	bool more = false;

	/* Get arguments */
	err = cmd_get_arg_direction(cmd, "direction", &dir);
	if (err || dir == DIR_UNKNOWN) {
		struct loc grid1;
		int n_traps, n_chests;

		n_traps = count_feats(&grid1, square_isdisarmabletrap, true);
		n_chests = count_chests(&grid1, CHEST_TRAPPED);

		if (n_traps + n_chests == 1) {
			dir = motion_dir(player->grid, grid1);
			cmd_set_arg_direction(cmd, "direction", dir);
		} else if (cmd_get_direction(cmd, "direction", &dir, n_chests > 0)) {
			/* If there are chests to disarm, 5 is allowed as a direction */
			return;
		}
	}

	/* Get location */
	grid = loc_sum(player->grid, ddgrid[dir]);

	/* Check for chests */
	obj = chest_check(player, grid, CHEST_TRAPPED);

	/* Verify legality */
	if (!obj && !do_cmd_disarm_test(grid)) {
		/* Cancel repeat */
		disturb(player);
		return;
	}

	/* Take a turn */
	player->upkeep->energy_use = z_info->move_energy;

	/* Apply confusion */
	if (player_confuse_dir(player, &dir, false)) {
		/* Get location */
		grid = loc_sum(player->grid, ddgrid[dir]);

		/* Check for chests */
		obj = chest_check(player, grid, CHEST_TRAPPED);
	}


	/* Monster */
	if (square(cave, grid)->mon > 0) {
		msg("There is a monster in the way!");
		py_attack(player, grid);
	} else if (obj)
		/* Chest */
		more = do_cmd_disarm_chest(obj);
	else if (square_iscloseddoor(cave, grid) &&
			 !square_islockeddoor(cave, grid))
		/* Door to lock */
		more = do_cmd_lock_door(grid);
	else
		/* Disarm trap */
		more = do_cmd_disarm_aux(grid);

	/* Cancel repeat unless told not to */
	if (!more) disturb(player);
}

/**
 * Some players may set traps.  A limited number of such traps may exist at any
 * one time,  but an old trap can be disarmed to free up equipment for a new
 * trap. -LM-
 */
static void do_cmd_set_trap(struct loc grid)
{
	int max_traps =	1 + ((player->lev >= 25) ? 1 : 0);

	/* Specialty ability Extra Trap */
	if (player_has(player, PF_EXTRA_TRAP)) max_traps++;

	if (player->timed[TMD_BLIND] || no_light(player)) {
		msg("You can not see to set a trap.");
		return;
	}

	if (player->timed[TMD_CONFUSED] || player->timed[TMD_IMAGE]) {
		msg("You are too confused.");
		return;
	}

	/* Forbid more than max_traps being set. */
	if (player->num_traps >= max_traps) {
		msg("You must disarm an existing trap to free up your equipment.");
		return;
	}

	/* Set the trap, and draw it. */
	place_trap(cave, grid, lookup_trap("basic trap")->tidx, 0);

	/* Notify the player. */
	msg("You set a monster trap.");
}

/**
 * Manipulate an adjacent grid in some way
 *
 * Attack monsters, tunnel through walls, disarm traps, open doors.
 *
 * This command must always take energy, to prevent free detection
 * of invisible monsters.
 *
 * The "semantics" of this command must be chosen before the player
 * is confused, and it must be verified against the new grid.
 */
static void do_cmd_alter_aux(int dir)
{
	struct loc grid;
	bool more = false;
	struct object *o_chest_closed;
	struct object *o_chest_trapped;

	/* Get location */
	grid = loc_sum(player->grid, ddgrid[dir]);

	/* Take a turn */
	player->upkeep->energy_use = z_info->move_energy;

	/* Apply confusion */
	if (player_confuse_dir(player, &dir, false)) {
		/* Get location */
		grid = loc_sum(player->grid, ddgrid[dir]);
	}

	/* Check for closed chest */
	o_chest_closed = chest_check(player, grid, CHEST_OPENABLE);
	/* Check for trapped chest */
	o_chest_trapped = chest_check(player, grid, CHEST_TRAPPED);

	/* Action depends on what's there */
	if (square(cave, grid)->mon > 0) {
		/* Attack monster */
		py_attack(player, grid);
	} else if (square_isdiggable(cave, grid)) {
		/* Tunnel through walls and rubble */
		more = do_cmd_tunnel_aux(grid);
	} else if (square_iscloseddoor(cave, grid)) {
		/* Open closed doors */
		more = do_cmd_open_aux(grid);
	} else if (square_isbasicmonstertrap(cave, grid)) {
		/* Modify monster traps */
		modify_monster_trap(grid);
	} else if (square_isdisarmabletrap(cave, grid) ||
			   square_isadvancedmonstertrap(cave, grid)) {
		/* Disarm traps */
		more = do_cmd_disarm_aux(grid);
	} else if (player_has(player, PF_TRAP) && square_istrappable(cave, grid)) {
		/* Set traps */
		do_cmd_set_trap(grid);
	} else if (o_chest_trapped) {
		/* Trapped chest */
		more = do_cmd_disarm_chest(o_chest_trapped);
	} else if (o_chest_closed) {
		/* Open chest */
		more = do_cmd_open_chest(grid, o_chest_closed);
	} else if (square_isopendoor(cave, grid)) {
		/* Close door */
        	more = do_cmd_close_aux(grid);
	} else {
		/* Oops */
		msg("You spin around.");
	}

	/* Cancel repetition unless we can continue */
	if (!more) disturb(player);
}

void do_cmd_alter(struct command *cmd)
{
	int dir;

	/* Get arguments */
	if (cmd_get_direction(cmd, "direction", &dir, false) != CMD_OK)
		return;

	do_cmd_alter_aux(dir);
}

static void do_cmd_steal_aux(int dir)
{
	/* Get location */
	struct loc grid = loc_sum(player->grid, ddgrid[dir]);

	/* Take a turn */
	player->upkeep->energy_use = z_info->move_energy;

	/* Apply confusion */
	if (player_confuse_dir(player, &dir, false)) {
		/* Get location */
		grid = loc_sum(player->grid, ddgrid[dir]);
	}

	/* Attack or steal from monsters */
	if ((square(cave, grid)->mon > 0) && player_has(player, PF_STEAL)) {
		steal_monster_item(square_monster(cave, grid), -1);
	} else {
		/* Oops */
		msg("You spin around.");
	}
}

void do_cmd_steal(struct command *cmd)
{
	int dir;

	/* Get arguments */
	if (cmd_get_direction(cmd, "direction", &dir, false) != CMD_OK)
		return;

	do_cmd_steal_aux(dir);
}

/**
 * Move player in the given direction.
 *
 * This routine should only be called when energy has been expended.
 *
 * Note that this routine handles monsters in the destination grid,
 * and also handles attempting to move into walls/doors/rubble/etc.
 */
void move_player(int dir, bool disarm)
{
	struct loc grid = loc_sum(player->grid, ddgrid[dir]);

	int m_idx = square(cave, grid)->mon;
	struct monster *mon = cave_monster(cave, m_idx);
	bool trapsafe = player_is_trapsafe(player);
	bool trap = square_isdisarmabletrap(cave, grid);
	bool door = square_iscloseddoor(cave, grid);

	/* Many things can happen on movement */
	if (m_idx > 0) {
		/* Attack monsters */
		if (monster_is_mimicking(mon)) {
			become_aware(cave, mon);

			/* Mimic wakes up and becomes aware*/
			monster_wake(mon, false, 100);
		} else {
			py_attack(player, grid);
		}
	} else if (((trap && disarm) || door) && square_isknown(cave, grid)) {
		/* Auto-repeat if not already repeating */
		if (cmd_get_nrepeats() == 0)
			cmd_set_repeat(99);
		do_cmd_alter_aux(dir);
	} else if (trap && player->upkeep->running && !trapsafe) {
		/* Stop running before known traps */
		disturb(player);
	} else if (!square_ispassable(cave, grid)) {
		disturb(player);

		/* Notice unknown obstacles, mention known obstacles */
		if (!square_isknown(cave, grid)) {
			if (square_isrubble(cave, grid)) {
				msgt(MSG_HITWALL,
					 "You feel a pile of rubble blocking your way.");
				square_memorize(cave, grid);
				square_light_spot(cave, grid);
			} else if (square_iscloseddoor(cave, grid)) {
				msgt(MSG_HITWALL, "You feel a door blocking your way.");
				square_memorize(cave, grid);
				square_light_spot(cave, grid);
			} else {
				msgt(MSG_HITWALL, "You feel a wall blocking your way.");
				square_memorize(cave, grid);
				square_light_spot(cave, grid);
			}
		} else {
			if (square_isrubble(cave, grid))
				msgt(MSG_HITWALL,
					 "There is a pile of rubble blocking your way.");
			else if (square_iscloseddoor(cave, grid))
				msgt(MSG_HITWALL, "There is a door blocking your way.");
			else
				msgt(MSG_HITWALL, "There is a wall blocking your way.");
		}
	} else {
		/* See if trap detection status will change */
		bool old_dtrap = square_isdtrap(cave, player->grid);
		bool new_dtrap = square_isdtrap(cave, grid);
		bool step = true;

		/* Note the change in the detect status */
		if (old_dtrap != new_dtrap)
			player->upkeep->redraw |= (PR_DTRAP);

		/* Disturb player if the player is about to leave the area */
		if (player->upkeep->running
				&& !player->upkeep->running_firststep
				&& old_dtrap && !new_dtrap) {
			disturb(player);
			return;
		}

		if (square_isdamaging(cave, grid) || square_isfall(cave, grid)) {
			/* Some terrain can damage the player */
			struct feature *feat = square_feat(cave, grid);
			int dam_taken =
				player_check_terrain_damage(player, grid);

			/*
			 * Check if running, or going to cost more than a
			 * third of hp.
			 */
			if (player->upkeep->running && dam_taken) {
				if (!get_check(feat->run_msg)) {
					player->upkeep->running = 0;
					step = false;
				}
			} else {
				if ((dam_taken > player->chp / 3) || square_isfall(cave, grid)){
					step = get_check(feat->walk_msg);
				}
			}
		}

		if (step) {
			/* Move player */
			monster_swap(player->grid, grid);
			player_handle_post_move(player, true);
		}
	}

	player->upkeep->running_firststep = false;
}

/**
 * Determine if a given grid may be "walked"
 */
static bool do_cmd_walk_test(struct loc grid)
{
	int m_idx = square(cave, grid)->mon;
	struct monster *mon = cave_monster(cave, m_idx);

	/* Allow attack on visible monsters if unafraid */
	if (m_idx > 0 && monster_is_visible(mon) &&	!monster_is_mimicking(mon)) {
		/* Handle player fear */
		if (player_of_has(player, OF_AFRAID)) {
			/* Extract monster name (or "it") */
			char m_name[80];
			monster_desc(m_name, sizeof(m_name), mon, MDESC_DEFAULT);

			/* Message */
			msgt(MSG_AFRAID, "You are too afraid to attack %s!", m_name);
			equip_learn_flag(player, OF_AFRAID);

			/* Nope */
			return (false);
		}

		return (true);
	}

	/* If we don't know the grid, allow attempts to walk into it */
	if (!square_isknown(cave, grid))
		return true;

	/* Require open space */
	if (!square_ispassable(cave, grid)) {
		if (square_isrubble(cave, grid)) {
			/* Rubble */
			msgt(MSG_HITWALL, "There is a pile of rubble in the way!");
		} else if (square_iscloseddoor(cave, grid)) {
			/* Door */
			return true;
		} else {
			/* Wall */
			msgt(MSG_HITWALL, "There is a wall in the way!");
		}

		/* Cancel repeat */
		disturb(player);

		/* Nope */
		return (false);
	}

	/* Okay */
	return (true);
}


/**
 * Walk in the given direction.
 */
void do_cmd_walk(struct command *cmd)
{
	struct loc grid;
	int dir;
	bool trapsafe = player_is_trapsafe(player) ? true : false;

	/* Get arguments */
	if (cmd_get_direction(cmd, "direction", &dir, false) != CMD_OK)
		return;

	/* If we're in a web, deal with that */
	if (square_iswebbed(cave, player->grid)) {
		/* Clear the web, finish turn */
		msg("You clear the web.");
		square_destroy_trap(cave, player->grid);
		player->upkeep->energy_use = z_info->move_energy;
		return;
	}

	/* Apply confusion if necessary */
	/* Confused movements use energy no matter what */
	if (player_confuse_dir(player, &dir, false))
		player->upkeep->energy_use = z_info->move_energy;
	
	/* Verify walkability */
	grid = loc_sum(player->grid, ddgrid[dir]);
	if (!do_cmd_walk_test(grid))
		return;

	player->upkeep->energy_use = energy_per_move(player);

	/* Attempt to disarm unless it's a trap and we're trapsafe */
	move_player(dir, !(square_isdisarmabletrap(cave, grid) && trapsafe));
}


/**
 * Walk into a trap.
 */
void do_cmd_jump(struct command *cmd)
{
	struct loc grid;
	int dir;

	/* Get arguments */
	if (cmd_get_direction(cmd, "direction", &dir, false) != CMD_OK)
		return;

	/* If we're in a web, deal with that */
	if (square_iswebbed(cave, player->grid)) {
		/* Clear the web, finish turn */
		msg("You clear the web.");
		square_destroy_trap(cave, player->grid);
		player->upkeep->energy_use = z_info->move_energy;
		return;
	}

	/* Apply confusion if necessary */
	if (player_confuse_dir(player, &dir, false))
		player->upkeep->energy_use = z_info->move_energy;

	/* Verify walkability */
	grid = loc_sum(player->grid, ddgrid[dir]);
	if (!do_cmd_walk_test(grid))
		return;

	player->upkeep->energy_use = energy_per_move(player);

	move_player(dir, false);
}


/**
 * Start running.
 *
 * Note that running while confused is not allowed.
 */
void do_cmd_run(struct command *cmd)
{
	struct loc grid;
	int dir;

	/* Get arguments */
	if (cmd_get_direction(cmd, "direction", &dir, false) != CMD_OK)
		return;

	/* If we're in a web, deal with that */
	if (square_iswebbed(cave, player->grid)) {
		/* Clear the web, finish turn */
		msg("You clear the web.");
		square_destroy_trap(cave, player->grid);
		player->upkeep->energy_use = z_info->move_energy;
		return;
	}

	if (player_confuse_dir(player, &dir, true))
		return;

	/* Get location */
	if (dir) {
		grid = loc_sum(player->grid, ddgrid[dir]);
		if (!do_cmd_walk_test(grid))
			return;
			
		/* Hack: convert repeat count to running count */
		if (cmd->nrepeats > 0) {
			player->upkeep->running = cmd->nrepeats;
			cmd->nrepeats = 0;
		}
		else {
			player->upkeep->running = 0;
		}
	}

	/* Start run */
	run_step(dir);
}


/**
 * Start running with pathfinder.
 *
 * Note that running while confused is not allowed.
 */
void do_cmd_pathfind(struct command *cmd)
{
	struct loc grid;

	/* XXX-AS Add better arg checking */
	cmd_get_arg_point(cmd, "point", &grid);

	if (player->timed[TMD_CONFUSED])
		return;

	if (find_path(grid)) {
		player->upkeep->running = 1000;
		/* Calculate torch radius */
		player->upkeep->update |= (PU_TORCH);
		player->upkeep->running_withpathfind = true;
		run_step(0);
	}
}



/**
 * Stay still.  Search.  Enter stores.
 * Pick up treasure if "pickup" is true.
 */
void do_cmd_hold(struct command *cmd)
{
	/* Take a turn */
	player->upkeep->energy_use = z_info->move_energy;

	/* Searching (probably not necessary - NRM)*/
	search(player);

	/* Enter a store if we are on one, otherwise look at the floor */
	if (square_isshop(cave, player->grid)) {
		if (player_is_shapechanged(player) &&
			!index_is_home(square_shopnum(cave, player->grid))) {
			msg("There is a scream and the door slams shut!");
			return;
		}
		disturb(player);
		event_signal(EVENT_ENTER_STORE);
		event_remove_handler_type(EVENT_ENTER_STORE);
		event_signal(EVENT_USE_STORE);
		event_remove_handler_type(EVENT_USE_STORE);
		event_signal(EVENT_LEAVE_STORE);
		event_remove_handler_type(EVENT_LEAVE_STORE);

		/* Turn will be taken exiting the shop */
		player->upkeep->energy_use = 0;
	} else {
		if (OPT(player, pickup_always)) {
			/* Pick things up, not using extra energy */
			cmdq_push(CMD_PICKUP);
			if (player->upkeep->energy_use > z_info->move_energy)
				player->upkeep->energy_use = z_info->move_energy;
		} else {
			event_signal(EVENT_SEEFLOOR);
			square_know_pile(cave, player->grid);
		}
	}
}


/**
 * Rest (restores hit points and mana and such)
 */
void do_cmd_rest(struct command *cmd)
{
	int n;

	/* XXX-AS need to insert UI here */
	if (cmd_get_arg_choice(cmd, "choice", &n) != CMD_OK)
		return;

	/* 
	 * A little sanity checking on the input - only the specified negative 
	 * values are valid. 
	 */
	if (n < 0 && !player_resting_is_special(n))
		return;

	/* Do some upkeep on the first turn of rest */
	if (!player_is_resting(player)) {
		player->upkeep->update |= (PU_BONUS);

		/* If a number of turns was entered, remember it */
		if (n > 1)
			player_set_resting_repeat_count(player, n);
		else if (n == 1)
			/* If we're repeating the command, use the same count */
			n = player_get_resting_repeat_count(player);
	}

	/* Set the counter, and stop if told to */
	player_resting_set_count(player, n);
	if (!player_is_resting(player))
		return;

	/* Take a turn */
	player_resting_step_turn(player);

	/* Redraw the state if requested */
	handle_stuff(player);

	/* Prepare to continue, or cancel and clean up */
	if (player_resting_count(player) > 0) {
		cmdq_push(CMD_REST);
		cmd_set_arg_choice(cmdq_peek(), "choice", n - 1);
	} else if (player_resting_is_special(n)) {
		cmdq_push(CMD_REST);
		cmd_set_arg_choice(cmdq_peek(), "choice", n);
		player_set_resting_repeat_count(player, 0);
	} else {
		player_resting_cancel(player, false);
	}

}


/**
 * Spend a turn doing nothing
 */
void do_cmd_sleep(struct command *cmd)
{
	/* Take a turn */
	player->upkeep->energy_use = z_info->move_energy;
}


/**
 * Array of feeling strings for object feelings.
 * Keep strings at 36 or less characters to keep the
 * combined feeling on one row.
 */
static const char *obj_feeling_text[] =
{
	"Looks like any other level.",
	"you sense an item of wondrous power!",
	"there are superb treasures here.",
	"there are excellent treasures here.",
	"there are very good treasures here.",
	"there are good treasures here.",
	"there may be something worthwhile here.",
	"there may not be much interesting here.",
	"there aren't many treasures here.",
	"there are only scraps of junk here.",
	"there is naught but cobwebs here."
};

/**
 * Array of feeling strings for monster feelings.
 * Keep strings at 36 or less characters to keep the
 * combined feeling on one row.
 */
static const char *mon_feeling_text[] =
{
	/* first string is just a place holder to 
	 * maintain symmetry with obj_feeling.
	 */
	"You are still uncertain about this place",
	"Omens of death haunt this place",
	"This place seems murderous",
	"This place seems terribly dangerous",
	"You feel anxious about this place",
	"You feel nervous about this place",
	"This place does not seem too risky",
	"This place seems reasonably safe",
	"This seems a tame, sheltered place",
	"This seems a quiet, peaceful place"
};

/**
 * Display the feeling.  Players always get a monster feeling.
 * Object feelings are delayed until the player has explored some
 * of the level.
 */
void display_feeling(bool obj_only)
{
	uint16_t obj_feeling = cave->feeling / 10;
	uint16_t mon_feeling = cave->feeling - (10 * obj_feeling);
	const char *join;

	/* Don't show feelings for cold-hearted characters */
	if (!OPT(player, birth_feelings)) return;

	/* No useful feeling in town */
	if (!player->depth) {
		msg("Looks like a typical town.");
		return;
	}

	/* Special message for themed levels */
	if (player->themed_level) {
		struct vault *level = themed_level(player->themed_level);
		msg("%s", level->typ);
		return;
	}

	/* Display only the object feeling when it's first discovered. */
	if (obj_only) {
		disturb(player);
		msg("You feel that %s", obj_feeling_text[obj_feeling]);
		return;
	}

	/* Players automatically get a monster feeling. */
	if (cave->feeling_squares < z_info->feeling_need) {
		msg("%s.", mon_feeling_text[mon_feeling]);
		return;
	}

	/* Verify the feelings */
	if (obj_feeling >= N_ELEMENTS(obj_feeling_text))
		obj_feeling = N_ELEMENTS(obj_feeling_text) - 1;

	if (mon_feeling >= N_ELEMENTS(mon_feeling_text))
		mon_feeling = N_ELEMENTS(mon_feeling_text) - 1;

	/* Decide the conjunction */
	if ((mon_feeling <= 5 && obj_feeling > 6) ||
			(mon_feeling > 5 && obj_feeling <= 6))
		join = ", yet";
	else
		join = ", and";

	/* Display the feeling */
	msg("%s%s %s", mon_feeling_text[mon_feeling], join,
		obj_feeling_text[obj_feeling]);
}


void do_cmd_feeling(void)
{
	display_feeling(false);
}

/**
 * Make a monster perform an action.
 *
 * Currently possible actions are cast a random spell, drop a random item,
 * stand still, or move (attacking any intervening monster).
 */
void do_cmd_mon_command(struct command *cmd)
{
	struct monster *mon = get_commanded_monster();
	struct monster_lore *lore = NULL;
	char m_name[80];

	assert(mon);
	lore = get_lore(mon->race);

	/* Get the monster name */
	monster_desc(m_name, sizeof(m_name), mon,
		MDESC_CAPITAL | MDESC_IND_HID | MDESC_COMMA);

	switch (cmd->code) {
		case CMD_READ_SCROLL: {
			/* Actually 'r'elease monster */
			mon_clear_timed(mon, MON_TMD_COMMAND, MON_TMD_FLG_NOTIFY);
			player_clear_timed(player, TMD_COMMAND, true);
			break;
		}
		case CMD_CAST: {
			int dir = DIR_UNKNOWN;
			struct monster *t_mon = NULL;
			bitflag f[RSF_SIZE];
			bool seen = player->timed[TMD_BLIND] ? false : true;
			int spell_index;

			/* Choose a target monster */
			target_set_monster(NULL);
			get_aim_dir(&dir);
			t_mon = target_get_monster();
			if (!t_mon) {
				msg("No target monster selected!");
				return;
			}
			mon->target.midx = t_mon->midx;

			/* Pick a random spell and cast it */
			rsf_copy(f, mon->race->spell_flags);
			spell_index = choose_attack_spell(f, true, true);
			if (!spell_index) {
				msg("This monster has no spells!");
				return;
			}
			do_mon_spell(spell_index, mon, seen);

			/* Remember what the monster did */
			if (seen) {
				rsf_on(lore->spell_flags, spell_index);
				if (mon_spell_is_innate(spell_index)) {
					/* Innate spell */
					if (lore->cast_innate < UCHAR_MAX)
						lore->cast_innate++;
				} else {
					/* Bolt or Ball, or Special spell */
					if (lore->cast_spell < UCHAR_MAX)
						lore->cast_spell++;
				}
			}
			if (player->is_dead && (lore->deaths < SHRT_MAX)) {
				lore->deaths++;
			}
			lore_update(mon->race, lore);
			mon->target.midx = -1;

			break;
		}
		case CMD_DROP: {
			char o_name[80];
			struct object *obj = get_random_monster_object(mon);
			if (!obj) break;
			obj->held_m_idx = 0;
			pile_excise(&mon->held_obj, obj);
			drop_near(cave, &obj, 0, mon->grid, true, false);
			object_desc(o_name, sizeof(o_name), obj,
				ODESC_PREFIX | ODESC_FULL, player);
			if (!ignore_item_ok(player, obj)) {
				msg("%s drops %s.", m_name, o_name);
			}

			break;
		}
		case CMD_HOLD: {
			/* Do nothing */
			break;
		}
		case CMD_WALK: {
			int dir;
			struct loc grid;
			bool can_move = false;
			bool has_hit = false;
			struct monster *t_mon = NULL;

			/* Get arguments */
			if (cmd_get_direction(cmd, "direction", &dir, false) != CMD_OK)
				return;
			grid = loc_sum(mon->grid, ddgrid[dir]);

			/* Monster there - attack */
			t_mon = square_monster(cave, grid);
			if (t_mon) {
				/* Attack the monster */
				if (monster_attack_monster(mon, t_mon)) {
					has_hit = true;
				} else {
					can_move = false;
				}
			} else if (square_ispassable(cave, grid)) {
				/* Floor is open? */
				can_move = true;
			} else if (square_isperm(cave, grid)) {
				/* Permanent wall in the way */
				can_move = false;
			} else {
				/* There's some kind of feature in the way, so learn about
				 * kill-wall and pass-wall now */
				if (monster_is_visible(mon)) {
					rf_on(lore->flags, RF_PASS_WALL);
					rf_on(lore->flags, RF_KILL_WALL);
					rf_on(lore->flags, RF_SMASH_WALL);
				}

				/* Monster may be able to deal with walls and doors */
				if (rf_has(mon->race->flags, RF_PASS_WALL)) {
					can_move = true;
				} else if (rf_has(mon->race->flags, RF_KILL_WALL)) {
					/* Remove the wall */
					square_destroy_wall(cave, grid);
					can_move = true;
				} else if (rf_has(mon->race->flags, RF_SMASH_WALL)) {
					/* Remove everything */
					square_smash_wall(cave, grid);
					can_move = true;
				} else if (square_iscloseddoor(cave, grid) ||
						   square_issecretdoor(cave, grid)) {
					bool can_open = rf_has(mon->race->flags, RF_OPEN_DOOR);
					bool can_bash = rf_has(mon->race->flags, RF_BASH_DOOR);

					/* Learn about door abilities */
					if (monster_is_visible(mon)) {
						rf_on(lore->flags, RF_OPEN_DOOR);
						rf_on(lore->flags, RF_BASH_DOOR);
					}

					/* If the monster can deal with doors, prefer to bash */
					if (can_bash || can_open) {
						/* Now outcome depends on type of door */
						if (square_islockeddoor(cave, grid)) {
							/* Test strength against door strength */
							int k = square_door_power(cave, grid);
							if (randint0(mon->hp / 10) > k) {
								if (can_bash) {
									msg("%s slams against the door.", m_name);
								} else {
									msg("%s fiddles with the lock.", m_name);
								}

								/* Reduce the power of the door by one */
								square_set_door_lock(cave, grid, k - 1);
							}
						} else {
							/* Closed or secret door -- always open or bash */
							if (can_bash) {
								square_smash_door(cave, grid);

								msg("You hear a door burst open!");

								/* Fall into doorway */
								can_move = true;
							} else {
								square_open_door(cave, grid);
								can_move = true;
							}
						}
					}
				}
			}

			if (has_hit) {
				break;
			} else if (can_move) {
				monster_swap(mon->grid, grid);
				player->upkeep->update |= (PU_UPDATE_VIEW | PU_MONSTERS);
			} else {
				msg("The way is blocked.");
			}
			break;
		}
		default: {
			msg("Valid commands: move, stand still, 'd'rop, 'm'agic, or 'r'elease.");
			return;
		}
	}


	/* Take a turn */
	player->upkeep->energy_use = z_info->move_energy;
}
