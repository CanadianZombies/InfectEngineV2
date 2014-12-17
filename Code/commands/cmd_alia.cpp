#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_alia )
{
	writeBuffer ( "I'm sorry, alias must be entered in full.\n\r", ch );
	return;
}
