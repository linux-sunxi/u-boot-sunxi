/*
 * COM1 SW161x support
 * originally from linux source (arch/ppc/boot/ns16550.c)
 * modified to use CFG_ISA_MEM and new defines
 */

#include "standby_i.h"
#include <stdarg.h>
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
void standby_serial_putc(char c)
{
	__u32 reg_val;

	do
	{
		reg_val = *(volatile unsigned int *)(0x01c2807C);
	}	while (!(reg_val & 0x02));

	*(volatile unsigned int *)(0x01c28000) = c;
}
#if 0
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
void standby_int_to_string_dec( int input , char * str)
{
	char stack[16];
	int sign_flag = 0 ;
	int i ;
	int j ;

	if( input == 0 )
	{
		str[0] = '0';
		str[1] = '\0';                   // 'str' must end with '\0'
		return ;
	}

	if( input < 0 )                      // If 'input' is negative, 'input' is assigned to its absolute value.
	{
		sign_flag = -1 ;
		input = -input ;
	}

	for( i = 0; input > 0; ++i )
	{
		stack[i] = input%10 + '0';      // characters in reverse order are put in 'stack' .
		input /= 10;
	}                                   // at the end of 'for' loop, 'i' is the number of characters.

    j = 0;
	if( sign_flag == -1 )
		str[j++] = '-';		            // If 'input' is negative, minus sign '-' is placed in the head.
	for( --i  ; i >= 0; --i, ++j )
		str[j] = stack[i];
	str[j] = '\0';				        // 'str' must end with '\0'


	return;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
void standby_int_to_string_hex( int input, char * str )
{
	int i;
	static char base[] = "0123456789abcdef";

	for( i = 9; i > 1; --i )
	{
		str[i] = base[input&0x0f];
		input >>= 4;
	}

	str[0] = '0';
	str[1] = 'x';                         // Hex format shoud start with "0x" or "0X".
	str[10] = '\0';                       // 'str' must end with '\0'

	return;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
static void standby_serial_puts( const char * str )
{
	while( *str != '\0' )
	{
		if( *str == '\n' )                      // if current character is '\n', insert and output '\r'
			standby_serial_putc( '\r' );

		standby_serial_putc( *str++ );
	}
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
void standby_printf( const char * str, ...)
{
	char string[16];
	char *p;
	va_list argp;

	va_start( argp, str );

	while( *str )
	{
		if( *str == '%' )
		{
			++str;
			p = string;
			switch( *str )
			{
				case 'd': standby_int_to_string_dec( va_arg( argp,  __s32 ), string );
                          standby_serial_puts( p );
						  ++str;
						  break;
				case 'x':
				case 'X': standby_int_to_string_hex( va_arg( argp,  __s32 ), string );
						  standby_serial_puts( p );
                          ++str;
						  break;
				case 'c': standby_serial_putc( va_arg( argp,  __s32 ) );
						  ++str;
						  break;
				case 's': standby_serial_puts( va_arg( argp, char * ) );
						  ++str;
						  break;
				default : standby_serial_putc( '%' );       // if current character is not Conversion Specifiers 'dxpXucs',
						  standby_serial_putc( *str );      // output directly '%' and current character, and then
						  ++str;                                         // let 'str' point to next character.
			}
		}
		else
		{
			if( *str == '\n' )                      // if current character is '\n', insert and output '\r'
				standby_serial_putc( '\r' );

			standby_serial_putc( *str++ );
		}
	}

	va_end( argp );

	return;
}

#endif

