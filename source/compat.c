/*
Copyright (C) 2023 Ivy Bowling

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// compat.c  -- Enhanced support for hipnotic, rogue, and other mods that make engine changes.

#include "quakedef.h"

//
// =======================
// SUPERHOT Quake
// =======================
//

cvar_t shq_superhot = {"shq_superhot", "1"};
cvar_t shq_gamemode = {"shq_gamemode", "0"};
cvar_t shq_hotswitch = {"shq_hotswitch", "1"};
cvar_t shq_permadeath = {"shq_permadeath", "0"};
cvar_t shq_fast_bullets = {"shq_fast_bullets", "0"};
cvar_t shq_one_bullet = {"shq_one_bullet", "0"};
cvar_t shq_slowmo_min = {"shq_slowmo_min", "0.05"};
cvar_t shq_slowmo_dec = {"shq_slowmo_dec", "0.1"};
cvar_t shq_slowmo_inc = {"shq_slowmo_inc", "0.1"};
cvar_t shq_pickups = {"shq_pickups", "114"};
cvar_t shq_weapon_drops = {"shq_weapon_drops", "1"};
cvar_t shq_zero_bullet = {"shq_zero_bullet", "0"};
cvar_t shq_lethal_melee = {"shq_lethal_melee", "0"};
cvar_t shq_overkill_melee = {"shq_overkill_melee", "0"};
cvar_t shq_crosshair_state = {"shq_crosshair_state", "4"};
cvar_t shq_crosshair_scale = {"shq_crosshair_scale", "1"};
cvar_t shq_challenge_speedrun = {"shq_challenge_speedrun", "0"};
cvar_t shq_challenge_speedrunrt = {"shq_challenge_speedrunrt", "0"};
cvar_t shq_challenge_axeonly = {"shq_challenge_axeonly", "0"};
cvar_t shq_challenge_fullstop = {"shq_challenge_fullstop", "0"};
cvar_t shq_challenge_ad2013 = {"shq_challenge_ad2013", "0"};
cvar_t shq_challenge_ad2017 = {"shq_challenge_ad2017", "0"};
cvar_t shq_challenge_hotswitch = {"shq_challenge_hotswitch", "1"};
cvar_t shq_challenge_barehands = {"shq_challenge_barehands", "0"};
cvar_t shq_challenge_throwing = {"shq_challenge_throwing", "0"};
cvar_t shq_challenge_superthrowing = {"shq_challenge_superthrowing", "0"};
cvar_t shq_challenge_ghost = {"shq_challenge_ghost", "0"};
cvar_t shq_challenge_hardmode = {"shq_challenge_hardmode", "0"};
cvar_t shq_challenge_impossible = {"shq_challenge_impossible", "0"};
cvar_t shq_challenge_norestart = {"shq_challenge_norestart", "0"};
cvar_t shq_challenge_speedrun_highscores = {"shq_challenge_speedrun_highscores", "0"};
cvar_t shq_challenge_speedrunrt_highscores = {"shq_challenge_speedrunrt_highscores", "0"};
cvar_t shq_crosshair_interactive = {"shq_crosshair_interactive", "0"};
cvar_t shq_endless_kills = {"shq_endless_kills", "0"};
cvar_t shq_lethal_throwable = {"shq_lethal_throwable", "0"};
cvar_t shq_overkill_throwable = {"shq_overkill_throwable", "0"};
cvar_t shq_disallowed_weapons = {"shq_disallowed_weapons", "0"};
cvar_t shq_enemy_flinch_chance_modifier = {"shq_enemy_flinch_chance_modifier", "1"};
cvar_t shq_hotswitch_interval = {"shq_hotswitch_interval", "5"};
cvar_t shq_lowhealth = {"shq_lowhealth", "3"};
cvar_t shq_lowammo = {"shq_lowammo", "3"};
cvar_t shq_lowdamage = {"shq_lowdamage", "3"};
cvar_t shq_quiet = {"shq_quiet", "1"};
cvar_t shq_pickup_on_touch = {"shq_pickup_on_touch", "0"};
cvar_t shq_pickup_only_with_empty_hands = {"shq_pickup_only_with_empty_hands", "1"};
cvar_t shq_pickup_prevent_accidental_discharge = {"shq_pickup_prevent_accidental_discharge", "0"};
cvar_t shq_game_hints = {"shq_game_hints", "1"};
cvar_t shq_bullet_slicing = {"shq_bullet_slicing", "1"};
cvar_t shq_ballistic = {"shq_ballistic", "1"};
cvar_t shq_no_persistent_inventory = {"shq_no_persistent_inventory", "1"};
cvar_t shq_stealth = {"shq_stealth", "1"};
cvar_t shq_inventory = {"shq_inventory", "0"};
cvar_t shq_noammo_warning = {"shq_noammo_warning", "1"};
cvar_t shq_ignore_weapon_safety = {"shq_ignore_weapon_safety", "1"};
cvar_t shq_supergib = {"shq_supergib", "1"};
cvar_t shq_gib_time = {"shq_gib_time", "0"};
cvar_t shq_noweaponswitch = {"shq_noweaponswitch", "1"};
cvar_t shq_noachievements = {"shq_noachievements", "0"};
cvar_t shq_slowmo = {"shq_slowmo", "2"};
cvar_t shq_slowmo_max = {"shq_slowmo_max", "1.0"};
cvar_t shq_slowmo_move_scale = {"shq_slowmo_move_scale", "1.0"};
cvar_t shq_slowmo_look_scale = {"shq_slowmo_look_scale", "0.2"};
cvar_t lifetype = {"lifetype", "0"};
cvar_t lifelimit = {"lifelimit", "0"};
cvar_t shq_achievement_accessdenied = {"shq_achievement_accessdenied", "0"};
cvar_t shq_achievement_bored = {"shq_achievement_bored", "0"};
cvar_t shq_achievement_cantgetenough = {"shq_achievement_cantgetenough", "0"};
cvar_t shq_achievement_charted = {"shq_achievement_charted", "0"};
cvar_t shq_achievement_conserveammo = {"shq_achievement_conserveammo", "0"};
cvar_t shq_achievement_deepweb = {"shq_achievement_deepweb", "0"};
cvar_t shq_achievement_dudes = {"shq_achievement_dudes", "0"};
cvar_t shq_achievement_fullpackage = {"shq_achievement_fullpackage", "0"};
cvar_t shq_achievement_getredy = {"shq_achievement_getredy", "0"};
cvar_t shq_achievement_hercules = {"shq_achievement_hercules", "0"};
cvar_t shq_achievement_itsamemario = {"shq_achievement_itsamemario", "0"};
cvar_t shq_achievement_just = {"shq_achievement_just", "0"};
cvar_t shq_achievement_killing = {"shq_achievement_killing", "0"};
cvar_t shq_achievement_notheydidnt = {"shq_achievement_notheydidnt", "0"};
cvar_t shq_achievement_oneofus = {"shq_achievement_oneofus", "0"};
cvar_t shq_achievement_pointlessbutcool = {"shq_achievement_pointlessbutcool", "0"};
cvar_t shq_achievement_ratatatata = {"shq_achievement_ratatatata", "0"};
cvar_t shq_achievement_red = {"shq_achievement_red", "0"};
cvar_t shq_achievement_shishkebap = {"shq_achievement_shishkebap", "0"};
cvar_t shq_achievement_soitispossible = {"shq_achievement_soitispossible", "0"};
cvar_t shq_achievement_spreadthesystem = {"shq_achievement_spreadthesystem", "0"};
cvar_t shq_achievement_standredy = {"shq_achievement_standredy", "0"};
cvar_t shq_achievement_wecanseeyou = {"shq_achievement_wecanseeyou", "0"};
cvar_t shq_achievement_wetoldyoutoleave = {"shq_achievement_wetoldyoutoleave", "0"};
cvar_t shq_achievement_withakatana = {"shq_achievement_withakatana", "0"};
cvar_t shq_achievement_enlightening = {"shq_achievement_enlightening", "0"};
cvar_t shq_achievement_whatashame = {"shq_achievement_whatashame", "0"};
cvar_t shq_achievement_garbagecollector = {"shq_achievement_garbagecollector", "0"};
cvar_t shq_achievement_predator = {"shq_achievement_predator", "0"};

cvar_t v_ofsx = {"v_ofsx", "0"};
cvar_t v_ofsy = {"v_ofsy", "0"};
cvar_t v_ofsz = {"v_ofsz", "0"};
cvar_t v_viewmodel_pitch = {"v_viewmodel_pitch", "0"};
cvar_t v_viewmodel_yaw = {"v_viewmodel_yaw", "0"};
cvar_t v_viewmodel_roll = {"v_viewmodel_roll", "0"};

void SHQ_CvarInit(void)
{
    if (IS_SUPERHOT) {
        Cvar_RegisterVariable(&shq_superhot, NULL);
        Cvar_RegisterVariable(&shq_gamemode, NULL);
        Cvar_RegisterVariable(&shq_challenge_speedrun, NULL);
        Cvar_RegisterVariable(&shq_challenge_speedrunrt, NULL);
        Cvar_RegisterVariable(&shq_challenge_axeonly, NULL);
        Cvar_RegisterVariable(&shq_challenge_fullstop, NULL);
        Cvar_RegisterVariable(&shq_challenge_ad2013, NULL);
        Cvar_RegisterVariable(&shq_challenge_ad2017, NULL);
        Cvar_RegisterVariable(&shq_challenge_hotswitch, NULL);
        Cvar_RegisterVariable(&shq_challenge_barehands, NULL);
        Cvar_RegisterVariable(&shq_challenge_throwing, NULL);
        Cvar_RegisterVariable(&shq_challenge_superthrowing, NULL);
        Cvar_RegisterVariable(&shq_challenge_ghost, NULL);
        Cvar_RegisterVariable(&shq_challenge_hardmode, NULL);
        Cvar_RegisterVariable(&shq_challenge_impossible, NULL);
        Cvar_RegisterVariable(&shq_challenge_norestart, NULL);
        Cvar_RegisterVariable(&shq_challenge_speedrun_highscores, NULL);
        Cvar_RegisterVariable(&shq_challenge_speedrunrt_highscores, NULL);
        Cvar_RegisterVariable(&shq_crosshair_interactive, NULL);
        Cvar_RegisterVariable(&shq_endless_kills, NULL);
        Cvar_RegisterVariable(&shq_crosshair_state, NULL);
        Cvar_RegisterVariable(&shq_crosshair_scale, NULL);
        Cvar_RegisterVariable(&shq_hotswitch, NULL);
        Cvar_RegisterVariable(&shq_permadeath, NULL);
        Cvar_RegisterVariable(&shq_fast_bullets, NULL);
        Cvar_RegisterVariable(&shq_one_bullet, NULL);
        Cvar_RegisterVariable(&shq_slowmo_min, NULL);
        Cvar_RegisterVariable(&shq_slowmo_dec, NULL);
        Cvar_RegisterVariable(&shq_slowmo_inc, NULL);
        Cvar_RegisterVariable(&shq_pickups, NULL);
        Cvar_RegisterVariable(&shq_weapon_drops, NULL);
        Cvar_RegisterVariable(&shq_zero_bullet, NULL);
        Cvar_RegisterVariable(&shq_lethal_melee, NULL);
        Cvar_RegisterVariable(&shq_overkill_melee, NULL);
        Cvar_RegisterVariable(&shq_lethal_throwable, NULL);
        Cvar_RegisterVariable(&shq_overkill_throwable, NULL);
        Cvar_RegisterVariable(&shq_disallowed_weapons, NULL);
        Cvar_RegisterVariable(&shq_enemy_flinch_chance_modifier, NULL);
        Cvar_RegisterVariable(&shq_hotswitch_interval, NULL);
        Cvar_RegisterVariable(&shq_lowhealth, NULL);
        Cvar_RegisterVariable(&shq_lowammo, NULL);
        Cvar_RegisterVariable(&shq_lowdamage, NULL);
        Cvar_RegisterVariable(&shq_quiet, NULL);
        Cvar_RegisterVariable(&shq_pickup_on_touch, NULL);
        Cvar_RegisterVariable(&shq_pickup_only_with_empty_hands, NULL);
        Cvar_RegisterVariable(&shq_pickup_prevent_accidental_discharge, NULL);
        Cvar_RegisterVariable(&shq_game_hints, NULL);
        Cvar_RegisterVariable(&shq_bullet_slicing, NULL);
        Cvar_RegisterVariable(&shq_ballistic, NULL);
        Cvar_RegisterVariable(&shq_no_persistent_inventory, NULL);
        Cvar_RegisterVariable(&shq_stealth, NULL);
        Cvar_RegisterVariable(&shq_inventory, NULL);
        Cvar_RegisterVariable(&shq_noammo_warning, NULL);
        Cvar_RegisterVariable(&shq_ignore_weapon_safety, NULL);
        Cvar_RegisterVariable(&shq_supergib, NULL);
        Cvar_RegisterVariable(&shq_gib_time, NULL);
        Cvar_RegisterVariable(&shq_noweaponswitch, NULL);
        Cvar_RegisterVariable(&shq_noachievements, NULL);
        Cvar_RegisterVariable(&shq_slowmo, NULL);
        Cvar_RegisterVariable(&shq_slowmo_max, NULL);
        Cvar_RegisterVariable(&shq_slowmo_move_scale, NULL);
        Cvar_RegisterVariable(&shq_slowmo_look_scale, NULL);
        Cvar_RegisterVariable(&lifetype, NULL);
        Cvar_RegisterVariable(&lifelimit, NULL);
        Cvar_RegisterVariable(&shq_achievement_accessdenied, NULL);
        Cvar_RegisterVariable(&shq_achievement_bored, NULL);
        Cvar_RegisterVariable(&shq_achievement_cantgetenough, NULL);
        Cvar_RegisterVariable(&shq_achievement_charted, NULL);
        Cvar_RegisterVariable(&shq_achievement_conserveammo, NULL);
        Cvar_RegisterVariable(&shq_achievement_deepweb, NULL);
        Cvar_RegisterVariable(&shq_achievement_dudes, NULL);
        Cvar_RegisterVariable(&shq_achievement_fullpackage, NULL);
        Cvar_RegisterVariable(&shq_achievement_getredy, NULL);
        Cvar_RegisterVariable(&shq_achievement_hercules, NULL);
        Cvar_RegisterVariable(&shq_achievement_itsamemario, NULL);
        Cvar_RegisterVariable(&shq_achievement_just, NULL);
        Cvar_RegisterVariable(&shq_achievement_killing, NULL);
        Cvar_RegisterVariable(&shq_achievement_notheydidnt, NULL);
        Cvar_RegisterVariable(&shq_achievement_oneofus, NULL);
        Cvar_RegisterVariable(&shq_achievement_pointlessbutcool, NULL);
        Cvar_RegisterVariable(&shq_achievement_ratatatata, NULL);
        Cvar_RegisterVariable(&shq_achievement_red, NULL);
        Cvar_RegisterVariable(&shq_achievement_shishkebap, NULL);
        Cvar_RegisterVariable(&shq_achievement_soitispossible, NULL);
        Cvar_RegisterVariable(&shq_achievement_spreadthesystem, NULL);
        Cvar_RegisterVariable(&shq_achievement_standredy, NULL);
        Cvar_RegisterVariable(&shq_achievement_wecanseeyou, NULL);
        Cvar_RegisterVariable(&shq_achievement_wetoldyoutoleave, NULL);
        Cvar_RegisterVariable(&shq_achievement_withakatana, NULL);
        Cvar_RegisterVariable(&shq_achievement_enlightening, NULL);
        Cvar_RegisterVariable(&shq_achievement_whatashame, NULL);
        Cvar_RegisterVariable(&shq_achievement_garbagecollector, NULL);
        Cvar_RegisterVariable(&shq_achievement_predator, NULL);

        Cvar_RegisterVariable(&v_ofsx, NULL);
        Cvar_RegisterVariable(&v_ofsy, NULL);
        Cvar_RegisterVariable(&v_ofsz, NULL);
        Cvar_RegisterVariable(&v_viewmodel_pitch, NULL);
        Cvar_RegisterVariable(&v_viewmodel_yaw, NULL);
        Cvar_RegisterVariable(&v_viewmodel_roll, NULL);
    }
}