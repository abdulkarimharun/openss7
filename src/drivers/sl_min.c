/*****************************************************************************

 @(#) File: src/drivers/sl_min.c

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2019  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 *****************************************************************************/

static char const ident[] = "src/drivers/sl_min.c (" PACKAGE_ENVR ") " PACKAGE_DATE;

#include <sys/os7/compat.h>

#include <ss7/lmi.h>
#include <ss7/sli.h>

#define SL_MIN_DESCRIP		"SS7/IP Signalling Link (SL) STREAMS Multiplexing Driver"
#define SL_MIN_EXTRA		"Part of the OpenSS7 SS7 Stack for Linux Fast-STREAMS"
#define SL_MIN_REVISION	"OpenSS7 src/drivers/sl_min.c (" PACKAGE_ENVR ") " PACKAGE_DATE
#define SL_MIN_COPYRIGHT	"Copyright (c) 2008-2019  Monavacon Limited.  All Rights Reserved."
#define SL_MIN_DEVICE		"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define SL_MIN_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define SL_MIN_LICENSE		"GPL"
#define SL_MIN_BANNER		SL_MIN_DESCRIP		"\n" \
				SL_MIN_EXTRA		"\n" \
				SL_MIN_REVISION		"\n" \
				SL_MIN_COPYRIGHT	"\n" \
				SL_MIN_DEVICE		"\n" \
				SL_MIN_CONTACT
#define SL_MIN_SPLASH		SL_MIN_DESCRIP		"\n" \
				SL_MIN_REVISION

#ifdef LINUX
MODULE_AUTHOR(SL_MIN_CONTACT);
MODULE_DESCRIPTION(SL_MIN_DESCRIP);
MODULE_SUPPORTED_DEVICE(SL_MIN_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SL_MIN_LICENSE);
#endif
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sl_min");
#endif
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif
#endif				/* LINUX */

#define SL_MIN_DRV_ID		CONFIG_STREAMS_SL_MIN_MODID
#define SL_MIN_DRV_NAME		CONFIG_STREAMS_SL_MIN_NAME
#define SL_MIN_CMAJORS		CONFIG_STREAMS_SL_MIN_NMAJORS
#define SL_MIN_CMAJOR_0		CONFIG_STREAMS_SL_MIN_MAJOR
#define SL_MIN_UNITS		CONFIG_STREAMS_SL_MIN_NMINORS

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define DRV_ID		SL_MIN_DRV_ID
#define DRV_NAME	SL_MIN_DRV_NAME
#define CMAJORS		SL_MIN_CMAJORS
#define CMAJOR_0	SL_MIN_CMAJOR_0
#define UNITS		SL_MIN_UNITS
#ifdef MODULE
#define DRV_BANNER	SL_MIN_BANNER
#else				/* MODULE */
#define DRV_BANNER	SL_MIN_SPLASH
#endif				/* MODULE */

STATIC struct module_info sl_minfo = {
	.mi_idnum = SL_MIN_DRV_ID,	/* id */
	.mi_idname = SL_MIN_DRV_NAME,	/* name */
	.mi_minpsz = 0,			/* min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* max packet size accepted */
	.mi_hiwat = 10240L,		/* high water mark */
	.mi_lowat = 512L,		/* low water mark */
};

STATIC int streamscall sl_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int streamscall sl_close(queue_t *, int, cred_t *);

STATIC streamscall int sl_r_prim(queue_t *q, mblk_t *mp);

STATIC struct qinit sl_rinit = {
	.qi_putp = NULL,		/* put */
	.qi_srvp = ss7_osrv,		/* service */
	.qi_qopen = sl_open,		/* open */
	.qi_qclose = sl_close,		/* close */
	.qi_qadmin = NULL,		/* admin */
	.qi_minfo = &sl_minfo,		/* info */
	.qi_mstat = NULL,		/* stat */
};

STATIC streamscall int sl_w_prim(queue_t *q, mblk_t *mp);

STATIC struct qinit sl_winit = {
	.qi_putp = ss7_iput,		/* put */
	.qi_srvp = NULL,		/* service */
	.qi_qopen = NULL,		/* open */
	.qi_qclose = NULL,		/* close */
	.qi_qadmin = NULL,		/* admin */
	.qi_minfo = &sl_minfo,		/* info */
	.qi_mstat = NULL,		/* stat */
};

/*
 * qinit structures (rd and wr side, lower) 
 */
STATIC struct module_info sl_lminfo = {
	.mi_idnum = 0,			/* id */
	.mi_idname = SL_MIN_DRV_NAME,	/* name */
	.mi_minpsz = 0,			/* min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* max packet size accepted */
	.mi_hiwat = 10240L,		/* high water mark */
	.mi_lowat = 512L,		/* low water mark */
};

STATIC streamscall int ls_r_prim(queue_t *q, mblk_t *mp);

STATIC struct qinit sl_lrinit = {
	.qi_putp = ss7_iput,		/* put */
	.qi_srvp = NULL,		/* service */
	.qi_qopen = NULL,		/* open */
	.qi_qclose = NULL,		/* close */
	.qi_qadmin = NULL,		/* admin */
	.qi_minfo = &sl_lminfo,		/* info */
	.qi_mstat = NULL,		/* stat */
};

STATIC streamscall int ls_w_prim(queue_t *q, mblk_t *mp);

STATIC struct qinit sl_lwinit = {
	.qi_putp = NULL,		/* put */
	.qi_srvp = ss7_osrv,		/* service */
	.qi_qopen = NULL,		/* open */
	.qi_qclose = NULL,		/* close */
	.qi_qadmin = NULL,		/* admin */
	.qi_minfo = &sl_lminfo,		/* info */
	.qi_mstat = NULL,		/* stat */
};

MODULE_STATIC struct streamtab sl_mininfo = {
	.st_rdinit = &sl_rinit,		/* read queue */
	.st_wrinit = &sl_winit,		/* write queue */
	.st_muxrinit = &sl_lrinit,	/* mux read queue */
	.st_muxwinit = &sl_lwinit,	/* mux write queue */
};

/*
 *  =========================================================================
 *
 *  Private structure and allocation
 *
 *  =========================================================================
 */

typedef struct sl {
	STR_DECLARATION (struct sl);
	toid_t sltm_timer;
	toid_t recovery_timer;
	size_t sltm_failures;
	size_t recovery_attempts;
	queue_t *qbot;
} sl_t;
#define SL_PRIV(__q) ((struct sl *)((__q)->q_ptr))

typedef struct ls {
	STR_DECLARATION (struct ls);
	toid_t sltm_timer;
	toid_t recovery_timer;
	size_t sltm_failures;
	size_t recovery_attempts;
	queue_t *qtop;
} ls_t;
#define SL_PRIV(__q) ((struct sl *)((__q)->q_ptr))

typedef struct df {
	spinlock_t lock;
	SLIST_HEAD (sl, sl);
	SLIST_HEAD (ls, ls);
} df_t;
STATIC struct df master = {
#if	defined __SPIN_LOCK_UNLOCKED
	.lock = __SPIN_LOCK_UNLOCKED(master.lock),
#elif	defined SPIN_LOCK_UNLOCKED
	.lock = SPIN_LOCK_UNLOCKED,
#else
#error cannot initialize spin locks
#endif
};

/*
 *  -------------------------------------------------------------------------
 *
 *  Caches
 *
 *  -------------------------------------------------------------------------
 */

STATIC kmem_cachep_t slm_sl_cachep = NULL;
STATIC kmem_cachep_t slm_ls_cachep = NULL;

STATIC int
slm_term_caches(void)
{
	int err = 0;
	if (slm_ls_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(slm_ls_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy slm_ls_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed slm_ls_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(slm_ls_cachep);
#endif
	}
	if (slm_sl_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(slm_sl_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy slm_sl_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed slm_sl_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(slm_sl_cachep);
#endif
	}
	return (err);
}
STATIC int
slm_init_caches(void)
{
	if (!slm_sl_cachep
	    && !(slm_sl_cachep =
		 kmem_create_cache("slm_sl_cachep", sizeof(struct sl), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate slm_sl_cachep", DRV_NAME);
		goto error;
	} else
		printd(("%s: initialized slm sl structure cache\n", DRV_NAME));
	if (!slm_ls_cachep
	    && !(slm_ls_cachep =
		 kmem_create_cache("slm_ls_cachep", sizeof(struct ls), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate slm_ls_cachep", DRV_NAME);
		goto error;
	} else
		printd(("%s: initialized slm ls structure cache\n", DRV_NAME));
	return (0);
      error:
	slm_term_caches();
	return (-ENOMEM);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Allocation and Deallocation
 *
 *  -------------------------------------------------------------------------
 */

STATIC struct sl *
sl_get(struct sl *sl)
{
	assure(sl);
	if (sl)
		atomic_inc(&sl->refcnt);
	return (sl);
}

STATIC void
sl_put(struct sl *sl)
{
	if (sl && atomic_dec_and_test(&sl->refcnt)) {
		kmem_cache_free(slm_sl_cachep, sl);
		printd(("%s: %s: %p: dallocated sl structure\n", DRV_NAME, __FUNCTION__, sl));
	}
}

STATIC struct sl *
slm_alloc_sl(queue_t *q, struct sl **spp, major_t cmajor, minor_t cminor, cred_t *crp)
{
	struct sl *sl;
	printd(("%s: %s: create sl device = %hu:%hu\n", DRV_NAME, __FUNCTION__, cmajor, cminor));
	if ((sl = kmem_cache_alloc(slm_sl_cachep, GFP_ATOMIC))) {
		bzero(sl, sizeof(*sl));
		sl_get(sl);	/* first get */
		sl->u.dev.cmajor = cmajor;
		sl->u.dev.cminor = cminor;
		sl->cred = *crp;
		spin_lock_init(&sl->qlock);
		(sl->iq = RD(q))->q_ptr = sl_get(sl);
		(sl->oq = WR(q))->q_ptr = sl_get(sl);
		sl->o_prim = sl_r_prim;
		sl->i_prim = sl_w_prim;
		sl->o_wakeup = NULL;
		sl->i_wakeup = NULL;
		sl->i_state = LMI_UNATTACHED;
		sl->i_style = LMI_STYLE2;
		sl->i_version = 2;
		spin_lock_init(&sl->lock);
		/* place in master list */
		if ((sl->next = *spp))
			sl->next->prev = &sl->next;
		sl->prev = spp;
		*spp = sl_get(sl);
		master.sl.numb++;
	} else
		printd(("%s: %s: ERROR: failed to allocate sl structure %hu:%hu\n", DRV_NAME,
			__FUNCTION__, cmajor, cminor));
	return (sl);
}

STATIC void
slm_free_sl(queue_t *q)
{
	struct sl *sl = (struct sl *) q->q_ptr;
	psw_t flags;
	ensure(sl, return);
	printd(("%s: %s: %p: free sl %hu:%hu\n", DRV_NAME, __FUNCTION__, sl, sl->u.dev.cmajor,
		sl->u.dev.cminor));
	spin_lock_irqsave(&sl->lock, flags);
	{
		/* stopping bufcalls */
		ss7_unbufcall((str_t *) sl);
		/* flush buffers */
		flushq(sl->oq, FLUSHALL);
		flushq(sl->iq, FLUSHALL);
		/* remove from master list */
		if ((*sl->prev = sl->next))
			sl->next->prev = sl->prev;
		sl->next = NULL;
		sl->prev = &sl->next;
		ensure(atomic_read(&sl->refcnt) > 1, sl_get(sl));
		sl_put(sl);
		assure(master.sl.numb > 0);
		master.sl.numb--;
		/* remove from queues */
		ensure(atomic_read(&sl->refcnt) > 1, sl_get(sl));
		sl_put(xchg(&sl->oq->q_ptr, NULL));
		ensure(atomic_read(&sl->refcnt) > 1, sl_get(sl));
		sl_put(xchg(&sl->iq->q_ptr, NULL));
		/* done, check final count */
		if (atomic_read(&sl->refcnt) != 1) {
			pswerr(("%s: %s: %p: ERROR: sl lingering reference count = %d\n",
				DRV_NAME, __FUNCTION__, sl, atomic_read(&sl->refcnt)));
			atomic_set(&sl->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	sl_put(sl);		/* final put */
	return;
}

STATIC struct ls *
ls_get(struct ls *ls)
{
	assure(ls);
	if (ls)
		atomic_inc(&ls->refcnt);
	return (ls);
}

STATIC void
ls_put(struct ls *ls)
{
	if (ls && atomic_dec_and_test(&ls->refcnt)) {
		kmem_cache_free(slm_ls_cachep, ls);
		printd(("%s: %s: %p: deallocated ls structure", DRV_NAME, __FUNCTION__, ls));
	}
}

STATIC struct ls *
slm_alloc_ls(queue_t *q, struct ls **lpp, ulong index, cred_t *crp)
{
	struct ls *ls;
	printd(("%s: %s: create ls index = %lu\n", DRV_NAME, __FUNCTION__, index));
	if ((ls = kmem_cache_alloc(slm_ls_cachep, GFP_ATOMIC))) {
		bzero(ls, sizeof(*ls));
		ls_get(ls);	/* first get */
		ls->u.mux.index = index;
		ls->cred = *crp;
		spin_lock_init(&ls->qlock);
		(ls->iq = RD(q))->q_ptr = ls_get(ls);
		(ls->oq = WR(q))->q_ptr = ls_get(ls);
		ls->i_prim = ls_r_prim;
		ls->o_prim = ls_w_prim;
		ls->i_wakeup = NULL;
		ls->o_wakeup = NULL;
		ls->i_state = LMI_UNATTACHED;
		ls->i_version = LMI_STYLE2;
		spin_lock_init(&ls->lock);
		/* place in master list */
		if ((ls->next = *lpp))
			ls->next->prev = &ls->next;
		ls->prev = lpp;
		*lpp = ls_get(ls);
		master.ls.numb++;
	} else
		printd(("%s: %s: ERROR: failed to allocate ls structure %lu\n", DRV_NAME,
			__FUNCTION__, index));
	return (ls);
}

STATIC void
slm_free_ls(queue_t *q)
{
	struct ls *ls = (struct ls *) q->q_ptr;
	psw_t flags;
	ensure(ls, return);
	printd(("%s: %s: %p: free ls %lu\n", DRV_NAME, __FUNCTION__, ls, ls->u.mux.index));
	spin_lock_irqsave(&ls->lock, flags);
	{
		/* stopping bufcalls */
		ss7_unbufcall((str_t *) ls);
		/* flush buffers */
		flushq(ls->oq, FLUSHALL);
		flushq(ls->iq, FLUSHALL);
		/* remove from master list */
		if ((*ls->prev = ls->next))
			ls->next->prev = ls->prev;
		ls->next = NULL;
		ls->prev = &ls->next;
		ensure(atomic_read(&ls->refcnt) > 1, ls_get(ls));
		ls_put(ls);
		assure(master.ls.numb > 0);
		master.ls.numb--;
		/* remove from queues */
		ensure(atomic_read(&ls->refcnt) > 1, ls_get(ls));
		ls_put(xchg(&ls->oq->q_ptr, NULL));
		ensure(atomic_read(&ls->refcnt) > 1, ls_get(ls));
		ls_put(xchg(&ls->iq->q_ptr, NULL));
		/* done, check final count */
		if (atomic_read(&ls->refcnt) != 1) {
			pswerr(("%s: %s: %p: ERROR: ls lingering reference count = %d\n",
				DRV_NAME, __FUNCTION__, ls, atomic_read(&ls->refcnt)));
			atomic_set(&ls->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&ls->lock, flags);
	ls_put(ls);		/* final put */
	return;
}

/*
 *  =========================================================================
 *
 *  Message handling and procedures
 *
 *  =========================================================================
 */

STATIC void streamscall
restart_link(caddr_t data)
{
	queue_t *q = (queue_t *) data;
	struct ls *ls = (struct ls *) q->q_ptr;
	mblk_t *dp;
	ls->recovery_timer = 0;
	if ((dp = allocb(sizeof(long), BPRI_MED))) {
		sl_start_req_t *p;
		dp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) dp->b_wptr;
		dp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_START_REQ;
		ptrace(("Sending SL_START_REQ\n"));
		ls->recovery_attempts++;
		qreply(q, dp);
		return;
	}
	ls->recovery_timer = timeout(&restart_link, (caddr_t) q, 12 * HZ);
}

STATIC void streamscall send_sltm(caddr_t data);

STATIC void streamscall
failed_sltm(caddr_t data)
{
	queue_t *q = (queue_t *) data;
	struct ls *ls = (struct ls *) q->q_ptr;
	ls->sltm_timer = 0;
	__printd(("mtp: SLTM Failed\n"));
	if (++(ls->sltm_failures) >= 2) {
		mblk_t *dp;
		if ((dp = allocb(sizeof(long), BPRI_MED))) {
			sl_stop_req_t *p;
			dp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) dp->b_wptr;
			dp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_STOP_REQ;
			ptrace(("Sending SL_STOP_REQ\n"));
			ls->recovery_timer = timeout(&restart_link, (caddr_t) q, 12 * HZ);
			qreply(q, dp);
		}
	}
	send_sltm(data);
}

STATIC void streamscall
send_sltm(caddr_t data)
{
	queue_t *q = (queue_t *) data;
	struct ls *ls = (struct ls *) q->q_ptr;
	mblk_t *mp;
	uint dpc = 0x11d8;
	uint opc = 0x11ae;
	uint sls = 0;
	uint h0 = 1;
	uint h1 = 1;
	uint tbyte = ((jiffies >> 8) & 0x0f);
	uint tlen = jiffies & 0x0f;
	if ((mp = allocb(7 + tlen, BPRI_MED))) {
		int i;
		unsigned char *x;
		mp->b_datap->db_type = M_DATA;
		mp->b_rptr += 6;
		mp->b_wptr += 6;
		x = mp->b_wptr;
		*x++ = 0x01;	/* sio == 1 */
		*x++ = dpc;
		*x++ = ((dpc >> 8) & 0x3f) | (opc << 6);
		*x++ = (opc >> 2);
		*x++ = ((opc >> 10) & 0x0f) | (sls << 4);
		*x++ = h0 | (h1 << 4);
		*x++ = (tlen << 4) & 0xf0;
		for (i = 0; i < tlen; i++)
			*x++ = tbyte;
		mp->b_wptr = x;
		__printd(("mtp: Sent SLTM\n"));
		qreply(q, mp);
	}
	ls->sltm_timer = timeout(&failed_sltm, (caddr_t) q, 4 * HZ);	/* T1T */
}

STATIC int
handle_message(queue_t *q, mblk_t *mp)
{
	struct ls *ls = (struct ls *) q->q_ptr;
	unsigned char *x = mp->b_rptr;
	uint sio, dpc, opc, sls, h0, h1, b;
	int i;
	__printd(("mtp: M_DATA: "));
	for (i = 0; i < mp->b_wptr - mp->b_rptr; i++)
		__printd(("%02x ", x[i] & 0x0ff));
	__printd(("\n"));
	sio = *x++;
	if ((sio & 0xf) == 1 || (sio & 0xf) == 2) {
		ptrace(("Got Test Message\n"));
		b = *x++ & 0x00ff;
		dpc = ((b << 0) & 0xff);
		b = *x++ & 0x00ff;
		dpc |= ((b << 8) & 0x3f00);
		opc = ((b >> 6) & 0x03);
		b = *x++ & 0x00ff;
		opc |= ((b << 2) & 0x3fc);
		b = *x++ & 0x00ff;
		opc |= ((b << 10) & 0x3c00);
		sls = (b >> 4) & 0x0f;
		b = *x++ & 0x00ff;
		h0 = b & 0x0f;
		h1 = b >> 4;
		if (h0 == 1 && h1 == 1) {
			mblk_t *dp;
			__printd(("mtp: Got SLTM\n"));
			if ((dp = copymsg(mp))) {
				x = dp->b_rptr;
				h1 = 2;
				x++;
				*x++ = opc;
				*x++ = ((opc >> 8) & 0x3f) | (dpc << 6);
				*x++ = (dpc >> 2);
				*x++ = ((dpc >> 10) & 0x0f) | (sls << 4);
				*x++ = h0 | (h1 << 4);
				/* 
				   send back */
				__printd(("mtp: Sent SLTA\n"));
				qreply(q, dp);
				freemsg(mp);
				return (1);
			}
		} else if (h0 == 1 && h1 == 2) {
			__printd(("mtp: Got SLTA\n"));
			if (ls->sltm_timer)
				untimeout(ls->sltm_timer);
			ls->sltm_failures = 0;
			ls->sltm_timer = timeout(&send_sltm, (caddr_t) q, 2 * 60 * HZ);	/* T2T */
		}
	}
	return (0);
}

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */

STATIC int
sl_r_data(queue_t *q, mblk_t *mp)
{
	if (handle_message(q, mp))
		return (QR_ABSORBED);
	return (QR_PASSFLOW);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */

STATIC int
sl_r_proto(queue_t *q, mblk_t *mp)
{
	struct ls *ls = (struct ls *) q->q_ptr;
	ptrace(("Got M_PROTO = %ld\n", *(long *) mp->b_rptr));
	switch (*(long *) mp->b_rptr) {
	case SL_OUT_OF_SERVICE_IND:
		__printd(("mtp: Got SL_OUT_OF_SERVICE_IND\n"));
		if (ls->sltm_timer)
			untimeout(xchg(&ls->sltm_timer, 0));
		if (ls->recovery_timer)
			untimeout(xchg(&ls->recovery_timer, 0));
		if (!ls->recovery_attempts)
			restart_link((caddr_t) q);
		else
			ls->recovery_timer = timeout(&restart_link, (caddr_t) q, 1 * HZ);	/* T17 
												 */
		return (QR_DONE);
	case SL_PDU_IND:
		__printd(("mtp: Got SL_PDU_IND\n"));
		if (handle_message(q, mp->b_cont))
			return (QR_TRIMMED);
		break;
	case SL_IN_SERVICE_IND:
		__printd(("mtp: Got SL_IN_SERVICE_IND\n"));
		ls->recovery_attempts = 0;
		send_sltm((caddr_t) q);
		return (QR_DONE);
	}
	return (QR_PASSFLOW);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_IOCTL Handling
 *
 *  -------------------------------------------------------------------------
 */

STATIC int
sl_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	int ret = 0;
	(void) sl;
	(void) size;
	(void) count;
	switch (type) {
	case _IOC_TYPE(__SID):
	{
		psw_t flags;
		struct ls *ls, **lpp;
		struct linkblk *lb = (struct linkblk *) arg;
		if (lb == NULL) {
			swerr();
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(I_PLINK):
			ptrace(("%s: %p: I_PLINK\n", DRV_NAME, sl));
#ifdef HAVE_KMEMB_STRUCT_CRED_UID_VAL
			if (iocp->ioc_cr->cr_uid.val != 0)
#else
			if (iocp->ioc_cr->cr_uid != 0)
#endif
			{
				ptrace(("%s: %p: ERROR: Non-root attempt to I_PLINK\n", DRV_NAME,
					sl));
				ret = -EPERM;
				break;
			}
			/* fall through */
		case _IOC_NR(I_LINK):
			ptrace(("%s: %p: I_LINK\n", DRV_NAME, sl));
			MOD_INC_USE_COUNT;	/* keep module from unloading */
			spin_lock_irqsave(&master.lock, flags);
			{
				/* place in list in ascending index order */
				for (lpp = &master.ls.list;
				     *lpp && (*lpp)->u.mux.index < lb->l_index;
				     lpp = &(*lpp)->next) ;
				if ((ls = slm_alloc_ls(lb->l_qbot, lpp, lb->l_index, iocp->ioc_cr))) {
					spin_unlock_irqrestore(&master.lock, flags);
					break;
				}
				MOD_DEC_USE_COUNT;
				ret = -ENOMEM;
			}
			spin_unlock_irqrestore(&master.lock, flags);
			break;
		case _IOC_NR(I_PUNLINK):
			ptrace(("%s: %p: I_PUNLINK\n", DRV_NAME, sl));
#ifdef HAVE_KMEMB_STRUCT_CRED_UID_VAL
			if (iocp->ioc_cr->cr_uid.val != 0)
#else
			if (iocp->ioc_cr->cr_uid != 0)
#endif
			{
				ptrace(("%s: %p: ERROR: Non-root attempt to I_PUNLINK\n", DRV_NAME,
					sl));
				ret = -EPERM;
				break;
			}
			/* fall through */
		case _IOC_NR(I_UNLINK):
			ptrace(("%s: %p: I_UNLINK\n", DRV_NAME, sl));
			spin_lock_irqsave(&master.lock, flags);
			{
				for (ls = master.ls.list; ls; ls = ls->next)
					if (ls->u.mux.index == lb->l_index)
						break;
				if (ls == NULL) {
					ret = -EINVAL;
					ptrace(("%s: %p: ERROR: Couldn't find I_UNLINK muxid %u\n",
						DRV_NAME, sl, lb->l_index));
					spin_unlock_irqrestore(&master.lock, flags);
					break;
				}
				slm_free_ls(ls->iq);
				MOD_DEC_USE_COUNT;
			}
			spin_unlock_irqrestore(&master.lock, flags);
			break;
		default:
		case _IOC_NR(I_STR):
			ptrace(("%s: %p: ERROR: Unsupported STREAMS ioctl %c, %d\n", DRV_NAME, sl,
				(char) type, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	default:
		ptrace(("%s: %p: ERROR: Unsupported ioctl %c, %d\n", DRV_NAME, sl, (char) type,
			nr));
		ret = -EOPNOTSUPP;
		break;
	}
	if (ret > 0) {
		return (ret);
	} else if (ret == 0) {
		mp->b_datap->db_type = M_IOCACK;
		iocp->ioc_error = 0;
		iocp->ioc_rval = 0;
	} else {
		mp->b_datap->db_type = M_IOCNAK;
		iocp->ioc_error = -ret;
		iocp->ioc_rval = -1;
	}
	qreply(q, mp);
	return (QR_ABSORBED);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Message Handling
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  UPPER MUX Queues
 *  -----------------------------------
 */

STATIC streamscall int
sl_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	default:
		return (QR_PASSFLOW);
	}
}

STATIC streamscall int
sl_w_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_IOCTL:
		return sl_w_ioctl(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	default:
		return (QR_PASSFLOW);
	}
}

/*
 *  LOWER MUX Queues
 *  -----------------------------------
 */

STATIC streamscall int
ls_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sl_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sl_r_proto(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	default:
		return (QR_PASSFLOW);
	}
}

STATIC streamscall int
ls_w_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	default:
		return (QR_PASSFLOW);
	}
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
STATIC major_t slm_majors[CMAJORS] = { CMAJOR_0, };

/*
 *  OPEN
 *  -------------------------------------------------------------------------
 */

STATIC streamscall int
sl_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	psw_t flags;
	int mindex = 0;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	struct sl *sl, **spp;
	MOD_INC_USE_COUNT;
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next) {
		ptrace(("%s: ERROR: cannot push as module\n", DRV_NAME));
		MOD_DEC_USE_COUNT;
		return (EIO);
	}
	if (cmajor != CMAJOR_0 || cminor > 0) {
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	/* allocate a new device */
	cminor = 1;
	spin_lock_irqsave(&master.lock, flags);
	for (spp = &master.sl.list; *spp; spp = &(*spp)->next) {
		major_t dmajor = (*spp)->u.dev.cmajor;
		if (cmajor != dmajor)
			break;
		if (cmajor == dmajor) {
			minor_t dminor = (*spp)->u.dev.cminor;
			if (cminor < dminor)
				break;
			if (cminor > dminor)
				continue;
			if (cminor == dminor) {
				if (++cminor >= NMINORS) {
					if (++mindex >= CMAJORS || !(cmajor = slm_majors[mindex]))
						break;
					cminor = 0;
				}
				continue;
			}
		}
	}
	if (mindex >= CMAJORS || !cmajor) {
		ptrace(("%s: ERROR: no device numbers available\n", DRV_NAME));
		spin_unlock_irqrestore(&master.lock, flags);
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	printd(("%s: opened character device %hu:%hu\n", DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(sl = slm_alloc_sl(q, spp, cmajor, cminor, crp))) {
		ptrace(("%s: ERROR: no memory\n", DRV_NAME));
		spin_unlock_irqrestore(&master.lock, flags);
		MOD_DEC_USE_COUNT;
		return (ENOMEM);
	}
	spin_unlock_irqrestore(&master.lock, flags);
	return (0);
}

/*
 *  CLOSE
 *  -------------------------------------------------------------------------
 */

STATIC streamscall int
sl_close(queue_t *q, int sflag, cred_t *crp)
{
	struct sl *sl = SL_PRIV(q);
	psw_t flags;
	(void) sl;
	printd(("%s: %p: closing character device %hu:%hu\n", DRV_NAME, sl, sl->u.dev.cmajor,
		sl->u.dev.cminor));
	spin_lock_irqsave(&master.lock, flags);
	{
		slm_free_sl(q);
	}
	spin_unlock_irqrestore(&master.lock, flags);
	MOD_DEC_USE_COUNT;
	return (0);
}

/*
 *  =========================================================================
 *
 *  Registration and initialization
 *
 *  =========================================================================
 */
#ifdef LINUX
/*
 *  Linux Registration
 *  -------------------------------------------------------------------------
 */

unsigned short modid = DRV_ID;
#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for the SDL-MUX driver. (0 for allocation.)");

major_t major = CMAJOR_0;
#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0444);
#endif
MODULE_PARM_DESC(major, "Device number for the SDL-MUX driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC struct cdevsw slm_cdev = {
	.d_name = DRV_NAME,
	.d_str = &sl_mininfo,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC int
slm_register_strdev(major_t major)
{
	int err;
	if ((err = register_strdev(&slm_cdev, major)) < 0)
		return (err);
	return (0);
}

STATIC int
slm_unregister_strdev(major_t major)
{
	int err;
	if ((err = unregister_strdev(&slm_cdev, major)) < 0)
		return (err);
	return (0);
}

MODULE_STATIC void
sl_minterminate(void)
{
	int err, mindex;
	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (slm_majors[mindex]) {
			if ((err = slm_unregister_strdev(slm_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					slm_majors[mindex]);
			if (mindex)
				slm_majors[mindex] = 0;
		}
	}
	if ((err = slm_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	return;
}

MODULE_STATIC int
sl_mininit(void)
{
	int err, mindex = 0;
	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = slm_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		sl_minterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = slm_register_strdev(slm_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					slm_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				sl_minterminate();
				return (err);
			}
		}
		if (slm_majors[mindex] == 0)
			slm_majors[mindex] = err;
#if 0
		LIS_DEVFLAGS(slm_majors[mindex]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = slm_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(sl_mininit);
module_exit(sl_minterminate);

#endif				/* LINUX */
