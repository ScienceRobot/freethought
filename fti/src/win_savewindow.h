//win_savewindow.h

int savewindow_CreateFTWindow(void);
void savewindow_Keypress(FTWindow *twin,unsigned char key);
int savewindow_AddFTControls(FTWindow *twin);
void txtSaveWinName_Keypress(FTWindow *twin,FTControl *tFTControl,unsigned char key);
void btnSaveWindow_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
