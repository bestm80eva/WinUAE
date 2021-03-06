/* testPP10() */
/* Rip_PP10() */
/* Depack_PP10() */


#include "globals.h"
#include "extern.h"


short testPP10 ( void )
{
  /* test #1 */
  if ( (PW_i < 3) || ((PW_i+246)>=PW_in_size))
  {
/*printf ( "#1 (PW_i:%ld)\n" , PW_i );*/
    return BAD;
  }
  PW_Start_Address = PW_i-3;

  /* noisetracker byte */
  if ( in_data[PW_Start_Address+249] > 0x7f )
  {
/*printf ( "#1,1 (start:%ld)\n" , PW_Start_Address );*/
    return BAD;
  }

  /* test #2 */
  PW_WholeSampleSize = 0;
  for ( PW_j=0 ; PW_j<31 ; PW_j++ )
  {
    PW_k = (((in_data[PW_Start_Address+PW_j*8]*256)+in_data[PW_Start_Address+1+PW_j*8])*2);
    PW_l = (((in_data[PW_Start_Address+PW_j*8+4]*256)+in_data[PW_Start_Address+5+PW_j*8])*2);
    /* loop size */
    PW_m = (((in_data[PW_Start_Address+PW_j*8+6]*256)+in_data[PW_Start_Address+7+PW_j*8])*2);
    if ( PW_m == 0 )
    {
/*printf ( "#1,98 (start:%ld) (PW_k:%ld) (PW_l:%ld) (PW_m:%ld)\n" , PW_Start_Address,PW_k,PW_l,PW_m );*/
      return BAD;
    }
    if ( (PW_l != 0) && (PW_m <= 2) )
    {
/*printf ( "#1,99 (start:%ld) (PW_k:%ld) (PW_l:%ld) (PW_m:%ld)\n" , PW_Start_Address,PW_k,PW_l,PW_m );*/
      return BAD;
    }
    if ( (PW_l+PW_m) > (PW_k+2) )
    {
/*printf ( "#2,0 (start:%ld) (PW_k:%ld) (PW_l:%ld) (PW_m:%ld)\n" , PW_Start_Address,PW_k,PW_l,PW_m );*/
      return BAD;
    }
    if ( (PW_l!=0) && (PW_m == 0) )
    {
/*printf ( "#2,01 (start:%ld)\n" , PW_Start_Address );*/
      return BAD;
    }
    PW_WholeSampleSize += PW_k;
    /* finetune > 0x0f ? */
    if ( in_data[PW_Start_Address+2+8*PW_j] > 0x0f )
    {
/*printf ( "#2 (start:%ld)\n" , PW_Start_Address );*/
      return BAD;
    }
    /* volume > 0x40 ? */
    if ( in_data[PW_Start_Address+3+8*PW_j] > 0x40 )
    {
/*printf ( "#2,1 (start:%ld)\n" , PW_Start_Address );*/
      return BAD;
    }
    /* loop start > size ? */
    if ( (((in_data[PW_Start_Address+4+PW_j*8]*256)+in_data[PW_Start_Address+5+PW_j*8])*2) > PW_k )
    {
/*printf ( "#2,2 (start:%ld)\n" , PW_Start_Address );*/
      return BAD;
    }
    /* size > 0xffff ? */
    if ( PW_k > 0xFFFF )
    {
/*printf ( "#2,3 (start:%ld)\n" , PW_Start_Address );*/
      return BAD;
    }
  }

  if ( PW_WholeSampleSize <= 2 )
  {
/*printf ( "#2,4 (start:%ld)\n" , PW_Start_Address );*/
    return BAD;
  }
  /* PW_WholeSampleSize = whole sample size */

  /* test #3   about size of pattern list */
  PW_l = in_data[PW_Start_Address+248];
  if ( (PW_l > 127) || (PW_l==0) )
  {
/*printf ( "#3 (start:%ld)\n" , PW_Start_Address );*/
    return BAD;
  }

  /* get the highest track value */
  PW_k=0;
  for ( PW_j=0 ; PW_j<512 ; PW_j++ )
  {
    PW_l = in_data[PW_Start_Address+250+PW_j];
    if ( PW_l>PW_k )
      PW_k = PW_l;
  }
  /* PW_k is the highest track number */
  PW_k += 1;
  PW_k *= 64;

  if ( PW_Start_Address + 762 + (PW_k*4) > PW_in_size )
  {
/*printf ( "#4 (start:%ld)\n" , PW_Start_Address );*/
    return BAD;
  }
  /* track data test */
  PW_l=0;
  for ( PW_j=0 ; PW_j<PW_k ; PW_j++ )
  {
    if ( in_data[PW_Start_Address+762+PW_j*4] > 0x13 )
    {
/*printf ( "#3,1 (start:%ld)\n" , PW_Start_Address );*/
      return BAD;
    }
    if ( ((in_data[PW_Start_Address+762+PW_j*4]&0x0f) == 0x00 ) && (in_data[PW_Start_Address+763+PW_j*4] < 0x71) && (in_data[PW_Start_Address+763+PW_j*4] != 0x00))
    {
/*printf ( "#3,2 (start:%ld)(where:%ld)\n",PW_Start_Address,762+PW_j*4 );*/
      return BAD;
    }
    if ( ((in_data[PW_Start_Address+762+PW_j*4]&0x0f) != 0x00 ) || (in_data[PW_Start_Address+763+PW_j*4] != 0x00 ))
      PW_l = 1;
  }
  if ( PW_l == 0 )
  {
    /* only some empty patterns */
    return BAD;
  }
  PW_k *= 4;

  /* PW_WholeSampleSize is the sample data size */
  /* PW_k is the track data size */
  return GOOD;
}



void Rip_PP10 ( void )
{
  /* PW_k is still the size of the track data */
  /* PW_WholeSampleSize is still the sample data size */

  OutputSize = PW_WholeSampleSize + PW_k + 762;

  CONVERT = GOOD;
  Save_Rip ( "ProPacker v1.0 Exe-file", PP10 );
  
  if ( Save_Status == GOOD )
    PW_i += (OutputSize - 4);  /* 3 should do but call it "just to be sure" :) */
}



/*
 *   ProPacker_v1.0   1997 (c) Asle / ReDoX
 *
 * Converts back to ptk ProPacker v1 MODs
 *
 * Update: 28/11/99
 *     - removed fopen() and all attached functions.
 *     - overall speed and size optimizings.
 * Update: 19/04/00 (all pointed out by Thomas Neumann)
 *     - replen bug correction
 * Update: 15/08/10
 *     - rewrote depacker for patternlist generation
 * update : 23 aug 2010
 *   - fixed yet another patternlist bug
*/

void Depack_PP10 ( void )
{
  Uchar *Header, *Pattern;
  Ulong ReadTrkPat[128][4], ReadPat[128];
  long Highest_Track = 0;
  long i=0,j=0,k=0,l=0,m=0;
  long WholeSampleSize=0;
  long Where=PW_Start_Address;
  FILE *out;

  if ( Save_Status == BAD )
    return;

  sprintf ( Depacked_OutName , "%ld.mod" , Cpt_Filename-1 );
  out = PW_fopen ( Depacked_OutName , "w+b" );

  Header = (Uchar *)malloc(1084);
  Pattern = (Uchar *)malloc(1024);
  BZERO ( Header , 1084 );
  BZERO ( Pattern , 1024 );

  /* read and write sample descriptions */
  for ( i=0 ; i<31 ; i++ )
  {
    WholeSampleSize += (((in_data[Where]*256)+in_data[Where+1])*2);
    /* siz,fine,vol,lstart,lsize */
    Header[42+i*30] = in_data[Where];
    Header[43+i*30] = in_data[Where+1];
    Header[44+i*30] = in_data[Where+2];
    Header[45+i*30] = in_data[Where+3];
    Header[46+i*30] = in_data[Where+4];
    Header[47+i*30] = in_data[Where+5];
    Header[48+i*30] = in_data[Where+6];
    Header[49+i*30] = in_data[Where+7];
    if ( (in_data[Where+6] == 0x00) && (in_data[Where+7] == 0x00) )
      Header[49+i*30] = 0x01;
    Where += 8;
  }
  /*printf ( "Whole sample size : %ld\n" , WholeSampleSize );*/

  /* read and write pattern table lenght */
  Header[950] = in_data[Where];
  Where += 1;
  /*printf ( "Size of pattern list : %d\n" , Header[950] );*/

  /* read and write NoiseTracker byte */
  Header[951] = in_data[Where];
  Where += 1;

  /* now, where = 0xFA*/
  for (i=0;i<Header[950];i++)
  {
    ReadPat[i] = (in_data[Where+i]*256*256*256) + 
      (in_data[Where+i+128]*256*256) +
      (in_data[Where+i+256]*256) +
      in_data[Where+i+384];
    ReadTrkPat[i][0] = in_data[Where+i];
    ReadTrkPat[i][1] = in_data[Where+i+128];
    ReadTrkPat[i][2] = in_data[Where+i+256];
    ReadTrkPat[i][3] = in_data[Where+i+384];
    if (ReadTrkPat[i][0] > Highest_Track)
      Highest_Track = ReadTrkPat[i][0];
    if (ReadTrkPat[i][1] > Highest_Track)
      Highest_Track = ReadTrkPat[i][1];
    if (ReadTrkPat[i][2] > Highest_Track)
      Highest_Track = ReadTrkPat[i][2];
    if (ReadTrkPat[i][3] > Highest_Track)
      Highest_Track = ReadTrkPat[i][3];
    /*printf ("%x-%x-%x-%x\n",ReadTrkPat[i][0],ReadTrkPat[i][1],ReadTrkPat[i][2],ReadTrkPat[i][3]);*/
  }
  /*printf ( "Number of tracks : %d\n" , Highest_Track+1 );*/

  /* sorting ?*/
  k = 0; /* next min */
  l = 0;

  /* put the first pattern number */
  /* could be stored several times */
  for (j=0; j<Header[950] ; j++)
  {
    m = 0x7fffffff; /* min */
    /*search for min */
    for (i=0; i<Header[950] ; i++)
      if ((ReadPat[i]<m) && (ReadPat[i]>k))
	    m = ReadPat[i];
    /* if k == m then an already existing ref was found */
    if (k==m)
      continue;
    /* m is the next minimum */
    k = m;
    for (i=0; i<Header[950] ; i++)
      if (ReadPat[i] == k)
      {
	    Header[952+i] = (unsigned char)l;
	    j++;
      }
    j--;
    l++;
  }
  if ( l != Header[950] )
    l -= 1;


  /* write ptk's ID */
  Header[1080] = 'M';
  Header[1081] = Header[1083] = '.';
  Header[1082] = 'K';
  fwrite (Header, 1084, 1, out);


  /* put 'where' at track data level - after track list */
  Where = PW_Start_Address + 762;

  /* track/pattern data */

  for (i=0;i<l;i++)
  {
    BZERO(Pattern,1024);
    /* which pattern is it now ? */
    for (j=0;j<Header[950];j++)
    {
      if (Header[952+j] == i)
        break; /* found */
    }
    for (k=0;k<4;k++) /* loop on 4 tracks' refs*/
    {
      long d;

      /* loop on notes */
      for (d=0;d<64;d++)
      {
        /* read one ref value to be fetch in the reference table */
        long val = Where+(ReadTrkPat[j][k]*256)+(d*4);

	    Pattern[k*4+d*16] = in_data[val];
        Pattern[k*4+d*16+1] = in_data[val + 1];
        Pattern[k*4+d*16+2] = in_data[val + 2];
        Pattern[k*4+d*16+3] = in_data[val + 3];
      }
    }
    fwrite ( Pattern , 1024 , 1 , out );
  }
  free ( Pattern );
  free ( Header );



  /* now, lets put file pointer at the beginning of the sample datas */
  Where = PW_Start_Address + 762 + ((Highest_Track+1)*256);

  /* sample data */
  fwrite ( &in_data[Where] , WholeSampleSize , 1 , out );

  /* crap */
  Crap ( "  ProPacker v1.0  " , BAD , BAD , out );

  fflush ( out );
  fclose ( out );

  printf ( "done\n" );
  return; /* useless ... but */
}
