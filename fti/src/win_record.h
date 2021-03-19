//win_record.h

int record_CreateFTWindow(void);
void win2key(FTWindow *twin,unsigned char key);
int record_AddFTControls(FTWindow *twin);
void btnStartRecord_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
void btnStopRecord_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
void btnCloseRecord_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
int record(void);
