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
#include "hybris.h"

/** generic function pointers **/
Object *struct_traverse( Object *me, int index ){
	Structure *sme   = ob_struct_ucast(me);
	Object    *child = (index >= sme->s_attributes.size() ? NULL : sme->s_attributes.at(index));

	return child;
}

Object *struct_clone( Object *me ){
    Structure *sclone = gc_new_struct(),
              *sme    = ob_struct_ucast(me);
    StructureAttributeIterator ai;

    itree_foreach( Object, ai, sme->s_attributes ){
    	sclone->s_attributes.insert( (char *)(*ai)->label.c_str(),
									 ob_clone( (*ai)->value ) );
    }

    sclone->items = sme->items;

    return (Object *)sclone;
}

size_t struct_get_size( Object *me ){
	return ob_struct_ucast(me)->items;
}

void struct_free( Object *me ){
    Structure *sme = ob_struct_ucast(me);

    sme->s_attributes.clear();
    sme->items = 0;
}

long struct_ivalue( Object *me ){
    return static_cast<long>( ob_struct_ucast(me)->items );
}

double struct_fvalue( Object *me ){
    return static_cast<double>( ob_struct_ucast(me)->items );
}

bool struct_lvalue( Object *me ){
    return static_cast<bool>( ob_struct_ucast(me)->items );
}

string struct_svalue( Object *me ){
    return string( "<struct>" );
}

void struct_print( Object *me, int tabs ){
    Structure *sme = ob_struct_ucast(me);
    StructureAttributeIterator ai;
    int i;

    fprintf( stdout, "struct {\n" );
    itree_foreach( Object, ai, sme->s_attributes ){
    	for( i = 0; i <= tabs; ++i ) fprintf( stdout, "\t" );
    	printf( "%s : ", (*ai)->label.c_str() );
		ob_print( (*ai)->value, tabs + 1 );
		printf( "\n" );
    }
    for( i = 0; i < tabs; ++i ) fprintf( stdout, "\t" );
    fprintf( stdout, "}\n" );
}

/** arithmetic operators **/
Object *struct_assign( Object *me, Object *op ){
    struct_free(me);

    Object *clone = ob_clone(op);

    return (me = clone);
}

/** structure operators **/
void struct_define_attribute( Object *me, char *name, access_t a, bool is_static /*= false*/ ){
	Structure *sme = ob_struct_ucast(me);

	sme->s_attributes.insert( name, H_VOID_VALUE );
	sme->items = sme->s_attributes.size();
}

void struct_add_attribute( Object *me, char *name ){
	struct_define_attribute( me, name, asPublic, false );
}

Object *struct_get_attribute( Object *me, char *name, bool with_descriptor ){
    Structure *sme = ob_struct_ucast(me);
    return sme->s_attributes.find(name);
}

void struct_set_attribute_reference( Object *me, char *name, Object *value ){
    Structure *sme = ob_struct_ucast(me);
    Object          *o;

    o = sme->s_attributes.find(name);
    if( o != NULL ){
    	sme->s_attributes.replace( name, o, ob_assign( o, value ) );
    }
    else{
    	sme->s_attributes.insert( name, value );
    	sme->items = sme->s_attributes.size();
    }
}

void struct_set_attribute( Object *me, char *name, Object *value ){
    return ob_set_attribute_reference( me, name, ob_clone(value) );
}

IMPLEMENT_TYPE(Structure) {
    /** type code **/
    otStructure,
	/** type name **/
    "struct",
	/** type basic size **/
    OB_COLLECTION_SIZE,
    /** type builtin methods **/
    NO_BUILTIN_METHODS,
	/** generic function pointers **/
    0, // type_name
    struct_traverse, // traverse
	struct_clone, // clone
	struct_free, // free
	struct_get_size, // get_size
	0, // serialize
	0, // deserialize
	0, // to_fd
	0, // from_fd
	0, // cmp
	struct_ivalue, // ivalue
	struct_fvalue, // fvalue
	struct_lvalue, // lvalue
	struct_svalue, // svalue
	struct_print, // print
	0, // scanf
	0, // to_string
	0, // to_int
	0, // range
	0, // regexp

	/** arithmetic operators **/
	struct_assign, // assign
    0, // factorial
    0, // increment
    0, // decrement
    0, // minus
    0, // add
    0, // sub
    0, // mul
    0, // div
    0, // mod
    0, // inplace_add
    0, // inplace_sub
    0, // inplace_mul
    0, // inplace_div
    0, // inplace_mod

	/** bitwise operators **/
	0, // bw_and
    0, // bw_or
    0, // bw_not
    0, // bw_xor
    0, // bw_lshift
    0, // bw_rshift
    0, // bw_inplace_and
    0, // bw_inplace_or
    0, // bw_inplace_xor
    0, // bw_inplace_lshift
    0, // bw_inplace_rshift

	/** logic operators **/
    0, // l_not
    0, // l_same
    0, // l_diff
    0, // l_less
    0, // l_greater
    0, // l_less_or_same
    0, // l_greater_or_same
    0, // l_or
    0, // l_and

	/** collection operators **/
	0, // cl_push
	0, // cl_push_reference
	0, // cl_pop
	0, // cl_remove
	0, // cl_at
	0, // cl_set
	0, // cl_set_reference

	/** structure operators **/
	struct_define_attribute, // define_attribute
	0, // attribute_access
	0, // attribute_is_static
	0, // set_attribute_access
    struct_add_attribute, // add_attribute
    struct_get_attribute, // get_attribute
    struct_set_attribute, // set_attribute
    struct_set_attribute_reference, // set_attribute_reference
    0, // define_method
    0, // get_method
    0  // call_method
};

