//win_outfile.h

int outfile_CreateFTWindow(void);
//int wincmbtest1_CreateFTWindow(void);
void outfile_Keypress(FTWindow *twin,unsigned char key);
int outfile_AddFTControls(FTWindow *twin);
//int wincmbtest1_AddFTControls(FTWindow *twin);
void txtOutFile_Keypress(FTWindow *twin,FTControl *tFTControl,unsigned char key);
void btnOutFile_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
void MakeOutFile(char *name);
