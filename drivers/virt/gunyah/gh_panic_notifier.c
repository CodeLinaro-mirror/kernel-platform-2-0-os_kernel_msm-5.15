// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2023 Qualcomm Innovation Center, Inc. All rights reserved.
 */
#include <linux/align.h>
#include <linux/delay.h>
#include <linux/dma-direct.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/kmsg_dump.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_reserved_mem.h>
#include <linux/panic_notifier.h>
#include <linux/platform_device.h>
#include <linux/pm.h>
#include <linux/qcom_scm.h>
#include <linux/gunyah/gh_dbl.h>
#include <linux/gunyah/gh_rm_drv.h>
#include <linux/gunyah/gh_vm.h>
#include <soc/qcom/secure_buffer.h>


char crash_msg[4096] = "VM crashed\n";

static int gh_panic_notifier_notify(struct notifier_block *this,
			  unsigned long event, void *ptr)
{
	int ret;

	pr_debug("generic gh notifier\n");
	ret = gh_rm_vm_set_crash_msg(crash_msg, sizeof(crash_msg));
	pr_debug("%d from crash msg\n", ret);
	mdelay(50);
	return 0;
}


static struct notifier_block gh_panic_blk = {
	.notifier_call = gh_panic_notifier_notify,
	.priority = 0,
};

static int __init gh_panic_notifier_init(void)
{
	int ret;

	pr_debug("setting up notifier chain\n");
	ret = atomic_notifier_chain_register(&panic_notifier_list, &gh_panic_blk);
	pr_debug("Notifer register %d\n", ret);

	return ret;

}

module_init(gh_panic_notifier_init);

static __exit void gh_panic_notifier_exit(void)
{
	pr_debug("Exiting gh_panic_notifer\n");
}
module_exit(gh_panic_notifier_exit);

MODULE_DESCRIPTION(" Qualcomm Technologies, Inc. Gunyah Panic Notifier Driver");
MODULE_LICENSE("GPL");
