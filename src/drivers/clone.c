/*****************************************************************************

 @(#) File: src/drivers/clone.c

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

static char const ident[] = "src/drivers/clone.c (" PACKAGE_ENVR ") " PACKAGE_DATE;

#ifdef NEED_LINUX_AUTOCONF_H
#include NEED_LINUX_AUTOCONF_H
#endif
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>

#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#include "sys/config.h"
#include "clone.h"		/* extern verification */

#define CLONE_DESCRIP	"SVR 4.2 STREAMS Clone Driver"
#define CLONE_EXTRA	"Part of UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define CLONE_COPYRIGHT	"Copyright (c) 2008-2019  Monavacon Limited.  All Rights Reserved."
#define CLONE_REVISION	"OpenSS7 src/drivers/clone.c (" PACKAGE_ENVR ") " PACKAGE_DATE
#define CLONE_DEVICE	"SVR 4.2 MP STREAMS CLONE Driver"
#define CLONE_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define CLONE_LICENSE	"GPL"
#define CLONE_BANNER	CLONE_DESCRIP	"\n" \
			CLONE_COPYRIGHT	"\n" \
			CLONE_REVISION	"\n" \
			CLONE_DEVICE	"\n" \
			CLONE_CONTACT	"\n"
#define CLONE_SPLASH	CLONE_DEVICE	" - " \
			CLONE_REVISION	"\n"

#ifdef CONFIG_STREAMS_CLONE_MODULE
MODULE_AUTHOR(CLONE_CONTACT);
MODULE_DESCRIPTION(CLONE_DESCRIP);
MODULE_SUPPORTED_DEVICE(CLONE_DEVICE);
MODULE_LICENSE(CLONE_LICENSE);
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif
#endif

#ifndef CONFIG_STREAMS_CLONE_NAME
//#define CONFIG_STREAMS_CLONE_NAME "clone"
#error "CONFIG_STREAMS_CLONE_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_CLONE_MODID
//#define CONFIG_STREAMS_CLONE_MODID 5
#error "CONFIG_STREAMS_CLONE_MODID must be defined."
#endif
#ifndef CONFIG_STREAMS_CLONE_MAJOR
//#define CONFIG_STREAMS_CLONE_MAJOR 54
#error "CONFIG_STREAMS_CLONE_MAJOR must be defined."
#endif

#ifdef CONFIG_STREAMS_CLONE_MODULE
modID_t clone_modid = CONFIG_STREAMS_CLONE_MODID;

#ifndef module_param
MODULE_PARM(clone_modid, "h");
#else
module_param(clone_modid, ushort, 0444);
#endif
MODULE_PARM_DESC(clone_modid, "Module id number for CLONE driver.");
#else				/* CONFIG_STREAMS_CLONE_MODULE */
static modID_t clone_modid = CONFIG_STREAMS_CLONE_MODID;
#endif				/* CONFIG_STREAMS_CLONE_MODULE */

#ifdef CONFIG_STREAMS_CLONE_MODULE
major_t major = CONFIG_STREAMS_CLONE_MAJOR;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0444);
#endif
MODULE_PARM_DESC(major, "Major device number for CLONE driver.");
#else				/* CONFIG_STREAMS_CLONE_MODULE */
static major_t major = CONFIG_STREAMS_CLONE_MAJOR;
#endif				/* CONFIG_STREAMS_CLONE_MODULE */

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-clone");
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_CLONE_MODID));
MODULE_ALIAS("streams-driver-clone");
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_CLONE_MAJOR) "-*");
MODULE_ALIAS("/dev/clone");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_CLONE_MAJOR));
MODULE_ALIAS("/dev/streams/clone");
MODULE_ALIAS("/dev/streams/clone/*");
#endif
#endif

static struct module_info clone_minfo = {
	.mi_idnum = CONFIG_STREAMS_CLONE_MODID,
	.mi_idname = CONFIG_STREAMS_CLONE_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat clone_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat clone_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit clone_rinit = {
	.qi_putp = strrput,
	.qi_srvp = strrsrv,
	.qi_qopen = str_open,
	.qi_qclose = str_close,
	.qi_minfo = &clone_minfo,
	.qi_mstat = &clone_rstat,
};

static struct qinit clone_winit = {
	.qi_putp = strwput,
	.qi_srvp = strwsrv,
	.qi_minfo = &clone_minfo,
	.qi_mstat = &clone_wstat,
};

#ifdef CONFIG_STREAMS_CLONE_MODULE
static
#endif
struct streamtab cloneinfo = {
	.st_rdinit = &clone_rinit,
	.st_wrinit = &clone_winit,
};

/**
 *  cloneopen: - open a clone special device
 *  @inode:	shadow special filesystem inode
 *  @file:	shadow special filesystem file pointer
 *
 *  cloneopen() is called only from within the shadow special filesystem.  This can occur by
 *  chaining from the external filesystem (e.g. openining a character device with clone major) or by
 *  direct open of the inode within the mounted shadow special filesystem.  Either way, the inode
 *  number has our extended device numbering as a inode number and we chain the call within the
 *  shadow special filesystem.
 */
static int
cloneopen(struct inode *inode, struct file *file)
{
	struct cdevsw *cdev;
	dev_t dev = inode->i_ino;

	if (file->private_data)
		/* Darn.  Somebody passed us a FIFO inode. */
		return (-EIO);

	if ((cdev = sdev_get(getminor(dev)))) {
		int err;

		dev = makedevice(cdev->d_modid, 0);
		err = spec_open(file, cdev, dev, CLONEOPEN);
		sdev_put(cdev);
		return (err);
	}
	return (-ENOENT);
}

struct file_operations clone_ops ____cacheline_aligned = {
	.owner = THIS_MODULE,
	.open = cloneopen,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  INITIALIZATION
 *
 *  -------------------------------------------------------------------------
 */

static struct cdevsw clone_cdev = {
	.d_name = "clone",
	.d_str = &cloneinfo,
	.d_flag = D_CLONE | D_MP,
	.d_fop = &clone_ops,
	.d_mode = S_IFCHR | S_IRUGO | S_IWUGO,
	.d_kmod = THIS_MODULE,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  Special open for character based streams, fifos and pipes.
 *
 *  -------------------------------------------------------------------------
 */

/**
 *  cdev_open: - open a character special device node
 *  @inode: the character device inode
 *  @file: the user file pointer
 *
 *  cdev_open() is only used to open a stream from a character device node in an external
 *  filesystem.  This is never called for direct opens of a specfs device node (for direct opens see
 *  spec_dev_open() in strspecfs.c).  It is also not used for direct opens of fifos, pipes or
 *  sockets.  Those devices provide their own file operations to the main operating system.  The
 *  character device number from the inode is used to determine the shadow special file system
 *  (internal) inode and chain the open call.
 *
 *  This is the separation point where we convert the external device number to an internal device
 *  number.  The external device number is contained in inode->i_rdev.
 *
 *  @inode is the inode in the external filesystem.
 *
 *  @file->f_op is the external file operations (character device, fifo) and must be replaced with
 *	our file operations.
 *
 *  @file->f_dentry is the external filesystem dentry for the device node.
 *  @file->f_vfsmnt is the external filesystem vfsmnt for the device node.
 *  @file exists on the file->f_dentry->d_inode->i_sb->s_files list.
 *
 *  What we should be doing here is get a fresh new dentry.  Find our inode from the device number,
 *  add it to the dentry.  Set the dentry->d_sb to the specfs super block, set dentry->d_parent =
 *  dget(file->f_dentry->d_parent), but do not add the dentry to the child list on the parent
 *  directory, nor do we hash the dentry.  Next we do a dentry open on the on the dentry and a file
 *  pointer swap on return.
 *
 *  Instead of farting around with dentries and such, just lookup the inode in the specfs replace
 *  the file->f_ops and chain the open with the specfs inode passed to the new open procedure.  For
 *  FIFOs we pass the external filesystem inode instead.
 */
STATIC int
cdev_open(struct inode *inode, struct file *file)
{
	int err;
	struct cdevsw *cdev;
	struct devnode *cmin;
	major_t major;
	minor_t minor;
	modID_t modid;
	dev_t dev;
	int sflag;

#if defined HAVE_KFUNC_TO_KDEV_T
	minor = MINOR(kdev_t_to_nr(inode->i_rdev));
	major = MAJOR(kdev_t_to_nr(inode->i_rdev));
#else
	minor = MINOR(inode->i_rdev);
	major = MAJOR(inode->i_rdev);
#endif
	if (!(cdev = sdev_get(major))) {
		return (-ENXIO);
	}
	minor = cdev_minor(cdev, major, minor);
	major = cdev->d_major;
	modid = cdev->d_modid;
	dev = makedevice(modid, minor);
	sflag = DRVOPEN;
	if (cdev->d_flag & D_CLONE)
		sflag = CLONEOPEN;
	else if ((cmin = cmin_get(cdev, minor)) && cmin->n_flag & D_CLONE)
		sflag = CLONEOPEN;
	err = spec_open(file, cdev, dev, sflag);
	sdev_put(cdev);
	return (err);
}

STATIC struct file_operations cdev_f_ops ____cacheline_aligned = {
	.owner = NULL,		/* yes NULL */
	.open = cdev_open,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  REGISTRATION
 *
 *  -------------------------------------------------------------------------
 */

streams_fastcall int
register_clone(struct cdevsw *cdev)
{
	int err;
	struct devnode *cmin;

	err = -ENOMEM;
	if (!(cmin = kmalloc(sizeof(*cmin), GFP_ATOMIC))) {
		goto error;
	}
	memset(cmin, 0, sizeof(*cmin));
	INIT_LIST_HEAD(&cmin->n_list);
	INIT_LIST_HEAD(&cmin->n_hash);
	cmin->n_name = cdev->d_name;
	cmin->n_str = cdev->d_str;
	cmin->n_flag = clone_cdev.d_flag;
	cmin->n_modid = cdev->d_modid;
	cmin->n_count = (atomic_t) ATOMIC_INIT(0);
	cmin->n_sqlvl = cdev->d_sqlvl;
	cmin->n_syncq = cdev->d_syncq;
	cmin->n_kmod = cdev->d_kmod;
	cmin->n_major = clone_cdev.d_major;
	cmin->n_mode = clone_cdev.d_mode;
	cmin->n_minor = cdev->d_major;
	cmin->n_dev = &clone_cdev;
	if ((err = register_strnod(&clone_cdev, cmin, cdev->d_major)) < 0) {
		kfree(cmin);
		goto error;
	}
      error:
	return (err);
}

EXPORT_SYMBOL_GPL(register_clone);

streams_fastcall int
unregister_clone(struct cdevsw *cdev)
{
	int err;
	struct devnode *cmin;

	err = -ENXIO;
	if (!(cmin = cmin_get(&clone_cdev, cdev->d_major)))
		goto error;
	if ((err = unregister_strnod(&clone_cdev, cdev->d_major)))
		goto error;
	kfree(cmin);
      error:
	return (err);
}

EXPORT_SYMBOL_GPL(unregister_clone);

/**
 *  register_strdev: - register a STREAMS device against a device major number
 *  @cdev: STREAMS character device structure to register
 *  @major: requested major device number or 0 for automatic major selection
 *
 *  register_strdev() registers the device specified by the @cdev to the device major number
 *  specified by @major.
 *
 *  register_strdev() will register the STREAMS character device specified by @cdev against the
 *  major device number @major.  If the major device number is zero, then it requests that
 *  register_strdev() allocate an available major device number and assign it to @cdev.
 *
 *  CONTEXT: register_strdev() is intended to be called from kernel __init() or module_init()
 *  routines only.  It cannot be called from in_irq() level.
 *
 *  Return Values: Upon success, register_strdev() will return the requested or assigned major
 *  device number as a positive integer value.  Upon failure, the registration is denied and a
 *  negative error number is returned.
 *
 *  Errors: Upon failure, register_strdev() returns on of the negative error numbers listed below.
 *
 *  -[%ENOMEM]	insufficient memory was available to complete the request.
 *
 *  -[%EINVAL]	@cdev was NULL
 *
 *  -[%EBUSY]	a device was already registered against the requested major device number, or no
 *	        device numbers were available for automatic major device number assignment.
 *
 *  Notes: Linux Fast-STREAMS provides improvements over LiS.
 *
 *  LfS uses a small hash instead of a cdevsw[] table and requires that the driver (statically)
 *  allocate its &struct cdevsw structure using an approach more likened to the Solaris &struct
 *  cb_ops.
 */
streams_fastcall int
register_strdev(struct cdevsw *cdev, major_t major)
{
	int err;

	if (!cdev->d_fop)
		cdev->d_fop = &strm_f_ops;
	if (!(cdev->d_mode & S_IFMT))
		cdev->d_mode = (cdev->d_mode & ~S_IFMT) | S_IFCHR;
	if ((err = register_cmajor(cdev, major, &cdev_f_ops)) < 0)
		return (err);
	register_clone(cdev);
	return (err);
}

EXPORT_SYMBOL(register_strdev);

/**
 *  unregister_strdev: - unregister previously registered STREAMS device
 *  @cdev: STREAMS character device structure to unregister
 *  @major: major device number to unregister or 0 for all majors
 *
 *  unregister_strdev() unregisters the device specified by the @cdev from the device major number
 *  specified by @dev.  Only the getmajor(@dev) component of @dev is significant and the
 *  getminor(@dev) component must be coded zero (0).
 *
 *  unregister_strdev() will unregister the STREAMS character device specified by @cdev from the
 *  major device number in getmajor(@dev).  If the major device number is zero, then it requests
 *  that unregister_strdev() unregister @cdev from any device majors with which it is currently
 *  registered.
 *
 *  CONTEXT: unregister_strdev() is intended to be called from kernel __exit() or module_exit()
 *  routines only.  It cannot be called from in_irq() level.
 *
 *  Return Values: Upon success, unregister_strdev() will return zero (0).  Upon failure, the
 *  deregistration is denied and a negative error number is returned.
 *
 *  Errors: Upon failure, unregister_strdev() returns one of the negative error numbers listed
 *  below.
 *
 *  -[%ENXIO]	The specified device does not exist in the registration tables.
 *
 *  -[%EINVAL]	@cdev is NULL, or the @d_name component associated with @cdev has changed since
 *              registration.
 *
 *  -[%EPERM]	The device number specified does not belong to the &struct cdev structure specified
 *		and permission is therefore denied.
 */
streams_fastcall int
unregister_strdev(struct cdevsw *cdev, major_t major)
{
	unregister_clone(cdev);
	return unregister_cmajor(cdev, major);
}

EXPORT_SYMBOL(unregister_strdev);

/* 
 *  -------------------------------------------------------------------------
 *
 *  Special open for clone devices.
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  clone_open: - open a clone device node
 *  @inode: the external filesystem inode
 *  @file: the external filesystem file pointer
 *
 *  clone_open() is only used to open a clone device from a character device node in an external
 *  filesystem.  This is never called for direct opens of a specfs device node (for direct opens see
 *  spec_dev_open() in strspecfs.c).  The character device number from the inode is used to
 *  determine the shadow special filesystem (internal) inode and chain the open call.
 *
 *  This is the separation point where we convert the external device number to an internal device
 *  number.  The external device number is contained in inode->i_rdev.
 */
STATIC int
clone_open(struct inode *inode, struct file *file)
{
	int err;
	struct cdevsw *cdev;
	major_t major;
	minor_t minor;
	modID_t modid, instance;

#if defined HAVE_KFUNC_TO_KDEV_T
	minor = MINOR(kdev_t_to_nr(inode->i_rdev));
	major = MAJOR(kdev_t_to_nr(inode->i_rdev));
#else
	minor = MINOR(inode->i_rdev);
	major = MAJOR(inode->i_rdev);
#endif
	minor = cdev_minor(&clone_cdev, major, minor);
	major = clone_cdev.d_major;
	modid = clone_cdev.d_modid;
	err = -ENXIO;
	if (!(cdev = sdev_get(minor))) {
		goto exit;
	}
	instance = cdev->d_modid;
	err = spec_open(file, cdev, makedevice(modid, instance), CLONEOPEN);
	sdev_put(cdev);
      exit:
	return (err);
}

STATIC struct file_operations clone_f_ops ____cacheline_aligned = {
	.owner = NULL,		/* yes NULL */
	.open = clone_open,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  INITIALIZATION
 *
 *  -------------------------------------------------------------------------
 */

#ifdef CONFIG_STREAMS_CLONE_MODULE
static int
#else
int __init
#endif
cloneinit(void)
{
	int err;

#ifdef CONFIG_STREAMS_CLONE_MODULE
	printk(KERN_INFO CLONE_BANNER);
#else
	printk(KERN_INFO CLONE_SPLASH);
#endif
	clone_minfo.mi_idnum = clone_modid;
	if ((err = register_cmajor(&clone_cdev, major, &clone_f_ops)) < 0)
		return (err);
	if (major == 0 && err > 0)
		major = err;
	return (0);
};

#ifdef CONFIG_STREAMS_CLONE_MODULE
static void
#else
void __exit
#endif
cloneexit(void)
{
	if (unregister_cmajor(&clone_cdev, major) != 0)
		swerr();
};

#ifdef CONFIG_STREAMS_CLONE_MODULE
module_init(cloneinit);
module_exit(cloneexit);
#endif
