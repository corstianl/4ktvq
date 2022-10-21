/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "It_los_task.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

static void HwiF01(void)
{
    UINT32 ret;

    TEST_HwiClear(HWI_NUM_TEST);

    ICUNIT_ASSERT_EQUAL_VOID(g_testCount, 1, g_testCount);
    g_testCount++;

    ret = LOS_TaskSuspend(g_testTaskID02);
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);
}

static void TaskF02(void)
{
    ICUNIT_ASSERT_EQUAL_VOID(g_testCount, 4, g_testCount); // 4, assert that g_testCount is equal to this.
    g_testCount++;

    LOS_TaskDelete(g_testTaskID02);
}

static void TaskF01(void)
{
    UINT32 ret;
    TSK_INIT_PARAM_S task1 = { 0 };
    task1.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF02;
    task1.uwStackSize = TASK_STACK_SIZE_TEST;
    task1.pcName = "Tsk054B";
    task1.usTaskPrio = TASK_PRIO_TEST_TASK - 1;
    task1.uwResved = LOS_TASK_STATUS_DETACHED;
#if (LOSCFG_KERNEL_SMP == YES)
    task1.usCpuAffiMask = CPUID_TO_AFFI_MASK(ArchCurrCpuid());
#endif
    g_testCount++;

    ret = LOS_TaskCreate(&g_testTaskID02, &task1);
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);

    ret = LOS_HwiCreate(HWI_NUM_TEST, 1, 0, HwiF01, 0);
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);

#if (LOSCFG_KERNEL_SMP == YES)
    HalIrqSetAffinity(HWI_NUM_TEST, CPUID_TO_AFFI_MASK(ArchCurrCpuid()));
#endif

    TestHwiTrigger(HWI_NUM_TEST);

    ICUNIT_ASSERT_EQUAL_VOID(g_testCount, 2, g_testCount); // 2, assert that g_testCount is equal to this.
    g_testCount++;

    LOS_TaskDelete(g_testTaskID01);
}

static UINT32 Testcase(void)
{
    UINT32 ret;
    TSK_INIT_PARAM_S task1 = { 0 };
    task1.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF01;
    task1.uwStackSize = TASK_STACK_SIZE_TEST;
    task1.pcName = "Tsk054A";
    // 2, It is used to calculate a priority relative to TASK_PRIO_TEST_TASK.
    task1.usTaskPrio = TASK_PRIO_TEST_TASK - 2;
    task1.uwResved = LOS_TASK_STATUS_DETACHED;
#if (LOSCFG_KERNEL_SMP == YES)
    task1.usCpuAffiMask = CPUID_TO_AFFI_MASK(ArchCurrCpuid());
#endif
    g_testCount = 0;

    ret = LOS_TaskCreate(&g_testTaskID01, &task1);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ICUNIT_ASSERT_EQUAL(g_testCount, 3, g_testCount); // 3, assert that g_testCount is equal to this.
    g_testCount++;

    ret = LOS_TaskResume(g_testTaskID02);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ICUNIT_GOTO_EQUAL(g_testCount, 5, g_testCount, EXIT); // 5, assert that g_testCount is equal to this.
EXIT:
    TEST_HwiDelete(HWI_NUM_TEST);
    LOS_TaskDelete(g_testTaskID01);
    LOS_TaskDelete(g_testTaskID02);
    return LOS_OK;
}

void ItLosTask054(void) // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("ItLosTask054", Testcase, TEST_LOS, TEST_TASK, TEST_LEVEL2, TEST_FUNCTION);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */
