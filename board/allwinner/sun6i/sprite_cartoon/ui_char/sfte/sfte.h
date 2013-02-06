/*
************************************************************************************************************************
*                                                       SFTE 2.0
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name   : sfte.h
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
#ifndef  __sfte_h
#define  __sfte_h



  /*************************************************************************/
  /*                                                                       */
  /* <Enum>                                                                */
  /*    SFTE_pixel_mode_e                                                  */
  /*                                                                       */
  /* <Description>                                                         */
  /*    An enumeration type used to describe the format of pixels in a     */
  /*    given bitmap.                                                      */
  /*                                                                       */
  /* <Values>                                                              */
  /*    SFTE_PIXEL_MODE_MONO ::                                            */
  /*      A monochrome bitmap, using 1 bit per pixel.  Note that pixels    */
  /*      are stored in most-significant order (MSB), which means that     */
  /*      the left-most pixel in a byte has value 128.                     */
  /*                                                                       */
  /*    SFTE_PIXEL_MODE_GRAY ::                                            */
  /*      An 8-bit bitmap, generally used to represent anti-aliased glyph  */
  /*      images.  Each pixel is stored in one byte.                       */
  /*                                                                       */
  /*************************************************************************/

/* pixel mode */

typedef enum _SFTE_pixel_mode_e
{
	SFTE_PIXEL_MODE_MONO,
    SFTE_PIXEL_MODE_GRAY,
}SFTE_pixel_mode_e;


typedef struct  SFTE_Vector_
{
  __u32  x;
  __u32  y;
} SFTE_Vector;



  /*************************************************************************/
  /*                                                                       */
  /* <Struct>                                                              */
  /*    SFTE_Bitmap                                                        */
  /*                                                                       */
  /* <Description>                                                         */
  /*    A structure used to describe a bitmap or pixmap to the raster.     */
  /*    Note that we now manage pixmaps of various depths through the      */
  /*    `pixel_mode' field.                                                */
  /*                                                                       */
  /* <Fields>                                                              */
  /*    rows         :: The number of bitmap rows.                         */
  /*                                                                       */
  /*    width        :: The number of pixels in bitmap row.                */
  /*                                                                       */
  /*    pitch        :: The pitch's absolute value is the number of bytes  */
  /*                    taken by one bitmap row, including padding.        */
  /*                    However, the pitch is positive when the bitmap has */
  /*                    a `down' flow, and negative when it has an `up'    */
  /*                    flow.  In all cases, the pitch is an offset to add */
  /*                    to a bitmap pointer in order to go down one row.   */
  /*                                                                       */
  /*    buffer       :: A typeless pointer to the bitmap buffer.  This     */
  /*                    value should be aligned on 32-bit boundaries in    */
  /*                    most cases.                                        */
  /*                                                                       */
  /*                                                                       */
  typedef struct  SFTE_Bitmap_
  {
    __u32            rows;
    __u32            width;
    __s32            pitch;
    __s32            pixel_mode;
    __u8            *buffer;
  } SFTE_Bitmap;


  /*************************************************************************/
  /*                                                                       */
  /* <Struct>                                                              */
  /*    SFTE_Size_Metrics                                                  */
  /*                                                                       */
  /* <Description>                                                         */
  /*    The size metrics structure gives the metrics of a size object.     */
  /*                                                                       */
  /* <Fields>                                                              */
  /*    ascender     :: The ascender in 26.6 fractional pixels.  See       */
  /*                    @SFTE_FaceRec for the details.                     */
  /*                                                                       */
  /*    descender    :: The descender in 26.6 fractional pixels.  See      */
  /*                    @SFTE_FaceRec for the details.                     */
  /*                                                                       */
  typedef struct  SFTE_Size_Metrics_
  {
    __u32     ascender;    /* ascender in 26.6 frac. pixels               */
    __u32     descender;   /* descender in 26.6 frac. pixels              */
  } SFTE_Size_Metrics;


  /*************************************************************************/
  /*                                                                       */
  /* <Struct>                                                              */
  /*    SFTE_SizeRec                                                       */
  /*                                                                       */
  /* <Description>                                                         */
  /*    FreeType root size class structure.  A size object models a face   */
  /*    object at a given size.                                            */
  /*                                                                       */
  /* <Fields>                                                              */
  /*    metrics :: Metrics for this size object.  This field is read-only. */
  /*                                                                       */
  typedef struct  SFTE_SizeRec_
  {
    SFTE_Size_Metrics   metrics;   /* size metrics                         */
  } SFTE_SizeRec, *SFTE_Size;


  /*************************************************************************/
  /*                                                                       */
  /* <Struct>                                                              */
  /*    SFTE_Glyph_Metrics                                                 */
  /*                                                                       */
  /* <Description>                                                         */
  /*    A structure used to model the metrics of a single glyph.  The      */
  /*    values are expressed in 26.6 fractional pixel format; if the flag  */
  /*    @SFTE_LOAD_NO_SCALE has been used while loading the glyph, values  */
  /*    are expressed in font units instead.                               */
  /*                                                                       */
  /* <Fields>                                                              */
  /*    height ::                                                          */
  /*      The glyph's height.                                              */
  /*                                                                       */
  /*    horiBearingY ::                                                    */
  /*      Top side bearing for horizontal layout.                          */
  /*                                                                       */
  typedef struct  SFTE_Glyph_Metrics_
  {
     __u32  height;
     __u32  horiBearingY;
  } SFTE_Glyph_Metrics;


  /*************************************************************************/
  /*                                                                       */
  /* <Struct>                                                              */
  /*    SFTE_GlyphSlotRec                                                  */
  /*                                                                       */
  /* <Description>                                                         */
  /*    FreeType root glyph slot class structure.  A glyph slot is a       */
  /*    container where individual glyphs can be loaded, be they in        */
  /*    outline or bitmap format.                                          */
  /*                                                                       */
  /* <Fields>                                                              */
  /*    metrics           :: The metrics of the last loaded glyph in the   */
  /*                         slot.  The returned values depend on the last */
  /*                         load flags (see the @SFTE_Load_Glyph API      */
  /*                         function) and can be expressed either in 26.6 */
  /*                         fractional pixels or font units.              */
  /*                                                                       */
  /*                         Note that even when the glyph image is        */
  /*                         transformed, the metrics are not.             */
  /*                                                                       */
  /*    advance           :: This is the transformed advance width for the */
  /*                         glyph.                                        */
  /*                                                                       */
  /*    bitmap            :: This field is used as a bitmap descriptor     */
  /*                         when the slot format is                       */
  /*                                                                       */
  /*    bitmap_left       :: This is the bitmap's left bearing expressed   */
  /*                         in integer pixels.                            */
  /*                                                                       */
  /*    bitmap_top        :: This is the bitmap's top bearing expressed in */
  /*                         integer pixels.  Remember that this is the    */
  /*                         distance from the baseline to the top-most    */
  /*                         glyph scanline, upwards y-coordinates being   */
  /*                         *positive*.                                   */
  /*                                                                       */
  /*                                                                       */
  typedef struct  SFTE_GlyphSlotRec_
  {
    SFTE_Glyph_Metrics  metrics;
    SFTE_Vector         advance;
    SFTE_Bitmap         bitmap;
    __s32               bitmap_left;
    __s32               bitmap_top;
  } SFTE_GlyphSlotRec;


  typedef struct SFTE_GlyphSlotRec_*  SFTE_GlyphSlot;




  /*************************************************************************/
  /*                                                                       */
  /* <Struct>                                                              */
  /*    SFTE_FaceRec                                                       */
  /*                                                                       */
  /* <Description>                                                         */
  /*    FreeType root face class structure.  A face object models a        */
  /*    typeface in a font file.                                           */
  /*                                                                       */
  /* <Fields>                                                              */
  /*    num_fixed_sizes     :: The number of bitmap strikes in the face.   */
  /*                           Even if the face is scalable, there might   */
  /*                           still be bitmap strikes, which are called   */
  /*                           `sbits' in that case.                       */
  /*                                                                       */
  /*    num_charmaps        :: The number of charmaps in the face.         */
  /*                                                                       */
  /*    charmaps            :: An array of the charmaps of the face.       */
  /*                                                                       */
  /*    units_per_EM        :: The number of font units per EM square for  */
  /*                           this face.  This is typically 2048 for      */
  /*                           TrueType fonts, and 1000 for Type 1 fonts.  */
  /*                                                                       */
  /*    ascender            :: The typographic ascender of the face,       */
  /*                           expressed in font units.                    */
  /*                                                                       */
  /*    descender           :: The typographic descender of the face,      */
  /*                           expressed in font units. Note that this     */
  /*                           field is usually negative.                  */
  /*                                                                       */
  /*    height              :: The height is the vertical distance         */
  /*                           between two consecutive baselines,          */
  /*                           expressed in font units.  It is always      */
  /*                           positive.                                   */
  /*                                                                       */
  /*    glyph               :: The face's associated glyph slot(s).        */
  /*                                                                       */
  /*    size                :: The current active size for this face.      */
  /*                                                                       */
  /*    charmap             :: The current active charmap for this face.   */
  /*                                                                       */
  typedef struct  SFTE_FaceRec_
  {
    __u32             num_fixed_sizes;
	__u32             num_glyphs;
    __u32             units_per_EM;
    __s32             ascender;
    __s32             descender;
    __u32             height;
    SFTE_GlyphSlot    glyph;
    SFTE_Size         size;
    void             *hidden;
  } SFTE_FaceRec;


  typedef struct SFTE_FaceRec_*  SFTE_Face;




  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    SFTE_New_Face                                                      */
  /*                                                                       */
  /* <Description>                                                         */
  /*    This function is to open a font by its pathname.                   */
  /*                                                                       */
  /* <Input>                                                               */
  /*    font_file  :: A path to the font file.                             */
  /*                                                                       */
  /* <Return>                                                              */
  /*    NULL       :: operation failed                                     */
  /*    !NULL      :: A handle to a new face object.                       */
  /*                                                                       */
extern SFTE_Face SFTE_New_Face( const char *font_file );



  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    SFTE_Done_Face                                                     */
  /*                                                                       */
  /* <Description>                                                         */
  /*    destroy a given face object.                                       */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face :: A handle to a target face object.                          */
  /*                                                                       */
  /* <Return>                                                              */
  /*    error code.  0 means success.                                      */
  /*                                                                       */
extern __s32     SFTE_Done_Face( SFTE_Face face );



  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    SFTE_Set_Pixel_Sizes                                               */
  /*                                                                       */
  /* <Description>                                                         */
  /*    This function set pixel size.                                      */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    face         :: A handle to the target face object.                */
  /*                                                                       */
  /* <Input>                                                               */
  /*    pixel_size   ::  pixel size                                        */
  /*                                                                       */
  /* <Return>                                                              */
  /*    error code.  0 means success.                                      */
  /*                                                                       */
extern __s32     SFTE_Set_Pixel_Sizes( SFTE_Face face, __u32 pixel_size );



  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    SFTE_Get_Glyph                                                     */
  /*                                                                       */
  /* <Description>                                                         */
  /*    get the bitmap info of a glyph of a character.                     */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    face         :: A handle to the target face object.                */
  /*                                                                       */
  /* <Input>                                                               */
  /*    unicode      :: the unicode of The character                       */
  /*                                                                       */
  /* <Return>                                                              */
  /*    error code.  0 means success.                                      */
  /*                                                                       */
extern __s32     SFTE_Get_Glyph( SFTE_Face face, __u16 unicode );



  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    SFTE_Get_XAdvance                                                  */
  /*                                                                       */
  /* <Description>                                                         */
  /*    get the x advance of a glyph of a character.                       */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face         :: A handle to the target face object.                */
  /*                                                                       */
  /*    unicode      :: the unicode of The character                       */
  /*                                                                       */
  /* <Return>                                                              */
  /*    the x advance of a glyph of a character. 0 means error.            */
  /*                                                                       */
extern __u32     SFTE_Get_XAdvance( SFTE_Face face, __u16 unicode );

extern __s32     SFTE_Get_Pixel_Size_Count( SFTE_Face face );
extern __s32     SFTE_Get_Pixel_Size_List( SFTE_Face face, __u32 *list_p, __u32 count );


#endif     //  ifndef __sfte_h

/* end of sfte.h */
