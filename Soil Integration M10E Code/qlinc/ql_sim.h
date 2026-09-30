
#ifndef __QL_SIM_H__
#define __QL_SIM_H__

#include "ql_type.h"

typedef void (*Ql_CallBack_SIM)(s8 SIM_type);

typedef enum
{
   QL_ID_READY,    // SIM ready
   QL_ID_SIM_PIN1, // PIN1 locked
   QL_ID_SIM_PUK1, // PUK1 locked
   QL_ID_SIM_BUSY =23  /* This add for WM CPIN? with SML check */
}QL_SIM_PIN_TYPE_enum;


/******************************************************************************
* Function:     Ql_SIM_GetLockState
*
* Description:
*               This function get current SIM card lock state.
*
* Parameters:
*               lock_type:
*                       [out] one of the of "QL_SIM_PIN_TYPE_enum"
*
* Return:
*               QL_RET_OK: indicates success.
*               QL_RET_ERR_PARAM: indicates param error.
*               Ql_RET_ERR_SIM_NOT_INSERTED: SIM card not Inserted.
******************************************************************************/
s32 Ql_SIM_GetLockState(s8* lock_type);

/******************************************************************************
* Function:     Ql_SIM_FeedPIN
*
* Description:
*               This function feed PIN1 code or PUK1 code.
*               note: if feed PUK1 success,the PIN1 code will be set to "1234" .
*
* Parameters:
*               pin_type:
*                       [in] the PIN type ,one of the of "QL_SIM_PIN_TYPE_enum"
*               pin_code: PIN1 code or PUK1 code
*
*               callback:
*                       the callback function will be invoked when feed PIN complete

*               (*Ql_CallBack_SIM)(s8 SIM_type) 
*                       SIM_type: 
*                               [out]  the SIM card state 
*
* Return:
*               QL_RET_OK: indicates success.
*               QL_RET_ERR_PARAM: indicates param error.
*               Ql_RET_ERR_SIM_TYPE_ERROR: PIN type not match
*
******************************************************************************/

s32 Ql_SIM_FeedPIN(QL_SIM_PIN_TYPE_enum pin_type, u8*pin_code, Ql_CallBack_SIM callback);
#endif 


