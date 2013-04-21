/*
 * include/linker_lists.h
 *
 * Implementation of linker-generated arrays
 *
 * Copyright (C) 2012 Marek Vasut <marex@denx.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

/*
 * There is no use in including this from ASM files, but that happens
 * anyway, e.g. PPC kgdb.S includes command.h which incluse us.
 * So just don't define anything when included from ASM.
 */

#if !defined(__ASSEMBLY__)

/**
 * A linker list is constructed by grouping together linker input
 * sections, each containning one entry of the list. Each input section
 * contains a constant initialized variable which holds the entry's
 * content. Linker list input sections are constructed from the list
 * and entry names, plus a prefix which allows grouping all lists
 * together. Assuming _list and _entry are the list and entry names,
 * then the corresponding input section name is
 *
 *   _u_boot_list + _2_ + @_list + _2_ + @_entry
 *
 * and the C variable name is
 *
 *   .u_boot_list_ + 2_ + @_list + _2_ + @_entry
 *
 * This ensures uniqueness for both input section and C variable name.
 *
 * Note that the names differ only in the first character, "." for the
 * setion and "_" for the variable, so that the linker cannot confuse
 * section and symbol names. From now on, both names will be referred
 * to as
 *
 *   %u_boot_list_ + 2_ + @_list + _2_ + @_entry
 *
 * Entry variables need never be referred to directly.
 *
 * The naming scheme for input sections allows grouping all linker lists
 * into a single linker output section and grouping all entries for a
 * single list.
 *
 * Note the two '_2_' constant components in the names: their presence
 * allows putting a start and end symbols around a list, by mapping
 * these symbols to sections names with components "1" (before) and
 * "3" (after) instead of "2" (within).
 * Start and end symbols for a list can generally be defined as
 *
 *   %u_boot_list_2_ + @_list + _1_...
 *   %u_boot_list_2_ + @_list + _3_...
 *
 * Start and end symbols for the whole of the linker lists area can be
 * defined as
 *
 *   %u_boot_list_1_...
 *   %u_boot_list_3_...
 *
 * Here is an example of the sorted sections which result from a list
 * "array" made up of three entries : "first", "second" and "third",
 * iterated at least once.
 *
 *   .u_boot_list_2_array_1
 *   .u_boot_list_2_array_2_first
 *   .u_boot_list_2_array_2_second
 *   .u_boot_list_2_array_2_third
 *   .u_boot_list_2_array_3
 *
 * If lists must be divided into sublists (e.g. for iterating only on
 * part of a list), one can simply give the list a name of the form
 * 'outer_2_inner', where 'outer' is the global list name and 'inner'
 * is the sub-list name. Iterators for the whole list should use the
 * global list name ("outer"); iterators for only a sub-list should use
 * the full sub-list name ("outer_2_inner").
 *
 *  Here is an example of the sections generated from a global list
 * named "drivers", two sub-lists named "i2c" and "pci", and iterators
 * defined for the whole list and each sub-list:
 *
 *   %u_boot_list_2_drivers_1
 *   %u_boot_list_2_drivers_2_i2c_1
 *   %u_boot_list_2_drivers_2_i2c_2_first
 *   %u_boot_list_2_drivers_2_i2c_2_first
 *   %u_boot_list_2_drivers_2_i2c_2_second
 *   %u_boot_list_2_drivers_2_i2c_2_third
 *   %u_boot_list_2_drivers_2_i2c_3
 *   %u_boot_list_2_drivers_2_pci_1
 *   %u_boot_list_2_drivers_2_pci_2_first
 *   %u_boot_list_2_drivers_2_pci_2_second
 *   %u_boot_list_2_drivers_2_pci_2_third
 *   %u_boot_list_2_drivers_2_pci_3
 *   %u_boot_list_2_drivers_3
 */

#ifndef __LINKER_LISTS_H__
#define __LINKER_LISTS_H__

/**
 * ll_entry_declare() - Declare linker-generated array entry
 * @_type:	Data type of the entry
 * @_name:	Name of the entry
 * @_list:	name of the list. Should contain only characters allowed
 *		in a C variable name!
 *
 * This macro declares a variable that is placed into a linker-generated
 * array. This is a basic building block for more advanced use of linker-
 * generated arrays. The user is expected to build their own macro wrapper
 * around this one.
 *
 * A variable declared using this macro must be compile-time initialized.
 *
 * Special precaution must be made when using this macro:
 *
 * 1) The _type must not contain the "static" keyword, otherwise the
 *    entry is generated and can be iterated but is listed in the map
 *    file and cannot be retrieved by name.
 *
 * 2) In case a section is declared that contains some array elements AND
 *    a subsection of this section is declared and contains some elements,
 *    it is imperative that the elements are of the same type.
 *
 * 4) In case an outer section is declared that contains some array elements
 *    AND an inner subsection of this section is declared and contains some
 *    elements, then when traversing the outer section, even the elements of
 *    the inner sections are present in the array.
 *
 * Example:
 * ll_entry_declare(struct my_sub_cmd, my_sub_cmd, cmd_sub, cmd.sub) = {
 *         .x = 3,
 *         .y = 4,
 * };
 */
#define ll_entry_declare(_type, _name, _list)				\
	_type _u_boot_list_2_##_list##_2_##_name __aligned(4)		\
			__attribute__((unused,				\
			section(".u_boot_list_2_"#_list"_2_"#_name)))

/**
 * We need a 0-byte-size type for iterator symbols, and the compiler
 * does not allow defining objects of C type 'void'. Using an empty
 * struct is allowed by the compiler, but causes gcc versions 4.4 and
 * below to complain about aliasing. Therefore we use the next best
 * thing: zero-sized arrays, which are both 0-byte-size and exempt from
 * aliasing warnings.
 */

/**
 * ll_entry_start() - Point to first entry of linker-generated array
 * @_type:	Data type of the entry
 * @_list:	Name of the list in which this entry is placed
 *
 * This function returns (_type *) pointer to the very first entry of a
 * linker-generated array placed into subsection of .u_boot_list section
 * specified by _list argument.
 *
 * Since this macro defines an array start symbol, its leftmost index
 * must be 2 and its rightmost index must be 1.
 *
 * Example:
 * struct my_sub_cmd *msc = ll_entry_start(struct my_sub_cmd, cmd_sub);
 */
#define ll_entry_start(_type, _list)					\
({									\
	static char start[0] __aligned(4) __attribute__((unused,	\
		section(".u_boot_list_2_"#_list"_1")));			\
	(_type *)&start;						\
})

/**
 * ll_entry_end() - Point after last entry of linker-generated array
 * @_type:	Data type of the entry
 * @_list:	Name of the list in which this entry is placed
 *		(with underscores instead of dots)
 *
 * This function returns (_type *) pointer after the very last entry of
 * a linker-generated array placed into subsection of .u_boot_list
 * section specified by _list argument.
 *
 * Since this macro defines an array end symbol, its leftmost index
 * must be 2 and its rightmost index must be 3.
 *
 * Example:
 * struct my_sub_cmd *msc = ll_entry_end(struct my_sub_cmd, cmd_sub);
 */
#define ll_entry_end(_type, _list)					\
({									\
	static char end[0] __aligned(4) __attribute__((unused,	\
		section(".u_boot_list_2_"#_list"_3")));			\
	(_type *)&end;							\
})
/**
 * ll_entry_count() - Return the number of elements in linker-generated array
 * @_type:	Data type of the entry
 * @_list:	Name of the list of which the number of elements is computed
 *
 * This function returns the number of elements of a linker-generated array
 * placed into subsection of .u_boot_list section specified by _list
 * argument. The result is of an unsigned int type.
 *
 * Example:
 * int i;
 * const unsigned int count = ll_entry_count(struct my_sub_cmd, cmd_sub);
 * struct my_sub_cmd *msc = ll_entry_start(struct my_sub_cmd, cmd_sub);
 * for (i = 0; i < count; i++, msc++)
 *         printf("Entry %i, x=%i y=%i\n", i, msc->x, msc->y);
 */
#define ll_entry_count(_type, _list)					\
	({								\
		_type *start = ll_entry_start(_type, _list);		\
		_type *end = ll_entry_end(_type, _list);		\
		unsigned int _ll_result = end - start;			\
		_ll_result;						\
	})

/**
 * ll_entry_get() - Retrieve entry from linker-generated array by name
 * @_type:	Data type of the entry
 * @_name:	Name of the entry
 * @_list:	Name of the list in which this entry is placed
 *
 * This function returns a pointer to a particular entry in LG-array
 * identified by the subsection of u_boot_list where the entry resides
 * and it's name.
 *
 * Example:
 * ll_entry_declare(struct my_sub_cmd, my_sub_cmd, cmd_sub, cmd.sub) = {
 *         .x = 3,
 *         .y = 4,
 * };
 * ...
 * struct my_sub_cmd *c = ll_entry_get(struct my_sub_cmd, my_sub_cmd, cmd_sub);
 */
#define ll_entry_get(_type, _name, _list)				\
	({								\
		extern _type _u_boot_list_2_##_list##_2_##_name;	\
		_type *_ll_result =					\
			&_u_boot_list_2_##_list##_2_##_name;	\
		_ll_result;						\
	})

/**
 * ll_start() - Point to first entry of first linker-generated array
 * @_type:	Data type of the entry
 *
 * This function returns (_type *) pointer to the very first entry of
 * the very first linker-generated array.
 *
 * Since this macro defines the start of the linker-generated arrays,
 * its leftmost index must be 1.
 *
 * Example:
 * struct my_sub_cmd *msc = ll_start(struct my_sub_cmd);
 */
#define ll_start(_type)							\
({									\
	static char start[0] __aligned(4) __attribute__((unused,	\
		section(".u_boot_list_1")));				\
	(_type *)&start;						\
})

/**
 * ll_entry_end() - Point after last entry of last linker-generated array
 * @_type:	Data type of the entry
 *
 * This function returns (_type *) pointer after the very last entry of
 * the very last linker-generated array.
 *
 * Since this macro defines the end of the linker-generated arrays,
 * its leftmost index must be 3.
 *
 * Example:
 * struct my_sub_cmd *msc = ll_end(struct my_sub_cmd);
 */
#define ll_end(_type)							\
({									\
	static char end[0] __aligned(4) __attribute__((unused,	\
		section(".u_boot_list_3")));				\
	(_type *)&end;							\
})

#endif /* __ASSEMBLY__ */

#endif	/* __LINKER_LISTS_H__ */
