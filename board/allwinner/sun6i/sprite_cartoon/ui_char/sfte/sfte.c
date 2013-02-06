/*
************************************************************************************************************************
*                                                       SFTE 2.0
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name   : sfte.c
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.11.23
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2009.11.23       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __sfte_c
#define  __sfte_c

#include "egon2.h"
#include "sfte.h"
#include "SFT.h"
#include <string.h>

#undef  _4_char_to_int32
#define _4_char_to_int32(a,b,c,d)       ( (a)<<24 | (b)<<16 | (c)<<8 | (d) )

#define SFTE_LABEL            _4_char_to_int32( 'S', 'F', 'T', 'E' )
#define INVALID_LABEL         _4_char_to_int32( 'X', 'X', 'X', 'X' )

#define INVALID_PIXEL_SIZE    0
#define INVALID_UNICODE       ((__u16)0xFFFF)


typedef struct SFTE_Bitmap_Size_
{
	size_info_t                  size_info;
	struct  SFTE_Bitmap_Size_   *next;
}SFTE_Bitmap_Size;


typedef struct _sfte_prvt_data_t
{
    __s32         label;                          // handle valid label
	void         *fp;                             // file pointer to the sft file
    size_info_t  *size_info_list;                 // list of all available pixel sizes
	size_info_t   cur_pixel_size;                 // current pixel size
	__u32         bitmap_buf_size;                // the size of the bitmap buffer
	__u32         unicode;                        // a cache for the current char
#ifdef SFT_USE_MUTEX
	g_mutex_t     mutex;                          // a mutex
#endif // #ifdef SFT_USE_MUTEX
}sfte_prvt_data_t;


static __s32  init_face( SFTE_Face face, void *fp );
static __Bool is_valid_face( SFTE_Face face );
static void   release_hdl_res( SFTE_Face face );
static __s32  SFTE_Get_Glyph_i( SFTE_Face face, __u16 unicode );
static __u32  SFTE_Get_XAdvance_i( SFTE_Face face, __u16 unicode );
static __s32  SFTE_Set_Pixel_Sizes_i( SFTE_Face face, __u32 pixel_size );
static __s32  SFTE_Get_Pixel_Size_Count_i( SFTE_Face face );
static __s32  SFTE_Get_Pixel_Size_List_i( SFTE_Face face, __u32 *list_p, __u32 count );


SFTE_Face SFTE_New_Face( const char *font_file )
{
	SFTE_Face   face = NULL;
	void       *fp   = NULL;

	/* open sft file */
	fp = wBoot_fopen( font_file, "rb+" );
	if( fp == NULL )
	{
		__wrn("Error in opening font file %s\n", font_file);
		return NULL;
	}

	face = (struct SFTE_FaceRec_ *) wBoot_malloc( sizeof(struct SFTE_FaceRec_) );
	if( face == NULL )
	{
		__wrn("Error in allocating memory\n");
		goto error;
	}
	memset( face, 0, sizeof(struct SFTE_FaceRec_) );

	if( init_face( face, fp ) != 0 )
	{
		__wrn("Error in initialising face\n");
		goto error;
	}

	return face;

error:
	if( fp != NULL )
		wBoot_fclose( fp );
	if( face != NULL )
		wBoot_free( face );

	return NULL;
}



__s32  SFTE_Done_Face( SFTE_Face face )
{
	if( !is_valid_face( face ) )
	{
		__wrn("Fail in destroying face. Because face is invalid\n");
		return -1;
	}

	release_hdl_res( face );

	wBoot_free( face );

	return 0;
}




__s32  SFTE_Set_Pixel_Sizes( SFTE_Face face, __u32 pixel_size )
{
	__s32              ret;

	if( !is_valid_face( face ) )
	{
		__wrn("Fail in setting pixel size %x. Because face is invalid\n", pixel_size);
		return 0;
	}

	ret = SFTE_Set_Pixel_Sizes_i( face, pixel_size );

	return ret;
}



__s32  SFTE_Get_Pixel_Sizes( SFTE_Face face, __u32 pixel_size )
{
	if( !is_valid_face( face ) )
	{
		__wrn("Fail in setting pixel size %x. Because face is invalid\n", pixel_size);
		return 0;
	}

	return SFTE_Get_Pixel_Size_Count_i( face );
}



__s32  SFTE_Get_Pixel_Size_List( SFTE_Face face, __u32 *list_p, __u32 count )
{
	if( !is_valid_face( face ) || list_p == NULL )
	{
		return 0;
	}

	return SFTE_Get_Pixel_Size_List_i( face, list_p, count );
}



__s32  SFTE_Get_Pixel_Size_Count( SFTE_Face face )
{
	__s32              ret;

	if( !is_valid_face( face ) )
	{
		return 0;
	}

	ret = SFTE_Get_Pixel_Size_Count_i( face );

	return ret;
}



__s32  SFTE_Get_Glyph( SFTE_Face face, __u16 unicode )
{
	__u32              ret;

	if( !is_valid_face( face ) )
	{
		__wrn("Fail in getting glyph of unicode %x. Because face is invalid\n", unicode);
		return 0;
	}

	ret = SFTE_Get_Glyph_i( face, unicode );

	return ret;
}



__u32  SFTE_Get_XAdvance( SFTE_Face face, __u16 unicode )
{
	__u32              ret;

	if( !is_valid_face( face ) )
	{
		__wrn("Fail in getting x advance of unicode %x. Because face is invalid\n", unicode);
		return 0;
	}

	ret = SFTE_Get_XAdvance_i( face, unicode );

	return ret;
}



static __s32  SFTE_Get_Glyph_i( SFTE_Face face, __u16 unicode )
{
	void              *fp;
	glyph_t            glyph;
	__u32              pitch_abs;
	__u32              bitmap_size;
	sfte_prvt_data_t  *pd;
	size_info_t       *p;
	__u32              offset;

	pd = (sfte_prvt_data_t *)face->hidden;

	if( pd->cur_pixel_size.pixel_size == INVALID_PIXEL_SIZE )
	{
		__wrn("Fail in getting glyph of unicode %x. Because pixel size is NOT valid.\n", unicode);
		return -1;
	}

	/* check whether the input unicode is the current unicode */
	if( pd->unicode != INVALID_UNICODE && pd->unicode == unicode )
	{
		return 0;
	}

	fp = pd->fp;
	p  = &(pd->cur_pixel_size);

	/* get glyph's offset */
	wBoot_fseek( fp, p->glyph_index_table_offset + unicode * sizeof(__u32), SEEK_SET );
	wBoot_fread( &offset, 1, sizeof(__u32), fp );
	if( offset == 0 )                   // the char doesn't been contained.
	{
		return -1;
	}

	/* get glyph's information */
	wBoot_fseek( fp, offset, SEEK_SET );
	wBoot_fread( &glyph, 1, sizeof(glyph), fp );
	face->glyph->advance.x    = glyph.advance.x  ;
	face->glyph->advance.y    = glyph.advance.y  ;
	face->glyph->bitmap_left  = glyph.bitmap_left;
	face->glyph->bitmap_top   = glyph.bitmap_top ;
	face->glyph->bitmap.rows  = glyph.rows;
	face->glyph->bitmap.width = glyph.width;
	face->glyph->bitmap.pitch = glyph.pitch;
	switch( glyph.pixel_mode )
	{
		case SFT_PIXEL_MODE_MONO:
			face->glyph->bitmap.pixel_mode = SFTE_PIXEL_MODE_MONO;
			break;
		case SFT_PIXEL_MODE_GRAY:
			face->glyph->bitmap.pixel_mode = SFTE_PIXEL_MODE_GRAY;
			break;
		default :
			__wrn("Error. Inlegal pixel mode %u\n", glyph.pixel_mode);
			return -1;
	}

	/* get bitmap matrix */
	if( glyph.pitch >0 )
		pitch_abs = glyph.pitch;
	else
		pitch_abs = (__u32)-glyph.pitch;
	bitmap_size = glyph.rows * pitch_abs;
	if( bitmap_size != 0 )
	{
		if(    face->glyph->bitmap.buffer != NULL
			&& pd->bitmap_buf_size < bitmap_size )
		{
			__wrn("current bitmap buffer size is %u and new bitmap size is %u.\n"
		    	  "pitch abs is %u and glyph rows is %u.\n", pd->bitmap_buf_size, bitmap_size, pitch_abs, glyph.rows);
			wBoot_free( face->glyph->bitmap.buffer );
			face->glyph->bitmap.buffer = NULL;
			pd->bitmap_buf_size        = 0;
		}
		if( face->glyph->bitmap.buffer == NULL )
		{
			face->glyph->bitmap.buffer = (__u8 *) wBoot_malloc( bitmap_size );
			if( face->glyph->bitmap.buffer == NULL )
			{
				__wrn("Error in getting glyph of unicode %x. can't allocate memory\n", unicode);
				return -1;
			}
			pd->bitmap_buf_size = bitmap_size;
		}
		wBoot_fread( face->glyph->bitmap.buffer, 1, bitmap_size, fp );
	}

	pd->unicode = unicode;

	return 0;
}



static __u32  SFTE_Get_XAdvance_i( SFTE_Face face, __u16 unicode )
{
	void              *fp;
	sfte_prvt_data_t  *pd;
	size_info_t       *p;
	__u8               xadvance;

	pd = (sfte_prvt_data_t *)face->hidden;

	if( pd->cur_pixel_size.pixel_size == INVALID_PIXEL_SIZE )
	{
		__wrn("Fail in getting glyph of unicode %x. Because pixel size is NOT valid.\n", unicode);
		return 0;
	}

	/* check whether the input unicode is the current unicode */
	if( pd->unicode != INVALID_UNICODE && pd->unicode == unicode )
	{
		return face->glyph->advance.x >> 6;
	}

	fp = pd->fp;
	p  = &(pd->cur_pixel_size);

	/* get glyph's xadvance */
	wBoot_fseek( fp, p->glyph_xadvance_table_offset + unicode * sizeof(__u8), SEEK_SET );
	wBoot_fread( &xadvance, 1, sizeof(__u8), fp );
	if( xadvance == 0 )                   // the char doesn't been contained.
	{
		return 0;
	}

	return xadvance;
}



static __s32  SFTE_Set_Pixel_Sizes_i( SFTE_Face face, __u32 pixel_size )
{
	size_info_t        *p;
	__u32               border_size;
	__u32               buf_size;
	sfte_prvt_data_t   *pd;
	__u32               i;

	pd = (sfte_prvt_data_t *)face->hidden;

	/* check whether the pixel size is supported by the sft file */
	for( i = 0;  i < face->num_fixed_sizes;  i++ )
	{
		p = pd->size_info_list + i;
		if( p->pixel_size == pixel_size )
		{
			if( pd->cur_pixel_size.pixel_size == pixel_size )    // is the current size
			{
				return 0;
			}

			/* allocate bitmap buffer */
			border_size = pixel_size + ( pixel_size >> 1 );
			buf_size    = border_size * border_size;
			if(    face->glyph->bitmap.buffer != NULL
				&& pd->bitmap_buf_size < buf_size )
			{
				wBoot_free( face->glyph->bitmap.buffer );
				face->glyph->bitmap.buffer = NULL;
				pd->bitmap_buf_size        = 0;
			}
			if( face->glyph->bitmap.buffer == NULL )
			{
				face->glyph->bitmap.buffer = (__u8 *) wBoot_malloc( buf_size );
				if( face->glyph->bitmap.buffer == NULL )
				{
					__wrn("Error in setting pixel size. can't allocate memory\n");
					return -1;
				}
				pd->bitmap_buf_size = buf_size;
			}
			/* fill size information */
			face->size->metrics.ascender      = p->ascender;
			face->size->metrics.descender     = p->descender;
			face->glyph->metrics.height       = p->height       ;
	        face->glyph->metrics.horiBearingY = p->horiBearingY ;
	        memcpy( &(pd->cur_pixel_size), p, sizeof(size_info_t) );
	        pd->unicode = INVALID_UNICODE;

	        return 0;
		}
	}

	__wrn("Fail in setting pixel size. the pixel size %u is not supported by the sft file.\n", pixel_size);
	return -1;
}



static __s32  SFTE_Get_Pixel_Size_Count_i( SFTE_Face face )
{
	return face->num_fixed_sizes;
}



static __s32  SFTE_Get_Pixel_Size_List_i( SFTE_Face face, __u32 *list_p, __u32 count )
{
	size_info_t        *p;
	sfte_prvt_data_t   *pd;
	__u32               i;
	__u32               nr;

	pd = (sfte_prvt_data_t *)face->hidden;

	/* get size list */
	nr = min( count, face->num_fixed_sizes );
	for( i = 0;  i < nr;  i++ )
	{
		p = pd->size_info_list + i;
		list_p[i] = p->pixel_size;
    }

    return nr;
}



static __Bool is_valid_face( SFTE_Face face )
{
	sfte_prvt_data_t  *pd = NULL;

	if( face == NULL )
		return 0;

	pd = (sfte_prvt_data_t *)face->hidden;
	if( pd->label != SFTE_LABEL )
		return 0;

	return 1;
}



static __s32 init_face( SFTE_Face face, void *fp )
{
	//size_info_t                *size_info_list = NULL;
	sfte_prvt_data_t           *pd = NULL;
	sft_file_head_t             h;

	wBoot_fread( &h, 1, sizeof(h), fp );
	face->num_fixed_sizes = h.pixel_size_nr;
	face->num_glyphs      = h.char_nr;
	face->units_per_EM    = h.units_per_EM;
	face->ascender        = h.ascender;
	face->descender       = h.descender;
	face->height          = h.height;
	face->size = (SFTE_SizeRec *) wBoot_malloc( sizeof(SFTE_SizeRec) );
	if( face->size == NULL )
	{
		__wrn("Error in allocating memory\n");
		return -1;
	}

	/* allcate a buffer to store glyph */
	face->glyph = (struct SFTE_GlyphSlotRec_ *) wBoot_malloc( sizeof(struct SFTE_GlyphSlotRec_) );
	if( face->glyph == NULL )
	{
		__wrn("Error in allocating memory\n");
		goto error;
	}
	memset( face->glyph, 0, sizeof(struct SFTE_GlyphSlotRec_) );

	face->hidden = (void *) wBoot_malloc( sizeof(sfte_prvt_data_t) );
	if( face->hidden == NULL )
	{
		__msg("Error in allocating memory\n");
		return -1;
	}
	memset( face->hidden, 0, sizeof(sfte_prvt_data_t) );
	pd = (sfte_prvt_data_t *)face->hidden;

	/* get the list of all the pixel sizes */
	pd->size_info_list = (size_info_t *) wBoot_malloc( sizeof(size_info_t) * face->num_fixed_sizes );
	if( pd->size_info_list == NULL )
	{
		__wrn("Error in allocating memory\n");
		goto error;
	}
	wBoot_fseek( fp, h.pixel_size_tbl_offset, SEEK_SET );
	wBoot_fread( pd->size_info_list, 1, sizeof(size_info_t) * face->num_fixed_sizes, fp );

	pd->label            = SFTE_LABEL;
	pd->fp               = fp;
	pd->cur_pixel_size.pixel_size = INVALID_PIXEL_SIZE;
	pd->bitmap_buf_size  = 0;
	pd->unicode          = INVALID_UNICODE;

#ifdef SFT_USE_MUTEX
	pd->mutex = g_create_mutex( );
	if( pd->mutex == NULL )
	{
		__wrn("Error in creating a mutex.\n");
		goto error;
	}
#endif // #ifdef SFT_USE_MUTEX

	return 0;

error:
	release_hdl_res( face );
	return -1;
}



static void   release_hdl_res( SFTE_Face face )
{
	if( face->hidden != NULL )
	{
		sfte_prvt_data_t  *pd = NULL;

		pd = (sfte_prvt_data_t *)face->hidden;
		if( pd->size_info_list != NULL )
		{
			wBoot_free( pd->size_info_list );
			pd->size_info_list = NULL;
		}
		if( pd->fp != NULL )
		{
			wBoot_fclose( pd->fp );
			pd->fp = NULL;
		}
#ifdef SFT_USE_MUTEX
		if( pd->mutex != NULL )
		{
			__u8  err;

			g_delete_mutex( pd->mutex, &err );
			pd->mutex = NULL;
		}
#endif // #ifdef SFT_USE_MUTEX
		pd->label = INVALID_LABEL;
		wBoot_free( face->hidden );
		face->hidden = NULL;
	}
	if( face->glyph != NULL )
	{
		if( face->glyph->bitmap.buffer != NULL )
		{
			wBoot_free( face->glyph->bitmap.buffer );
			face->glyph->bitmap.buffer = NULL;
		}
		wBoot_free( face->glyph );
		face->glyph = NULL;
	}
	if( face->size != NULL )
	{
		wBoot_free( face->size );
		face->size = NULL;
	}
}



#endif     //  ifndef __sfte_c

/* end of sfte.c */
