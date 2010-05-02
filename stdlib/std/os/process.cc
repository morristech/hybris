/*
 * This file is part of the Hybris programming language interpreter.
 *
 * Copyleft of Simone Margaritelli aka evilsocket <evilsocket@gmail.com>
 *
 * Hybris is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Hybris is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Hybris.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <sys/types.h>
#include <sys/wait.h>
#include <hybris.h>

HYBRIS_DEFINE_FUNCTION(henv);
HYBRIS_DEFINE_FUNCTION(hexec);
HYBRIS_DEFINE_FUNCTION(hfork);
HYBRIS_DEFINE_FUNCTION(hgetpid);
HYBRIS_DEFINE_FUNCTION(hwait);
HYBRIS_DEFINE_FUNCTION(hpopen);
HYBRIS_DEFINE_FUNCTION(hpclose);
HYBRIS_DEFINE_FUNCTION(hexit);
HYBRIS_DEFINE_FUNCTION(hkill);

HYBRIS_EXPORTED_FUNCTIONS() {
	{ "env",  henv },
	{ "exec", hexec },
	{ "fork", hfork },
	{ "getpid", hgetpid },
	{ "wait", hwait },
	{ "popen", hpopen },
	{ "pclose", hpclose },
	{ "exit", hexit },
	{ "kill", hkill },
	{ "", NULL }
};

extern "C" void hybris_module_init( VM * vmachine ){
	extern VM  *__hyb_vm;

	/*
	 * This module is linked against libhybris.so.1 which contains a compiled
	 * parser.cpp, which has an uninitialized __hyb_vm pointer.
	 * The real VM is passed to this function by the core, so we have to initialize
	 * the pointer with the right data.
	 */
	__hyb_vm = vmachine;

	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGHUP", gc_new_integer(SIGHUP) ); /* Hangup (POSIX).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGINT", gc_new_integer(SIGINT) ); /* Interrupt (ANSI).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGQUIT", gc_new_integer(SIGQUIT) ); /* Quit (POSIX).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGILL", gc_new_integer(SIGILL) ); /* Illegal instruction (ANSI).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGTRAP", gc_new_integer(SIGTRAP) ); /* Trace trap (POSIX).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGABRT", gc_new_integer(SIGABRT) ); /* Abort (ANSI).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGIOT", gc_new_integer(SIGIOT) ); /* IOT trap (4.2 BSD).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGBUS", gc_new_integer(SIGBUS) ); /* BUS error (4.2 BSD).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGFPE", gc_new_integer(SIGFPE) ); /* Floating-point exception (ANSI).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGKILL", gc_new_integer(SIGKILL) ); /* Kill, unblockable (POSIX).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGSEGV", gc_new_integer(SIGSEGV) ); /* Segmentation violation (ANSI).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGPIPE", gc_new_integer(SIGPIPE) ); /* Broken pipe (POSIX).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGALRM", gc_new_integer(SIGALRM) ); /* Alarm clock (POSIX).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGTERM", gc_new_integer(SIGTERM) ); /* Termination (ANSI).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGSTKFLT", gc_new_integer(SIGSTKFLT) ); /* Stack fault.  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGCLD", gc_new_integer(SIGCHLD) ); /* Same as SIGCHLD (System V).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGCHLD", gc_new_integer(SIGCHLD) ); /* Child status has changed (POSIX).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGCONT", gc_new_integer(SIGCONT) ); /* Continue (POSIX).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGSTOP", gc_new_integer(SIGSTOP) ); /* Stop, unblockable (POSIX).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGTSTP", gc_new_integer(SIGTSTP) ); /* Keyboard stop (POSIX).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGTTIN", gc_new_integer(SIGTTIN) ); /* Background read from tty (POSIX).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGTTOU", gc_new_integer(SIGTTOU) ); /* Background write to tty (POSIX).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGURG", gc_new_integer(SIGURG) ); /* Urgent condition on socket (4.2 BSD).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGXCPU", gc_new_integer(SIGXCPU) ); /* CPU limit exceeded (4.2 BSD).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGXFSZ", gc_new_integer(SIGXFSZ) ); /* File size limit exceeded (4.2 BSD).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGVTALRM", gc_new_integer(SIGVTALRM) ); /* Virtual alarm clock (4.2 BSD).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGPROF", gc_new_integer(SIGPROF) ); /* Profiling alarm clock (4.2 BSD).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGWINCH", gc_new_integer(SIGWINCH) ); /* Window size change (4.3 BSD, Sun).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGPOLL", gc_new_integer(SIGIO) ); /* Pollable event occurred (System V).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGIO", gc_new_integer(SIGIO) ); /* I/O now possible (4.2 BSD).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGPWR", gc_new_integer(SIGPWR) ); /* Power failure restart (System V).  */
	HYBRIS_DEFINE_CONSTANT( vmachine, "SIGSYS", gc_new_integer(SIGSYS) ); /* Bad system call.  */
}

HYBRIS_DEFINE_FUNCTION(henv){
	char   *duple,
		   *marker,
	      **env,
		  **envp = vmachine->env;
	Object *map  = (Object *)gc_new_map();

	for( env = envp; *env != 0; env++ ){
		duple   = *env;
		marker  = strchr( duple, '=' );
		*marker = 0x00;

		ob_cl_set( map, (Object *)gc_new_string(duple), (Object *)gc_new_string( marker + 1 ) );
	}

	return map;
}

HYBRIS_DEFINE_FUNCTION(hexec){
	ob_type_assert( ob_argv(0), otString );
    Object *_return = NULL;
    if( ob_argc() ){
        _return = ob_dcast( gc_new_integer( system( string_argv(0).c_str() ) ) );
    }
	else{
		hyb_error( H_ET_SYNTAX, "function 'exec' requires 1 parameter (called with %d)", ob_argc() );
	}
    return _return;
}

HYBRIS_DEFINE_FUNCTION(hfork){
    return ob_dcast( gc_new_integer( fork() ) );
}

HYBRIS_DEFINE_FUNCTION(hgetpid){
    return ob_dcast( gc_new_integer( getpid() ) );
}

HYBRIS_DEFINE_FUNCTION(hwait){
	if( ob_argc() != 1 ){
		hyb_error( H_ET_SYNTAX, "function 'wait' requires 1 parameter (called with %d)", ob_argc() );
	}
	ob_type_assert( ob_argv(0), otInteger );

	return ob_dcast( gc_new_integer( wait( &(int_argv(0)) ) ) );
}

HYBRIS_DEFINE_FUNCTION(hpopen){
	ob_type_assert( ob_argv(0), otString );
	ob_type_assert( ob_argv(1), otString );

    if( ob_argc() == 2 ){
        return  ob_dcast( PTR_TO_INT_OBJ( popen( string_argv(0).c_str(), string_argv(1).c_str() ) ) );
    }
	else{
		hyb_error( H_ET_SYNTAX, "function 'popen' requires 2 parameters (called with %d)", ob_argc() );
	}
}

HYBRIS_DEFINE_FUNCTION(hpclose){
	ob_type_assert( ob_argv(0), otInteger );
    if( ob_argc() ){
		pclose( (FILE *)int_argv(0) );
    }
    return H_DEFAULT_RETURN;
}

HYBRIS_DEFINE_FUNCTION(hexit){
    int code = 0;
    if( ob_argc() > 0 ){
		ob_type_assert( ob_argv(0), otInteger );
		code = (long)int_argv(0);
	}
	exit(code);

    return H_DEFAULT_RETURN;
}

HYBRIS_DEFINE_FUNCTION(hkill){
	if( ob_argc() != 2 ){
		hyb_error( H_ET_SYNTAX, "function 'kill' requires 2 parameters (called with %d)", ob_argc() );
	}
	ob_type_assert( ob_argv(0), otInteger );
	ob_type_assert( ob_argv(1), otInteger );

	return (Object *)gc_new_integer( kill( int_argv(0), int_argv(1) ) );
}
