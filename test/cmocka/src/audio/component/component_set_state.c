/*
 * Copyright (c) 2018, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the Intel Corporation nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Author: Bartosz Kokoszko <bartoszx.kokoszko@linux.intel.com>
 */

#include <sof/audio/component.h>
#include <errno.h>

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <sof/sof.h>

struct test_case {
	uint16_t in_state;
	int cmd;
	uint16_t out_state;
};

static struct test_case tests_succeed[] = {
	{COMP_STATE_PREPARE,	COMP_TRIGGER_START,
		COMP_STATE_ACTIVE},
	{COMP_STATE_PAUSED,	COMP_TRIGGER_START,
		COMP_STATE_ACTIVE},
	{COMP_STATE_PAUSED,	COMP_TRIGGER_RELEASE,
		COMP_STATE_ACTIVE},
	{COMP_STATE_ACTIVE,	COMP_TRIGGER_STOP,
		COMP_STATE_PREPARE},
	{COMP_STATE_ACTIVE,	COMP_TRIGGER_XRUN,
		COMP_STATE_PREPARE},
	{COMP_STATE_ACTIVE,	COMP_TRIGGER_PAUSE,
		COMP_STATE_PAUSED},
	{COMP_STATE_INIT,	COMP_TRIGGER_RESET,
		COMP_STATE_READY},
	{COMP_STATE_READY,	COMP_TRIGGER_RESET,
		COMP_STATE_READY},
	{COMP_STATE_SUSPEND,	COMP_TRIGGER_RESET,
		COMP_STATE_READY},
	{COMP_STATE_PREPARE,	COMP_TRIGGER_RESET,
		COMP_STATE_READY},
	{COMP_STATE_PAUSED,	COMP_TRIGGER_RESET,
		COMP_STATE_READY},
	{COMP_STATE_ACTIVE,	COMP_TRIGGER_RESET,
		COMP_STATE_READY},
	{COMP_STATE_PREPARE,	COMP_TRIGGER_PREPARE,
		COMP_STATE_PREPARE},
	{COMP_STATE_READY,	COMP_TRIGGER_PREPARE,
		COMP_STATE_PREPARE}
};

static struct test_case tests_fail[] = {
	{COMP_STATE_INIT,	COMP_TRIGGER_START},
	{COMP_STATE_READY,	COMP_TRIGGER_START},
	{COMP_STATE_SUSPEND,	COMP_TRIGGER_START},
	{COMP_STATE_ACTIVE,	COMP_TRIGGER_START},
	{COMP_STATE_INIT,	COMP_TRIGGER_RELEASE},
	{COMP_STATE_READY,	COMP_TRIGGER_RELEASE},
	{COMP_STATE_SUSPEND,	COMP_TRIGGER_RELEASE},
	{COMP_STATE_PREPARE,	COMP_TRIGGER_RELEASE},
	{COMP_STATE_INIT,	COMP_TRIGGER_STOP},
	{COMP_STATE_READY,	COMP_TRIGGER_STOP},
	{COMP_STATE_SUSPEND,	COMP_TRIGGER_STOP},
	{COMP_STATE_PREPARE,	COMP_TRIGGER_STOP},
	{COMP_STATE_PAUSED,	COMP_TRIGGER_STOP},
	{COMP_STATE_INIT,	COMP_TRIGGER_XRUN},
	{COMP_STATE_READY,	COMP_TRIGGER_XRUN},
	{COMP_STATE_SUSPEND,	COMP_TRIGGER_XRUN},
	{COMP_STATE_PREPARE,	COMP_TRIGGER_XRUN},
	{COMP_STATE_PAUSED,	COMP_TRIGGER_XRUN},
	{COMP_STATE_INIT,	COMP_TRIGGER_PAUSE},
	{COMP_STATE_READY,	COMP_TRIGGER_PAUSE},
	{COMP_STATE_SUSPEND,	COMP_TRIGGER_PAUSE},
	{COMP_STATE_PREPARE,	COMP_TRIGGER_PAUSE},
	{COMP_STATE_PAUSED,	COMP_TRIGGER_PAUSE},
	{COMP_STATE_INIT,	COMP_TRIGGER_PREPARE},
	{COMP_STATE_SUSPEND,	COMP_TRIGGER_PREPARE},
	{COMP_STATE_PAUSED,	COMP_TRIGGER_PREPARE},
	{COMP_STATE_ACTIVE,	COMP_TRIGGER_PREPARE}
};

static void test_audio_component_comp_set_state_succeed(void **state)
{
	(void)state; /* unused */

	int i;
	struct comp_dev test_dev;

	for (i = 0; i < ARRAY_SIZE(tests_succeed); i++) {
		test_dev.state = tests_succeed[i].in_state;
		assert_int_equal(comp_set_state(&test_dev,
			tests_succeed[i].cmd), 0);
	}
}

static void test_audio_component_comp_set_state_correct_output_state(void **state)
{
	(void)state; /* unused */

	int i;
	struct comp_dev test_dev;

	for (i = 0; i < ARRAY_SIZE(tests_succeed); i++) {
		test_dev.state = tests_succeed[i].in_state;
		comp_set_state(&test_dev, tests_succeed[i].cmd);
		assert_int_equal(test_dev.state, tests_succeed[i].out_state);
	}
}

static void test_audio_component_comp_set_state_fail(void **state)
{
	(void)state; /* unused */

	int i;
	struct comp_dev test_drv;

	for (i = 0; i < ARRAY_SIZE(tests_fail); i++) {
		test_drv.state = tests_fail[i].in_state;
		assert_int_equal(comp_set_state(&test_drv,
			tests_fail[i].cmd), -EINVAL);
	}
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_audio_component_comp_set_state_succeed),
		cmocka_unit_test(test_audio_component_comp_set_state_correct_output_state),
		cmocka_unit_test(test_audio_component_comp_set_state_fail)
	};

	cmocka_set_message_output(CM_OUTPUT_TAP);

	return cmocka_run_group_tests(tests, NULL, NULL);
}
