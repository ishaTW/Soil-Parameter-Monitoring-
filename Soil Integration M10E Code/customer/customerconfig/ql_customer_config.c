/***************************************************************************
 *
 *            Quectel Open CPU
 *
 *           Copyright (c) 2009 The Quectel Ltd.
 *
 ***************************************************************************
 *
 *  Designed by    :  Jay xin
 *  Coded by       :   Jay xin
 *  Tested by      :
 *
 ***************************************************************************
 *
 * File Description
 * ----------------
 *
 *
 ***************************************************************************
 *
 ***************************************************************************/


/****************************************************************************
 * Nested Include Files
 ***************************************************************************/
#include "ql_type.h"
#include "ql_appinit.h"


/*=========================================================================
 * Heap Configuration for Application
 *=========================================================================*/
/*
 * Heap size for application:
 * min value is 1, and the default value is 10K
 *----------------------------------------------------*/
#define QL_MEMORY_HEAP_SIZE (10*1024)
const u32 qlOpenCPUHeapMemSize = QL_MEMORY_HEAP_SIZE;
u8 qlOpenCPUHeapMem[QL_MEMORY_HEAP_SIZE];


/*=========================================================================
 * Configurations for Main Task
 *=========================================================================*/
/*
 * Stack size for main task:
 * range from 1K to 10K, and the default value is 4K.
 *----------------------------------------------------*/
#define QL_TASK_STACK_SIZE (10*1024)
const u32 qlMainTaskStackSize = QL_TASK_STACK_SIZE;
const u32 qlMainTaskPriority = 200;  // 200-255, main task priority. the smaller, the greater priority
const u32 qlMainTaskExtqsize = 10;   // 10-30, the length of message queue of main task 



/*=========================================================================
 * <<< Begin: Configurations for Subtasks
 *=========================================================================*/
#ifdef __EXAMPLE_MULTITASK__
extern void example_subtask1_entry(s32 TaskId);
extern void example_subtask2_entry(s32 TaskId);
extern void example_subtask3_entry(s32 TaskId);
extern void example_subtask4_entry(s32 TaskId);
extern void example_subtask5_entry(s32 TaskId);
extern void example_subtask6_entry(s32 TaskId);
extern void example_subtask7_entry(s32 TaskId);
extern void example_subtask8_entry(s32 TaskId);
extern void example_subtask9_entry(s32 TaskId);
extern void example_subtask10_entry(s32 TaskId);
#endif
#ifdef __EXAMPLE_MULTITASK_PORT__
extern void example_subtask1_entry(s32 TaskId);
extern void example_subtask2_entry(s32 TaskId);
extern void example_subtask3_entry(s32 TaskId);
extern void example_subtask4_entry(s32 TaskId);
extern void example_subtask5_entry(s32 TaskId);
#endif
#ifdef __EXAMPLE_MULTIMEMORY__
extern void example_subtask1_entry(s32 TaskId);
extern void example_subtask2_entry(s32 TaskId);
extern void example_subtask3_entry(s32 TaskId);
#endif


/*
*--------------------------
*  Subtasks definition
*--------------------------
*   TaskStackSize:
*           The stack size of subtask. Range from 1K to 10K.
*           If there are any file operations to do in subtask, the stack size for this subtask must be set to at least 4K.
*   TaskPriority:
*           The subtask priority. Range from 200 to 255, the smaller, the greater priority.
*   TaskExtqsize:
*           The length of message queue of subtask. Range from 10 to 30.
*/
QlMutitask SubMutitaskArray[] =  /*max 10 subtask*/ 
{
#ifdef __EXAMPLE_MULTITASK__
    /*------------------------------------------------------------------------------------
     |  Subtask Entrance Procedure | Stack Size | Subtask Priority | Length of Msg Queue |
     *-----------------------------------------------------------------------------------*/
    {example_subtask1_entry, 1024, 200, 11},
    {example_subtask2_entry, 1024, 200, 12},
    {example_subtask3_entry, 1024, 200, 13},
    {example_subtask4_entry, 1024, 200, 14},
    {example_subtask5_entry, 1024, 200, 15},
    {example_subtask6_entry, 1024, 200, 16},
    {example_subtask7_entry, 1024, 200, 17},
    {example_subtask8_entry, 1024, 200, 18},
    {example_subtask9_entry, 1024, 200, 19},
    {example_subtask10_entry, 1024,200, 20},
#endif

#ifdef __EXAMPLE_MULTITASK_PORT__
    {example_subtask1_entry, 1024, 200, 10},
    {example_subtask2_entry, 1024, 200, 10},
    {example_subtask3_entry, 1024, 200, 10},
    {example_subtask4_entry, 2*1024, 200, 10},
    {example_subtask5_entry, 2*1024, 200, 10},
#endif

#ifdef __EXAMPLE_MULTIMEMORY__
    {example_subtask1_entry, 1024, 200, 10},
    {example_subtask2_entry, 1024, 200, 10},
    {example_subtask3_entry, 1024, 200, 10},
#endif

    {NULL, 0, 0, 0}, // KEEP THIS LINE, DON'T DO MODIFICATION
};

/*=========================================================================
 * End-of: Configurations for Subtasks >>>
 *=========================================================================*/

