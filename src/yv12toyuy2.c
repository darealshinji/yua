#include "conversion.h"
#include "yv12toyuy2.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

#define KERNEL(x1, x2, x, y) ( ( (x1) * (x) + (x2) * (y) + 4 ) >> 3 )

#define KERNEL_0_8(x1, x2) (x2)
#define KERNEL_1_7(x1, x2) KERNEL( x1, x2, 1, 7 )
#define KERNEL_2_6(x1, x2) KERNEL( x1, x2, 2, 6 )
#define KERNEL_3_5(x1, x2) KERNEL( x1, x2, 3, 5 )
#define KERNEL_4_4(x1, x2) KERNEL( x1, x2, 4, 4 )
#define KERNEL_5_3(x1, x2) KERNEL( x1, x2, 5, 3 )
#define KERNEL_6_2(x1, x2) KERNEL( x1, x2, 6, 2 )
#define KERNEL_7_1(x1, x2) KERNEL( x1, x2, 7, 1 )
#define KERNEL_8_0(x1, x2) (x1)

#define DECLARE_LINE_FUNCTION(kernel,name) \
static void yv12toyuy2_line_##name(uint8_t *pDst, const uint8_t *pSrcY, \
                                   const uint8_t *pSrcU1, const uint8_t *pSrcU2, \
                                   const uint8_t *pSrcV1, const uint8_t *pSrcV2, \
                                   int nWidth, int nHeight) \
{ \
   const int nHalfWidth = nWidth >> 1; \
   int x; \
\
   for ( x = 0; x < nHalfWidth; x++ ) \
   { \
      pDst[x*4] = pSrcY[x*2]; \
      pDst[x*4+2] = pSrcY[x*2+1]; \
\
      pDst[x*4+1] = kernel( pSrcU1[x], pSrcU2[x] ); \
      pDst[x*4+3] = kernel( pSrcV1[x], pSrcV2[x] ); \
   } \
}

#define DECLARE_FRAME_FUNCTION(name, name_odd, name_even) \
void yv12toyuy2_frame_##name(uint8_t *pDst, int nDstPitch, \
                             const uint8_t *pSrcY, int nSrcYPitch, \
                             const uint8_t *pSrcU, int nSrcUPitch, \
                             const uint8_t *pSrcV, int nSrcVPitch, \
                             int nWidth, int nHeight) \
{ \
   int y; \
\
   yv12toyuy2_line_##name_even( pDst, pSrcY, \
                                pSrcU, pSrcU, \
                                pSrcV, pSrcV, \
                                nWidth, nHeight ); \
\
   pDst += nDstPitch; \
   pSrcY += nSrcYPitch; \
\
   for ( y = 0; y < ((nHeight - 2) >> 1); y++ ) \
   { \
      yv12toyuy2_line_##name_odd( pDst, pSrcY, \
                                  pSrcU, pSrcU + nSrcUPitch, \
                                  pSrcV, pSrcV + nSrcVPitch, \
                                  nWidth, nHeight ); \
      yv12toyuy2_line_##name_even( pDst + nDstPitch, pSrcY + nSrcYPitch, \
                                   pSrcU, pSrcU + nSrcUPitch, \
                                   pSrcV, pSrcV + nSrcVPitch, \
                                   nWidth, nHeight ); \
\
      pDst += 2 * nDstPitch; \
      pSrcY += 2 * nSrcYPitch; \
      pSrcU += nSrcUPitch; \
      pSrcV += nSrcVPitch; \
   } \
\
   yv12toyuy2_line_##name_odd( pDst, pSrcY, \
                               pSrcU, pSrcU, \
                               pSrcV, pSrcV, \
                               nWidth, nHeight ); \
}

#define DECLARE_FIELD_FUNCTION(name, name_top, name_bottom) \
void yv12toyuy2_field_##name(uint8_t *pDst, int nDstPitch, \
                             const uint8_t *pSrcY, int nSrcYPitch, \
                             const uint8_t *pSrcU, int nSrcUPitch, \
                             const uint8_t *pSrcV, int nSrcVPitch, \
                             int nWidth, int nHeight) \
{ \
   yv12toyuy2_frame_##name_top( pDst, nDstPitch * 2, \
                                pSrcY, nSrcYPitch * 2, \
                                pSrcU, nSrcUPitch * 2, \
                                pSrcV, nSrcVPitch * 2, \
                                nWidth, nHeight / 2 ); \
\
   yv12toyuy2_frame_##name_bottom( pDst + nDstPitch, nDstPitch * 2, \
                                   pSrcY + nSrcYPitch, nSrcYPitch * 2, \
                                   pSrcU + nSrcUPitch, nSrcUPitch * 2, \
                                   pSrcV + nSrcVPitch, nSrcVPitch * 2, \
                                   nWidth, nHeight / 2 ); \
}

DECLARE_LINE_FUNCTION( KERNEL_6_2, 3_1 )
DECLARE_LINE_FUNCTION( KERNEL_2_6, 1_3 )
DECLARE_LINE_FUNCTION( KERNEL_7_1, 7_1 )
DECLARE_LINE_FUNCTION( KERNEL_1_7, 1_7 )
DECLARE_LINE_FUNCTION( KERNEL_5_3, 5_3 )
DECLARE_LINE_FUNCTION( KERNEL_3_5, 3_5 )
DECLARE_LINE_FUNCTION( KERNEL_4_4, 1_1 )
DECLARE_LINE_FUNCTION( KERNEL_8_0, 1_0 )
DECLARE_LINE_FUNCTION( KERNEL_0_8, 0_1 )

DECLARE_FRAME_FUNCTION( 1_top, 5_3, 1_7 );
DECLARE_FRAME_FUNCTION( 1_bottom, 7_1, 3_5 );
DECLARE_FRAME_FUNCTION( 1, 3_1, 1_3 );
DECLARE_FIELD_FUNCTION( 1, 1_top, 1_bottom );

DECLARE_FRAME_FUNCTION( 2, 1_1, 0_1 );
DECLARE_FRAME_FUNCTION( 3, 1_0, 1_1 );
DECLARE_FIELD_FUNCTION( 2, 2, 1 );
DECLARE_FIELD_FUNCTION( 3, 1, 3 );

//extern yv12toyuy2_f yv12toyuy2_frame_1_top_isse;
//extern yv12toyuy2_f yv12toyuy2_frame_1_bottom_isse;
//DECLARE_FIELD_FUNCTION( 1_isse, 1_top_isse, 1_bottom_isse );
//DECLARE_FIELD_FUNCTION( 2_isse, 2_isse, 1_isse );
//DECLARE_FIELD_FUNCTION( 3_isse, 1_isse, 3_isse );
