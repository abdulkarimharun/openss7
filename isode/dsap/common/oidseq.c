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

/* oidseq.c - OID Sequence utility routines */

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/oidseq.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/dsap/common/RCS/oidseq.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: oidseq.c,v
 * Revision 9.0  1992/06/16  12:12:39  isode
 * Release 8.0
 *
 */

/*
 *                                NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

/* LINTLIBRARY */

#include "quipu/util.h"
#include "quipu/entry.h"

extern int oidformat;

oid_seq_free(ptr)
	struct oid_seq *ptr;
{
	register struct oid_seq *loop;
	register struct oid_seq *next;

	for (loop = ptr; loop != NULLOIDSEQ; loop = next) {
		next = loop->oid_next;
		oid_free(loop->oid_oid);
		free((char *) loop);
	}
}

oid_seq_free_aux(ptr)
	struct oid_seq *ptr;
{
	register struct oid_seq *loop;
	register struct oid_seq *next;

	for (loop = ptr; loop != NULLOIDSEQ; loop = next) {
		next = loop->oid_next;
		free((char *) loop);
	}
}

struct oid_seq *
oid_seq_merge(a, b)
	struct oid_seq *a;
	struct oid_seq *b;
{
	register struct oid_seq *aptr, *bptr, *result, *trail;

	if (a == NULLOIDSEQ)
		return (b);
	if (b == NULLOIDSEQ)
		return (a);

	/* start sequence off, make sure 'a' is the first */
	switch (oid_cmp(a->oid_oid, b->oid_oid)) {
	case 0:		/* equal */
		result = a;
		oid_free(b->oid_oid);
		free((char *) b);
		aptr = a->oid_next;
		bptr = b->oid_next;
		break;
	case -1:
		result = b;
		aptr = a;
		bptr = b->oid_next;
		break;
	case 1:
		result = a;
		aptr = a->oid_next;
		bptr = b;
		break;
	}

	trail = result;
	while ((aptr != NULLOIDSEQ) && (bptr != NULLOIDSEQ)) {

		switch (oid_cmp(aptr->oid_oid, bptr->oid_oid)) {
		case 0:	/* equal */
			trail->oid_next = aptr;
			trail = aptr;
			oid_free(bptr->oid_oid);
			free((char *) bptr);
			aptr = aptr->oid_next;
			bptr = bptr->oid_next;
			break;
		case -1:
			trail->oid_next = bptr;
			trail = bptr;
			bptr = bptr->oid_next;
			break;
		case 1:
			trail->oid_next = aptr;
			trail = aptr;
			aptr = aptr->oid_next;
			break;
		}
	}
	if (aptr == NULLOIDSEQ)
		trail->oid_next = bptr;
	else
		trail->oid_next = aptr;

	return (result);
}

oid_seq_cmp(a, b)
	struct oid_seq *a, *b;
{
	struct oid_seq *aa1;
	struct oid_seq *aa2;

	if ((a == NULLOIDSEQ) && (b == NULLOIDSEQ))
		return (0);

	if (a == NULLOIDSEQ)
		return (-1);

	if (b == NULLOIDSEQ)
		return (1);

	for (aa1 = a; aa1 != NULLOIDSEQ; aa1 = aa1->oid_next) {
		for (aa2 = b; aa2 != NULLOIDSEQ; aa2 = aa2->oid_next) {
			if (oid_cmp(aa1->oid_oid, aa2->oid_oid) == 0)
				break;
		}
		if (aa2 == NULLOIDSEQ)
			return (1);
	}

	for (aa2 = b; aa2 != NULLOIDSEQ; aa2 = aa2->oid_next) {
		for (aa1 = a; aa1 != NULLOIDSEQ; aa1 = aa1->oid_next) {
			if (oid_cmp(aa1->oid_oid, aa2->oid_oid) == 0)
				break;
		}
		if (aa1 == NULLOIDSEQ)
			return (-1);
	}

	return (0);
}

struct oid_seq *
oid_seq_cpy(a)
	struct oid_seq *a;
{
	register struct oid_seq *b;
	register struct oid_seq *c;
	register struct oid_seq *d;
	struct oid_seq *result;

	result = oid_seq_alloc();
	result->oid_oid = oid_cpy(a->oid_oid);
	result->oid_next = NULLOIDSEQ;
	b = result;

	for (c = a->oid_next; c != NULLOIDSEQ; c = c->oid_next) {
		d = oid_seq_alloc();
		d->oid_oid = oid_cpy(c->oid_oid);
		d->oid_next = NULLOIDSEQ;
		b->oid_next = d;
		b = d;
	}
	return (result);
}

oid_seq_print(ps, ptr, format)
	PS ps;
	register struct oid_seq *ptr;
	int format;
{
	register int i = 4;

	ps_printf(ps, "%s", oid2name(ptr->oid_oid, oidformat));
	for (ptr = ptr->oid_next; ptr != NULLOIDSEQ; ptr = ptr->oid_next, i++)
		if (format == READOUT) {
			if (i > 3) {
				i = 0;
				ps_print(ps, ",\n\t\t\t");
			} else
				ps_print(ps, ", ");
			ps_printf(ps, "%s", oid2name(ptr->oid_oid, oidformat));
		} else
			ps_printf(ps, "$%s", oid2name(ptr->oid_oid, oidformat));
}

struct oid_seq *
str2oidseq(str)
	char *str;
{
	register char *ptr;
	register char *save, val;
	struct oid_seq *ois = NULLOIDSEQ;
	struct oid_seq *newois;
	OID oid;
	char *SkipSpace();

	while ((ptr = index(str, '$')) != 0) {
		save = ptr++;
		save--;
		if (!isspace(*save))
			save++;
		val = *save;
		*save = 0;
		newois = oid_seq_alloc();
		if ((oid = name2oid(SkipSpace(str))) == NULLOID) {
			parse_error("invalid name in sequence %s", str);
			oid_seq_free(ois);
			free((char *) newois);
			return (NULLOIDSEQ);
		}
		newois->oid_oid = oid;
		newois->oid_next = ois;
		ois = newois;
		*save = val;
		str = ptr;
	}

	newois = oid_seq_alloc();
	if ((oid = name2oid(SkipSpace(str))) == NULLOID) {
		parse_error("invalid name in sequence (2) %s", str);
		oid_seq_free(ois);
		free((char *) newois);
		return (NULLOIDSEQ);
	}
	newois->oid_oid = oid;
	newois->oid_next = ois;
	ois = newois;

	return (ois);
}