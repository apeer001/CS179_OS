1 /*
2 Big Brother File System
3 Copyright (C) 2012 Joseph J. Pfeiffer, Jr., Ph.D. <pfeiffer@cs.nmsu.edu>
4
5 This program can be distributed under the terms of the GNU GPLv3.
6 See the file COPYING.
7
8 This code is derived from function prototypes found /usr/include/fuse/fuse.h
9 Copyright (C) 2001−2007 Miklos Szeredi <miklos@szeredi.hu>
10 His code is licensed under the LGPLv2.
11 A copy of that code is included in the file fuse.h
12
13 The point of this FUSE filesystem is to provide an introduction to
14 FUSE. It was my first FUSE filesystem as I got to know the
15 software; hopefully, the comments in this code will help people who
16 follow later to get a gentler introduction.
17
18 This might be called a no−op filesystem: it doesn’t impose
19 filesystem semantics on top of any other existing structure. It
20 simply reports the requests that come in, and passes them to an
21 underlying filesystem. The information is saved in a logfile named
22 bbfs.log, in the directory from which you run bbfs.
23
24 gcc −Wall ‘pkg−config fuse −−cflags −−libs‘ −o bbfs bbfs.c
25 */
26 /* As modified by Thomas Payne (thp) on 10/24/15. See note at end of this file. */
27 #include "params.h"
28 #include <ctype.h>
29 #include <dirent.h>
30 #include <errno.h>
31 #include <fcntl.h>
32 #include <fuse.h>
33 #include <libgen.h>
34 #include <limits.h>
35 #include <stdlib.h>
36 #include <stdio.h>
37 #include <string.h>
38 #include <unistd.h>
39 #include <sys/types.h>
40
41 #ifdef HAVE_SYS_XATTR_H
42 #include <sys/xattr.h>
43 #endif
44
45 #include "log.h"
46 #include "my_stubs.H" /* added (thp) */
47
48 // Report errors to logfile and give −errno to caller
49 static int bb_error(char *str)
50 {
51 int ret = −errno;
52
53 log_msg(" ERROR %s: %s\n", str, strerror(errno));
54
55 return ret;
56 }
57
58 // Check whether the given user is permitted to perform the given operation on the given
59
60 // All the paths I see are relative to the root of the mounted
61 // filesystem. In order to get to the underlying filesystem, I need to
62 // have the mountpoint. I’ll save it away early on in main(), and then
63 // whenever I need a path for something I’ll call this to construct
64 // it.
65 static void bb_fullpath(char fpath[PATH_MAX], const char *path)
66 {
67 strcpy(fpath, BB_DATA−>rootdir);
68 strncat(fpath, path, PATH_MAX); // ridiculously long paths will
69 // break here
70
71 log_msg(" bb_fullpath: rootdir = \"%s\", path = \"%s\", fpath = \"%s\"\n",
72 BB_DATA−>rootdir, path, fpath);
73 }
74
75 ///////////////////////////////////////////////////////////
76 //
77 // Prototypes for all these functions, and the C−style comments,
78 // come indirectly from /usr/include/fuse.h
79 //
80 /** Get file attributes.
81 *
82 * Similar to stat(). The ’st_dev’ and ’st_blksize’ fields are
83 * ignored. The ’st_ino’ field is ignored except if the ’use_ino’
84 * mount option is given.
85 */
86 int bb_getattr(const char *path, struct stat *statbuf)
87 {
88 int retstat = 0;
89 char fpath[PATH_MAX];
90
91 log_msg("\nbb_getattr(path=\"%s\", statbuf=0x%08x)\n",
92 path, statbuf);
93 bb_fullpath(fpath, path);
94
95 retstat = my_lstat(fpath, statbuf);
96 if (retstat != 0)
97 retstat = bb_error("bb_getattr lstat");
98
99 log_stat(statbuf);
100
101 return retstat;
102 }
103
104 /** Read the target of a symbolic link
105 *
106 * The buffer should be filled with a null terminated string. The
107 * buffer size argument includes the space for the terminating
108 * null character. If the linkname is too long to fit in the
109 * buffer, it should be truncated. The return value should be 0
110 * for success.
111 */
112 // Note the system readlink() will truncate and lose the terminating
113 // null. So, the size passed to to the system readlink() must be one
114 // less than the size passed to bb_readlink()
115 // bb_readlink() code by Bernardo F Costa (thanks!)
116 int bb_readlink(const char *path, char *link, size_t size)
117 {
118 int retstat = 0;
119 char fpath[PATH_MAX];
120
121 log_msg("bb_readlink(path=\"%s\", link=\"%s\", size=%d)\n",
122 path, link, size);
123 bb_fullpath(fpath, path);
124
125 retstat = my_readlink(fpath, link, size − 1);
126 if (retstat < 0)
127 retstat = bb_error("bb_readlink readlink");
128 else {
129 link[retstat] = ’\0’;
130 retstat = 0;
131 }
132
133 return retstat;
134 }
135
136 /** Create a file node
137 *
138 * There is no create() operation, mknod() will be called for
139 * creation of all non−directory, non−symlink nodes.
140 */
141 // shouldn’t that comment be "if" there is no.... ?
142 int bb_mknod(const char *path, mode_t mode, dev_t dev)
143 {
144 int retstat = 0;
145 char fpath[PATH_MAX];
146
147 log_msg("\nbb_mknod(path=\"%s\", mode=0%3o, dev=%lld)\n",
148 path, mode, dev);
149 bb_fullpath(fpath, path);
150
151 // On Linux this could just be ’mknod(path, mode, rdev)’ but this
152 // is more portable
153 if (S_ISREG(mode)) {
154 retstat = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
155 if (retstat < 0)
156 retstat = bb_error("bb_mknod open");
157 else {
158 retstat = close(retstat);
159 if (retstat < 0)
160 retstat = bb_error("bb_mknod close");
161 }
162 } else
163 if (S_ISFIFO(mode)) {
164 retstat = mkfifo(fpath, mode);
165 if (retstat < 0)
166 retstat = bb_error("bb_mknod mkfifo");
167 } else {
168 retstat = my_mknod(fpath, mode, dev);
169 if (retstat < 0)
170 retstat = bb_error("bb_mknod mknod");
171 }
172
173 return retstat;
174 }
175
176 /** Create a directory */
177 int bb_mkdir(const char *path, mode_t mode)
178 {
179 int retstat = 0;
180 char fpath[PATH_MAX];
181
182 log_msg("\nbb_mkdir(path=\"%s\", mode=0%3o)\n",
183 path, mode);
184 bb_fullpath(fpath, path);
185
186 retstat = my_mkdir(fpath, mode);
187 if (retstat < 0)
188 retstat = bb_error("bb_mkdir mkdir");
189
190 return retstat;
191 }
192
193 /** Remove a file */
194 int bb_unlink(const char *path)
195 {
196 int retstat = 0;
197 char fpath[PATH_MAX];
198
199 log_msg("bb_unlink(path=\"%s\")\n",
200 path);
201 bb_fullpath(fpath, path);
202
203 retstat = my_unlink(fpath);
204 if (retstat < 0)
205 retstat = bb_error("bb_unlink unlink");
206
207 return retstat;
208 }
209
210 /** Remove a directory */
211 int bb_rmdir(const char *path)
212 {
213 int retstat = 0;
214 char fpath[PATH_MAX];
215
216 log_msg("bb_rmdir(path=\"%s\")\n",
217 path);
218 bb_fullpath(fpath, path);
219
220 retstat = my_rmdir(fpath);
221 if (retstat < 0)
222 retstat = bb_error("bb_rmdir rmdir");
223
224 return retstat;
225 }
226
227 /** Create a symbolic link */
228 // The parameters here are a little bit confusing, but do correspond
229 // to the symlink() system call. The ’path’ is where the link points,
230 // while the ’link’ is the link itself. So we need to leave the path
231 // unaltered, but insert the link into the mounted directory.
232 int bb_symlink(const char *path, const char *link)
233 {
234 int retstat = 0;
235 char flink[PATH_MAX];
236
237 log_msg("\nbb_symlink(path=\"%s\", link=\"%s\")\n",
238 path, link);
239 bb_fullpath(flink, link);
240
241 retstat = my_symlink(path, flink);
242 if (retstat < 0)
243 retstat = bb_error("bb_symlink symlink");
244
245 return retstat;
246 }
247
248 /** Rename a file */
249 // both path and newpath are fs−relative
250 int bb_rename(const char *path, const char *newpath)
251 {
252 int retstat = 0;
253 char fpath[PATH_MAX];
254 char fnewpath[PATH_MAX];
255
256 log_msg("\nbb_rename(fpath=\"%s\", newpath=\"%s\")\n",
257 path, newpath);
258 bb_fullpath(fpath, path);
259 bb_fullpath(fnewpath, newpath);
260
261 retstat = my_rename(fpath, fnewpath);
262 if (retstat < 0)
263 retstat = bb_error("bb_rename rename");
264
265 return retstat;
266 }
267
268 /** Create a hard link to a file */
269 int bb_link(const char *path, const char *newpath)
270 {
271 int retstat = 0;
272 char fpath[PATH_MAX], fnewpath[PATH_MAX];
273
274 log_msg("\nbb_link(path=\"%s\", newpath=\"%s\")\n",
275 path, newpath);
276 bb_fullpath(fpath, path);
277 bb_fullpath(fnewpath, newpath);
278
279 retstat = my_link(fpath, fnewpath);
280 if (retstat < 0)
281 retstat = bb_error("bb_link link");
282
283 return retstat;
284 }
285
286 /** Change the permission bits of a file */
287 int bb_chmod(const char *path, mode_t mode)
288 {
289 int retstat = 0;
290 char fpath[PATH_MAX];
291
292 log_msg("\nbb_chmod(fpath=\"%s\", mode=0%03o)\n",
293 path, mode);
294 bb_fullpath(fpath, path);
295
296 retstat = my_chmod(fpath, mode);
297 if (retstat < 0)
298 retstat = bb_error("bb_chmod chmod");
299
300 return retstat;
301 }
302
303 /** Change the owner and group of a file */
304 int bb_chown(const char *path, uid_t uid, gid_t gid)
305
306 {
307 int retstat = 0;
308 char fpath[PATH_MAX];
309
310 log_msg("\nbb_chown(path=\"%s\", uid=%d, gid=%d)\n",
311 path, uid, gid);
312 bb_fullpath(fpath, path);
313
314 retstat = my_chown(fpath, uid, gid);
315 if (retstat < 0)
316 retstat = bb_error("bb_chown chown");
317
318 return retstat;
319 }
320
321 /** Change the size of a file */
322 int bb_truncate(const char *path, off_t newsize)
323 {
324 int retstat = 0;
325 char fpath[PATH_MAX];
326
327 log_msg("\nbb_truncate(path=\"%s\", newsize=%lld)\n",
328 path, newsize);
329 bb_fullpath(fpath, path);
330
331 retstat = my_truncate(fpath, newsize);
332 if (retstat < 0)
333 bb_error("bb_truncate truncate");
334
335 return retstat;
336 }
337
338 /** Change the access and/or modification times of a file */
339 /* note −− I’ll want to change this as soon as 2.6 is in debian testing */
340 int bb_utime(const char *path, struct utimbuf *ubuf)
341 {
342 int retstat = 0;
343 char fpath[PATH_MAX];
344
345 log_msg("\nbb_utime(path=\"%s\", ubuf=0x%08x)\n",
346 path, ubuf);
347 bb_fullpath(fpath, path);
348
349 retstat = my_utime(fpath, ubuf);
350 if (retstat < 0)
351 retstat = bb_error("bb_utime utime");
352
353 return retstat;
354 }
355
356 /** File open operation
357 *
358 * No creation, or truncation flags (O_CREAT, O_EXCL, O_TRUNC)
359 * will be passed to open(). Open should check if the operation
360 * is permitted for the given flags. Optionally open may also
361 * return an arbitrary filehandle in the fuse_file_info structure,
362 * which will be passed to all file operations.
363 *
364 * Changed in version 2.2
365 */
366 int bb_open(const char *path, struct fuse_file_info *fi)
367 {
368 int retstat = 0;
369 int fd;
370 char fpath[PATH_MAX];
371
372 log_msg("\nbb_open(path\"%s\", fi=0x%08x)\n",
373 path, fi);
374 bb_fullpath(fpath, path);
375
376 fd = my_open(fpath, fi−>flags);
377 if (fd < 0)
378 retstat = bb_error("bb_open open");
379
380 fi−>fh = fd;
381 log_fi(fi);
382
383 return retstat;
384 }
385
386 /** Read data from an open file
387 *
388 * Read should return exactly the number of bytes requested except
389 * on EOF or error, otherwise the rest of the data will be
390 * substituted with zeroes. An exception to this is when the
391 * ’direct_io’ mount option is specified, in which case the return
392 * value of the read system call will reflect the return value of
393 * this operation.
394 *
395 * Changed in version 2.2
396 */
397 // I don’t fully understand the documentation above −− it doesn’t
398 // match the documentation for the read() system call which says it
399 // can return with anything up to the amount of data requested. nor
400 // with the fusexmp code which returns the amount of data also
401 // returned by read.
402 int bb_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
403 {
404 int retstat = 0;
405
406 log_msg("\nbb_read(path=\"%s\", buf=0x%08x, size=%d, offset=%lld, fi=0x%08x)\n",
407 path, buf, size, offset, fi);
408 // no need to get fpath on this one, since I work from fi−>fh not the path
409 log_fi(fi);
410
411 retstat = my_pread(fi−>fh, buf, size, offset);
412 if (retstat < 0)
413 retstat = bb_error("bb_read read");
414
415 return retstat;
416 }
417
418 /** Write data to an open file
419 *
420 * Write should return exactly the number of bytes requested
421 * except on error. An exception to this is when the ’direct_io’
422 * mount option is specified (see read operation).
423 *
424 * Changed in version 2.2
425 */
426 // As with read(), the documentation above is inconsistent with the
427 // documentation for the write() system call.
428 int bb_write(const char *path, const char *buf, size_t size, off_t offset,
429 struct fuse_file_info *fi)
430 {
431 int retstat = 0;
432
433 log_msg("\nbb_write(path=\"%s\", buf=0x%08x, size=%d, offset=%lld, fi=0x%08x)\n",
434 path, buf, size, offset, fi
435 );
436 // no need to get fpath on this one, since I work from fi−>fh not the path
437 log_fi(fi);
438
439 retstat = my_pwrite(fi−>fh, buf, size, offset);
440 if (retstat < 0)
441 retstat = bb_error("bb_write pwrite");
442
443 return retstat;
444 }
445
446 /** Get file system statistics
447 *
448 * The ’f_frsize’, ’f_favail’, ’f_fsid’ and ’f_flag’ fields are ignored
449 *
450 * Replaced ’struct statfs’ parameter with ’struct statvfs’ in
451 * version 2.5
452 */
453 int bb_statfs(const char *path, struct statvfs *statv)
454 {
455 int retstat = 0;
456 char fpath[PATH_MAX];
457
458 log_msg("\nbb_statfs(path=\"%s\", statv=0x%08x)\n",
459 path, statv);
460 bb_fullpath(fpath, path);
461
462 // get stats for underlying filesystem
463 retstat = my_statvfs(fpath, statv);
464 if (retstat < 0)
465 retstat = bb_error("bb_statfs statvfs");
466
467 log_statvfs(statv);
468
469 return retstat;
470 }
471
472 /** Possibly flush cached data
473 *
474 * BIG NOTE: This is not equivalent to fsync(). It’s not a
475 * request to sync dirty data.
476 *
477 * Flush is called on each close() of a file descriptor. So if a
478 * filesystem wants to return write errors in close() and the file
479 * has cached dirty data, this is a good place to write back data
480 * and return any errors. Since many applications ignore close()
481 * errors this is not always useful.
482 *
483 * NOTE: The flush() method may be called more than once for each
484 * open(). This happens if more than one file descriptor refers
485 * to an opened file due to dup(), dup2() or fork() calls. It is
486 * not possible to determine if a flush is final, so each flush
487 * should be treated equally. Multiple write−flush sequences are
488 * relatively rare, so this shouldn’t be a problem.
489 *
490 * Filesystems shouldn’t assume that flush will always be called
491 * after some writes, or that if will be called at all.
492 *
493 * Changed in version 2.2
494 */
495 int bb_flush(const char *path, struct fuse_file_info *fi)
496 {
497 int retstat = 0;
498
499 log_msg("\nbb_flush(path=\"%s\", fi=0x%08x)\n", path, fi);
500 // no need to get fpath on this one, since I work from fi−>fh not the path
501 log_fi(fi);
502
503 return retstat;
504 }
505
506 /** Release an open file
507 *
508 * Release is called when there are no more references to an open
509 * file: all file descriptors are closed and all memory mappings
510 * are unmapped.
511 *
512 * For every open() call there will be exactly one release() call
513 * with the same flags and file descriptor. It is possible to
514 * have a file opened more than once, in which case only the last
515 * release will mean, that no more reads/writes will happen on the
516 * file. The return value of release is ignored.
517 *
518 * Changed in version 2.2
519 */
520 int bb_release(const char *path, struct fuse_file_info *fi)
521 {
522 int retstat = 0;
523
524 log_msg("\nbb_release(path=\"%s\", fi=0x%08x)\n",
525 path, fi);
526 log_fi(fi);
527
528 // We need to close the file. Had we allocated any resources
529 // (buffers etc) we’d need to free them here as well.
530 retstat = my_close(fi−>fh);
531
532 return retstat;
533 }
534
535 /** Synchronize file contents
536 *
537 * If the datasync parameter is non−zero, then only the user data
538 * should be flushed, not the meta data.
539 *
540 * Changed in version 2.2
541 */
542 int bb_fsync(const char *path, int datasync, struct fuse_file_info *fi)
543 {
544 int retstat = 0;
545
546 log_msg("\nbb_fsync(path=\"%s\", datasync=%d, fi=0x%08x)\n",
547 path, datasync, fi);
548 log_fi(fi);
549
550 // some unix−like systems (notably freebsd) don’t have a datasync call
551 #ifdef HAVE_FDATASYNC
552 if (datasync)
553 retstat = my_fdatasync(fi−>fh);
554 else
555 #endif
556 retstat = my_fsync(fi−>fh);
557
558 if (retstat < 0)
559 bb_error("bb_fsync fsync");
560
561 return retstat;
562 }
563
564 #ifdef HAVE_SYS_XATTR_H
565 /** Set extended attributes */
566 int bb_setxattr(const char *path, const char *name, const char *value, size_t size, int flags)
567 {
568 int retstat = 0;
569 char fpath[PATH_MAX];
570
571 log_msg("\nbb_setxattr(path=\"%s\", name=\"%s\", value=\"%s\", size=%d, flags=0x%08x)\n",
572 path, name, value, size, flags);
573 bb_fullpath(fpath, path);
574
575 retstat = my_lsetxattr(fpath, name, value, size, flags);
576 if (retstat < 0)
577 retstat = bb_error("bb_setxattr lsetxattr");
578
579 return retstat;
580 }
581
582 /** Get extended attributes */
583 int bb_getxattr(const char *path, const char *name, char *value, size_t size)
584 {
585 int retstat = 0;
586 char fpath[PATH_MAX];
587
588 log_msg("\nbb_getxattr(path = \"%s\", name = \"%s\", value = 0x%08x, size = %d)\n",
589 path, name, value, size);
590 bb_fullpath(fpath, path);
591
592 retstat = my_lgetxattr(fpath, name, value, size);
593 if (retstat < 0)
594 retstat = bb_error("bb_getxattr lgetxattr");
595 else
596 log_msg(" value = \"%s\"\n", value);
597
598 return retstat;
599 }
600
601 /** List extended attributes */
602 int bb_listxattr(const char *path, char *list, size_t size)
603 {
604 int retstat = 0;
605 char fpath[PATH_MAX];
606 char *ptr;
607
608 log_msg("bb_listxattr(path=\"%s\", list=0x%08x, size=%d)\n",
609 path, list, size
610 );
611 bb_fullpath(fpath, path);
612
613 retstat = my_llistxattr(fpath, list, size);
614 if (retstat < 0)
615 retstat = bb_error("bb_listxattr llistxattr");
616
617 log_msg(" returned attributes (length %d):\n", retstat);
618 for (ptr = list; ptr < list + retstat; ptr += strlen(ptr)+1)
619 log_msg(" \"%s\"\n", ptr);
620
621 return retstat;
622 }
623
624 /** Remove extended attributes */
625 int bb_removexattr(const char *path, const char *name)
626 {
627 int retstat = 0;
628 char fpath[PATH_MAX];
629
630 log_msg("\nbb_removexattr(path=\"%s\", name=\"%s\")\n",
631 path, name);
632 bb_fullpath(fpath, path);
633
634 retstat = my_lremovexattr(fpath, name);
635 if (retstat < 0)
636 retstat = bb_error("bb_removexattr lrmovexattr");
637
638 return retstat;
639 }
640 #endif
641
642 /** Open directory
643 *
644 * This method should check if the open operation is permitted for
645 * this directory
646 *
647 * Introduced in version 2.3
648 */
649 int bb_opendir(const char *path, struct fuse_file_info *fi)
650 {
651 MY_DIR *dp;
652 int retstat = 0;
653 char fpath[PATH_MAX];
654
655 log_msg("\nbb_opendir(path=\"%s\", fi=0x%08x)\n",
656 path, fi);
657 bb_fullpath(fpath, path);
658
659 dp = my_opendir(fpath);
660 if (dp == NULL)
661 retstat = bb_error("bb_opendir opendir");
662
663 fi−>fh = (intptr_t) dp;
664
665 log_fi(fi);
666
667 return retstat;
668 }
669
670 /** Read directory
671 *
672 * This supersedes the old getdir() interface. New applications
673 * should use this.
674 *
675 * The filesystem may choose between two modes of operation:
676 *
677 * 1) The readdir implementation ignores the offset parameter, and
678 * passes zero to the filler function’s offset. The filler
679 * function will not return ’1’ (unless an error happens), so the
680 * whole directory is read in a single readdir operation. This
681 * works just like the old getdir() method.
682 *
683 * 2) The readdir implementation keeps track of the offsets of the
684 * directory entries. It uses the offset parameter and always
685 * passes non−zero offset to the filler function. When the buffer
686 * is full (or an error happens) the filler function will return
687 * ’1’.
688 *
689 * Introduced in version 2.3
690 */
691 int bb_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset,
692 struct fuse_file_info *fi)
693 {
694 int retstat = 0;
695 MY_DIR *dp;
696 struct dirent *de;
697
698 log_msg("\nbb_readdir(path=\"%s\", buf=0x%08x, filler=0x%08x, offset=%lld, fi=0x%08x)\n",
699 path, buf, filler, offset, fi);
700 // once again, no need for fullpath −− but note that I need to cast fi−>fh
701 dp = (MY_DIR *) (uintptr_t) fi−>fh;
702
703 // Every directory contains at least two entries: . and .. If my
704 // first call to the system readdir() returns NULL I’ve got an
705 // error; near as I can tell, that’s the only condition under
706 // which I can get an error from readdir()
707 de = my_readdir(dp);
708 if (de == 0) {
709 retstat = bb_error("bb_readdir readdir");
710 return retstat;
711 }
712
713 // This will copy the entire directory into the buffer. The loop exits
714 // when either the system readdir() returns NULL, or filler()
715 // returns something non−zero. The first case just means I’ve
716 // read the whole directory; the second means the buffer is full.
717 do {
718 log_msg("calling filler with name %s\n", de−>d_name);
719 if (filler(buf, de−>d_name, NULL, 0) != 0) {
720 log_msg(" ERROR bb_readdir filler: buffer full");
721 return −ENOMEM;
722 }
723 } while ((de = my_readdir(dp)) != NULL);
724
725 log_fi(fi);
726
727 return retstat;
728 }
729
730 /** Release directory
731 *
732 * Introduced in version 2.3
733 */
734 int bb_releasedir(const char *path, struct fuse_file_info *fi)
735 {
736 int retstat = 0;
737
738 log_msg("\nbb_releasedir(path=\"%s\", fi=0x%08x)\n",
739 path, fi);
740 log_fi(fi);
741
742 my_closedir((MY_DIR *) (uintptr_t) fi−>fh);
743
744 return retstat;
745 }
746
747 /** Synchronize directory contents
748 *
749 * If the datasync parameter is non−zero, then only the user data
750 * should be flushed, not the meta data
751 *
752 * Introduced in version 2.3
753 */
754 // when exactly is this called? when a user calls fsync and it
755 // happens to be a directory? ???
756 int bb_fsyncdir(const char *path, int datasync, struct fuse_file_info *fi)
757 {
758 int retstat = 0;
759
760 log_msg("\nbb_fsyncdir(path=\"%s\", datasync=%d, fi=0x%08x)\n",
761 path, datasync, fi);
762 log_fi(fi);
763
764 return retstat;
765 }
766
767 /**
768 * Initialize filesystem
769 *
770 * The return value will passed in the private_data field of
771 * fuse_context to all file operations and as a parameter to the
772 * destroy() method.
773 *
774 * Introduced in version 2.3
775 * Changed in version 2.6
776 */
777 // Undocumented but extraordinarily useful fact: the fuse_context is
778 // set up before this function is called, and
779 // fuse_get_context()−>private_data returns the user_data passed to
780 // fuse_main(). Really seems like either it should be a third
781 // parameter coming in here, or else the fact should be documented
782 // (and this might as well return void, as it did in older versions of
783 // FUSE).
784 void *bb_init(struct fuse_conn_info *conn)
785 {
786 log_msg("\nbb_init()\n");
787
788 log_conn(conn);
789 log_fuse_context(fuse_get_context());
790
791 return BB_DATA;
792 }
793
794 /**
795 * Clean up filesystem
796 *
797 * Called on filesystem exit.
798 *
799 * Introduced in version 2.3
800 */
801 void bb_destroy(void *userdata)
802 {
803 log_msg("\nbb_destroy(userdata=0x%08x)\n", userdata);
804 }
805
806 /**
807 * Check file access permissions
808 *
809 * This will be called for the access() system call. If the
810 * ’default_permissions’ mount option is given, this method is not
811 * called.
812 *
813 * This method is not called under Linux kernel versions 2.4.x
814 *
815 * Introduced in version 2.5
816 */
817 int bb_access(const char *path, int mask)
818 {
819 int retstat = 0;
820 char fpath[PATH_MAX];
821
822 log_msg("\nbb_access(path=\"%s\", mask=0%o)\n",
823 path, mask);
824 bb_fullpath(fpath, path);
825
826 retstat = my_access(fpath, mask);
827
828 if (retstat < 0)
829 retstat = bb_error("bb_access access");
830
831 return retstat;
832 }
833
834 /**
835 * Create and open a file
836 *
837 * If the file does not exist, first create it with the specified
838 * mode, and then open it.
839 *
840 * If this method is not implemented or under Linux kernel
841 * versions earlier than 2.6.15, the mknod() and open() methods
842 * will be called instead.
843 *
844 * Introduced in version 2.5
845 */
846 int bb_create(const char *path, mode_t mode, struct fuse_file_info *fi)
847 {
848 int retstat = 0;
849 char fpath[PATH_MAX];
850 int fd;
851
852 log_msg("\nbb_create(path=\"%s\", mode=0%03o, fi=0x%08x)\n",
853 path, mode, fi);
854 bb_fullpath(fpath, path);
855
856 fd = my_creat(fpath, mode);
857 if (fd < 0)
858 retstat = bb_error("bb_create creat");
859
860 fi−>fh = fd;
861
862 log_fi(fi);
863
864 return retstat;
865 }
866
867 /**
868 * Change the size of an open file
869 *
870 * This method is called instead of the truncate() method if the
871 * truncation was invoked from an ftruncate() system call.
872 *
873 * If this method is not implemented or under Linux kernel
874 * versions earlier than 2.6.15, the truncate() method will be
875 * called instead.
876 *
877 * Introduced in version 2.5
878 */
879 int bb_ftruncate(const char *path, off_t offset, struct fuse_file_info *fi)
880 {
881 int retstat = 0;
882
883 log_msg("\nbb_ftruncate(path=\"%s\", offset=%lld, fi=0x%08x)\n",
884 path, offset, fi);
885 log_fi(fi);
886
887 retstat = my_ftruncate(fi−>fh, offset);
888 if (retstat < 0)
889 retstat = bb_error("bb_ftruncate ftruncate");
890
891 return retstat;
892 }
893
894 /**
895 * Get attributes from an open file
896 *
897 * This method is called instead of the getattr() method if the
898 * file information is available.
899 *
900 * Currently this is only called after the create() method if that
901 * is implemented (see above). Later it may be called for
902 * invocations of fstat() too.
903 *
904 * Introduced in version 2.5
905 */
906 int bb_fgetattr(const char *path, struct stat *statbuf, struct fuse_file_info *fi)
907 {
908 int retstat = 0;
909
910 log_msg("\nbb_fgetattr(path=\"%s\", statbuf=0x%08x, fi=0x%08x)\n",
911 path, statbuf, fi);
912 log_fi(fi);
913
914 // On FreeBSD, trying to do anything with the mountpoint ends up
915 // opening it, and then using the FD for an fgetattr. So in the
916 // special case of a path of "/", I need to do a getattr on the
917 // underlying root directory instead of doing the fgetattr().
918 if (!strcmp(path, "/"))
919 return bb_getattr(path, statbuf);
920
921 retstat = my_fstat(fi−>fh, statbuf);
922 if (retstat < 0)
923 retstat = bb_error("bb_fgetattr fstat");
924
925 log_stat(statbuf);
926
927 return retstat;
928 }
929
930 struct fuse_operations bb_oper = {
931 .getattr = bb_getattr,
932 .readlink = bb_readlink,
933 // no .getdir −− that’s deprecated
934 .getdir = NULL,
935 .mknod = bb_mknod,
936 .mkdir = bb_mkdir,
937 .unlink = bb_unlink,
938 .rmdir = bb_rmdir,
939 .symlink = bb_symlink,
940 .rename = bb_rename,
941 .link = bb_link,
942 .chmod = bb_chmod,
943 .chown = bb_chown,
944 .truncate = bb_truncate,
945 .utime = bb_utime,
946 .open = bb_open,
947 .read = bb_read,
948 .write = bb_write,
949 /** Just a placeholder, don’t set */ // huh???
950 .statfs = bb_statfs,
951 .flush = bb_flush,
952 .release = bb_release,
953 .fsync = bb_fsync,
954
955 #ifdef HAVE_SYS_XATTR_H
956 .setxattr = bb_setxattr,
957 .getxattr = bb_getxattr,
958 .listxattr = bb_listxattr,
959 .removexattr = bb_removexattr,
960 #endif
961
962 .opendir = bb_opendir,
963 .readdir = bb_readdir,
964 .releasedir = bb_releasedir,
965 .fsyncdir = bb_fsyncdir,
966 .init = bb_init,
967 .destroy = bb_destroy,
968 .access = bb_access,
969 .create = bb_create,
970 .ftruncate = bb_ftruncate,
971 .fgetattr = bb_fgetattr
972 };
973
974 void bb_usage()
975 {
976 fprintf(stderr, "usage: bbfs [FUSE and mount options] rootDir mountPoint\n");
977 abort();
978 }
979
980 int main(int argc, char *argv[])
981 {
982 int fuse_stat;
983 struct bb_state *bb_data;
984
985 // bbfs doesn’t do any access checking on its own (the comment
986 // blocks in fuse.h mention some of the functions that need
987 // accesses checked −− but note there are other functions, like
988 // chown(), that also need checking!). Since running bbfs as root
989 // will therefore open Metrodome−sized holes in the system
990 // security, we’ll check if root is trying to mount the filesystem
991 // and refuse if it is. The somewhat smaller hole of an ordinary
992 // user doing it with the allow_other flag is still there because
993 // I don’t want to parse the options string.
994 if ((getuid() == 0) || (geteuid() == 0)) {
995 fprintf(stderr, "Running BBFS as root opens unnacceptable security holes\n");
996 return 1;
997 }
998
999 // Perform some sanity checking on the command line: make sure
1000 // there are enough arguments, and that neither of the last two
1001 // start with a hyphen (this will break if you actually have a
1002 // rootpoint or mountpoint whose name starts with a hyphen, but so
1003 // will a zillion other programs)
1004 if ((argc < 3) || (argv[argc−2][0] == ’−’) || (argv[argc−1][0] == ’−’))
1005 bb_usage();
1006
1007 bb_data = malloc(sizeof(struct bb_state));
1008 if (bb_data == NULL) {
1009 perror("main calloc");
1010 abort();
1011 }
1012
1013 // Pull the rootdir out of the argument list and save it in my
1014 // internal data
1015 bb_data−>rootdir = realpath(argv[argc−2], NULL);
1016 argv[argc−2] = argv[argc−1];
1017 argv[argc−1] = NULL;
1018 argc−−;
1019
1020 bb_data−>logfile = log_open();
1021
1022 // turn over control to fuse
1023 fprintf(stderr, "about to call fuse_main\n");
1024 fuse_stat = fuse_main(argc, argv, &bb_oper, bb_data);
1025 fprintf(stderr, "fuse_main returned %d\n", fuse_stat);
1026
1027 return fuse_stat;
1028 }
1029
1030 /* Thomas Payne 10/24/2015 This a slightly modified version of version
1031 of Joseph Pfeiffer’s bbfs.c. I’ve placed the prefix "my_" onto all
1032 calls to the underlying Unix library, which I presume is glibc and
1033 "MY_" onto all occurences of the macro "DIR". Also, I’ve added the
1034 line "#include my_stubs.H".
1035
1036 I intend to substitute my own definitions of those functions and
1037 DIR, which I will place in the C++ files my_stubs.cc and my_stubs.H.
1038
1039 This code makes one semantic shift: open(), close(), pread() and
1040 pwrite() work in terms of file handles, which are id numbers for
1041 inodes (a.k.a. "file handles"), rather than file descriptors, which
1042 are id numbers for entries in per−process open−file tables. Note
1043 that Pfeiffer’s code stores the value returned by open into the
1044 fh (file handle) field of a fuse_file_info struct.
1045 */