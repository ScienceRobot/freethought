#pragma once

//win_file_open_project.h
int win_file_open_project_CreateFTWindow(void);
void file_open_project_Keypress(FTWindow *twin,unsigned char key);
int file_open_project_AddFTControls(FTWindow *twin);
void btn_file_open_project_Exit_Click(FTWindow *twin,FTControl *tFTControl,int x,int y);
void win_file_open_project_Keypress(FTWindow *twin,KeySym key);