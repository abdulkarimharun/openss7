/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/* qb2pe.c - create a variable-depth Inline CONStructor PElement */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/psap/RCS/qb2pe.c,v 9.0 1992/06/16 12:25:44 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/psap/RCS/qb2pe.c,v 9.0 1992/06/16 12:25:44 isode Rel
 *
 *
 * Log: qb2pe.c,v
 * Revision 9.0  1992/06/16  12:25:44  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

/* LINTLIBRARY */

#include <stdio.h>
#include "psap.h"
#include "tailor.h"

/*  */

static PE qb2pe_aux();

/*  */

PE
qb2pe(qb, len, depth, result)
	register struct qbuf *qb;
	int len, depth;
	int *result;
{
	char *sp;
	register struct qbuf *qp;
	PE pe;

	*result = PS_ERR_NONE;
	if (depth <= 0)
		return NULLPE;

	if ((qp = qb->qb_forw) != qb && qp->qb_forw == qb)
		sp = qp->qb_data;
	else {
		qp = NULL;

		if ((sp = qb2str(qb)) == NULL) {
			*result = PS_ERR_NMEM;
			return NULLPE;
		}
	}

	if (pe = qb2pe_aux(sp, len, depth, result)) {
		if (qp) {
			pe->pe_realbase = (char *) qp;

			remque(qp);
		} else {
			pe->pe_realbase = sp;

			QBFREE(qb);
		}
		pe->pe_inline = 0;
	} else if (qp == NULL)
		free(sp);

#ifdef	DEBUG
	if (pe && (psap_log->ll_events & LLOG_PDUS))
		pe2text(psap_log, pe, 1, len);
#endif

	return pe;
}

/*  */

static PE
qb2pe_aux(s, len, depth, result)
	register char *s;
	register int len;
	int depth;
	int *result;
{
	int i;
	register PElementData data;
	register PE pe, p, q;
	PE *r, *rp;

	depth--;

	if ((pe = str2pe(s, len, &i, result)) == NULLPE)
		return NULLPE;

	if (pe->pe_form == PE_FORM_ICONS) {
		pe->pe_form = PE_FORM_CONS;
		pe->pe_prim = NULLPED, pe->pe_inline = 0;
		pe->pe_len -= pe->pe_ilen;

		p = NULLPE, r = &pe->pe_cons;
		for (s += pe->pe_ilen, len -= pe->pe_ilen; len > 0; s += i, len -= i) {
			if ((p = str2pe(s, len, &i, result)) == NULLPE)
				goto out;

			if (p->pe_form == PE_FORM_ICONS) {
				if (depth > 0) {
					if ((q = qb2pe_aux((char *) p->pe_prim, i, depth,
							   result)) == NULLPE)
						goto out;
					pe_free(p);
					p = q;
				} else {
					if ((data = PEDalloc(i)) == NULL) {
						*result = PS_ERR_NMEM;
						goto out;
					}
					PEDcpy(p->pe_prim, data, i);
					p->pe_prim = data, p->pe_inline = 0;
				}
			}

			*r = p, rp = r, r = &p->pe_next;
		}

		if (p && p->pe_class == PE_CLASS_UNIV && p->pe_id == PE_UNIV_EOC) {
			pe_free(p);
			*rp = NULLPE;
		}
	}

	return pe;

      out:;
	pe_free(pe);
	return NULLPE;
}