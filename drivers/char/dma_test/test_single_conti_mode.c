/*
 * drivers/char/dma_test/test_single_conti_mode.c
 * (C) Copyright 2010-2015
 * Reuuimlla Technology Co., Ltd. <www.reuuimllatech.com>
 * liugang <liugang@reuuimllatech.com>
 *
 * sun6i dma test driver
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 */

#include "sun6i_dma_test.h"
#include <linux/random.h>

#define DTC_TOTAL_LEN	SZ_512K
#define DTC_ONE_LEN	DTC_TOTAL_LEN

static u32 g_src_addr = 0, g_dst_addr = 0;
static u32 g_qd_cnt = 0;
static atomic_t g_acur_cnt = ATOMIC_INIT(0);

u32 __cb_qd_sgct_mode(dm_hdl_t dma_hdl, void *parg, enum dma_cb_cause_e cause)
{
	u32 	uret = 0;

	pr_info("%s: called!\n", __func__);
	switch(cause) {
	case DMA_CB_OK:
		pr_info("%s: DMA_CB_OK!\n", __func__);
		atomic_set(&g_adma_done, 1);
		wake_up_interruptible(&g_dtc_queue[DTC_SINGLE_CONT_MODE]);
		break;
	case DMA_CB_ABORT:
		pr_info("%s: DMA_CB_ABORT!\n", __func__);
		break;
	default:
		uret = __LINE__;
		goto end;
	}

end:
	if(0 != uret)
		pr_err("%s err, line %d!\n", __func__, uret);
	return uret;
}

u32 __cb_fd_sgct_mode(dm_hdl_t dma_hdl, void *parg, enum dma_cb_cause_e cause)
{
	u32 	uret = 0;

	pr_info("%s: called!\n", __func__);
	switch(cause) {
	case DMA_CB_OK:
		pr_info("%s: DMA_CB_OK!\n", __func__);
		break;
	case DMA_CB_ABORT:
		pr_info("%s: DMA_CB_ABORT!\n", __func__);
		break;
	default:
		uret = __LINE__;
		goto end;
	}

end:
	if(0 != uret)
		pr_err("%s err, line %d!\n", __func__, uret);
	return uret;
}

u32 __cb_hd_sgct_mode(dm_hdl_t dma_hdl, void *parg, enum dma_cb_cause_e cause)
{
	u32 	uret = 0;

	pr_info("%s: called!\n", __func__);

	switch(cause) {
	case DMA_CB_OK:
		pr_info("%s: DMA_CB_OK!\n", __func__);
		break;
	case DMA_CB_ABORT:
		pr_info("%s: DMA_CB_ABORT!\n", __func__);
		break;
	default:
		uret = __LINE__;
		goto end;
	}

end:
	if(0 != uret)
		pr_err("%s err, line %d!\n", __func__, uret);
	return uret;
}

u32 __cb_op_sgct_mode(dm_hdl_t dma_hdl, void *parg, enum dma_op_type_e op)
{
	pr_info("%s: called!\n", __func__);
	switch(op) {
	case DMA_OP_START:
		pr_info("%s: op DMA_OP_START!\n", __func__);
		atomic_set(&g_adma_done, 0);
		break;
	case DMA_OP_STOP:
		pr_info("%s: op DMA_OP_STOP!\n", __func__);
		break;
	case DMA_OP_SET_HD_CB:
		pr_info("%s: op DMA_OP_SET_HD_CB!\n", __func__);
		break;
	case DMA_OP_SET_FD_CB:
		pr_info("%s: op DMA_OP_SET_FD_CB!\n", __func__);
		break;
	case DMA_OP_SET_OP_CB:
		pr_info("%s: op DMA_OP_SET_OP_CB!\n", __func__);
		break;
	default:
		break;
	}

	return 0;
}

u32 __waitdone_sgct_mode(void)
{
	long 	ret = 0;
	long 	timeout = 5 * HZ; /* 5s */

	ret = wait_event_interruptible_timeout(g_dtc_queue[DTC_SINGLE_CONT_MODE], \
		atomic_read(&g_adma_done)== 1, timeout);
	atomic_set(&g_adma_done, 0);

	if(-ERESTARTSYS == ret) {
		pr_info("%s success!\n", __func__);
		return 0;
	} else if(0 == ret) {
		pr_info("%s err, time out!\n", __func__);
		return __LINE__;
	} else {
		pr_info("%s success with condition match, ret %d!\n", __func__, (int)ret);
		return 0;
	}
}

/**
 * __dtc_single_conti_mode - dma test case for DTC_SINGLE_CONT_MODE
 *
 * Returns 0 if success, the err line number if failed.
 */
u32 __dtc_single_conti_mode(void)
{
	u32 	uret = 0;
	u32	uqueued_normal = 0;
	void 	*src_vaddr = NULL, *dst_vaddr = NULL;
	u32 	usrc_paddr = 0, udst_paddr = 0;
	dm_hdl_t dma_hdl = (dm_hdl_t)NULL;
	struct dma_cb_t done_cb;
	struct dma_op_cb_t op_cb;
	struct dma_config_t DmaConfig;

	pr_info("%s enter\n", __func__);

	src_vaddr = dma_alloc_coherent(NULL, DTC_TOTAL_LEN, (dma_addr_t *)&usrc_paddr, GFP_KERNEL);
	if(NULL == src_vaddr) {
		uret = __LINE__;
		goto end;
	}
	pr_info("%s: src_vaddr 0x%08x, usrc_paddr 0x%08x\n", __func__, (u32)src_vaddr, usrc_paddr);
	dst_vaddr = dma_alloc_coherent(NULL, DTC_TOTAL_LEN, (dma_addr_t *)&udst_paddr, GFP_KERNEL);
	if(NULL == dst_vaddr) {
		uret = __LINE__;
		goto end;
	}
	pr_info("%s: dst_vaddr 0x%08x, udst_paddr 0x%08x\n", __func__, (u32)dst_vaddr, udst_paddr);

	get_random_bytes(src_vaddr, DTC_TOTAL_LEN);
	memset(dst_vaddr, 0x54, DTC_TOTAL_LEN);

	atomic_set(&g_acur_cnt, 0);
	g_src_addr = usrc_paddr;
	g_dst_addr = udst_paddr;

	dma_hdl = sw_dma_request("m2m_dma", DMA_WORK_MODE_SINGLE);
	if(NULL == dma_hdl) {
		uret = __LINE__;
		goto end;
	}
	pr_info("%s: sw_dma_request success, dma_hdl 0x%08x\n", __func__, (u32)dma_hdl);

	/* set callback */
	memset(&done_cb, 0, sizeof(done_cb));
	memset(&op_cb, 0, sizeof(op_cb));
	done_cb.func = __cb_qd_sgct_mode;
	done_cb.parg = NULL;
	if(0 != sw_dma_ctl(dma_hdl, DMA_OP_SET_QD_CB, (void *)&done_cb)) {
		uret = __LINE__;
		goto end;
	}
	pr_info("%s: set queuedone_cb success\n", __func__);
	done_cb.func = __cb_fd_sgct_mode;
	done_cb.parg = NULL;
	if(0 != sw_dma_ctl(dma_hdl, DMA_OP_SET_FD_CB, (void *)&done_cb)) {
		uret = __LINE__;
		goto end;
	}
	pr_info("%s: set fulldone_cb success\n", __func__);
	done_cb.func = __cb_hd_sgct_mode;
	done_cb.parg = NULL;
	if(0 != sw_dma_ctl(dma_hdl, DMA_OP_SET_HD_CB, (void *)&done_cb)) {
		uret = __LINE__;
		goto end;
	}
	pr_info("%s: set halfdone_cb success\n", __func__);
	op_cb.func = __cb_op_sgct_mode;
	op_cb.parg = NULL;
	if(0 != sw_dma_ctl(dma_hdl, DMA_OP_SET_OP_CB, (void *)&op_cb)) {
		uret = __LINE__;
		goto end;
	}
	pr_info("%s: set op_cb success\n", __func__);

	memset(&DmaConfig, 0, sizeof(DmaConfig));
	DmaConfig.xfer_type 	= DMAXFER_D_BWORD_S_BWORD;
	DmaConfig.address_type 	= DMAADDRT_D_LN_S_LN;
	DmaConfig.para 		= 0;
	DmaConfig.irq_spt 	= CHAN_IRQ_HD | CHAN_IRQ_FD | CHAN_IRQ_QD;
	DmaConfig.src_addr 	= usrc_paddr;
	DmaConfig.dst_addr 	= udst_paddr;
	DmaConfig.byte_cnt 	= DTC_ONE_LEN;
	DmaConfig.bconti_mode 	= true; /* test for continue mode */
	DmaConfig.src_drq_type 	= DRQSRC_SDRAM;
	DmaConfig.dst_drq_type 	= DRQDST_SDRAM;
	if(0 != sw_dma_config(dma_hdl, &DmaConfig, ENQUE_PHASE_NORMAL)) {
		uret = __LINE__;
		goto end;
	}
	uqueued_normal++;
	pr_info("%s: sw_dma_config success\n", __func__);
	sw_dma_dump_chan(dma_hdl);

	g_qd_cnt = 0;

	/* start dma */
	if(0 != sw_dma_ctl(dma_hdl, DMA_OP_START, NULL)) {
		uret = __LINE__;
		goto end;
	}
	pr_info("%s: sw_dma_start success\n", __func__);

	/* wait dma done */
	if(0 != __waitdone_sgct_mode()) {
		uret = __LINE__;
		goto end;
	}
	pr_info("%s: __waitdone_sgct_mode sucess\n", __func__);

	msleep_interruptible(5000); /* one buffer continue transfer */

	/* check if data ok */
	if(0 == memcmp(src_vaddr, dst_vaddr, DTC_TOTAL_LEN))
		pr_info("%s: data check ok!\n", __func__);
	else {
		pr_err("%s: data check err!\n", __func__);
		uret = __LINE__; /* return err */
		goto end;
	}

	/* stop and free dma channel */
	if(0 != sw_dma_ctl(dma_hdl, DMA_OP_STOP, NULL)) {
		uret = __LINE__;
		goto end;
	}
	pr_info("%s: sw_dma_stop success\n", __func__);
	if(0 != sw_dma_release(dma_hdl)) {
		uret = __LINE__;
		goto end;
	}
	dma_hdl = (dm_hdl_t)NULL;
	pr_info("%s: sw_dma_release success\n", __func__);

end:
	if(0 != uret)
		pr_err("%s err, line %d!\n", __func__, uret);
	else
		pr_info("%s, success!\n", __func__);

	if((dm_hdl_t)NULL != dma_hdl) {
		pr_err("%s, stop and release dma handle now!\n", __func__);
		if(0 != sw_dma_ctl(dma_hdl, DMA_OP_STOP, NULL))
			pr_err("%s err, line %d!\n", __func__, __LINE__);
		if(0 != sw_dma_release(dma_hdl))
			pr_err("%s err, line %d!\n", __func__, __LINE__);
	}
	pr_err("%s, line %d!\n", __func__, __LINE__);

	if(NULL != src_vaddr)
		dma_free_coherent(NULL, DTC_TOTAL_LEN, src_vaddr, usrc_paddr);
	if(NULL != dst_vaddr)
		dma_free_coherent(NULL, DTC_TOTAL_LEN, dst_vaddr, udst_paddr);

	pr_err("%s, end!\n", __func__);
	return uret;
}

