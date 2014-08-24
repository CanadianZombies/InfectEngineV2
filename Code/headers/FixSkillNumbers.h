#ifndef _FixSkillNumber_H
#define _FixSkillNumber_H
#if defined(Database_Cpp)
	#define SkillNumber(gsn) sh_int gsn;
#else
	#define SkillNumber(gsn) extern sh_int gsn;
#endif

SkillNumber(gsn_backstab);
SkillNumber(gsn_dodge);
SkillNumber(gsn_envenom);
SkillNumber(gsn_hide);
SkillNumber(gsn_peek);
SkillNumber(gsn_pick_lock);
SkillNumber(gsn_sneak);
SkillNumber(gsn_steal)
SkillNumber(gsn_disarm);
SkillNumber(gsn_enhanced_damage);
SkillNumber(gsn_kick);
SkillNumber(gsn_parry);
SkillNumber(gsn_rescue);
SkillNumber(gsn_second_attack);
SkillNumber(gsn_third_attack);
SkillNumber(gsn_blindness);
SkillNumber(gsn_charm_person);
SkillNumber(gsn_curse);
SkillNumber(gsn_invis);
SkillNumber(gsn_mass_invis);
SkillNumber(gsn_plague);
SkillNumber(gsn_poison);
SkillNumber(gsn_sleep);
SkillNumber(gsn_fly);
SkillNumber(gsn_sanctuary);
SkillNumber(gsn_axe);
SkillNumber(gsn_dagger);
SkillNumber(gsn_flail);
SkillNumber(gsn_mace);
SkillNumber(gsn_polearm);
SkillNumber(gsn_shield_block);
SkillNumber(gsn_spear);
SkillNumber(gsn_sword);
SkillNumber(gsn_whip);
SkillNumber(gsn_bash);
SkillNumber(gsn_berserk);
SkillNumber(gsn_dirt);
SkillNumber(gsn_hand_to_hand);
SkillNumber(gsn_trip);
SkillNumber(gsn_fast_healing);
SkillNumber(gsn_haggle);
SkillNumber(gsn_lore);
SkillNumber(gsn_meditation);
SkillNumber(gsn_scrolls);
SkillNumber(gsn_staves);
SkillNumber(gsn_wands);
SkillNumber(gsn_recall);

#endif
