/* 
 *  A Ruby object wrapper, allowing this module's functions to be called from Ruby.
 */

#include "ruby.h"
#include "accessories.h"

static VALUE t_algorithm(VALUE self)
{
	run(kbd_filename);
	return Qnil;
}

VALUE cTyping;

void Init_rb_wrapper()
{
	cTyping = rb_define_class("Typing", rb_cObject);
	rb_define_method(cTyping, "algorithm", t_algorithm, 0);
}