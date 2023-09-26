
/*
 * Include Files
 *
 */
#include "simstruc.h"



/* %%%-SFUNWIZ_wrapper_includes_Changes_BEGIN --- EDIT HERE TO _END */
#include "boost/interprocess/managed_shared_memory.hpp"
#include <windows.h>
#include <stdlib.h>
#include <string>
#include <stdio.h>
using namespace boost::interprocess;
/* %%%-SFUNWIZ_wrapper_includes_Changes_END --- EDIT HERE TO _BEGIN */
#define y_width 1
#define y_1_width 1
#define y_2_width 1
#define y_3_width 1
#define y_4_width 1
#define y_5_width 1

/*
 * Create external references here.  
 *
 */
/* %%%-SFUNWIZ_wrapper_externs_Changes_BEGIN --- EDIT HERE TO _END */
 
/* %%%-SFUNWIZ_wrapper_externs_Changes_END --- EDIT HERE TO _BEGIN */

/*
 * Start function
 *
 */
void ReadAccel_Start_wrapper(SimStruct *S)
{
/* %%%-SFUNWIZ_wrapper_Start_Changes_BEGIN --- EDIT HERE TO _END */
// Inicia la aplicación minimizada
    ShellExecute(NULL, NULL, "MCP2210_SHAREDMEM.exe", NULL, NULL, 2);
/* %%%-SFUNWIZ_wrapper_Start_Changes_END --- EDIT HERE TO _BEGIN */
}
/*
 * Output function
 *
 */
void ReadAccel_Outputs_wrapper(real_T *y0,
			real_T *y1,
			real_T *y2,
			real_T *y3,
			real_T *y4,
			real_T *y5,
			SimStruct *S)
{
/* %%%-SFUNWIZ_wrapper_Outputs_Changes_BEGIN --- EDIT HERE TO _END */
managed_shared_memory segment(open_only, "AccelMemory");
int16_t* data = segment.find<int16_t>("AccelData").first;
    if(!data){
        printf("Dato no encontrado en la memoria compartida\n");
    }else{
        //printf("ax = %d\n", data[0]);
        y0[0] = (double)data[0];    // ax
        y1[0] = (double)data[1];    // ay
        y2[0] = (double)data[2];    // az

        y3[0] = (double)data[4];    // gx
        y4[0] = (double)data[5];    // gy
        y5[0] = (double)data[6];    // gz
    }
/* %%%-SFUNWIZ_wrapper_Outputs_Changes_END --- EDIT HERE TO _BEGIN */
}

/*
 * Terminate function
 *
 */
void ReadAccel_Terminate_wrapper(SimStruct *S)
{
/* %%%-SFUNWIZ_wrapper_Terminate_Changes_BEGIN --- EDIT HERE TO _END */
    managed_shared_memory segment(open_only, "AccelMemory");
    bool* isSimulinkOpen = segment.find<bool>("isSimulinkRunning").first;
    //Cierra la aplicacion externa
    *isSimulinkOpen = false;

/* %%%-SFUNWIZ_wrapper_Terminate_Changes_END --- EDIT HERE TO _BEGIN */
}

