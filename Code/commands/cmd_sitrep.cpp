#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_sitrep )
{
	Creature *cr = ch;

	struct staff_rep {
		const char *name;
		int bitvector;
		int level;
		const char *description;
	};

	const struct staff_rep rep_table[] = {
		{"Logs", CF_SEE_LOGS, MAX_LEVEL, "View all logs as they happen" },
		{"Errors", CF_SEE_BUGS, MAX_LEVEL,  "View all bugs as they happen" },
		{"Security", CF_SEE_SECURITY, MAX_LEVEL, "View all security logs as they happen." },
		{"Commands", CF_SEE_COMMANDS, MAX_LEVEL, "See All Issued commands" },
		{"Debug",    CF_SEE_DEBUG,    MAX_LEVEL, "See all DEBUG messages." },
		{"script", CF_SEE_SCRIPT, MAX_LEVEL, "See all Scripting Logs." },
		{"suicide",  CF_SEE_ABORT, MAX_LEVEL, "See logs that cause the mud to commit suicide." },
		{NULL, 0, 0, NULL }
	};

	if ( argument[0] == '\0' || argument == NULL ) {
		for ( int x = 0; rep_table[x].name != NULL; x++ ) {
			if ( cr->level >= rep_table[x].level ) {
				writeBuffer ( Format ( "[%15s][%3s] : %s\n\r", rep_table[x].name, IS_SET ( cr->sitrep, rep_table[x].bitvector ) ? "On" : "Off", rep_table[x].description ), cr );
			}
		}
		writeBuffer ( "Type staffrep <flagname> <on|off>\n\r", cr );
		return;
	}

	char first[MAX_STRING_LENGTH];
	char *second;
	second = ChopC ( argument, first );
	if ( second[0] == '\0' || second == NULL ) {
		writeBuffer ( "Syntax: staffrep <flagname> <on|off>\n\r", cr );
		return;
	}

	for ( int x = 0; rep_table[x].name != NULL; x++ ) {
		if ( cr->level >= rep_table[x].level ) {
			if ( SameString ( rep_table[x].name, first ) ) {
				if ( SameString ( second, "on" ) ) {
					SET_BIT ( cr->sitrep, rep_table[x].bitvector );
					writeBuffer ( Format ( "%s has been enabled.\n\r", rep_table[x].name ), cr );
				} else if ( SameString ( second, "off" ) ) {
					REMOVE_BIT ( cr->sitrep, rep_table[x].bitvector );
					writeBuffer ( Format ( "%s has been disabled.\n\r", rep_table[x].name ), cr );
				} else {
					writeBuffer ( "Syntax: staffrep <flagname> <on|off>\n\r", cr );
				}
				return;
			}
		}
	}
	writeBuffer ( "Unknown option!\n\r", cr );
}

