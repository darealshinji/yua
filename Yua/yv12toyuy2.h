#ifndef __Yv12_To_Yuy2_H__
#define __Yv12_To_Yuy2_H__

/* 

   YV12 planar to YUY2 packed colorspace conversions

   The classic pitfall of YV12 to YUY2 colorspace conversions
   is to forget to take into account where chroma pixels lie
   spatially, relatively to the luma pixels.

   Neither is that position isn't exactly defined. You can find 
   YV12 colorspaces that differ only by the position of their
   chroma pixels.

   That implies that several converters must be implemented. 
   Ideally, one converter for each possible pair of 
   ( input YV12 colorspace, output YUY2 colorspace ). 

   Note also that we don't care about the horizontal position 
   of the chroma pixel, which we'll assume fixed and the same
   in both colorspace.

   Note the following convention : X will be a luma pixel, O a
   chroma one.

   Luckily, the chroma pixel, in YUY2 progressive, can only be on
   the luma pixel, so that case is easily solved

yuy2 progressive :

   X   O   X   O   X   O   X   O   X



   X   O   X   O   X   O   X   O   X



   X   O   X   O   X   O   X   O   X



   X   O   X   O   X   O   X   O   X
   

   Let's see now YV12 progressive : we have three possible positions :


yv12 progressive 1 ( most classic one ) : 

   X       X       X       X       X

       O       O       O       O

   X       X       X       X       X



   X       X       X       X       X

       O       O       O       O

   X       X       X       X       X

yv12 progressive 2 : 

   X   O   X   O   X   O   X   O   X



   X       X       X       X       X



   X   O   X   O   X   O   X   O   X



   X       X       X       X       X

yv12 progressive 3 : 

   X       X       X       X       X



   X   O   X   O   X   O   X   O   X



   X       X       X       X       X



   X   O   X   O   X   O   X   O   X

   Now, when we want to make a conversion from yv12 to yuy2, we
   must use the proper interpolation according to the original
   yv12 colorspace.

   In YV12 1 ---> YUY2, the bilinear kernels used will be (3, 1) and (1, 3)
   In YV12 2 ---> YUY2, the bilinear kernels used will be (1, 1) and (0, 1)
   In YV12 3 ---> YUY2, the bilinear kernels used will be (1, 0) and (1, 1)

   Now, lets think a little about how interlace is stored in YV12.
   It's not as abvious as it seems. One would first think that chroma
   pixels lies in their respective field as if the field was itself
   progressive. But what happens most is actually chroma pixels been
   where they should be on the whole progressive frame.

   That implies that if we consider a field alone, chroma pixels are
   even more weirdly placed.

   Let's examined each YV12 colorspace successively : 

YV12 interlaced 1, top field :

   X       X       X       X       X 
       O       O       O       O     


   X       X       X       X       X 

                                     

   X       X       X       X       X 
       O       O       O       O     


   X       X       X       X       X 

YV12 interlaced 1, bottom field :

   X       X       X       X       X 

   
       O       O       O       O     
   X       X       X       X       X 

                                     

   X       X       X       X       X 

   
       O       O       O       O     
   X       X       X       X       X 

YV12 interlaced 2, top field :

   X   O   X   O   X   O   X   O   X



   X       X       X       X       X



   X   O   X   O   X   O   X   O   X



   X       X       X       X       X

YV12 interlaced 2, bottom field, and YV12 interlaced 3, top field :

   X       X       X       X       X

       O       O       O       O

   X       X       X       X       X



   X       X       X       X       X

       O       O       O       O

   X       X       X       X       X

YV12 interlaced 3, bottom field :

   X       X       X       X       X



   X   O   X   O   X   O   X   O   X



   X       X       X       X       X



   X   O   X   O   X   O   X   O   X

   That implies the following kernels :

   YV12 interlaced 1, top     ---> (7, 1) and (5, 3)
   YV12 interlaced 1, bottom  ---> (5, 3) and (7, 1)
   YV12 interlaced 2, top     ---> (1, 0) and (1, 1)
   YV12 interlaced 2, bottom  ---> (3, 1) and (1, 3)
   YV12 interlaced 3, top     ---> (3, 1) and (1, 3)
   YV12 interlaced 3, bottom  ---> (1, 1) and (1, 0)

*/

typedef void (yv12toyuy2_f)(uint8_t *pDst, int nDstPitch,
                            const uint8_t *pSrcY, int nSrcYPitch,
                            const uint8_t *pSrcU, int nSrcUPitch,
                            const uint8_t *pSrcV, int nSrcVPitch,
                            int nWidth, int nHeight);

yv12toyuy2_f yv12toyuy2_frame_1;
yv12toyuy2_f yv12toyuy2_frame_2;
yv12toyuy2_f yv12toyuy2_frame_3;
yv12toyuy2_f yv12toyuy2_field_1;
yv12toyuy2_f yv12toyuy2_field_2;
yv12toyuy2_f yv12toyuy2_field_3;

//extern yv12toyuy2_f yv12toyuy2_frame_1_isse;
//extern yv12toyuy2_f yv12toyuy2_frame_2_isse;
//extern yv12toyuy2_f yv12toyuy2_frame_3_isse;
//extern yv12toyuy2_f yv12toyuy2_field_1_isse;
//extern yv12toyuy2_f yv12toyuy2_field_2_isse;
//extern yv12toyuy2_f yv12toyuy2_field_3_isse;

#endif // __Yv12_To_Yuy2_H__
