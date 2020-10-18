#include "sgl.h"
#include "sddrvs.dat"
#include "sgl_cd.h"

#define        FPS_DIV        1
#define        FPS            (60 / FPS_DIV)
#define GOUR_REAL_MAX   737 
#define    GRaddr    0x1c38 //0xe000

#define		SystemWork		0x060ffc00		/* System Variable Address */
#define		SystemSize		(0x06100000-0x060ffc00)		/* System Variable Size */

static FIXED light[XYZ];
Uint16 frame;

typedef struct cam{
    FIXED pos [XYZ];
    FIXED target[XYZ];
    ANGLE ang[XYZ];
} CAMERA;
    
static CAMERA cmbuf;
static ANGLE tmp = DEGtoANG(0.0);
static FIXED pos[XYZ];



extern XPDATA	xpdata_0[];

FIXED time;
extern TEXTURE  tex_textures[];
GOURAUDTBL       gourRealMax[GOUR_REAL_MAX];
Uint8            vwork[GOUR_REAL_MAX];
TEXTURE  tex_textures[];

extern Uint32 _bstart, _bend;
extern short int _bitmap[];

void clearScreen()
{
     int i, j;

     for(i = 0; i < 30; i++)
     {
          for(j = 0; j < 40; j++)
          {
               //slPrint(" ", slLocate(j, i));
          }
     }
}

void title(){
    
    pos[X] = toFIXED(0.0);
    pos[Y] = toFIXED(0.0);
    pos[Z] = toFIXED(0.0);
 
    cmbuf.ang[X] = cmbuf.ang[Y] = cmbuf.ang[Z] = DEGtoANG(0.0);
    cmbuf.target[X] = cmbuf.target[Y] = cmbuf.target[Z] = toFIXED(0.0);
    cmbuf.target[Z] = toFIXED(120.0);
    cmbuf.pos[X] = cmbuf.pos[Y] = toFIXED(0.0); 
    cmbuf.pos[Z] = toFIXED(-150.0);   
  
    Uint8 blink = 0;
    while(1){
  
  
  
        light[X] = slCos(DEGtoANG(frame));
        light[Y] = toFIXED(-1.0);
        light[Z] = slSin(DEGtoANG(frame));
    
        slUnitMatrix(CURRENT);
        
        slLookAt(cmbuf.pos,cmbuf.target,DEGtoANG(0.0));
        slLight(light);
        slScrPosNbg1(toFIXED(0.0 + frame), toFIXED(0.0 + frame));
       
        slPushMatrix();
        {
           
            slRotY(DEGtoANG(-50 + frame * 2));
            
            
            slTranslate(toFIXED(-70.0),pos[Y],toFIXED(0.0));
            slScale(toFIXED(5.0),toFIXED(5.0),toFIXED(5.0));
            //slPutObject((OBJECT *)object_0);
            slPutPolygonX((XPDATA *)xpdata_0,light);
        }
        slPopMatrix();
 
  
         slPushMatrix();
        {
           
            slRotY(DEGtoANG(50 - frame * 2));
            
            
            slTranslate(toFIXED(0.0),toFIXED(30.0),toFIXED(0.0));
            slScale(toFIXED(2.0),toFIXED(2.0),toFIXED(2.0));
            //slPutObject((OBJECT *)object_0);
            slPutPolygonX((XPDATA *)xpdata_0,light);
        }
        slPopMatrix();
            
        
        Uint16 push;
        push = Smpc_Peripheral[0].push;

        if (frame % 30 == 0 && blink == 1)
        blink = 0;
        else
        if (frame % 60 == 0 && blink == 0)
        blink = 1;

        clearScreen();
        if (blink == 1)
        slPrint("PRESS [A] BUTTON", slLocate(12,26));
        
        slPrint("A BORING FRIDAY NIGHT IV", slLocate(8,23));
        slSynch();
 
        frame++;
        time = slDivFX(FPS , frame);

        if (((push & PER_DGT_TA) == 0)){
            clearScreen();
            slPrint("1", slLocate(12,26));
            //break;       
        }
        
         //slSynch();
    }
}


void init_VDP2()
{
    
   
// the address for background color in VRAM is taken from various examples,
// don't know why
// !!! beware, 1st try the bgcol was the only thing to see on real Saturn
// what exactly is the back_col_adr? maybe the color at this adress is 
// interpreted as background color and wheereever such color occurs,
// it's replaced by the given one
// !! all i see is the background color....
    slBack1ColSet((void *)BACK_CRAM, CD_Black);
    
    
    
// the debug text lib take the NBG0 scroll <- the only one with 24bits color depth
// priority = 0 -> scroll not displayed
// take farest away    
    slPriorityNbg0(7);
    slPriorityNbg1(6);
//    slPrioritySpr0-7(0);    

// we want bitmap scrolls
// only NBG0 and NBG1 can handle bitmap mode
// color modes:
//      palettet:           COL_TYPE_16, COL_TYPE_256, COL_TYPE_2048
//      rgb:                COL_TYPE_32768, COL_TYPE_1M
// bitmap sizes(BM_HxV):    BM_512x256, BM_512x512, BM_1024x256, BM_1024x512
    slBitMapNbg1(COL_TYPE_32768, BM_512x256, (void *)VDP2_VRAM_A0);
//    memset((void *)VDP2_VRAM_A0, CD_Black, 256*512*2);

// monitor position in scroll coordinate space
// sets the position of monitors topleft corner(fixed format)
//    slScrPosNbg1(toFIXED(-33.680/2.0), toFIXED(0.0));
    slScrPosNbg1(toFIXED(0.0), toFIXED(0.0));

// reduction setting: only NGB0 and NBG1 are able to RBG, too)
// modes:   ZOOM_1, ZOOM_HALF, ZOOM_QUATER      each untill x256
    slZoomModeNbg1(ZOOM_1);
    
// zoom
// fixed point reziprocal of scale ratio in x and y direction
    slZoomNbg1(toFIXED(1.0), toFIXED(1.0));            

// registeres scrolls
// call only after all scroll function settings
// returns 0 or -1(ERROR)
    slScrAutoDisp(NBG0ON | NBG1ON);
    

}


void ss_main(void)
{
    
    
    //Sint32 ndir;
        
    Uint16 push;
    slInitSystem(TV_320x240, tex_textures, 1);
    //ndir = slCdInit(MAX_FILE, dirwork);
  
   
   //lDMACopy((void *)0x200000, (void *)(SpriteVRAM + CGADDRESS), (Uint32)244768);
   
     //slDMACopy((Uint16*) _bitmap, (void *)VDP2_VRAM_A0, (Uint16)244768);
     
    
     //char *vdp2b = VDP2_VRAM_A0;
     
     
      //init_VDP2();
    
    frame = 0;
    
    slInitGouraud(gourRealMax, GOUR_REAL_MAX, GRaddr, vwork);
    
    slIntFunction(slGouraudTblCopy);
  
    
    //have title screen here ?
    

    slTVOff();

    init_VDP2();
    
    
    
    
     memcpy((Uint16 *) VDP2_VRAM_A0, (Uint16 *) _bitmap, 256*512*2);
     
     //Uint16* p = (Uint16 *) VDP2_VRAM_A0; 
     
     //slPrintHex(p[100000], slLocate(1, 1));
    slTVOn();
    title();
}


int	main( void )
{
	Uint8	*dst;
	Uint32	i;

	/* 1.Zero Set .bss Section */
	for( dst = (Uint8 *)&_bstart; dst < (Uint8 *)&_bend; dst++ ) {
		*dst = 0;
	}
	/* 2.ROM has data at end of text; copy it. */

	/* 3.SGL System Variable Clear */
	for( dst = (Uint8 *)SystemWork, i = 0;i < SystemSize; i++) {
		*dst = 0;
	}

	/* Application Call */
	ss_main();
}
