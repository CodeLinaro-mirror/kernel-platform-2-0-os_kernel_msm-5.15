/* SPDX-License-Identifier: GPL-2.0-only
 * Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
 */
#ifndef __QRTR_H_
#define __QRTR_H_

#include <linux/types.h>

struct sk_buff;

/* endpoint node id auto assignment */
#define QRTR_EP_NID_AUTO (-1)
#define QRTR_EP_NET_ID_AUTO (1)

#define QRTR_DEL_PROC_MAGIC	0xe111
#define AID_VENDOR_QRTR	KGIDT_INIT(2906)

/**
 * struct qrtr_endpoint - endpoint handle
 * @xmit: Callback for outgoing packets
 *
 * The socket buffer passed to the xmit function becomes owned by the endpoint
 * driver.  As such, when the driver is done with the buffer, it should
 * call kfree_skb() on failure, or consume_skb() on success.
 */
struct qrtr_endpoint {
	int (*xmit)(struct qrtr_endpoint *ep, struct sk_buff *skb);
	/* private: not for endpoint use */
	struct qrtr_node *node;
};

/**
 * struct qrtr_array - array with size
 * @arr: elements in the array
 * @size: number of elements
 *
 * An array with its size provided.
 */
struct qrtr_array {
	u32 *arr;
	size_t size;
};

/**
 * struct qrtr_cb - struct to store pkt info
 * @src_node: source node of the pkt
 * @src_port: source port of the pkt
 * @dst_node: destination node of the pkt
 * @dst_port: destination port of the pkt
 * @type: type of the pkt
 * @confirm_rx: flag to indicate low watermark exceeded
 */
struct qrtr_cb {
	u32 src_node;
	u32 src_port;
	u32 dst_node;
	u32 dst_port;

	u8 type;
	u8 confirm_rx;
};

int qrtr_endpoint_register(struct qrtr_endpoint *ep, unsigned int net_id,
			   bool rt, struct qrtr_array *no_wake);

void qrtr_endpoint_unregister(struct qrtr_endpoint *ep);

int qrtr_endpoint_post(struct qrtr_endpoint *ep, const void *data, size_t len);

int qrtr_ns_init(void);

void qrtr_ns_remove(void);

int qrtr_peek_pkt_size(const void *data);

int qrtr_get_service_id(unsigned int node_id, unsigned int port_id);

void qrtr_print_wakeup_reason(const void *data);

int qrtr_get_header_size(const void *data);

#ifdef CONFIG_QRTR_WAKEUP_INFO
int qrtr_wakeup_info_init(void);
void qrtr_wakeup_info_exit(void);
void qrtr_save_wakeup_reason(u64 preview, struct qrtr_cb cb, int pid, char *name,
			     int service_id);
#else
static inline int qrtr_wakeup_info_init(void)
{
	return 0;
}

static inline void qrtr_wakeup_info_exit(void) { }
static inline void qrtr_save_wakeup_reason(u64 preview, struct qrtr_cb cb, int pid, char *name,
					   int service_id)
{ }
#endif /*CONFIG_QRTR_WAKEUP_INFO*/
#endif
