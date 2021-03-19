//win_openproject.h

int openproject_CreateFTWindow(void);
//void win4key(FTWindow *twin,u8 key);
void win4key(FTWindow *twin,KeySym key);
int openproject_AddFTControls(FTWindow *twin);
void openproject_OnResize(void);
//void foOpenFile2_DownUp(FTWindow *twin,FTControl *tFTControl,int x,int y);
void foOpenFile2_DoubleClick(FTWindow *twin,FTControl *tFTControl,int x,int y);
//void s1_foOpenFile2_DownUp(FTWindow *twin,FTControl *tFTControl,int x,int y);
//void s2_foOpenFile2_DownUp(FTWindow *twin,FTControl *tFTControl,int x,int y);
//void foOpenFile2_Keypress(FTWindow *twin,FTControl *tFTControl,u8 key);
void foOpenFile2_Keypress(FTWindow *twin,FTControl *tFTControl,KeySym key);
//void foOpenFile2_OnDraw(FTWindow *twin,FTControl *tFTControl);
//void sm_foOpenFile2_DownUp(FTWindow *twin,FTControl *tFTControl,int x,int y);
void openproject_GotFocus(void);
void openproject_OnOpen(FTWindow *twin);
//void txtDir_Keypress(FTWindow *twin,FTControl *tFTControl,u8 key);
void txtDir_Keypress(FTWindow *twin,FTControl *tFTControl,KeySym key);
void FTI_DrawImage(void);
//void sb_foOpenFile2_MouseMove(FTWindow *twin,FTControl *tFTControl,int x,int y);
//void update_sb_foOpenFile2(void);
void foOpenFile2_OnChange(FTWindow *twin,FTControl *tFTControl);
void btnOPExit_ButtonDownUp(FTWindow *twin,FTControl *tcontrol,int x,int y);
