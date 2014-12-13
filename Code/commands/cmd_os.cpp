#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_os )
{
	std::string cmpstring ( "" );
	cmpstring.append ( "---------------------------------------------------------\n\r" );
	cmpstring.append ( Format ( "InfectEngine is compiled with:        g++\n\r" ) );
	//      cmpstring.append("Last Compile TimeStamp:            %s\n\r", __TIME_STAMP__ );
	cmpstring.append ( Format ( "Nested Include Level:              %d\n\r", __INCLUDE_LEVEL__ ) );
	cmpstring.append ( Format ( "Compiler Version:                  %s\n\r", __VERSION__ ) ) ;
#ifndef __APPLE__
	cmpstring.append ( Format ( "Compiler Flags: %s\n\r", wrapstr ( COMPILED_WITH ) ) );
#endif
	//      cmpstring.append("Compiler Register Prefix:          %s\n\r", __REGISTER_PREFIX__);
	cmpstring.append ( "---------------------------------------------------------\n\r" );
	cmpstring.append ( "                   Compiler Specifics                    \n\r" );
	cmpstring.append ( "---------------------------------------------------------\n\r" );
	cmpstring.append ( Format ( "CHAR_BIT:   %10d    SCHAR_MIN:   %10d\n\r", CHAR_BIT, SCHAR_MIN ) );
	cmpstring.append ( Format ( "SCHAR_MAX:  %10d    UCHAR_MAX:   %10d\n\r", SCHAR_MAX, UCHAR_MAX ) );
	cmpstring.append ( Format ( "CHAR_MIN:   %10d    CHAR_MAX:    %10d\n\r", CHAR_MIN, CHAR_MAX ) );
	cmpstring.append ( Format ( "MB_LEN_MAX: %10d    SHRT_MIN:    %10d\n\r", MB_LEN_MAX, SHRT_MIN ) );
	cmpstring.append ( Format ( "SHRT_MAX:   %10d    USHRT_MAX:   %10d\n\r", SHRT_MAX, USHRT_MAX ) );
	cmpstring.append ( Format ( "INT_MIN:    %10d    INT_MAX:     %10d\n\r", INT_MIN, INT_MAX ) );
	cmpstring.append ( Format ( "UINT_MAX:   %10d    LONG_MIN:    %10ld\n\r", UINT_MAX, LONG_MIN ) );
	cmpstring.append ( Format ( "LONG_MAX:   %10ld    ULONG_MAX:   %10ld\n\r", LONG_MAX, ULONG_MAX ) );
	cmpstring.append ( Format ( "DBL_MIN:    %10e    DBL_MAX:     %10e\n\r", DBL_MIN, DBL_MAX ) );
	cmpstring.append ( Format ( "FLT_MIN:    %10e    FLT_MAX:     %10e\n\r", FLT_MIN, FLT_MAX ) );
	cmpstring.append ( "---------------------------------------------------------\n\r" );
#if defined(HAVE_SYS_UTSNAME_H)
	{
		struct utsname name;
		if ( uname ( &name ) == -1 ) {
			cmpstring.append ( Format ( "PlatformID: Unknown-uname_error%d", errno ) );
		} else {
			cmpstring.append ( Format ( "PlatformID: sysname='%s' nodename='%s' "
										"release='%s' version='%s' machine='%s'",
										name.sysname,
										name.nodename,
										name.release,
										name.version,
										name.machine ) );
		}
	}
#endif
#if defined HAVE_SYS_SYSCTL_H && defined(CTL_KERN) && defined(KERN_VERSION)
	{
		// tag on the KERN_VERSION info if available
		int mib[2];
		size_t len;
		mib[0] = CTL_KERN;
		mib[1] = KERN_VERSION;
		char kernver[512];
		len = sizeof ( kernver );
		if ( sysctl ( mib, 2, &kernver, &len, NULL, 0 ) == 0 ) {
			kernver[sizeof ( kernver ) - 1] = '\0';
			cmpstring.append ( Format ( "Kernel: %s", kernver ) );
		}
	}
#endif

	uid_t uid;
	gid_t gid;
	struct passwd *pw;
	struct utsname uts;
	char computer[256];

	// *Ya, found is true to start.. Only if failed attempts are false  :P* //
	bool found = true;

	uid = getuid();
	gid = getgid();

	cmpstring.append ( Format ( "The mud was last compiled on " __DATE__ " at " __TIME__ ".\n" ) );

	cmpstring.append ( Format ( "User is %s\n", whoami() ) );
	cmpstring.append ( Format ( "User IDS: uid=%d, gid=%d\n",  uid, gid ) );

	cmpstring.append ( "---------------------------------------------------------\n\r" );

	if ( gethostname ( computer, 255 ) != 0 || uname ( &uts ) < 0 ) {
		found = false;
	}

	// *Am i smart.... Ya, i'd like to think so :P* //
	if ( found ) {
		cmpstring.append ( Format ( "Computer host name is %s\n\r", computer ) );
		cmpstring.append ( Format ( "System is %s on %s hardware\n\r", uts.sysname, uts.machine ) );
		cmpstring.append ( Format ( "Nodename is %s\n\r", uts.nodename ) );
		cmpstring.append ( Format ( "Version is %s, %s\n\r", uts.release, wrapstr ( uts.version ) ) );
		cmpstring.append ( "---------------------------------------------------------\n\r" );
	}

	pw = getpwuid ( uid );
	cmpstring.append ( Format ( "UID passwd entry:\n name=%s, uid=%d, gid=%d, home=%s, shell=%s\n\r", pw->pw_name, pw->pw_uid, pw->pw_gid, pw->pw_dir, pw->pw_shell ) );

	writePage ( C_STR ( cmpstring ), ch );
	return;
}

