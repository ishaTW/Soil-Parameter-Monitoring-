/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Quectel Co., Ltd. 2013
*
*****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   Cell_ID.h
 *
 * Project:
 * --------
 *   OpenCPU
 *
 * Description:
 * ------------
 *   The module defines the information, and APIs related Cell ID capture.
 *
 * Author:
 * -------
 * -------
 *
 *============================================================================
 *             HISTORY
 *----------------------------------------------------------------------------
 *
 ****************************************************************************/
s32 ATResponse_Location_handler_CellID(char* line, u32 len);
void RIL_Multi_Cell_Id(void);
void RIL_Multi_Cell_Id_OFF(void);
s32 ATResponse_CellId_Off_Handler(char* line, u32 len, void* userdata);
void wrt_Cell_ID_flg(u8 CellBKFlg);
void read_Cell_ID_flg(void);
