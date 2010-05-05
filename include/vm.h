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
#ifndef _HCONTEXT_H_
#   define _HCONTEXT_H_

#include <vector>
#include <string>
#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>
#include "types.h"
#include "memory.h"
#include "code.h"
#include "engine.h"

using std::string;
using std::vector;

typedef vector<string> vstr_t;

/* pre declaration of structure vm_t */
typedef struct _vm_t vm_t;
/* pre declaration of structure engine_t */
typedef struct _engine_t engine_t;

/* initializer function pointer prototype */
typedef void     (*initializer_t)( vm_t * );
/* function pointer prototype */
typedef Object * (*function_t)( vm_t *, vmem_t * );

/* macro to declare a hybris function */
#define HYBRIS_DEFINE_FUNCTION(name) Object *name( vm_t *vm, vmem_t *data )
/* macro to define a constant value */
#define HYBRIS_DEFINE_CONSTANT( vm, name, value ) vm->vmem.addConstant( (char *)name, (Object *)value )
/* macro to define a new structure type given its name and its attribute names */
#define HYBRIS_DEFINE_STRUCTURE( vm, name, n, attrs ) vm_define_structure( vm, name, n, attrs )
/* macro to define module exported functions structure */
#define HYBRIS_EXPORTED_FUNCTIONS() extern "C" named_function_t hybris_module_functions[] =

/* macro to easily access hybris functions parameters */
#define ob_argv(i)    (data->at(i))
/* macro to easily access hybris functions parameters number */
#define ob_argc()     (data->size())
/* typed versions of ob_argv macro */
#define int_argv(i)    ob_int_val( data->at(i) )
#define alias_argv(i)  ob_alias_val( data->at(i) )
#define extern_argv(i) ob_extern_val( data->at(i) )
#define float_argv(i)  ob_float_val( data->at(i) )
#define char_argv(i)   ob_char_val( data->at(i) )
#define string_argv(i) ob_string_val( data->at(i) )
#define binary_argv(i) ob_binary_val( data->at(i) )
#define vector_argv(i) ob_vector_val( data->at(i) )
#define map_argv(i)    ob_map_val( data->at(i) )
#define matrix_argv(i) ob_matrix_val( data->at(i) )
#define struct_argv(i) ob_struct_val( data->at(i) )
#define class_argv(i)  ob_class_val( data->at(i) )

/* macros to assert an object type */
#define ob_type_assert(o,t)      if( !(o->type->code == t) ){ \
                                     hyb_error( H_ET_SYNTAX, "'%s' is not a valid variable type", o->type->name ); \
                                  }
#define ob_types_assert(o,t1,t2) if( !(o->type->code == t1) && !(o->type->code == t2) ){ \
                                     hyb_error( H_ET_SYNTAX, "'%s' is not a valid variable type", o->type->name ); \
                                  }

#define HYB_TIMER_START 1
#define HYB_TIMER_STOP  0

typedef struct _named_function_t {
    string     identifier;
    function_t function;

    _named_function_t( string i, function_t f ) :
        identifier(i),
        function(f){

    }
}
named_function_t;

typedef vector<named_function_t *> named_functions_t;

/* module structure definition */
typedef struct _module_t {
	void			 *handle;
    vstr_t            tree;
    string            name;
    initializer_t     initializer;
    named_functions_t functions;
}
module_t;

/* module initializer function pointer prototype */
typedef void (*initializer_t)( vm_t * );

typedef vector<module_t *>        h_modules_t;
typedef HashMap<named_function_t> h_mcache_t;

typedef struct _vm_t {
	/*
	 * Running threads pool vector, used to hard terminate remaining threads
	 * when the vm is released.
	 */
	vector<pthread_t> th_pool;
	/*
	 * Threads mutex, used on each write operation on the vm.
	 */
	pthread_mutex_t   th_mutex;
	/*
	 * The list of active memory frames.
	 */
	list<vframe_t *> frames;
	/*
	 * Source file handle
	 */
	FILE          *fp;
	/*
	 * Pointer to environment variables listing.
	 */
	char         **env;
	/*
	 * Main memory segment.
	 */
	vmem_t         vmem;
	/*
	 * Main code segment.
	 */
	vcode_t        vcode;
	/*
	 * Type definitions segment, where structures and classes
	 * prototypes are defined.
	 */
	vmem_t         vtypes;
	/*
	 * VM command line arguments.
	 */
	h_args_t       args;
	/*
	 * Functions lookup hashtable cache.
	 */
	h_mcache_t	   mcache;
	/*
	 * Dynamically loaded modules instances.
	 */
	h_modules_t    modules;
	/*
	 * Compiled regular expressions cache.
	 */
	HashMap<pcre>  pcre_cache;
	/*
	 * Code execution engine.
	 */
	engine_t      *engine;
	/*
	 * Flag set to true when vm_release is called, used to prevent
	 * recursive calls when an error is triggered inside a class destructor.
	 */
	bool 		   releasing;
}
vm_t;
/*
 * Alloc a virtual machine instance.
 */
vm_t 	   *vm_create();
/*
 * Initialize the context attributes and global constants.
 */
void 	    vm_init( vm_t *vm, int argc, char *argv[], char *envp[] );
/*
 * Release the context (free memory).
 */
void 		vm_release( vm_t *vm );
/*
 * Open the file given in 'args' structure by main, otherwise
 * return the stdin handler.
 */
FILE 	   *vm_fopen( vm_t *vm );
/*
 * If vm->fp != stdin close it.
 */
void 		vm_fclose( vm_t *vm );
/*
 * Change working directory to the script one.
 */
int 		vm_chdir( vm_t *vm );
/*
 * The main SIGSEGV handler to print the stack trace.
 */
static void vm_signal_handler( int signo );
/*
 * Split 'str' into 'tokens' vector using 'delimiters'.
 */
void 		vm_str_split( string& str, string delimiters, vector<string>& tokens );
/*
 * Load a .so module given its full path and name.
 */
void    	vm_load_module( vm_t *vm, string path, string name );
/*
 * Load a dynamic module given its namespace or load an entire
 * namespace tree if the '*' token is given.
 *
 * i.e. std.* will load the entire std modules tree.
 *
 */
void		vm_load_module( vm_t *vm, char *module );
/*
 * Handle an entire namespace modules loading.
 */
void   		vm_load_namespace( vm_t *vm, string path );
/*
 * Print the calling stack trace.
 */
void 		vm_print_stack_trace( vm_t *vm, bool force = false );
/*
 * Lock the vm pthread mutex.
 */
__force_inline void vm_lock( vm_t *vm ){
	pthread_mutex_lock( &vm->th_mutex );
}
/*
 * Release the pthread mutex.
 */
__force_inline void vm_unlock( vm_t *vm ){
	pthread_mutex_unlock( &vm->th_mutex );
}
/*
 * Add a thread to the threads pool.
 */
__force_inline void vm_pool( vm_t *vm, pthread_t tid = 0 ){
	tid = (tid == 0 ? pthread_self() : tid);
	vm_lock(vm);
		vm->th_pool.push_back(tid);
	vm_unlock(vm);
}
/*
 * Remove a thread from the threads pool.
 */
__force_inline void vm_depool( vm_t *vm, pthread_t tid = 0 ){
	tid = (tid == 0 ? pthread_self() : tid);
	int size( vm->th_pool.size() );
	vm_lock( vm );
	for( int pool_i = 0; pool_i < size; ++pool_i ){
		if( vm->th_pool[pool_i] == tid ){
			vm->th_pool.erase( vm->th_pool.begin() + pool_i );
			break;
		}
	}
	vm_unlock( vm );
}
/*
 * Push a frame to the trace stack.
 */
__force_inline void vm_add_frame( vm_t *vm, vframe_t *frame ){
	vm_lock( vm );
		vm->frames.push_back(frame);
	vm_unlock( vm );
}
/*
 * Remove the last frame from the trace stack.
 */
__force_inline void vm_pop_frame( vm_t *vm ){
	vm_lock( vm );
		vm->frames.pop_back();
	vm_unlock( vm );
}
/*
 * Set the active frame (from threaded calls).
 */
__force_inline void vm_set_frame( vm_t *vm, vframe_t *frame ){
	vm_pop_frame( vm );
	vm_add_frame( vm, frame );
}
/*
 * Return the active frame pointer (last in the list).
 */
__force_inline vframe_t *vm_frame( vm_t *vm ){
	return (vm->frames.size() ? vm->frames.back() : &vm->vmem);
}
/*
 * Compute execution time and print it.
 */
__force_inline void vm_timer( vm_t *vm, int start = 0 ){
    if( vm->args.tm_timer ){
        /* first call */
        if( vm->args.tm_start == 0 && start ){
            vm->args.tm_start = hyb_uticks();
        }
        /* last call */
        else if( !start ){
            vm->args.tm_end = hyb_uticks();
            char buffer[0xFF] = {0};
            hyb_timediff( vm->args.tm_end - vm->args.tm_start, buffer );
            fprintf( stdout, "\033[01;33m[TIME] Elapsed %s .\n\033[00m", buffer );
        }
    }
}
/*
 * Find out if a function has been registered by some previously
 * loaded module and return its pointer.
 * Handle function pointer caching.
 */
__force_inline function_t vm_get_function( vm_t *vm, char *identifier ){
	unsigned int i, j,
				 ndyns( vm->modules.size() ),
				 nfuncs;

	/* first check if it's already in cache */
	named_function_t * cache = vm->mcache.find(identifier);
	if( cache != H_UNDEFINED ){
		return cache->function;
	}

	/* search it in dynamic loaded modules */
	for( i = 0; i < ndyns; ++i ){
		/* for each function of the module */
		nfuncs = vm->modules[i]->functions.size();
		for( j = 0; j < nfuncs; ++j ){
			if( vm->modules[i]->functions[j]->identifier == identifier ){
				// fix issue #0000014
				vm_lock( vm );
					/* found it, add to the cache and return */
					cache = vm->modules[i]->functions[j];
					vm->mcache.insert( identifier, cache );
				vm_unlock( vm );

				return cache->function;
			}
		}
	}

	return H_UNDEFINED;
}
/*
 * Define a structure inside the vtypes member.
 */
__force_inline Object * vm_define_structure( vm_t *vm, char *name, int nattrs, char *attributes[] ){
   StructureObject *type = new StructureObject();
   unsigned int     i;

   for( i = 0; i < nattrs; ++i ){
	   ob_add_attribute( (Object *)type, attributes[i] );
   }
  /*
   * Prevent the structure or class definition from being deleted by the gc.
   */
   return vm->vtypes.addConstant( name, (Object *)type );
}
/*
 * Same as before, but the structure or the class will be defined from an alreay
 * created object.
 */
__force_inline void vm_define_type( vm_t *vm, char *name, Object *type ){
  /*
   * Prevent the structure or class definition from being deleted by the gc.
   */
   vm->vtypes.addConstant( name, type );
}
/*
 * Find the object pointer of a user defined type (i.e. structures or classes).
 */
__force_inline Object * vm_get_type( vm_t *vm, char *name ){
   return vm->vtypes.find(name);
}
/*
 * Compile a regular expression and put it in a global cache.
 */
__force_inline pcre *vm_pcre_compile( vm_t *vm, string& pattern, int opts, const char **perror, int *poffset ){
	pcre *compiled;
	/*
	 * Is it cached ?
	 */
	if( (compiled = vm->pcre_cache.find((char *)pattern.c_str())) == NULL ){
		/*
		 * Not cached, compile it.
		 */
		compiled = ::pcre_compile( pattern.c_str(), opts, perror, poffset, 0 );
		/*
		 * If it's a valid regex, put the compilation result to the cache.
		 */
		if( compiled ){
			vm_lock( vm );
				vm->pcre_cache.insert( (char *)pattern.c_str(), compiled );
			vm_unlock( vm );
		}
	}
	/*
	 * Return compiled regex.
	 */
	return compiled;
}

#endif
