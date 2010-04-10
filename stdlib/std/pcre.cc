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
#include <hybris.h>
#include <pcrecpp.h>

HYBRIS_DEFINE_FUNCTION(hrex_match);
HYBRIS_DEFINE_FUNCTION(hrex_matches);
HYBRIS_DEFINE_FUNCTION(hrex_replace);

extern "C" named_function_t hybris_module_functions[] = {
    { "rex_match", hrex_match },
    { "rex_matches", hrex_matches },
    { "rex_replace", hrex_replace },
    { "", NULL }
};

HYBRIS_DEFINE_FUNCTION(hrex_match){
	if( HYB_ARGC() != 2 ){
		hyb_throw( H_ET_SYNTAX, "function 'rex_match' requires 2 parameters (called with %d)", HYB_ARGC() );
	}
	HYB_TYPE_ASSERT( HYB_ARGV(0), H_OT_STRING );
	HYB_TYPE_ASSERT( HYB_ARGV(1), H_OT_STRING );

	string rawreg  = HYB_ARGV(0)->value.m_string,
		   subject = HYB_ARGV(1)->value.m_string,
		   regex;
	int    opts;

	Object::parse_pcre( rawreg, regex, opts );

	pcrecpp::RE_Options OPTS(opts);
	pcrecpp::RE         REGEX( regex.c_str(), OPTS );

	return new Object( (long)REGEX.PartialMatch(subject.c_str()) );
}

HYBRIS_DEFINE_FUNCTION(hrex_matches){
	if( HYB_ARGC() != 2 ){
		hyb_throw( H_ET_SYNTAX, "function 'rex_matches' requires 2 parameters (called with %d)", HYB_ARGC() );
	}
	HYB_TYPE_ASSERT( HYB_ARGV(0), H_OT_STRING );
	HYB_TYPE_ASSERT( HYB_ARGV(1), H_OT_STRING );

	string rawreg  = HYB_ARGV(0)->value.m_string,
		   subject = HYB_ARGV(1)->value.m_string,
		   regex;
	int    opts, i = 0;

	Object::parse_pcre( rawreg, regex, opts );

	pcrecpp::RE_Options  OPTS(opts);
	pcrecpp::RE          REGEX( regex.c_str(), OPTS );
	pcrecpp::StringPiece SUBJECT( subject.c_str() );
	string   match;
	Object  *matches = new Object();

    while( REGEX.FindAndConsume( &SUBJECT, &match ) == true ){
		if( i++ > H_PCRE_MAX_MATCHES ){
			hyb_throw( H_ET_GENERIC, "something of your regex is forcing infinite matches" );
		}
		matches->push( new Object((char *)match.c_str()) );
	}

	return matches;
}

HYBRIS_DEFINE_FUNCTION(hrex_replace){
	if( HYB_ARGC() != 3 ){
		hyb_throw( H_ET_SYNTAX, "function 'rex_replace' requires 2 parameters (called with %d)", HYB_ARGC() );
	}
	HYB_TYPE_ASSERT( HYB_ARGV(0), H_OT_STRING );
	HYB_TYPE_ASSERT( HYB_ARGV(1), H_OT_STRING );
	HYB_TYPES_ASSERT( HYB_ARGV(2), H_OT_STRING, H_OT_CHAR );

	string rawreg  = HYB_ARGV(0)->value.m_string,
		   subject = HYB_ARGV(1)->value.m_string,
		   replace = (HYB_ARGV(2)->type == H_OT_STRING ? HYB_ARGV(2)->value.m_string : string("") + HYB_ARGV(2)->value.m_char),
		   regex;
	int    opts;

	Object::parse_pcre( rawreg, regex, opts );

	pcrecpp::RE_Options OPTS(opts);
	pcrecpp::RE         REGEX( regex.c_str(), OPTS );

	REGEX.GlobalReplace( replace.c_str(), &subject );

	return new Object( (char *)subject.c_str() );
}