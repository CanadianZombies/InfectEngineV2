#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_version )
{
	time_t lt = time ( 0 );
	struct tm *tmt_ptr = localtime ( &lt );

	writeBuffer ( Format ( "+------------------------------------------------+\n\r" ), ch );
	writeBuffer ( Format ( "| The Infected City: V%-25s  |\n\r", getVersion() ), ch );
	writeBuffer ( Format ( "| Written by: David Simmerson  (Omega)           |\n\r" ), ch );
	writeBuffer ( Format ( "| Infect Engine Copyright (c) 2013-%4d.         |\n\r", ( tmt_ptr->tm_year + 1900 ) ), ch );
	writeBuffer ( Format ( "+------------------------------------------------+\n\r" ), ch );
	if ( IsStaff ( ch ) ) {
		writeBuffer ( Format ( "| Engine Name:        %-25s  |\r\n", mudEngineName ), ch );
		writeBuffer ( Format ( "| Backup Name:        %-25s  |\r\n", mudBackupName ), ch );
		writeBuffer ( Format ( "| Compiled With:      %-25s  |\r\n", mudCompiler ), ch );
		writeBuffer ( Format ( "+------------------------------------------------+\n\r" ), ch );
	}

}
