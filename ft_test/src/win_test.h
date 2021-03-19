#pragma once

//win_test.h
int main_CreateFTWindow(void);
int main_AddFTControls(FTWindow *twin);
void btnOpenTest_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
void btnExit_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
void btn_fmFile_About_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
void btn_fmFile_New_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
void btn_fmFile_Exit_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
void winMain_Keypress(FTWindow *twin,KeySym key);
void btn_fmFile_Open_Project_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
//void btnSendUSBInstructionWindow_Click(FTWindow *twin,FTControl *tcontrol,int x,int y);
void btnRobotArmWindow_Click(FTWindow *twin,FTControl *tcontrol,int x,int y);
