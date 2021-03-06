/*-
 *   BSD LICENSE
 *
 *   Copyright (c) Intel Corporation.
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of Intel Corporation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __NVMF_H__
#define __NVMF_H__

#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>

#include <rte_config.h>
#include <rte_mempool.h>

#include "spdk/nvmf_spec.h"
#include "spdk/nvme.h"

/**
 * \file
 *
 */

#define SMALL_BB_MAX_SIZE 4096
#define LARGE_BB_MAX_SIZE (128 * 1024)

/*
 * NVMf target supports a maximum transfer size that is equal to
 * a single allocated bounce buffer per request.
 */
#define SPDK_NVMF_MAX_RECV_DATA_TRANSFER_SIZE  LARGE_BB_MAX_SIZE


#define SPDK_NVMF_BUILD_ETC "/usr/local/etc/nvmf"
#define SPDK_NVMF_DEFAULT_NUM_SESSIONS_PER_LCORE 1
#define SPDK_NVMF_DEFAULT_AUTHFILE SPDK_NVMF_BUILD_ETC "/auth.conf"
#define SPDK_NVMF_DEFAULT_NODEBASE "iqn.2013-10.com.intel.spdk"
#define SPDK_NVMF_DEFAULT_IN_CAPSULE_DATA_SIZE 1024
#define SPDK_NVMF_DEFAULT_MAX_SESSIONS_PER_SUBSYSTEM 1
#define SPDK_NVMF_DEFAULT_MAX_QUEUE_DEPTH 128
#define SPDK_NVMF_DEFAULT_MAX_CONNECTIONS_PER_SESSION 4
#define SPDK_NVMF_DEFAULT_SIN_PORT ((uint16_t)7174)

#define OBJECT_NVMF_IO				0x30

#define TRACE_GROUP_NVMF			0x3
#define TRACE_NVMF_IO_START			SPDK_TPOINT_ID(TRACE_GROUP_NVMF, 0x0)
#define TRACE_RDMA_READ_START			SPDK_TPOINT_ID(TRACE_GROUP_NVMF, 0x1)
#define TRACE_RDMA_WRITE_START			SPDK_TPOINT_ID(TRACE_GROUP_NVMF, 0x2)
#define TRACE_RDMA_READ_COMPLETE      		SPDK_TPOINT_ID(TRACE_GROUP_NVMF, 0x3)
#define TRACE_RDMA_WRITE_COMPLETE		SPDK_TPOINT_ID(TRACE_GROUP_NVMF, 0x4)
#define TRACE_NVMF_LIB_READ_START		SPDK_TPOINT_ID(TRACE_GROUP_NVMF, 0x5)
#define TRACE_NVMF_LIB_WRITE_START		SPDK_TPOINT_ID(TRACE_GROUP_NVMF, 0x6)
#define TRACE_NVMF_LIB_COMPLETE			SPDK_TPOINT_ID(TRACE_GROUP_NVMF, 0x7)
#define TRACE_NVMF_IO_COMPLETE			SPDK_TPOINT_ID(TRACE_GROUP_NVMF, 0x8)

struct spdk_nvmf_globals {
	char *authfile;

	char *nodebase;

	pthread_mutex_t mutex;

	int MaxInCapsuleData;
	int MaxSessionsPerSubsystem;
	int MaxQueueDepth;
	int MaxConnectionsPerSession;
	int MaxRecvDataSegmentLength;

	struct rte_mempool *rx_desc_pool;
	struct rte_mempool *tx_desc_pool;
	struct rte_mempool *nvme_request_pool;
	struct rte_mempool *bb_small_pool;
	struct rte_mempool *bb_large_pool;
	uint16_t	   sin_port;
};

extern struct spdk_nvmf_globals g_nvmf_tgt;

int
nvmf_tgt_init(char *authfile, char *nodebase, int max_in_capsule_data, int max_sessions,
	      int max_queue_depth, int max_conn_per_sess, int max_recv_seg_len, int listen_port);

struct nvmf_request;

int
spdk_nvmf_check_pools(struct spdk_nvmf_globals *spdk_nvmf);

int
nvmf_initialize(void);

void
nvmf_shutdown(void);

#endif /* __NVMF_H__ */
