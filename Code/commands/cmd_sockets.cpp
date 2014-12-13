#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_sockets )
{
	char buf[2 * MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	Socket *d;
	int count;

	count	= 0;
	buf[0]	= '\0';

	ChopC ( argument, arg );
	for ( d = socket_list; d != NULL; d = d->next ) {
		if ( d->character != NULL && can_see ( ch, d->character )
				&& ( arg[0] == '\0' || is_name ( arg, d->character->name )
					 || ( d->original && is_name ( arg, d->original->name ) ) ) ) {
			count++;
			sprintf ( buf + strlen ( buf ), "[%3d %2d] %s@%s\n\r",
					  d->descriptor,
					  d->connected,
					  d->original  ? d->original->name  :
					  d->character ? d->character->name : "(none)",
					  d->host
					);
		}
	}
	if ( count == 0 ) {
		writeBuffer ( "No one by that name is connected.\n\r", ch );
		return;
	}

	sprintf ( buf2, "%d user%s\n\r", count, count == 1 ? "" : "s" );
	strcat ( buf, buf2 );
	writePage ( buf, ch );
	return;
}

