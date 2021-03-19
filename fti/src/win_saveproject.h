//win_saveproject.h

int saveproject_CreateFTWindow(void);
//void win5key(FTWindow *twin,u8 key);
void win5key(FTWindow *twin,KeySym key);
int saveproject_AddFTControls(FTWindow *twin);
//void txtSave_Keypress(FTWindow *twin,FTControl *tFTControl,u8 key);
void txtSave_Keypress(FTWindow *twin,FTControl *tFTControl,KeySym key);
void btnSaveProject_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
