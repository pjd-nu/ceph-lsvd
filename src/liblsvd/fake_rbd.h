/*
 * file:        fake_rbd.h
 * description: replacement for <rbd/librbd.h>
 * author:      Peter Desnoyers, Northeastern University
 * Copyright 2021, 2022 Peter Desnoyers
 * license:     GNU LGPL v2.1 or newer
 *              LGPL-2.1-or-later
 */

#ifndef __FAKE_RBD_H__
#define __FAKE_RBD_H__

#if __GNUC__ >= 4
  #define CEPH_RBD_API          __attribute__ ((visibility ("default")))
  #define CEPH_RBD_DEPRECATED   __attribute__((deprecated))
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#else
  #define CEPH_RBD_API
  #define CEPH_RBD_DEPRECATED
#endif

#include <stdint.h>
#include <stddef.h>
#include <sys/uio.h>
#include <rados/librados.h>

/* the following types have to be compatible with the real librbd.h
 */
enum {
    EVENT_TYPE_PIPE = 1,
    EVENT_TYPE_EVENTFD = 2
};

typedef void *rbd_image_t;
typedef void *rbd_image_options_t;
typedef void *rbd_pool_stats_t;

typedef void *rbd_completion_t;
typedef void (*rbd_callback_t)(rbd_completion_t cb, void *arg);

#define RBD_MAX_BLOCK_NAME_SIZE 24
#define RBD_MAX_IMAGE_NAME_SIZE 96

/* fio only looks at 'size' */
typedef struct {
    uint64_t size;
    uint64_t obj_size;
    uint64_t num_objs;
    int order;
    char block_name_prefix[RBD_MAX_BLOCK_NAME_SIZE]; /* deprecated */
    int64_t parent_pool;                             /* deprecated */
    char parent_name[RBD_MAX_IMAGE_NAME_SIZE];       /* deprecated */
} rbd_image_info_t;

typedef struct {
    uint64_t id;
    uint64_t size;
    const char *name;
} rbd_snap_info_t;

extern "C" CEPH_RBD_API int rbd_poll_io_events(rbd_image_t image,
                                               rbd_completion_t *comps, int numcomp);

extern "C" CEPH_RBD_API int rbd_set_image_notification(rbd_image_t image, int fd, int type);

extern "C" CEPH_RBD_API int rbd_aio_create_completion(void *cb_arg,
                                                      rbd_callback_t complete_cb,
                                                      rbd_completion_t *c);

extern "C" CEPH_RBD_API void rbd_aio_release(rbd_completion_t c);

extern "C" CEPH_RBD_API int rbd_aio_discard(rbd_image_t image, uint64_t off,
                                            uint64_t len, rbd_completion_t c);

extern "C" CEPH_RBD_API int rbd_aio_flush(rbd_image_t image, rbd_completion_t c);

extern "C" CEPH_RBD_API int rbd_flush(rbd_image_t image);

extern "C" CEPH_RBD_API void *rbd_aio_get_arg(rbd_completion_t c);

extern "C" CEPH_RBD_API ssize_t rbd_aio_get_return_value(rbd_completion_t c);

extern "C" CEPH_RBD_API int rbd_aio_read(rbd_image_t image, uint64_t offset,
                                         size_t len, char *buf, rbd_completion_t c);

extern "C" CEPH_RBD_API int rbd_aio_readv(rbd_image_t image, const iovec *iov,
                                          int iovcnt, uint64_t off, rbd_completion_t c);

extern "C" CEPH_RBD_API int rbd_aio_writev(rbd_image_t image, const struct iovec *iov,
                                           int iovcnt, uint64_t off, rbd_completion_t c);

extern "C" CEPH_RBD_API int rbd_aio_write(rbd_image_t image, uint64_t off,
                                          size_t len, const char *buf, rbd_completion_t c);

extern "C" CEPH_RBD_API int rbd_read(rbd_image_t image, uint64_t off, size_t len, char *buf);

extern "C" CEPH_RBD_API int rbd_write(rbd_image_t image, uint64_t off, size_t len,
                                      const char *buf);

extern "C" CEPH_RBD_API int rbd_aio_writesame(rbd_image_t image, uint64_t off,
                                              size_t len,
                                              const char *buf, size_t data_len,
                                              rbd_completion_t c, int op_flags);

extern "C" CEPH_RBD_API int rbd_aio_write_zeroes(rbd_image_t image,
                                                 uint64_t off,
                                                 size_t len, rbd_completion_t c,
                                                 int zero_flags, int op_flags);

extern "C" CEPH_RBD_API int rbd_aio_wait_for_complete(rbd_completion_t c);

extern "C" CEPH_RBD_API int rbd_stat(rbd_image_t image, rbd_image_info_t *info,
                                     size_t infosize);

extern "C" CEPH_RBD_API int rbd_get_size(rbd_image_t image, uint64_t *size);

extern "C" CEPH_RBD_API int rbd_open(rados_ioctx_t io, const char *name,
                                     rbd_image_t *image, const char *snap_name);
extern "C" CEPH_RBD_API int rbd_close(rbd_image_t image);
extern "C" CEPH_RBD_API int rbd_invalidate_cache(rbd_image_t image);

extern "C" CEPH_RBD_API int rbd_create(rados_ioctx_t io, const char *name, uint64_t size, int *order);
extern "C" CEPH_RBD_API int rbd_remove(rados_ioctx_t io, const char *name);
typedef int (*librbd_progress_fn_t)(uint64_t offset, uint64_t total, void *ptr);
extern "C" CEPH_RBD_API int rbd_remove_with_progress(rados_ioctx_t io, const char *name,
                                                     librbd_progress_fn_t cb, void *cbdata);

/* These RBD functions are unimplemented and return errors
 */
extern "C" CEPH_RBD_API int rbd_resize(rbd_image_t image, uint64_t size);
extern "C" CEPH_RBD_API int rbd_snap_create(rbd_image_t image, const char *snapname);
extern "C" CEPH_RBD_API int rbd_snap_list(rbd_image_t image, rbd_snap_info_t *snaps, int *max_snaps);
extern "C" CEPH_RBD_API void rbd_snap_list_end(rbd_snap_info_t *snaps);
extern "C" CEPH_RBD_API int rbd_snap_remove(rbd_image_t image, const char *snapname);
extern "C" CEPH_RBD_API int rbd_snap_rollback(rbd_image_t image, const char *snapname);

#endif
