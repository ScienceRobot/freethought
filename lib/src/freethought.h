
//freethought.h
//Freethought library 
/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
*/
#ifndef _FREETHOUGHT_H
#define _FREETHOUGHT_H


//extern "C" {


#if WIN32
#define _WINSOCKAPI_    // stops windows.h including winsock.h
#include <windows.h>
#include <windowsx.h>
//#include <WinSock.h>
#include <WinSock2.h> //for timeval struct
#include <ws2tcpip.h>
#include <direct.h> //for _chdir, _getcwd
#include <io.h> //for _open,_close
#include "resource.h"
#endif //WIN32

#include <stdlib.h>
#include <stdio.h>   /* Standard input/output definitions */
#include <stddef.h>
#include <errno.h>   /* Error number definitions */
#if Linux
#include <sys/time.h>
#include <sys/signal.h> //needed for setitimer SIGALARM
#include <unistd.h>  /* UNIX standard function definitions */
#include <sys/ioctl.h>
#if !MacOS
#include <sys/soundcard.h>
//#include <linux/soundcard.h>
#endif //!MacOS
#include <termios.h>
#include <dirent.h> //for looking at folders (directories) and files
#include <inttypes.h> //for uint64_t and int64_t
//XWindows
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>
//#include <X11/Xmu/Atoms.h>
//#include <X11/Xm/MwmUtil.h>
#endif //Linux
#if WIN32
#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "Windows.h"
#endif //WIN32

//CONSTANTS
#if MacOS
#define SNDCTL_DSP_RESET 0
#define SNDCTL_DSP_SETFMT 0
#define SNDCTL_DSP_CHANNELS 0
#define SNDCTL_DSP_SPEED 0
#define AFMT_S16_LE 0
#endif

#if MacOS
#define BAUDRATE 0
#else
#define BAUDRATE B921600  //not used as far as I know
#endif //MacOS
#define MODEMDEVICE "/dev/ttyUSB0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
//#define MaxNumWindows 100
#define FTMedStr 512  //maximum size of medium strings in FT
#define FTBigStr 4096  //2^16=65536 65KB,2^20=1,048,576 (1MB), 2^24=16,777,216 16MB
#define FTBiggestStr 1048576  //2^16=65536 65KB,2^20=1,048,576 (1MB), 2^24=16,777,216 16MB
#define MaxDataSize 1000000  //1Mb  for data controls, only for data drawn on screen
#define MaxNumChannels 100  //maximum numbers of channels for any data
#define MaxTracks 256  //Maximum number of tracks
#define MaxNumData 2000  //Maximum FTControls with data
//#define MaxNumSubMenu 5 //Maximum number of submenus
//#define MaxNumMenuFTItems 20 //Maximum number of menu FTItems (can be number of files)


//FTMessageBox constants, are in order they are drawn in mb
#define FTMB_NONE   0x00000000
#define FTMB_YES    0x00000001
#define FTMB_NO     0x00000002
#define FTMB_YESALL 0x00000004
#define FTMB_NOALL  0x00000008
#define FTMB_OK     0x00000010
#define FTMB_CANCEL 0x00000020
#define FTMB_CENTER 0x00000040  //center MB
#define FTMB_TIMER_CLOSE 0x00000080  //window closed because end of timer was reached

#define FT_NUM_MOUSE_BUTTONS 5 //number of possible mouse buttons (in Linux mousewheel up=[3] down=[4]
#define FT_MIN_SLIDE_SIZE 15 //minimum height or width of scroll bar slide button
#define FT_DEFAULT_FOLDERTAB_HEIGHT 32 //default foldertab control height
#define FT_DEFAULT_FOLDERTAB_WIDTH 16 //default foldertab control width
#define FT_DEFAULT_DROPDOWN_WIDTH 2 //default DropDown width in character font width units


#if Linux
//for fseeko to be 64-bit
#define _FILE_OFFSET_BITS 64
#endif

//I am running out of FTControl bits - may need to make
//flags, and flags2 or xflags or possibly make type a bit field too
//One idea is make CFlags (control flags), SFlags (Scroll flags), CPFlags (control properties flags), CTFlags (Control Text flags), etc. 
#define CMove                  0x00000001ULL  //FTControl can be moved by freethought
#define CErase                 0x00000002ULL  //store background behind FTControl (for human move)
#define CResize                0x00000004ULL //FTControl can be resized (all 4 sides)
#define C2D                    0x00000008ULL  //FTControl is not 3d (do not draw 3D lines)
#define CNoChangeOnMouseOver    0x00000010ULL //Don't draw a border or changed image on control when mouse over
#define CDoNotShowText              0x00000020ULL  //show text on FTControl
//#define CDataSigned            0x00000040ULL //data is unsigned or signed
#define CToggle                0x00000080ULL //button is toggle button
#define CNotVisible	           0x00000100ULL //FTControl is not showing/visible (same as a "NoDraw" bit)-is set by user
//FileMenus are invisible FTControls the "File" part is a different button that opens the invisible
//#define CSubMenu               0x00000200ULL //Is used to hide menus, and combo boxes, but not all filemenu of combo FTControls 
#define CItemListOpen   	   0x00000400ULL //FTControl menu is open, currently only used for filemenu and combo box
#define CVScroll               0x00000800ULL //FTControl has a vertical scroll bar (currently only file open and textarea)
#define CHScroll               0x00001000ULL //FTControl has a horizontal scroll bar (currently only file open)
#define CMouseOver             0x00002000ULL //Mouse is over FTControl
#define CEraseMenu             0x00004000ULL //trivial bit=if filemenu is clicked on when filemenu is open, erase on button up - still used?
#define CCursor               0x00008000ULL  //erase any resize cursor later when mouse is not over FTControl
//#define CVCursor               0x00010000ULL  //erase horiz resize cursor later when mouse is not over FTControl
//later there could be vert cursor, rtbt,ltbt,rttp,lftp (right-botton, etc...)
//start 64 bit
#define CResizeX1      0x0000000000020000ULL //FTControl can resize left side
#define CResizeX2      0x0000000000040000ULL //FTControl can resize right side
#define CResizeY1      0x0000000000080000ULL //FTControl can resize top side
#define CResizeY2      0x0000000000100000ULL //FTControl can resize bottom side
#define CIsMoving      0x0000000000200000ULL  //FTControl is moving
#define CDoneMoving    0x0000000000400000ULL  //FTControl is done being moved (for human funcs)
//#define CIsResizing //FTControl is being resized
#define CVScrollUp      0x0000000000800000ULL  //Scroll Up button - used internally - possibly these should be removed
#define CVScrollDown    0x0000000001000000ULL  //Scroll Down button
#define CHScrollLeft    0x0000000002000000ULL  //Scroll Left button
#define CHScrollRight   0x0000000004000000ULL  //Scroll Right button
#define CVScrollSlide  0x0000000008000000ULL  //Scroll Vertical Slide Button
#define CHScrollSlide  0x0000000010000000ULL  //Scroll Horizontal Slide Button
#define CVScrollBack   0x0000000020000000ULL  //Scroll Vertical Background
#define CHScrollBack   0x0000000040000000ULL  //Scroll Horizontal Background
#define CScaleX1       0x0000000080000000ULL //% of change in win resize - used internally for fileopen control name, date, size, etc buttons which do not have full resize, but only partial resize determined by tcontrol->sx,sy
#define CScaleY1       0x0000000100000000ULL //
#define CScaleX2       0x0000000200000000ULL //
#define CScaleY2       0x0000000400000000ULL //
#define CGrowX1   0x0000000800000000ULL //add change in window resize to control X1 (perhaps should be CGrowX1, etc)
#define CGrowY1   0x0000001000000000ULL // " Y1
#define CGrowX2   0x0000002000000000ULL // " X2
#define CGrowY2   0x0000004000000000ULL // " Y2
#define CFOName        0x0000008000000000ULL //FileOpen Name,Size, Date buttons
#define CFOSize        0x0000010000000000ULL //
#define CFODate        0x0000020000000000ULL //
#define CNoMoveOverlap 0x0000040000000000ULL //No move if would overlap other FTControl, still used?
#define CEraseCursor   0x0000080000000000ULL // Erase Cursor
#define CNoTab         0x0000100000000000ULL //FTControl is ignore when tabbing between FTControls (maybe CNoFocus - then stops from getting focus by mouse button too
#define CReadOnly      0x0000200000000000ULL //FTControl cannot be edited (textboxes)
#define CNoBorder			 0x0000400000000000ULL //no border (for CTFrame)
#define CSetDefaultSize			 0x0000800000000000ULL //Set Control default size (defines x2, y2 and x3 for dropdown controls)
#define CSetDefaultColors			 0x0001000000000000ULL //Set Control default colors (color[0], color[1], etc)
#define CNotShowing	           0x0002000000000000ULL //FTControl is not currently showing/visible - because is in a FolderTab that is not open
//#define CFrame - for now if FrameFTControl!=0, the control belongs to a frame control
//FTItem=fileopen,combo FTControl linked to this scroll bar FTControl

//no constant arrays, why?
//currently max of 4 buttons, and only coding for 3
#define CNoColorChangeButton(a) \
                      (0x0010000000000000ULL<<a)
#define CButtonDown(a) \
                      (0x0100000000000000ULL<<a)
#define CButtonHoldStart(a) \
                      (0x1000000000000000ULL<<a)
//#define CAutoScroll (0x1000000000000000ULL) //control autoscrolls (CTTextArea) - this is the default - use DoNotScrollY and/or CVScrollSlideHoldsText for non autoscroll
//out of flags, make a flags2?

//START "flags2" flags
#define CDataFileOpen       		0x00000000000000001ULL  //Data file has been opened
#define CDoNotReadInData    		0x00000000000000002ULL //Do not read in data from data file on data controls
#define CDoNotDrawData      		0x00000000000000004ULL //Do not draw data on data controls
//#define COnlyShowOneChannel 		0x00000000000000008ULL //Used to view each channel (2 data controls with 1 stereo data file)
//make CDoNotScrollMinusX, CDoNotScrollPlusX?
#define CDoNotScrollX       		0x00000000000000010ULL  //control does not scroll in X direction if in a frame
#define CDoNotScrollY       		0x00000000000000020ULL  //control does not scroll in Y direction if in a frame
#define CVScrollSlideHoldsText 	0x00000000000000040ULL //scroll slide not being at bottom stops textarea from scrolling
#define CDataDrawnInitially			0x00000000000000080ULL //indicates data control data was drawn at least once
#define CTextNotCentered			0x00000000000000100ULL //do not center text on control (alt: Unformated)
#define CTextFullJustify			0x00000000000000200ULL //full justify text on control - not implemented
#define CTextLeftJustify			0x00000000000000400ULL //left justify text on control - not implemented
#define CTextRightJustify			0x00000000000000800ULL //right justify text on control - not implemented
//#define CTimeLine //Associate timeline with this control - usually for frame
//#define DoNotFrameClip - do not clip control, for example timeline connected to frame of data controls
//#define CComboAutoFill (0x1000000000000000ULL) //Combo autofills when selected - or CComboNoAutofill? - combo autofill is only option now
//CDataLabel
//CDataTextBox
//CDataButton
//CChkBoxOnRight

//#define CItemListOpenOnButtonDown			0x1000000000000000ULL	
//button down on control while itemlist was open - done with PStatus.flags|=PCloseItemListOnUp


//0=button,1=label,2=textbox,3=checkbox,4=radio,5=combo,6=File Menu FTItem,7=File Open Menu
#define CTButton   					0x00000000  //button
#define CTLabel    					0x00000001  //text label
#define CTTextBox  					0x00000002  //textbox
#define CTCheckBox 					0x00000003  //Checkbox
#define CTOption   					0x00000004  //CTOption is prefered over CTRadio
#define CTRadio    					0x00000004  //radio button -change to CTRadio?
#define CTCombo    					0x00000005  //Combo or Drop-down box
#define CTDropdown 					0x00000005  //Combo or Drop-down box
#define CTFileMenu 					0x00000006 //"File" at top of window
#define CTFileOpen 					0x00000007 //File Open window, lists folders, files - is now itemlist window
#define CTImage    					0x00000008 //bmp,jpg 
#define CTTextArea 					0x00000009 //textarea
#define CTGrid     					0x0000000a //to be done - includes List FTControls
#define CTData     					0x0000000b //data can be wav,mp3,avi,mpg,dat (8 bit logic analyzer data)
#define CTCustom   					0x0000000c //must be drawn and funcs handled in human code
#define CTTimer	   					0x0000000d //generic timer control, not drawn
#define CTFrame		 				0x0000000e //Frame 
#define CTTimeLine 					0x0000000f //TimeLine control, contains time lines based on tcontrol->TimeScale, scrolls on frame
#define CTFolderTabGroup			0x00000010  //folder with tabs - like frame but with tabs - make FolderTabFrame?
#define CTFolderTab					0x00000011  //single tab of FolderTab control - make CTFolderTabButton ?
#define CTItemList					0x00000012  //an itemlist - items drawn in a textarea
//#define CTVScroll,CTHScroll,CTVScrollUp,CTVScrollDown,CTHScrollLeft,CTHScrollRight,CTVScrollSlide,CTVScrollBack,CTHScrollSlide,CTHScrollBack
//

#if 0 
//these are for 24 and 32 bit color, 16 bit is different
#define wingray  0xe0e0e0 //window gray
#define ltgray 0xb0b0b0
#define ltgray2 0xd0d0d0
#define gray 0x808080
#define dkgray 0x707070
#define white 0xffffff
#define black 0
#define ltgreen 0x90b090
#define green 0x00a000
#define dkgreen 0x007f00
//#if Linux
#define ltblue 0xd0b0b0
#define blue 0xff0000
#define red 0x0000ff
#define yellow 0x00ffff 
//#endif
#endif 

#if WIN32
#define FT_DefaultFontWidth 8
#define FT_DefaultFontHeight 14
#endif //WIN32

#if 0//WIN32
#define ltblue 0xb0b0d0
#define blue 0x0000ff
#define red 0xff0000
#endif
#if 0 
#define wingray16 0x4108  //window gray
#define purple 0x7f007f
#endif 

#if 0 
typedef unsigned long long u64;
typedef long long i64;
typedef unsigned int u32;
typedef int i32;
typedef unsigned short int u16;
typedef short int i16;
typedef unsigned char u8;
#endif



//forward declaration needed for structures that contain a pointer to them selves (like a "next" list pointer)
typedef struct FTControl FTControl;
typedef struct FTWindow FTWindow;
typedef struct FTItem FTItem;
typedef struct FTItemList FTItemList;

/*
struct _FTControl; //forward declaration - why needed?
typedef struct _FTControl FTControl;
struct _FTWindow; //forward declaration - should not be needed, but yet is.
typedef struct _FTWindow FTWindow;
struct _FTItem; //forward declaration - should not be needed, but yet is.
typedef struct _FTItem FTItem;
struct _FTItemList; //forward declaration - should not be needed, but yet is.
typedef struct _FTItemList FTItemList;
*/
//struct _FTSelectedItemList; //forward declaration
//typedef struct _FTSelectedItemList FTSelectedItemList;
//forward declaration needed for structures that contain a pointer to them selves (like a "next" list pointer)
//struct _ProgramStatus; //needed because structure contains a pointer to ProgramStatus struct
//typedef struct _ProgramStatus ProgramStatus;


#if WIN32
typedef unsigned int KeySym;
#endif

typedef unsigned int FTControlfunc(void);
//typedef unsigned int FTControlfunck(unsigned char key);
typedef unsigned int FTControlfunck(KeySym key);
typedef unsigned int FTControlfuncxy(int x, int y);
typedef unsigned int FTControlfuncw(FTWindow *twin);
//typedef unsigned int FTControlfuncwk(FTWindow *twin,unsigned char key);
//typedef unsigned int FTControlfuncwck(FTWindow *twin,FTControl *tcontrol,unsigned char key);
typedef unsigned int FTControlfuncwk(FTWindow *twin, KeySym key);
typedef unsigned int FTControlfuncwck(FTWindow *twin, FTControl *tcontrol, KeySym key);
typedef unsigned int FTControlfuncwxy(FTWindow *twin, int x, int y);
typedef unsigned int FTControlfunccxy(FTControl *tcontrol, int x, int y);
typedef unsigned int FTControlfuncwcxy(FTWindow *twin, FTControl *tcontrol, int x, int y);
typedef unsigned int FTControlfuncwc(FTWindow *twin, FTControl *tcontrol);

//Note: Probably all char should be converted to unicode (u16 *)

//note structure - holds each time and key
typedef struct tagNoteSt
{
	unsigned long long time;
	int key;
}NoteSt;

typedef struct tagKeyMap
{
	int key;
	char *filename;
}KeyMap;



//#define MAX_FTItem_HOTKEY 2
//possibly 'FTItem' is a key word
//flags for FTItem "flags" structure
#define FTI_FOLDER 0x1 //is a folder
#define FTI_SELECTED 0x2 //item is selected
struct FTItem {
	int flags; //make flags
	int num; //FTItem number 1,2,3,...
			 //int i;  //FTItem number 1,2,3,...
			 //char text[FTMedStr]?
	char name[FTMedStr];
#if WIN32
	//short name_16[FTMedStr];
	wchar_t name_16[FTMedStr];
#endif
	//char *name; //name must be malloc'd in the FTItem sent to AddFTItem(), but AddFTItem() will malloc a diff addr
	unsigned int hotkey[2]; //hot key that opens item or item subitemlist
							//if a control - items would inherit all functions, etc
							//add *keypress function?
							//add image?
	FTControlfuncwcxy *click[FT_NUM_MOUSE_BUTTONS];  //[3] function called when this FTItem is clicked on by button[0,1, or 2  and 3,4 for mousewheel in Linux]
#if Linux
	XFontStruct *font; //font of Item
#endif
					   //char *submenu; //was this
					   //char submenu[FTMedStr];  //name of submenu FTControl (if any) for this FTItem (opened on mouseover)
	FTItemList *parent; //parent itemlist of this item
	FTItemList *ilist; //list to subFTItems
#if Linux
	struct tm idate;  //date last changed
#endif
#if WIN32
					  //FILETIME idate; //date last changed
	SYSTEMTIME idate; //date last changed
#endif
					  //int size;
#if Linux
	uint64_t size;
#endif
#if WIN32
	unsigned __int64 size;
#endif
	//date_time
	//permissions, owner, group (permiss,uid,gid)
	//filetype,link, date of origin
	//int value; //associated integer value with this item
	FTItem *next;
	//unsigned int *next;
};


#define FT_SCROLL_FTITEMLIST_SELECTION 0x00000001 //scroll the selection in addition to the itemlist (used for function ScrollFTItemList)
#define FT_SCROLL_FTITEMLIST_DOWN 0x00000002 //scroll the itemlist down (a single entry)
#define FT_SCROLL_FTITEMLIST_UP 0x00000004 //scroll the itemlist up (a single entry)
#define FT_SCROLL_FTITEMLIST_PAGE_DOWN 0x00000008 //scroll the itemlist down a page
#define FT_SCROLL_FTITEMLIST_PAGE_UP 0x00000010 //scroll the itemlist down a page
#define FT_SCROLL_FTITEMLIST_MOUSEWHEEL 0x00000020 //scrolling with the MouseWheel- currently scrolls an itemlist 3 items at a time


#define FTIL_SELECT_SINGLE 0  //selecting only 1 item with SelectFTItem
#define FTIL_SELECT_MULTIPLE_WITH_SHIFT 1 //selecting multiple items with SelectFTItem using the shift key
#define FTIL_SELECT_MULTIPLE_WITH_CONTROL 2 //selecting multiple items with SelectFTItem using the control key

//FTItemList flags
#define FTIL_REREAD 0x0001 //reread files
#define FTIL_USE_CONTROL_FONT 0x0002 //use the parent control's font for the this itemlist's font- used when a person adds an itemlist to a control before adding the control to know when the control is added to copy the control font to the itemlist
//#define FTIL_OPEN 	0x0002 //ItemList is open (window is showing)
//#define IL_REDRAW 0x2 //redraw files
#define FTIL_HAS_NO_HOT_KEYS 0x0004 //currently only CTFileMenu has hot keys where text like &File is rendered with F underlined
#define FTIL_IS_IN_A_CONTROL 0x0008 //itemlist is a control not a window
//#define FTIL_NO_BACKGROUND_FILL 0x0010 //don't draw white behind text (because it was already drawn)
#define FTIL_HAS_SELECTED_ITEMS  0x0010//itemlist currently has selected items
//I decided to make an ItemList head which holds flags, current item, etc.
struct  FTItemList {
	int flags; //make flags
			   //char name[FTMedStr];
	int num;  //number of FTItems in this FTItem list
	int cur; //the last selected or unselected item 0=none
			 //int sel; //selected but not current - an escape key will close combo and not set cur
	int orig; //original selection - an escape key will reset cur to orig
	int width; //total width of itemlist text (in pixels) - depends on font and length of largest item or dropdown control length if not long enough
	int height;  //total height of itemlist text (in pixels) - depends on font and number of items
#if Linux
	XFontStruct *font; //font of ItemList
#endif
	int fontwidth;
	int fontheight;
	FTControl *control; //pointer to FTControl (parent) this itemlist is descended from (for arrow key navigation)
	FTWindow *window; //pointer to FTWindow this itemlist is drawn on - to redraw only this window
	FTItemList *parent; //parent of this itemlist - if descended from itemlist (for arrow key navigation)
	FTItem *item;  //point to first FTItem in list of FTItems
				   //FTSelectedItemList *iSelectedItemList;  //list of all selected item numbers
	int NumSelectedItems; //serves as a fast method to determine if we need to clear any selections instead of looking through selected item list
#if WIN32
	HFONT font;
	TEXTMETRIC textmet;
#endif
};


/*
//List of selected item numbers (for selecting multiple items in an itemlist)
struct _FTSelectedItemList
{
FTItem *item;  //pointer to selected item
FTSelectedItemList *next;
};
*/


#if Linux
#pragma pack(push,1) //push default alignment and set alignment to 1 byte aligned
//otherwise sizeof(BITMAPFILEHEADER) returns 16 instad of 14
typedef struct tagBITMAPFILEHEADER
{
unsigned short int bfType;
unsigned int bfSize;
unsigned short int bfReserved1;
unsigned short int bfReserved2;
unsigned int bfOffBits;
}BITMAPFILEHEADER;
#pragma pack(pop) //restore original alignment

typedef struct tagBITMAPINFOHEADER
{
int biSize;
int biWidth;
int biHeight;
short int biPlanes;
short int biBitCount;
int biCompression;
int biSizeImage;
int biXPelsPerMeter;
int biYPelsPerMeter;
int biClrUsed;
int biClrImportant;
}BITMAPINFOHEADER;
#endif



typedef struct tagWaveHdr
{
   unsigned int	ID;		//0x52,0x49,0x46,0x46 "RIFF"
   unsigned int	hssize;		//header size
   unsigned int	hssize2;	//0x57,0x41,0x56,0x45  sample size?
   unsigned int	ckID;		//"fmt "
   unsigned int	nchunkSize;	//length of format data bytes
   unsigned short int       wFormatTag;     //Format category (PCM=1)
   unsigned short int       Channels;      //Number of channels (mono=1, stereo=2)
   unsigned int       SamplesPerSecond;	//Sampling rate [Hz]
   unsigned int       AvgBytesPerSecond;//average number of samples a player must play
//- redundant samprate*numchn*numbytprsamp
   unsigned short int       nBlockAlign;	//BYTESPERSAMP*NUMCHAN
   unsigned short int       BitsPerSample;	//8 or 16 (24 or 32)
}WaveHdr;

typedef struct tagWaveDataHdr
{
   unsigned int	ID;		//0x52,0x49,0x46,0x46 "RIFF"
   unsigned int	chunkSize;        //size of data
   //unsigned char Data[]
}WaveDataHdr;



#define FTControl_MAX_NUM_IMAGES 4  //maximum number of images any control can have
#define FTControl_MAX_NUM_BUTTONS 5  //maximum number of buttons any control can have
struct FTControl {
	char name[FTMedStr];  //perhaps make strings dynamic
						  //unsigned long long flags;
	unsigned long long flags;  //CMove|CResize|C3D|CButtonDownChange|CDoNotShowText
	unsigned long long flags2;
	int type;  //0=button,1=label,2=textbox,3=checkbox,4=radio,5=combo,6=File Menu FTItem
	int x1, y1, x2, y2, x3, y3, x4, y4;
	int cx1, cy1, cx2, cy2;//clipped coordinates ,x3,y3,x4,y4;
	int textx, texty; //position of top left of text on control (button)
					  //long long lx1,ly1,lx2,ly2;  //data controls get very large, and are clipped by a frame - use ia[0-3]?
					  //this lx1,ly1 issue relates to the debate between - using a frame for large data controls, or handling data controls differently - scrolling by user callback
	float sx1, sy1, sx2, sy2; //% of new/lost window space to add/sub (with 100 new pixels, .5= add 50)
	int z; //depth, 0=top level, -1 above, +1 below, etc...usually only negative numbers used
		   //to not slow down clicking on FTControl processing, x2 and y2 should always define clickable FTControl
		   //for checkbox and radio, people can click on the text
		   //the alternative is to change the CheckClick to test between x2,y1 and x3,y3 for CTCheckBox and CTRadio
		   //x3,y3 are used for radio, check and combo boxes that extend because of text
		   //x3,y3 are the length of the actual box,button,  x2,y2=+string
		   //y3 is for FolderTabGroup height of associated FolderTab controls
		   //x4,y4 are for combo box (and possibly filemenu) that has to remember the original size of the combo box (before opening)
		   //char label[FTMedStr]; //text for labels, buttons, ...
		   //text used for text in textbox,textarea, and selected file in FileOpen control
	union {
		char text[FTBigStr];  //malloc versus static array:
		char FileOpenFile[FTBigStr];  //in FileOpen controls this holds any currently selected file(s) - probably should be FileList[], because can be for saving files too
	};
	union {
		char text2[FTBigStr]; //text2 is currently only used to determine if user changed text in a textarea - in order to know if to autoscroll to bottom, and to know if text changed to call OnChange function
		char FileOpenPath[FTBigStr];  //in FileOpen controls this holds the current path
	};
#if WIN32
	unsigned short int text_16[FTBigStr];  //16-bit character (unicode)
	unsigned short int text2_16[FTBigStr];
#endif
	//and contains the original combo text
	//current view: just use large array 
	//either use text[1000000] or malloc (freed in DelFTControl)
	//malloc: 
	//+May grow large (like instant message) and dont want to reserve large for example 4k memory on each FTControl - is freed in DelFTControls 
	//+is memory efficient 
	//-is a hassle to malloc and may cause crashes when freeing 
	//-each button and label text needs to be malloc'd and freed
	//+can be user modified, user not limited to small text size - or large unused blocks of reserved memory
	//static: 
	//+Seems like beginner friendly (VB, VC etc - don't malloc text FTControls)
	//+No need to malloc or free
	//+can use "data" for large text blocks for CTTEXTAREAs (using "text" is more intuitive)
	//-if larger memory needed - need to customize freethought, change FTBigStr variable
	//other options:
	//could use label[FTMedStr] for button and label text with malloc or [FTBigStr] for text, -if using big array for text - dont bother with label - "text" is more intuitive and seemless
	//could use bit Usetextbig
	//char text2[FTMedStr]; //original combo text, (was textbox selected text - but this text might be too large - just copied from text now using i[2] and i[3])
	//see if text2 can be eliminated or reduced
	//char submenu[FTMedStr]; //name of submenu
	//possibly have seltext field for selected text
	//I don't know if I need parent anymore since parent is in itemlist
	//FTControl *parent; //parent (or supermenu) of this FTControl (if any), used when using left arrow on filemenu submenu
	//instead of searching thru all FTControl FTItems to find wher the submenu came from (and could be from 2 or more different FTControls in theory, but probably not often in reality)
	//int winnum;  //window number this FTControl is in, added automatically in addFTControl
	FTWindow *window; //FTWindow this FTControl belongs to
	FTControl *parent; //for fileopen buttons (name, size, etc), and scroll bars
					   //possibly make all these controls:
					   //FTControl *FTC_Frame,*FTC_TimeLine,*FTC_DataLabel, etc
					   //could have a union here:
	FTControl *FrameFTControl; //handle of frame control this control belongs to if any (control is clipped to frame)
	FTControl *FolderTabGroup; //handle of FolderTabGroup this control belongs to if any (control is clipped to FolderTabGroup dimensions)
	FTControl *FolderTab; //the FolderTab this control belongs on (but todo: probably this should point to a linked list of controls on this FolderTab control- then we don't need to go through all controls to set/unset CNotShowingwhen CurrentFolderTab is changed)
	FTControl *CurrentFolderTab;  //the CTFolderTab control that is currently open on a CTFolderTabGroup (there can be sub CTFolderTabGroup controls and so FolderTab cannot be reused)

								  //FTFolderTabList *iFolderTab; //list to each foldertab control
								  //or i[0] contains number of foldertab in FolderTabGroup
	FTControl *TimeLineFTControl; //an associated timeline control (currently for data only)
	FTControl *DataLabelFTControl; //Label control connected to data control (for scrolling a button connected to data, such as a track number, without being on same frame)
	FTControl *StartTimeSelectedFTControl; //textbox that prints timeline StartTimeSelected
	FTControl *EndTimeSelectedFTControl; //textbox that prints timeline EndTimeSelected
	FTControl *DurationSelectedFTControl; //textbox that prints timeline selected duration
	FTControl *TimeLinePlusFTControl; //button that will magnify (zoom in) timeline
	FTControl *TimeLineMinusFTControl; //button that will microfy (zoom out) timeline
	FTControl *TimeScaleFTControl; //Label that contains TimeScale of TimeLine
	FTControl *FTC_VScrollUp, *FTC_VScrollDown, *FTC_VScrollSlide, *FTC_VScrollBack; //connected scrollbar controls
	FTControl *FTC_HScrollLeft, *FTC_HScrollRight, *FTC_HScrollSlide, *FTC_HScrollBack; //connected scrollbar controls
	FTControl *TextBoxPath; //in a textbox control this can point to a fileopen control which when the path changes updates the textbox text
	FTControl *TextBoxFile; //in a textbox control this can point to a fileopen control which when the selected file changes updates the textbox text
	int num;  //number of FTControls - used to track what FTControl has focus
	int timer; //time in ms to call any OnTimer function if FTControl has OnTimer function
	struct timeval timertime;   //last time timer called tv_sec and tv_usec
	int fontwidth;
	int fontheight;
#if Linux
	XFontStruct *font;
#endif
#if WIN32
	//HFONT font; //possibly something in font or textmet is corrupted, because when fontwidth and fontheight are after it- they get corrupted: after AddFTControl, then GetFTControl- font!=0 but fontwidth=0 and fontheight=0
	//TEXTMETRIC textmet; //moved to end- because of currently unsolved error where memory after it becomes==0
#endif
	FTControlfuncwcxy *ButtonDown[FTControl_MAX_NUM_BUTTONS]; //the function that is called when there is a button 1-3 down on this FTControl
	FTControlfuncwcxy *ButtonUp[FTControl_MAX_NUM_BUTTONS]; //the function that is called when there is a button 1-3 up on this FTControl
															//buttondownup could be called "click"
	FTControlfuncwcxy *ButtonDownUp[FTControl_MAX_NUM_BUTTONS]; //function called when there is a button down and then up on this FTControl
	FTControlfuncwcxy *ButtonClick[FTControl_MAX_NUM_BUTTONS]; //function is identical to ButtonDownUp
	FTControlfuncwcxy *ButtonDownHold[FTControl_MAX_NUM_BUTTONS]; //when button is held down call this func at time defined by delay
																  //FTControlfuncwcxy *ButtonDownUpAnyWhere[FTControl_MAX_NUM_BUTTONS]; //function called when button down on a FTControl, and up anywhere
	FTControlfuncwcxy *ButtonDoubleClick[FTControl_MAX_NUM_BUTTONS];  //the function that is called when button is double-clicked
	FTControlfuncwcxy *MouseMove;  //Mousemove on FTControl (but also if button is down on FTControl, but not over)
								   //"KeyPress" is defined in X.h 
	FTControlfuncwck *Keypress;  //the function that is called when a key is pressed when this FTControl has focus
								 //FTControlfuncwc *BeforeDraw;  //Before drawFTControl (this FTControl is drawn)
	FTControlfuncwc *OnDraw;  //after drawFTControl (this FTControl is drawn)
	FTControlfuncwc *GotFocus;  //
	FTControlfuncwc *LostFocus;  //
	FTControlfuncwcxy *OnMove; //FTControl was moved xy=mouse location
	FTControlfuncwcxy *OnResize; //FTControl was resized xy=mouse location
	FTControlfuncwc *OnChange; //text or filename property changed
							   //FTControls can have timers, and windows can have timers
	FTControlfuncwc *OnTimer;  //called depending on timer interval variable
	FTControlfuncwc *HotKeyActivate; //a hotkey activated the control
									 //FTControlfuncwcxy *VScrollUp
									 //FTControlfuncwcxy *VScrollDown
									 //FTControlfuncwcxy *HScrollLeft
									 //FTControlfuncwcxy *HScrollRight
									 //FTControlfuncwcxy *VScrollPgUp
									 //FTControlfuncwcxy *VScrollPgDown
									 //FTControlfuncwcxy *HScrollPgLeft
									 //FTControlfuncwcxy *HScrollPgRight
									 //SlideUp, etc?
									 //there should be a pointer to various FTControl structures=button,textbox,scroll,etc..
									 //could be ax,bx,cx,dx or i0,i1,i2,i3...
									 //todo: make union {} with various fields like "width", "height", etc for all the different controls
	union {
		int i[10];  //generic index 
		struct {  //textbox+textarea
			int CursorLocationInChars; //i[0]=textbox-cursor location in string (in characters, 0=first character), fileopen-index of first showing
			int FirstCharacterShowing; //i[1]=textbox-index of 1st character showing in textbox (0=first character), fileopen-selected index
			int StartSelectedTextInChars;//i[2]-textbox first position of selected text (in characters)
			int EndSelectedTextInChars;//i[3]-textbox last position of selected text (in characters)
			int	CursorX; //i[4]-for textarea, cursor X position in characters - possibly add WidthInChar and HeightInChar for textarea- but could change if user changes font so probably should be calculated any time needed)
			int CursorY; //i[5]-for textarea, cursor Y position in characters -CursorX and CursorY are tracked in order to more easily handle text scrolling- otherwise we would need to use CursorLocationInChars and FirstCharacterShowing, and determining how to draw text would require more processing time
			int ButtonDownCursorLocationInChars;//i[6]-to store the original cursor location when selecting text and the current cursor location is changing
												//		int StartSelectedTextInPixels;//i[7]-textbox first position of selected text (in pixels)- may want StartSelectionInPixels
												//		int EndSelectedTextInPixels;//i[8]-textbox last position of selected text (in pixels)
												//		int TextUnknown[1]; //i[9]-generic int for user		
		};
		struct {  //ItemList (includes FileOpen)
			int CurrentItem; //i[0]=currently selected item
			int FirstItemCharacterShowing;	//i[1]=the first character of all the items showing in the FileOpen control- horizontal scrollbar changes this
			int FirstItemShowing;	//i[2]=the first item showing in the FileOpen control- vertical scrollbar changes this		
			int SortKind; //i[3] sort FileOpen list by 0=name,1=size,2=date
			int SortDirection; //i[4] sort direction 0=asc,1=desc
							   //int ItemUnknown[9]; 
		};
		//0=textbox-cursor location in string (in characters), fileopen-index of first showing
		//1=textbox-index of 1st character showing in textbox, fileopen-selected index
		//2-textbox first position of selected text (in pixels)
		//3-textbox last position of selected text (in pixels)
		//4-textbox first position of selected text (in characters)
		//5-textbox last position of selected text (in characters)
		//6-textarea could be row
		//7-textarea could be column
		//  struct {
		//		int CurFolderTab; //CTFolderTabGroup - current foldertab showing
		//	};
	}; //union
	long long ia[10];  //64 bit generic integer variables for users
	int tab;  //=0 will give this FTControl focus, of is order of tab
			  //font - each FTControl can have a different font
			  //FTControl moves/grows with window resize
			  //int gx1,gy1,gx2,gy2,gx3,gy3;  //amount off border (-10,0=on border, ...) gx3 for checkbox w/ text
			  //make textcolor array of [4]?
	union {
		unsigned int textcolor[6];
		struct {
			unsigned int TextColor;
			unsigned int TextBackgroundColor;
			unsigned int SelectedTextColor;
			unsigned int SelectedTextBackgroundColor;
			unsigned int MouseOverTextColor;
			unsigned int MouseOverTextBackgroundColor;
		};
		//textcolor[0]=textcolor,bgcolor,mocolor (mouse over), clickoncolor (black)
		//textcolor[1]=text background color (white) also applies to lines in data controls
		//textcolor[2]=selected text color (white)
		//textcolor[3]=selected background color (black)
		//make color array of [4]?
	}; //union
	union {
		unsigned int color[6];
		struct {
			unsigned int ControlBackgroundColor;
			unsigned int ControlButtonDownColor;
			unsigned int ControlSelectedBackgroundColor;
			unsigned int ControlMouseOverBackgroundColor;
			unsigned int ControlBorderColor;
			unsigned int ControlShadedBackgroundColor; //currently only for FolderTabGroup controls- the shading on the unused top of the folder group
		};
		//color[0]=control color (rectangle) (for text,textarea,data is color of control outline)
		//color[1]=control color when button down | background color |bordercolor for FolderTabGroup
		//color[2]=control color when selected, when mouse is over button, or tab has selected, and color of combo box behind triangle - perhaps should be color[1]
		//color[3]=color4;  //control is selected and button 0 is down - actually control must be selected to have button down so perhaps this should be deleted
	}; //union
	int value;
	FTItemList *ilist;  //for filemenu FTItems, and fileopen FTItems
						//FTItemList *ilist2;  //for fileopen top buttons (name,size,date) (list of selected FTItems)
	int numvisible; //number of FTItems FTControl shows at any one time (fileopen)
	unsigned int hotkey[4]; //4 possible hot keys use PShift|0x53, etc with PControl and PAlt
							//int altkey;  //alt key that opens FTControl (such as file menu alt-f)
							//int altkey2;
							//int ctlkey;  //control key that opens FTControl (such as file menu ctl-F)
							//int ctlkey2;
	struct timeval lasttime[FT_NUM_MOUSE_BUTTONS];  //button down last time, last time button down event was called in sec and us
	struct timeval interval[FT_NUM_MOUSE_BUTTONS];  //button down interval - time between button down events is button is being held down
	struct timeval delay[FT_NUM_MOUSE_BUTTONS];  //amount of delay before button hold down events start to be called at regular intervals
												 //perhaps filename should be removed in the interest of saving space and consoldating
												 //change to "filenames"?, because with multiple selection this can hold a comma delimited list of files
												 //note that msvc uses a list of quoted space delimited characters
	char filename[FTBigStr];  //a file is attached to FTControl (including bmp), also used for fileopen selected filename (although ->text does also and so this probably should be removed), and CTData because data may want to be read/written in parts without holding the full file data in memory
							  //FILE *fptr; //pointer to file - perhaps this should be tracked in user global struct
							  //char filename2[FTMedStr];  //a second file is attached to FTControl (only used for a button second bitmap)
	unsigned char *data;  //pointer to data in file is signed for audio
	long long filesize;  //number of bytes of data in file
	long long datasize;  //number of bytes of data in file
	FILE *datafptr; //data file pointer
					//struct stat filestat; //file information
					//perhaps FTData structure could have these values
					//these can be done in i[]
					//make union with i[]
	int Channels;  //number of channels in this file
	int Channel;  //channel of this particular FTControl/track 0=left, 1=right, 2=backl, 3=backr, 4=up/ceiling, 5=bottom/floor...
				  //int Signed; //0=data has no sign (in unsigned), 1=data is signed
	int track;  //track this sample is on should be done by human using i[0]
				//made many of these long long because sometimes a calculation will result in a negative number, that will then be trimmed to 0.  I was looking out for these, but better to look at these variables as allowing negative numbers for normal calculations
				//argument against: better for this variable better to be consistant with rtai
				//could be in i[], although is common for many types of data
				//use this info locally - read from file
	int BitsPerSample;  //bits per sample  8,16,...
	int BytesPerSample;  //bytes per sample  1,2,...
	int SamplesPerSecond; //frequency of data 44100,48000,1000000,....
	int BitsPerChannel;  //bits per channel 1 bit of 8 byte sample for usb data - need to draw individual bit tracks/datacontrols
	int BytesPerChannel;
	int Signed; //data is signed=no=0, yes=1
				//long long timescale; //for data controls. timescale is the current number of pixels per nanosecond for displaying data in data controls. In theory a user could be required to draw the data, there should be a CDontDrawData bit
	long double TimeScale; //for data controls. TimeScale is the current number of pixels per second, the data in this data control should be displayed at. The user can set the CDontDrawData bit to draw data differently.
						   //these can be done in ia[]
	int TimeSpace; //for TimeLine controls, how many pixels equals one time line - used to scroll one timeline unit - 
				   //int ScrollXPixels,ScrollYPixels; //number of pixels to scroll on one click, make i[0],i[1], etc.? make CCustomScrollX - to know to use instead of system default?
				   //possibly these should be in number of bytes, and/or samples
				   //changed to double in order to go to picoseconds and beyond
				   //perhaps I should just use start+duration instead of start and end time variables
				   //I chose start+end because it is easier to visualize and understand in my mind
	long double StartTime;  //Start time of data control in ns relative to a user timeline control (this StartTime may be offscreen)
							//long double Duration;  //duration of entire data control (in ns) (not just visible portion)
	long double EndTime; //End time of data in data control (in ns) (not just visible portion)
	long double StartTimeVisible;  //start time of FTControl (for data/track display) in s. This is the start time of the data control relative to a timeline control, not the starttime of the data relative to the start of the control - that is StartTimeData (and EndTimeData), then StartTimeBuffer is the start of the data that is on screen.
								   //long double DurationVisible;  //duration of FTControl relative to TimeScale (in s)
	long double EndTimeVisible;  //start time of FTControl (for track display) in ns, VisStartTime or CurStartTime
	long double StartTimeData;  //start time of data relative to beginning of control (in s) (initially StartTime and StartTimeData are the same, but change when a control is cut or resized to contain only part of some data). StartTimeData is the time into the data from the file (does not include header size) in seconds that is at the start of the control. StartTimeData is needed because the data at the start of a data control may not be the beginning of the data file
								//long double DurationData;  //duration of data relative to start of control (in s) (This only includes duration of data relative to StartTimeData not relative to the entire datasize - some of which may not be in the control)
	long double EndTimeData;  //end time of data relative to beginning of control (in s)
	long double StartTimeVisibleData; //start time of data relative to visible StartTime of control (in seconds)
	long double StartTimeBuffer;  //StartTime of data control at last drawing (to determine if data needs to be reloaded)
								  //was: start time of data showing in buffer (FT only loads visible data)
								  //long double DurationBuffer;  //duration of data showing read into buffer (same as Duration - need?)
	long double EndTimeBuffer;    //EndTime of data control at last drawing (to determine if data needs to be reloaded)
								  //was: end time of data showing read into buffer
	long double TimeScaleBuffer;  //TimeScale Buffer data was loaded at
	long double StartTimeSelected;  //start time of selected data 
	long double EndTimeSelected;  //end time of selected data
	long double StartTimeCursor; //where on timeline cursor is drawn
	unsigned long long StartByte; //starting byte of data visible on screen
	unsigned long long BufferSize; //number of bytes loaded in tcontrol->data
								   //unsigned long long DurationBytes; //number of bytes of data visible on screen
								   //unsigned long long EndByte; //ending byte of data visible on screen
								   //unsigned long long EndByte; //ending byte of data visible on screen
								   //long long StartTime;  //start time of FTControl (for track display) in ns, VisStartTime or CurStartTime
								   //long long Duration;  //duration of video/audio FTControl (in ns)
								   //long long StartTime;  //original start time of data in ns
								   //long long oDuration;  //original duration of video/audio FTControl (in ns)
	int OptionGroup;  //for option or "radio" buttons
					  //make ilist[] array?
					  //make image[] array?
	union {
		unsigned char *image[FTControl_MAX_NUM_IMAGES];
		struct {
			unsigned char *Normal_Image;
			unsigned char *MouseOver_Image;//image of control when mouse over
			unsigned char *ButtonDown_Image; //image of control when button is down or space bar is press when the control is selected or toggled on
			unsigned char *Focus_Image; //image of control when it has the focus (for example from tabbing)
		};
	};
	unsigned char *erase;
#if Linux
	union {
		XImage *ximage[FTControl_MAX_NUM_IMAGES];  //image underneath FileMenu, image on when button is up - should be array
		struct {
			XImage *Normal_XImage;
			XImage *MouseOver_XImage;
			XImage *ButtonDown_XImage;
			XImage *Focus_XImage;
		};
	};

	//need track number for delete? (is scaled to button by default, but can be not scaled)
	//XImage *ximage2;  //when mouse is over button
	//XImage *ximage3;  //when button is selected with button down or toggle
	XImage *xerase;  //background of movable/resizable FTControls
#endif //Linux
#if WIN32
					 //HBRUSH brush; //now using GetStockObject and SetDCBrushCOlor
					 //HPEN pen;
	RECT rect;
	union {
		HBITMAP hbmp[FTControl_MAX_NUM_IMAGES];  //hbmp of image
		struct {
			HBITMAP Normal_hbmp;
			HBITMAP MouseOver_hbmp;//info of image of control when mouse over
			HBITMAP ButtonDown_hbmp; //info of image of control when button is down or space bar is press when the control is selected or toggled on
			HBITMAP Focus_hbmp; //info of image when control has focus (is tabbed to)
		};
	};


#endif
	//make bmpinfo static? - otherwise need to constantly malloc
	union {
		BITMAPINFOHEADER *bmpinfo[FTControl_MAX_NUM_IMAGES];  //used to get dimensions of tcontrol->image, bpp - possibly make array
		struct {
			BITMAPINFOHEADER *Normal_bmpinfo;
			BITMAPINFOHEADER *MouseOver_bmpinfo;//info of image of control when mouse over
			BITMAPINFOHEADER *ButtonDown_bmpinfo; //info of image of control when button is down or space bar is press when the control is selected or toggled on
			BITMAPINFOHEADER *Focus_bmpinfo; //info of image when control has focus (is tabbed to)
		};
	};
	union {
		unsigned char *bmpdata[FTControl_MAX_NUM_IMAGES];  //used to get dimensions of tcontrol->image, bpp - possibly make array
		struct {
			unsigned char *Normal_bmpdata;
			unsigned char *MouseOver_bmpdata;//data of image of control when mouse over
			unsigned char *ButtonDown_bmpdata; //data of image of control when button is down or space bar is press when the control is selected or toggled on
			unsigned char *Focus_bmpdata; //data of image when control has focus (is tabbed to)
		};
	};

	//int bmpwidth; - to have a quick reference to 4-byte aligned padding width
	FTControl *last; //pointer to last FTControl in linked list
	FTControl *next;  //pointer to next FTControl in linked list
#if WIN32
	HFONT font; //possibly something in font or textmet is corrupted, because when fontwidth and fontheight are after it- they get corrupted: after AddFTControl, then GetFTControl- font!=0 but fontwidth=0 and fontheight=0
	TEXTMETRIC textmet;  //this occurred again- debug version didn't show it, but release was overwriting ButtonClick[0] function to 0- moving to bottom of control solved it
						 //possibly I found the error: tlist->textmet=tcontrol->textmet; - I can't just set textmet=, it needs a memcpy
#endif
};





			  //PStatus is used for all global variables
			  //either HStatus (HumanStatus variables) for each app
			  //or add global variables to PStatus - I guess PStatus would stop this from being a library?
			  //Program Status flags
			  //change to PButton...etc
			  //#define EButton1Hold    0x00000001  //button 1 is being held down
#define EButtonHold(a) 			(1<<a) //button a is being held down
#define ERecording      		0x00000008  //recording (audio) data -make local
#define EPlaying        		0x00000010  //playing (audio) data -make local
#define EMouseMove      		0x00000020  //after initial button down,mouse moving
#define EChanged        		0x00000040  //need to save again something changed
#define EResizeX1       		0x00000080  //resizing a FTControl to the left
#define EResizeX2       		0x00000100  //resizing a FTControl to the right
#define EResizeY1       		0x00000200  //resizing a FTControl to the top
#define EResizeY2       		0x00000400  //resizing a FTControl to the bottom
#define PExit           		0x00000800  //exit main()
#define PInfo           		0x00001000  //show info for finding errors
#define PAlt            		0x00002000  //alt key is pressed
#define PControl				0x00004000  //Control key is pressed
#define PShift          		0x00008000  //shift key is pressed
#define PNumLock        		0x00010000  //Number Lock is down
#define PScrollLock     		0x00020000  //Scroll Lock is down
#define PExclusive      		0x00040000  //some window has exclusive FTControl
#define PGotSelection   		0x00080000  //this program got a SelectionNotify 
#define PItemListOpen			0x00100000	//An FTItemList is open in some window in this program
#define PCloseItemListOnUp		0x00200000  //buttondown on an open filemenu sets this
#define PNoFocusHighlight		0x00400000	//do not draw outline around the control that has the focus
#define PNoMouseOverHighlight	0x00800000	//do not draw outline around the control that has the focus
#define PSelectingText			0x01000000 //user is selecting text
			  //#define PMouseMoveFunc  0x00100000  //Can only process 1 mousemove function at a time, because new
			  //mouse moves might cause a human func to have errors
			  //In theory the program could have OnTimer, and other callback functions
			  //and these would be in the ProgramStatus structure
typedef struct ProgramStatus {
	unsigned int flags;  //Program Status flags/flags EButtonHold, etc...
						 //FTWindow *ButtonWin[4];  //window that has FTControl button 1,2,3 is being held down on is
						 //int timespace;   ///space between time unit lines
	time_t ttime;  //stores time (for ctime)
	struct tm *ctime;  //current time 10:14:32  should be pointer? 
					   //ctime->tm_year=100 (2000) ctime->tm_hour=6;  ctime->tm_min=0;  ctime->tm_sec=0;
					   //int secs; //for now only store seconds for expose
					   //time_t ttime2;  //stores time (for lastexpose)
					   //struct tm *lastexpose;  //redraw on expose only after 1 second
	struct timeval utime;   //current time seconds and microseconds .tv_usec
	struct timeval timertime; //last time timer check ran - runs every 1ms
	struct timeval timertime2; //last time 100ms "selecting with mouse" timer check ran - runs every 100ms
	FTWindow *iwindow;
	FTWindow *ExclusiveFTWindow; //window that has exclusive control of input events
	FTControl *ExclusiveFTControl; //control that has exclusive control of input events
	FTWindow *ButtonFTWindow[FT_NUM_MOUSE_BUTTONS];  //window that has FTControl button 1,2,3 is being held down on is
	int ButtonX[FT_NUM_MOUSE_BUTTONS], ButtonY[FT_NUM_MOUSE_BUTTONS];  //location of button 1-3 being held down
	int DeltaButtonX[FT_NUM_MOUSE_BUTTONS], DeltaButtonY[FT_NUM_MOUSE_BUTTONS];  //difference between location of button 1-3 being held down and control x1 and y1]
	FTControl *ButtonDownFTControl[FT_NUM_MOUSE_BUTTONS]; //button down was on this control - so no need to loop through controls
	FTItem *SelectedFTItem; //currently selected FTItem
	FTItemList *SelectedFTItemList; //currently selected FTItemList - may not have selected item
									//int ExclWin; //window number that has exclusive FTControl (-1 is none?)
									//int tab; //current tab with focus
	FTControl *MouseOverFTControl; //FTControl mouse is over
	FTWindow *FocusFTWindow; //window that has focus
							 //FTWindow *curwin; //current window mouse is in
	int MouseX, MouseY;  //current mouse move location x,y
	int MouseXRoot, MouseYRoot;  //current mouse move location x,y absolute position
	int LastMouseX, LastMouseY;  //last mouse location
								 //int ButtonDownMouseX[FT_NUM_MOUSE_BUTTONS],ButtonDownMouseY[FT_NUM_MOUSE_BUTTONS]; //stores the mouse x,y when a button was pressed down, currently to remember where the button down was when selecting text in a textbox or textarea (because it could be between characters)
	int MouseHWheelDelta; //positive=mouse wheel rotated to right, negative=to left
	int MouseVWheelDelta; //positive=mouse wheel rotated forward away from user, negative=backward toward from user
						  //long long timescale;  //is now on control. for data FTControls. This represents the current number of pixels per nanosecond for displaying data in data controls. - perhaps should be on control or window
						  //unsigned char key; //needs to be unsigned char because of RCTL = 0x84
	KeySym key; //changed to KeySym codes, which are standard in X (XK_A, XK_a, XK_Shift_L, XK_Shift_R, etc)
	unsigned char asckey; //key in Latin-1 mapping
						  //unsigned int utfkey; //key in UTF8
	char Project[FTMedStr];
	char ProjectPath[FTBigStr];
	int PadX;  //not used, but available;//amount to pad text in horizontal file menus
	int PadY;  //amount to pad text in vertical file menus
	int ResizePix;  //number of pixels from edge of FTControl that will allow a resize
	int dclickdelay;
	int ScrollPixelsX, ScrollPixelsY; //number of pixels to scroll on each click on scroll arrow button
	unsigned int mb, mbflags; //FTMessageBox button pressed, arg1
	char mbmesg[FTMedStr];  //arg2 - need to store mb args because AddFTControls on has twin param
							//Cursor icursor;  //holds resize cursor
	FTControlfunc *FirstWindow; //function main_AddWindow the first window
#if Linux
	Display *xdisplay;
	Visual *visual;
	GC xgc;
#endif
	int depth; //screen depth
	int xscreen;
#if Linux
	Window RootWin;
#endif
	int rw, rh;  //root window width and height

				 //perhaps make these static strings
				 //FT_Prefix_ScrollUp
	char PrefixVScrollUp[20];  //prefix string for button controls created automatically for scroll bars
	char PrefixVScrollDown[20];
	char PrefixVScrollSlide[20];
	char PrefixVScrollBack[20];
	char PrefixHScrollLeft[20];  //prefix string for button controls created automatically for scroll bars
	char PrefixHScrollRight[20];
	char PrefixHScrollSlide[20];
	char PrefixHScrollBack[20];
	char PrefixName[20];  //prefix string for button controls created automatically for fileopen controls
	char PrefixSize[20];
	char PrefixDate[20];
#if WIN32
	HINSTANCE hInstance; //need from main to create windows
#endif
	int jpgbufsize;  //for compressing and decompressing jpgs
	unsigned char *jpgbufdata;  //for compressing and decompressing jpgs
								//pthread_mutex_t GetInput_Lock; //Lock to stop processing input (like control timers). Used when creating a window and the windo controls so the control timers are not processed in the middle of creating the window and controls
								//pthread_cond_t GetInput_Cond; //conditional variable that indicates that GetInput can be processed
#if WIN32
	HANDLE GetInputThread; //thread that periodically checks input
	DWORD GetInputThreadID;
	HANDLE GetInputMutex; //to lock GetInput while a window and its controls are being created
	HANDLE DrawFTControlsMutex; //to lock DrawFTControls from new calls- otherwise image data may get corrupted, functions should get this lock before updating image data
#endif
} ProgramStatus;





				 //change to FTW_
#define WOpen			0x0001 //Window is Open (and showing)
				 //#define WClose        0x0002 //now using simply no WOpen
				 //WItemListOpen - not needed is done by seeing if itemlist window is WOpen
				 //#define WItemListOpen 0x0004 //if a menu FTControl is open, do not do button highlighting
				 //#define WButtonDown   0x0004 //if a button is down, no mousemove highlighting (for now any button)
#define WOnlyOne      0x0008 //only one window with this name can be open at a time, used only for human reference
#define WExclusive    0x0010
#define WMsgBox       0x0020  //is a FTMessageBox window, if destroyed send FTMB_CANCEL
				 //#define WNotVisible   0x0080  //window is visible - replaced by WOpen
				 //perhaps should be Popup,FileMenu, etc...
				 //#define WMenu        0x0080  //filemenu and combo submenu no titlebar, etc...processing only looks for WMenu
#define WItemList		0x0040 //filemenu, combo, etc. and SubItem itemlist windows - no titlebar
#define WSplash      0x0080  //splash screen no titlebar, etc..
				 //#define WFileMenuOpen	0x0100	//a filemenu control itemlist is open in this window - not used
				 //#define WCursor     0x0004 //there is a cursor that will need to be changed back to normal icon
				 //#define WResize       0x0080 //resize just happened
				 //#define	WDisableTimer	0x0200	//disable any timer functions - can also be done with timer=-1  maybe WEnableTimer
#define WButtonDown(a) (0x0100<<a) //buttondown on this window
#define WWaitUntilDoneDrawing	0x1000 //wait for Window to be done drawing
#define WAVCodec_Init           0x2000 //FFMPEG AVCodecs are initialized
				 //#define WImage                  0x4000 //Image window - the only difference is that no window drawing occurs
#define FTW_SIMULATE_BUTTON_CLICK	0x4000 //When a hot key or tab selects a control, the CheckFTWindowButtonDown code can be used again but don't call any user buttondown functions
#define FTW_NEEDS_WM_PAINT			0x8000 //for windows WM_PAINT- otherwise DrawFTWindow is called everytime
#define FTW_CloseOnTimer	0x10000 //automatically close window after a length of time
struct FTWindow {
	int num;
	//int type; //Normal,Popup,FileMenu
	unsigned int flags;
	char name[FTMedStr];
	char title[FTMedStr];
	char ititle[FTMedStr];
	//int x1,y1;
	int w, h;  //should be x2 y2? need x1,x2? x1 y1 always=0?
	int x, y;  //position of window
	int z; //depth, only for WMenu
		   //int ow,oh; //original w,h (for resize that is done in fractions)
		   //int dw,dh; //last change in w,h (for resize)
	int minw, minh; ///minimum dimensions of window (when resizing)
	int visible;
	FTControl *FocusFTControl; //control with focus
							   //int focus;  //=tab# of control with focus, 0=window has focus,x=FTControl x has focus, current tab
							   //possibly implement focus as WHasFocus
	int inst; //instance number, could use inum, or could require unique window names 
	int timer; //time in ms to call any OnTimer function if window has an OnTimer function
	struct timeval timertime;   //last time timer called tv_sec and tv_usec
								//long CloseTime; //time that window should close (in milliseconds)
	struct timeval CloseTime;   //time the window should close (when utime is >= CloseTime)
	FTControlfuncwk *Keypress;  //windows (and also FTControls) have functions
	FTControlfuncw *OnOpen;  //after window opens
	FTControlfuncw *OnClose;  //before window closes
	FTControlfuncw *OnDraw;  //after drawwindow function (window and all FTControls are drawn)
	FTControlfuncw *OnMove;  //after move event
	FTControlfuncw *OnResize;  //after resize event
	FTControlfuncw *AddFTControls;  //function to add all FTControls
	FTControlfuncw *GotFocus;  //window got focus
	FTControlfuncw *LostFocus;  //window lost focus
	FTControlfuncwxy *ButtonDown[FT_NUM_MOUSE_BUTTONS];  //function is called when button 1-3 down in this win 
	FTControlfuncwxy *ButtonUp[FT_NUM_MOUSE_BUTTONS];  //function is called when button 1 up in this win
													   //buttondownup could be called "click"
	FTControlfuncwxy *ButtonDownUp[FT_NUM_MOUSE_BUTTONS];  //function is called when button up after down on this win
	FTControlfuncwxy *ButtonClick[FT_NUM_MOUSE_BUTTONS];  //function is identical to ButtonDownUp
	FTControlfuncwxy *ButtonDoubleClick[FT_NUM_MOUSE_BUTTONS];  //function called when button double-clicked in this win
	FTControlfuncwcxy *ButtonDownUpOnFTControl[FT_NUM_MOUSE_BUTTONS]; //need?  button 1 down up on FTControl (not on window)
	FTControlfuncwxy *MouseMove;  //mouse moved function
	FTControlfuncwxy *MouseVWheelMove;  //mouse vertical wheel moved function
	FTControlfuncwxy *MouseHWheelMove;  //mouse horizontal wheel moved function
	FTControlfuncw *OnTimer;  //called depending on timer interval variable
	FTControlfuncwc *HotKeyActivate; //a hotkey activated a control- need to find control before calling
									 //FTControl *iFTControl;  //all the FTControls on this window
	FTControl *icontrol;  //all the FTControls on this window
	FTItemList *ilist; //FTItemList associated with this window (only for WItemList)
	unsigned char *image;
#if Linux
	XImage *ximage;  //windows can have an image on them
#endif
					 //make bmpinfo static - otherwise need to constantly malloc
	BITMAPINFOHEADER *bmpinfo;
	unsigned char *bmpdata; //in order to keep track of to free malloc'd memory
							//start FFMPEG variables
	unsigned char *jpgdata; //possibly just *data? or *data[10]
#if Linux
#if USE_FFMPEG
	AVCodec *avc;  //format to save video calls - currently set to MJPEG
	AVCodecContext *avcc;
	struct SwsContext  *swsctx;
	uint8_t *sws_data_out;
	uint8_t *sws_dest_out[3];
	int sws_stride_out[3];
	uint8_t *mjpeg_frame;
	int mjpeg_frame_size;
	AVFrame *mjpeg_bmp;  //
	AVFrame *mjpeg_yuv;
	//end FMMPEG variables
#endif //#if USE_FFMPEG
#endif  //Linux
#if Linux
	Time lastclick[FT_NUM_MOUSE_BUTTONS]; //last button up buttons 0-4
#endif
#if WIN32
	long lastclick[FT_NUM_MOUSE_BUTTONS]; //last button up buttons 0-4
#endif
	float scalex, scaley; //when resize store ratio
#if Linux
	Window xwindow;  //X11 window pointer, can be as many windows as wanted
	GC xgc;  //X11 pointer to GC
	XFontStruct *font; //Needed for ItemList windows
#endif
#if WIN32
	HWND hwindow; //pointer to window
	WNDCLASS wclass; //window class
	HINSTANCE hinst; // current instance
	HDC hdcWin;
	HDC hdcMem;
	HBRUSH brush;
	HPEN pen;
	RECT rect;
	HFONT font;
	TEXTMETRIC textmet;
	HBITMAP hbmp;
	int WinXOffset; //Windows X coordinates (button click locations, etc) are offset by titlebars, frames, etc.- this stores that offset 
	int WinYOffset; //Windows Y coordinates (button click locations, etc) are offset by titlebars, frames, etc.- this stores that offset 
#endif
	int fontwidth;
	int fontheight;
	unsigned int color; //color of window
						//FTWindow *last; //or *prev or *previous - pointer to previous window
	FTWindow *next;
};







			 //for centering text on buttons, this struct holds number of words, start index of word, and length of word
typedef struct FTWords {
	int num; //number of words in list
	int i[FTMedStr]; //start index of word
	int len[FTMedStr]; //length of word
} FTWords;


#define NumLockMask Mod2Mask //0x10
#define ScrollLockMask Mod5Mask //0x80
#define xeventmask (ButtonPressMask|ButtonReleaseMask|KeyPressMask|KeyReleaseMask|ButtonMotionMask|ExposureMask|PointerMotionMask|FocusChangeMask|StructureNotifyMask|PropertyChangeMask|VisibilityChangeMask)


//This is not good, have to define just to make a splash window (a window with no titlebar, etc...)
/* bit definitions for MwmHints.flags */
#define MWM_HINTS_FUNCTIONS	(1L << 0)
#define MWM_HINTS_DECORATIONS	(1L << 1)
#define MWM_HINTS_INPUT_MODE	(1L << 2)
#define MWM_HINTS_STATUS	(1L << 3)

/* bit definitions for MwmHints.decorations */
#define MWM_DECOR_ALL		(1L << 0)
#define MWM_DECOR_BORDER	(1L << 1)
#define MWM_DECOR_RESIZEH	(1L << 2)
#define MWM_DECOR_TITLE		(1L << 3)
#define MWM_DECOR_MENU		(1L << 4)
#define MWM_DECOR_MINIMIZE	(1L << 5)
#define MWM_DECOR_MAXIMIZE	(1L << 6)

#define MWM_INPUT_MODELESS 0
#define MWM_INPUT_PRIMARY_APPLICATION_MODAL 1
#define MWM_INPUT_SYSTEM_MODAL 2
#define MWM_INPUT_FULL_APPLICATION_MODAL 3
#define MWM_INPUT_APPLICATION_MODAL MWM_INPUT_PRIMARY_APPLICATION_MODAL

#define MWM_TEAROFF_WINDOW	(1L<<0)

typedef struct
{
	unsigned long	flags;
	unsigned long	functions;
	unsigned long	decorations;
	long 	        inputMode;
	unsigned long	status;
} PropMotifWmHints;

#define PROP_MOTIF_WM_HINTS_ELEMENTS	5

typedef struct _extended_hints
{
	int flags;
	int desktop;
}
ExtendedHints;

#define MWM_HINTS_FUNCTIONS     (1L << 0)
#define MWM_HINTS_DECORATIONS   (1L << 1)
#define MWM_HINTS_INPUT_MODE    (1L << 2)
#define MWM_HINTS_STATUS        (1L << 3)

#define MWM_FUNC_ALL            (1L << 0)
#define MWM_FUNC_RESIZE         (1L << 1)
#define MWM_FUNC_MOVE           (1L << 2)
#define MWM_FUNC_MINIMIZE       (1L << 3)
#define MWM_FUNC_MAXIMIZE       (1L << 4)
#define MWM_FUNC_CLOSE          (1L << 5)

#define MWM_DECOR_ALL           (1L << 0)
#define MWM_DECOR_BORDER        (1L << 1)
#define MWM_DECOR_RESIZEH       (1L << 2)
#define MWM_DECOR_TITLE         (1L << 3)
#define MWM_DECOR_MENU          (1L << 4)
#define MWM_DECOR_MINIMIZE      (1L << 5)
#define MWM_DECOR_MAXIMIZE      (1L << 6)

#define MWM_INPUT_MODELESS 0
#define MWM_INPUT_PRIMARY_APPLICATION_MODAL 1
#define MWM_INPUT_SYSTEM_MODAL 2
#define MWM_INPUT_FULL_APPLICATION_MODAL 3
#define MWM_INPUT_APPLICATION_MODAL MWM_INPUT_PRIMARY_APPLICATION_MODAL

#define MWM_TEAROFF_WINDOW	(1L<<0)


#define PROP_MWM_HINTS_ELEMENTS       PROP_MOTIF_WM_HINTS_ELEMENTS

#define EXTENDED_HINT_STICKY            (1<<0)
#define EXTENDED_HINT_ONTOP             (1<<1)
#define EXTENDED_HINT_ONBOTTOM          (1<<2)
#define EXTENDED_HINT_NEVER_USE_AREA    (1<<3)
#define EXTENDED_HINT_DESKTOP           (1<<4)


#if WIN32
//probably FT_ESCAPE would be a more generic form instead of using XWindows names
//Windows Virtual Key codes:
//http://msdn.microsoft.com/en-us/library/ms927178.aspx?ppud=4
#define XK_Page_Up 240 //VK_PRIOR=33 (0x21)  //had to be changed - see WM_KEYDOWN: in freethought.c
#define XK_Page_Down 241 //VK_NEXT
#define XK_Escape VK_ESCAPE
#define XK_Return VK_RETURN
#define XK_Up 242 //VK_UP   //had to be changed - see WM_KEYDOWN: in freethought.c
#define XK_Down 243 //VK_DOWN  40 0x28
#define XK_Left 244 //VK_LEFT  //0x25
#define XK_Right 245 //VK_RIGHT  //00x27
#define XK_Tab VK_TAB
#define XK_Caps_Lock VK_CAPITAL
#define XK_Control_L VK_LCONTROL
#define XK_Control_R VK_RCONTROL
#define XK_Shift_L VK_LSHIFT
#define XK_Shift_R VK_RSHIFT
#define XK_Alt_L VK_LMENU
#define XK_Alt_R VK_RMENU
#define XK_Num_Lock VK_NUMLOCK
#define XK_Scroll_Lock VK_SCROLL
#define XK_Home 246 //had to be changed - VK_HOME  - case WM_KEYDOWN: in freethought.c
#define XK_End 247 //VK_END
#define XK_Delete 248 //VK_DELETE
#define	XK_Insert 249 //?
#define XK_BackSpace VK_BACK
#define XK_A 65
#define XK_B 66
#define XK_C 67
#define XK_D 68
#define XK_E 69
#define XK_F 70
#define XK_G 71
#define XK_H 72
#define XK_I 73
#define XK_J 74
#define XK_K 75
#define XK_L 76
#define XK_M 77
#define XK_N 78
#define XK_O 79
#define XK_P 80
#define XK_Q 81
#define XK_R 82
#define XK_S 83
#define XK_T 84
#define XK_U 85
#define XK_V 86
#define XK_W 87
#define XK_X 88
#define XK_Y 89
#define XK_Z 90
#define XK_a 97
#define XK_b 98
#define XK_c 99
#define XK_d 100
#define XK_e 101
#define XK_f 102
#define XK_g 103
#define XK_h 104
#define XK_i 105
#define XK_j 106
#define XK_k 107
#define XK_l 108
#define XK_m 109
#define XK_n 110
#define XK_o 111
#define XK_p 112
#define XK_q 113
#define XK_r 114
#define XK_s 115
#define XK_t 116
#define XK_u 117
#define XK_v 118
#define XK_w 119
#define XK_x 120
#define XK_y 121
#define XK_z 122
//I decided currently not to try to remap all the XK keys from Xwindows to the Windows system which treats all numpad keys the same
//#define XK_KP_Left 250//VK_NUMPAD4 //- mapped to a new number to give a key code to when a numpad key is pressed with numlock on
//#define XK_KP_Right 251//VK_NUMPAD6
//#define XK_KP_Down 252//VK_NUMPAD2
//#define XK_KP_Up 253//VK_NUMPAD8
//todo: add Insert Delete Help and others see http://msdn.microsoft.com/en-us/library/ms927178.aspx
#define XK_KP_Begin VK_SELECT //?middle key
#define XK_KP_0 VK_NUMPAD0
#define XK_KP_1 VK_NUMPAD1
#define XK_KP_2 VK_NUMPAD2
#define XK_KP_3 VK_NUMPAD3
#define XK_KP_4 VK_NUMPAD4
#define XK_KP_5 VK_NUMPAD5
#define XK_KP_6 VK_NUMPAD6
#define XK_KP_7 VK_NUMPAD7
#define XK_KP_8 VK_NUMPAD8
#define XK_KP_9 VK_NUMPAD9
//#define XK_KP_Insert VK_NUMPAD0
#define XK_KP_Delete VK_DELETE 
#define XK_KP_Multiply VK_MULTIPLY
#define XK_KP_Add VK_ADD
#define XK_KP_Separator VK_SEPARATOR
#define XK_KP_Subtract VK_SUBTRACT
#define XK_KP_Decimal VK_DECIMAL
#define XK_KP_Divide VK_DIVIDE
#define XK_period 190
#define XK_slash 191
#define XK_space VK_SPACE
#define XK_comma VK_OEM_COMMA
#define XK_0 48
#define XK_1 49
#define XK_2 50
#define XK_3 51
#define XK_4 52
#define XK_5 53
#define XK_6 54
#define XK_7 55
#define XK_8 56
#define XK_9 57
#define XK_Num_Lock VK_NUMLOCK
#define XK_Scroll_Lock VK_SCROLL
#define XK_F1 VK_F1
#define XK_F2 VK_F2
#define XK_F3 VK_F3
#define XK_F4 VK_F4
#define XK_F5 VK_F5
#define XK_F6 VK_F6
#define XK_F7 VK_F7
#define XK_F8 VK_F8
#define XK_F9 VK_F9
#define XK_F10 VK_F10
#define XK_F11 VK_F11
#define XK_F12 VK_F12
#define XK_F13 VK_F13
#define XK_F14 VK_F14
#define XK_F15 VK_F15
#define XK_F16 VK_F16
#define XK_F17 VK_F17
#define XK_F18 VK_F18
#define XK_F19 VK_F19
#define XK_F20 VK_F20
#define XK_F21 VK_F21
#define XK_F22 VK_F22
#define XK_F23 VK_F23
#define XK_F24 VK_F24

#endif //WIN32

//these are for 24 and 32 bit color, 16 bit is different
#define FT_BLACK			0x000000
#define FT_WHITE			0xffffff
#define FT_DARKEST_GRAY		0x1f1f1f
#define FT_DARKER_GRAY		0x3f3f3f
#define FT_DARK_GRAY		0x5f5f5f
#define FT_GRAY				0x7f7f7f
#define FT_LIGHT_GRAY		0x9f9f9f
#define FT_WINDOW_GRAY		0xdfdfdf
#define FT_WINDOW_GRAY16	0x4108
#define FT_LIGHTER_GRAY		0xbfbfbf
#define FT_LIGHTEST_GRAY	0xdfdfdf
#define FT_DARKEST_GREEN	0x003f00
#define FT_DARKER_GREEN		0x007f00
#define FT_DARK_GREEN		0x00bf00
#define FT_GREEN			0x00ff00
#define FT_LIGHT_GREEN		0x3fff3f
#define FT_LIGHTER_GREEN	0x7fff7f
#define FT_LIGHTEST_GREEN	0xbfffbf
#define FT_DARKEST_PINK		0xff1fff
#define FT_DARKER_PINK		0xff3fff
#define FT_DARK_PINK		0xff4fff
#define FT_PINK				0xff7fff
#define FT_LIGHT_PINK		0xff9fff
#define FT_LIGHTER_PINK		0xffbfff
#define FT_LIGHTEST_PINK	0xffdfff
#if Linux
#define FT_DARKEST_RED		0x3f0000
#define FT_DARKER_RED		0x7f0000
#define FT_DARK_RED			0xbf0000
#define FT_RED				0xff0000
#define FT_LIGHT_RED		0xff3f3f
#define FT_LIGHTER_RED		0xff7f7f
#define FT_LIGHTEST_RED		0xffbfbf
#define FT_DARKEST_YELLOW	0x9f9f00
#define FT_DARKER_YELLOW	0xbfbf00
#define FT_DARK_YELLOW		0xdfdf00
#define FT_YELLOW			0xffff00
#define FT_LIGHT_YELLOW		0xffff1f
#define FT_LIGHTER_YELLOW	0xffff3f
#define FT_LIGHTEST_YELLOW	0xffff7f
#define FT_DARKEST_BLUE		0x00003f
#define FT_DARKER_BLUE		0x00007f
#define FT_DARK_BLUE		0x0000bf
#define FT_BLUE				0x0000ff
#define FT_LIGHT_BLUE		0x3f3fff
#define FT_LIGHTER_BLUE		0x7f7fff
#define FT_LIGHTEST_BLUE	0xbfbfff
#define FT_DARKEST_PURPLE	0x1f009f
#define FT_DARKER_PURPLE	0x3f00bf
#define FT_DARK_PURPLE		0xdf005f
#define FT_PURPLE			0x7f00ff
#define FT_LIGHT_PURPLE		0x9f1fff
#define FT_LIGHTER_PURPLE	0xbf3fff
#define FT_LIGHTEST_PURPLE	0xdf5fff
#define FT_DARKEST_ORANGE	0x9f1f00
#define FT_DARKER_ORANGE	0xbf3f00
#define FT_DARK_ORANGE		0xdf5f00
#define FT_ORANGE			0xff7f00
#define FT_LIGHT_ORANGE		0xff9f1f
#define FT_LIGHTER_ORANGE	0xffbf3f
#define FT_LIGHTEST_ORANGE	0xffdf5f
#define FT_DARKEST_BROWN	0x3f3100
#define FT_DARKER_BROWN		0x564300
#define FT_DARK_BROWN		0x685100
#define FT_BROWN			0x8e6f00
#define FT_LIGHT_BROWN		0xa07d00
#define FT_LIGHTER_BROWN	0xc19700
#define FT_LIGHTEST_BROWN	0xd8a900
#endif //Linux
#if WIN32
#define FT_DARKEST_RED		0x00003f
#define FT_DARKER_RED		0x00007f
#define FT_DARK_RED			0x0000bf
#define FT_RED				0x0000ff
#define FT_LIGHT_RED		0x3f3fff
#define FT_LIGHTER_RED		0x7f7fff
#define FT_LIGHTEST_RED		0xbfbfff
#define FT_DARKEST_YELLOW	0x009f9f
#define FT_DARKER_YELLOW	0x00bfbf
#define FT_DARK_YELLOW		0x00dfdf
#define FT_YELLOW			0x00ffff
#define FT_LIGHT_YELLOW		0x1fffff
#define FT_LIGHTER_YELLOW	0x3fffff
#define FT_LIGHTEST_YELLOW	0x7fffff
#define FT_DARKEST_BLUE		0x3f0000
#define FT_DARKER_BLUE		0x7f0000
#define FT_DARK_BLUE		0xbf0000
#define FT_BLUE				0xff0000
#define FT_LIGHT_BLUE		0xff3f3f
#define FT_LIGHTER_BLUE		0xff7f7f
#define FT_LIGHTEST_BLUE	0xffbfbf
#define FT_DARKEST_PURPLE	0x9f001f
#define FT_DARKER_PURPLE	0xbf003f
#define FT_DARK_PURPLE		0xdf005f
#define FT_PURPLE			0xff007f
#define FT_LIGHT_PURPLE		0xff1f9f
#define FT_LIGHTER_PURPLE	0xff3fbf
#define FT_LIGHTEST_PURPLE	0xff5fdf
#define FT_DARKEST_ORANGE	0x001f9f
#define FT_DARKER_ORANGE	0x003fbf
#define FT_DARK_ORANGE		0x005fdf
#define FT_ORANGE			0x007fff
#define FT_LIGHT_ORANGE		0x1f9fff
#define FT_LIGHTER_ORANGE	0x3fbfff
#define FT_LIGHTEST_ORANGE	0x5fdfff
#define FT_DARKEST_BROWN	0x00313f
#define FT_DARKER_BROWN		0x004356
#define FT_DARK_BROWN		0x005168
#define FT_BROWN			0x006f8e
#define FT_LIGHT_BROWN		0x007da0
#define FT_LIGHTER_BROWN	0x0097c1
#define FT_LIGHTEST_BROWN	0x00a9d8
#endif //WIN32


#define FT_MAXJPGSIZE 1000000 //1 mb
//Flags for FTLoadImageToFTControl()
#define FT_NORMAL_IMAGE 0x0
#define FT_SELECTED_IMAGE 0x1
#define FT_BUTTONDOWN_IMAGE 0x2
#define FT_SCALE_IMAGE					0x0100
#define FT_SHADE_IMAGE					0x0200
#define FT_ADD_BORDER_TO_IMAGE			0x0400
#define FT_ADD_DASHED_BORDER_TO_IMAGE	0x0800



//NOTE: freethought.h currently includes all function prototypes in this single header file so that users can simply add a single:
//#include "freethought.h
//statement without having to track down every include file the function they are using is referring to, in addition for backward
//compatability.

//PROTOTYPES FROM OTHER SOURCE FILES
//FT_Windows.h
	//PROTOTYPES
	int SetDefaultFontForFTWindow(FTWindow *twin);
	int GetFreeFTWindowNum(void);
	FTWindow *CreateFTWindow(FTWindow *w);
	FTWindow * GetFTWindowN(int num);
	FTWindow * GetFTWindow(char *name);
	FTWindow * GetFTWindowI(char *name, int inst);
#if WIN32
	FTWindow * GetFTWindowFromhWnd(HWND hwindow);
#endif
	void CreatehWindow(FTWindow *twin);
	void OpenFTWindow(FTWindow *twin);
	void ResizeFTWindow(FTWindow *twin);
	void DrawFTWindowN(int num);
	void DrawFTWindow(FTWindow *twin);
	int GetFTWindowInstance(char *name);
	void CloseFTWindowNum(int num);
	void CloseFTWindowByName(char *name);
	void CloseFTWindowI(char *name, int inst);
	void CloseFTWindow(FTWindow *twin);
	void DestroyFTWindow(FTWindow *twin);
	void FT_SetFocus(FTWindow *twin, FTControl *tcontrol);
	void FT_LoseFocus(FTWindow *twin, FTControl *tcontrol);
#if Linux
	FTWindow * GetwinFromX(Window xwin);
#endif
	void ResizeFTWindowControls(FTWindow *twin, int x, int y, int dw, int dh);
	unsigned int FTMessageBox(char *message, unsigned int flags, char *title, long delay);
	void FTMB_AddFTControls(FTWindow *twin);
	void mbok_ButtonDownUp(FTWindow *twin, FTControl *tcontrol, int x, int y);
	void mbcancel_ButtonDownUp(FTWindow *twin, FTControl *tcontrol, int x, int y);
	void mbyes_ButtonDownUp(FTWindow *twin, FTControl *tcontrol, int x, int y);
	void mbno_ButtonDownUp(FTWindow *twin, FTControl *tcontrol, int x, int y);
	void mbyesall_ButtonDownUp(FTWindow *twin, FTControl *tcontrol, int x, int y);
	void mbnoall_ButtonDownUp(FTWindow *twin, FTControl *tcontrol, int x, int y);

//End FT_Windows.h
//FT_Controls.h

FTControl * GetFTControl(char *name);
FTControl * GetFTControlW(FTWindow *twin, char *name);
int GetFreeOptionGroupNumber(FTWindow *twin);
int CheckTab(FTControl *c);
int GetNextTab(FTWindow *twin, int tab);
int GetPrevTab(FTWindow *twin, int tab);
FTControl * GetFTControlWithTab(FTWindow *twin, int tab);
FTControl * GetNextTabFTControl(FTWindow *twin, int onlyfm);
FTControl * GetPrevTabFTControl(FTWindow *twin, int onlyfm);
long long GetOpenDataNum(void);
FTControl *AddFTControl(FTWindow *twin, FTControl *c);
void AddFileOpenFTControls(FTWindow *twin, FTControl *tcontrol);
void AddScrollFTControls(FTWindow *twin, FTControl *tcontrol);
void DelFTControlAll(FTWindow *twin);
void DelFTControl(FTControl *tcontrol);
void DelFileOpenFTControls(FTControl *tcontrol);
void DelScrollFTControls(FTControl *tcontrol);
int SetDefaultFontForFTControl(FTControl *tcontrol);
void AddFTItem(FTControl *tcontrol, FTItem *titem);
void AddFTSubItem(FTItem *titem, FTItem *titem2);
int FTstrlen(char *tstr);
FTItem * GetFTItemFromFTControl(FTControl *tcontrol, char *name);
FTItem * GetFTItemFromFTControlByNum(FTControl *tcontrol, int num);
FTItem * GetFTItemFromFTItemList(FTItemList *tlist, char *name);
FTItem * GetFTItemFromFTItemListByNum(FTItemList *tlist, int num);
int SelectFTItemByName(FTItemList *tlist, char *name, unsigned int flags);
int UnselectFTItemByName(FTItemList *tlist, char *name);
int SelectFTItemByNum(FTItemList *tlist, int num, unsigned int flags);
int UnselectFTItemByNum(FTItemList *tlist, int num);
int SelectFTItem(FTItem *titem, unsigned int flags);
int UnselectFTItem(FTItem *titem);
int UnselectAllFTItems(FTItemList *tlist);
int DelFTItem(FTControl *tcontrol, char *name);
int DelFTItemByNum(FTControl *tcontrol, int num);
int DelAllFTItems(FTControl *tcontrol);
int DelFTItemList(FTItemList *tlist);
int DelFTSubItem(FTItem *titem, char *name);
int DelFTSubItemByNum(FTItem *titem, int num);
void SortFTItemList(FTItemList *tlist, int order, int direct);
void OpenNextFileControl(FTControl *tcontrol);
void OpenPrevFileControl(FTControl *tcontrol);
int OpenFTItemList(FTItemList *tlist);
void DrawFTItemList(FTItemList *tlist);
void DrawSelectedFTItem(FTItemList *tlist, int num);
void DrawUnselectedFTItem(FTItemList *tlist, int num);
void CloseFTItemList(FTItemList *tlist);
void CloseFTItemListControl(FTControl *tcontrol);
void CloseAllItemLists(void);
void ScrollFTItemList(FTWindow *twin, FTControl *tcontrol, unsigned int flags,int ScrollInc);
FTItem *GetFTItemFromFTItemListXY(FTItemList *tlist, int x, int y);
void FTItemButtonUp(FTItemList *tlist, int x, int y, int button);
int GetNumCharsInRowFromStart(char *tmpstr, int i, int maxwidth);
int GetNumCharsInRowFromEnd(char *tmpstr, int i, int maxwidth);
void GetFTControlBackground(FTControl *tcontrol);
void EraseFTControl(FTControl *tcontrol);
int FTDrawText(FTWindow *twin, char *tstr, int numc, int fw, int fh, int cx, int cy);
void DrawButtonFTControls(FTControl *tcontrol);
int SplitTextIntoWords(FTWords *twords, char *tstr);
void DrawFTControl(FTControl *tcontrol);
void DrawFTControls(FTWindow *twin, FTControl *tcontrol);
void SetFolderTabDimensions(FTControl *tcontrol);
//End FT_Controls.h

//Include FT_Graphics.h
#if Linux
#if USE_FFMPEG
//FFMPEG
#include "libavcodec/avcodec.h"  
#include "libswscale/swscale.h"
#endif //USE_FFMPEG 
#endif //Linux
#if WIN32
#include <MMSystem.h>
#include <time.h>
#include <signal.h> //needed for setitimer SIGALARM
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>   /* File control definitions */
#include <string.h>  /* String function definitions */
#include <math.h>   /* for pow - why not ^? */

//LIBJPEG
#include "jpeglib.h"
#include "jconfig.h"
#include "jerror.h"
#include "jmorecfg.h"
//#include <setjmp.h>

unsigned char *FT_CreateBitmap(int w,int h,int bpp,unsigned char *data);
unsigned char *FT_CreateBitmapHeader(int w,int h,int bpp);
#if Linux
XImage * FT_LoadBitmapXImage(char *filename);
#endif
unsigned char * FT_LoadBitmap(char *filename);
unsigned char * FT_LoadBitmapFileAndScale(char *filename,int x,int y);
unsigned char *FT_LoadBitmapFileAndShade(char *filename,int x,int y,unsigned int mixcolor);
unsigned char * FT_LoadBitmapAndShade(unsigned char *bmfile,unsigned int mixcolor);
int LoadImageFileToFTControl(char *filename,FTControl *tcontrol,unsigned int flags,unsigned int mixcolor);
int LoadBitmapToFTControl(unsigned char *bmpfile,FTControl *tcontrol,unsigned int flags,unsigned int mixcolor);
int LoadBitmapFileToFTControl(char *filename,FTControl *tcontrol,unsigned int flags,unsigned int mixcolor);
void WriteBitmap(unsigned char *name, unsigned char *bmfile);
int LoadBitmapToFTWindow(unsigned char *bmfile,FTWindow *twin,int ResizeWindow);
unsigned char * LoadBitmapDataToFTWindow(unsigned char *bmpdata,FTWindow *twin,int ResizeWindow,int width,int height,int depth);
int LoadBitmapFileToFTWindow(char *filename,FTWindow *twin);
int LoadJPGFileToFTWindow(char *filename,FTWindow *twin);
int LoadJPGFileToFTControl(char *filename,FTControl *tcontrol,unsigned int flags,unsigned int mixcolor);
int LoadJPGToFTControl(unsigned char *jpgdata, int jpglen, FTControl *tcontrol, unsigned int flags, unsigned int mixcolor);
int CheckJPG(unsigned char *jpgdata, int jpglen);
void mem_init_source (j_decompress_ptr cinfo);
boolean mem_fill_input_buffer (j_decompress_ptr cinfo);
void mem_skip_input_data (j_decompress_ptr cinfo, long num_bytes);
boolean mem_resync_to_restart (j_decompress_ptr cinfo, int desired);
void mem_term_source (j_decompress_ptr cinfo);
#if Linux
#if USE_FFMPEG
void FTInit_AVCodec(FTWindow *twin);
int ConvertFTWinJPGtoBMP(FTWindow *twin);
int ConvertJPGtoBMPmalloc(unsigned char *jpg,int jpglen,unsigned char **bmp);
int ConvertJPGtoBMPData(unsigned char *jpg,int jpglen,unsigned char *bmp,int w,int h, int d);
void mem_init_destination(j_compress_ptr cinfo);
boolean mem_empty_output_buffer(j_compress_ptr cinfo);
void mem_term_destination(j_compress_ptr cinfo);
void ConvertBMPtoJPGFile(unsigned char *bmp,char *name,int quality);
void ConvertBMPtoJPG(unsigned char *bmp,int quality,unsigned char *jpg,int *jlen);
void ConvertBMPDatatoJPG(unsigned char *bdata,int quality,int w,int h,int BytesPerPixel,unsigned char *jpg,int *jlen);
void ConvertBMPtoMJPGFrame(unsigned char *bmp,int quality,unsigned char *jpg,int *jlen);
int ConvertJPGtoBMPData_LibJPEG(unsigned char *jpg,int jpglen,unsigned char *bmpdata,int w,int h, int d);
#endif //USE_FFMPEG
#endif //Linux
//End FT_Graphics.h


//FT_Main.h
#if Linux
	int InitX(void);
#endif
#if WIN32
	void InitWindows(void);
#endif
	void InitFreeThought(void);
	int CloseFreeThought(void);
	void catch_alarm(int sig);
	void printhelp(void);
//End FT_Main.h

//FT_UserInput.h

//PROTOTYPES
	void FT_GetInput(void);
#if Linux
	void ProcessEvents(void);
#endif
#if WIN32
	LRESULT CALLBACK ProcessEvents(HWND hwindow, UINT message, WPARAM wParam, LPARAM lParam);
#endif
	void btnfoName_OnResize(FTWindow *twin, FTControl *tcontrol, int x, int y);
	void btnfoSize_OnResize(FTWindow *twin, FTControl *tcontrol, int x, int y);
	void btnfoDate_OnResize(FTWindow *twin, FTControl *tcontrol, int x, int y);
	void btnfoName_DownUp(FTWindow *twin, FTControl *tcontrol, int x, int y);
	void btnfoSize_DownUp(FTWindow *twin, FTControl *tcontrol, int x, int y);
	void btnfoDate_DownUp(FTWindow *twin, FTControl *tcontrol, int x, int y);
	int UnselectFolderTab(FTControl *tcontrol);
	int SelectFolderTab(FTControl *tcontrol);
	int UnselectFolderTabGroup(FTControl *tcontrol);
	int SelectFolderTabGroup(FTControl *tcontrol);
	void CheckHotKeys(FTWindow *twin, KeySym key);
	int CheckHotKeyFTControl(FTControl *tcontrol, KeySym key);
	int CheckHotKeyFTItem(FTItem *tFTItem, KeySym key);
	//void ButtonDownUp(FTControl *tcontrol);
	void CheckFTWindowButtonDownHold(FTWindow *twin, int x, int y, int button);
	void SimulateButtonClick(FTWindow *twin, int x, int y, int button);
	void CheckFTWindowButtonDown(FTWindow *twin, int x, int y, int button);
	void CheckFTWindowButtonUp(FTWindow *twin, int x, int y, int button);
	void CheckFTWindowMouseMove(FTWindow *twin, int x, int y);
	void CheckFTWindowMouseVWheelMove(FTWindow *twin, int x, int y, int Delta);
	void CheckFTWindowButtonDoubleClick(FTWindow *twin, int x, int y, int button);
	void DelSelect(FTControl *tcontrol);
	void GoToNextTab(FTWindow *twin);
	void GoToPrevTab(FTWindow *twin);
	void CheckFTWindowKeypress(FTWindow *twin, KeySym key);
	void VScrollUp_ButtonDown(FTWindow *twin, FTControl *tcontrol, int x, int y, int button);
	void VScrollDown_ButtonDown(FTWindow *twin, FTControl *tcontrol, int x, int y, int button);
	void VScrollBack_ButtonDown(FTWindow *twin, FTControl *tcontrol, int x, int y, int button);
	void VScrollSlide_Button0DownMouseMove(FTWindow *twin, FTControl *tcontrol, int x, int y);
	void CalcVScrollSlide(FTWindow *twin, FTControl *tcontrol);
	void UpdateVScrollSlide(FTControl *tcontrol);
	void FT_TextAreaPgUp(FTControl *tcontrol, int MoveCursor);
	void FT_TextAreaPgDown(FTControl *tcontrol, int MoveCursor);
	void FT_TextAreaUp(FTControl *tcontrol, int MoveCursor);
	void FT_TextAreaDown(FTControl *tcontrol, int MoveCursor);
	void FT_FrameScrollDown(FTWindow *twin, FTControl *tcontrol, int Page);
	void FT_FrameScrollUp(FTWindow *twin, FTControl *tcontrol, int Page);
	void HScrollLeft_ButtonDown(FTWindow *twin, FTControl *tcontrol, int x, int y, int button);
	void HScrollRight_ButtonDown(FTWindow *twin, FTControl *tcontrol, int x, int y, int button);
	void HScrollBack_ButtonDown(FTWindow *twin, FTControl *tcontrol, int x, int y, int button);
	void UpdateItemList(FTControl *tcontrol);
	void HScrollSlide_Button0DownMouseMove(FTWindow *twin, FTControl *tcontrol, int x, int y, int custom);
	void CalcHScrollSlide(FTWindow *twin, FTControl *tcontrol);
	void UpdateHScrollSlide(FTControl *tcontrol);
	void FT_FrameScrollLeft(FTWindow *twin, FTControl *tcontrol, int Page);
	void FT_FrameScrollRight(FTWindow *twin, FTControl *tcontrol, int Page);
	void AdjustDataFTControlDimensions(FTControl *tcontrol);
	void FT_AdvanceTimeLine(FTWindow *twin, FTControl *tTimeLine, int x, int y);
	void FT_SelectData(FTWindow *twin, FTControl *tcontrol, int x, int y);
//End FT_UserInput.h

//FT_Utils.h
#ifdef WIN32
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

struct timezone
{
	int  tz_minuteswest; /* minutes W of Greenwich */
	int  tz_dsttime;     /* type of dst correction */
};
#endif //ifdef WIN32

#ifdef WIN32
int gettimeofday(struct timeval *tv, struct timezone *tz);
#endif //WIN32
void GetTime(void);
void FT_FormatTime(char *text, long double itime, int format, long double TimeScale, int TimeSpace);
//End FT_Utils.h




//END PROTOTYPES FROM OTHER SOURCEFILES

#endif //_FREETHOUGHT_H
