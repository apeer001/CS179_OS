2 // Note: this code is a work−in−progress. This version: 3:15PM PDT
3 // 10/26/15.
4
5 // The difference between the functions in my_stubs.cc and those of
6 // the Unix system (say those in glibc) is that those in glibc have
7 // code in them and mine don’t. I intend that their prototypes and
8 // behavior be identical to those of their glibc counterparts, with
9 // three minor exceptions:
10
11 // * open(...) returns the handle of a file rather than the
12 // descriptor of an open file.
13
14 // * pread(...) and pwrite(...) take
15 // the handle of a file rather than the descriptor of an open file
16 // as their respective first arguments.
17
18 // Recall that openfiles are to files what stringstreams are to strings.
19
20 // In our case, we have no need to create an open file from the file
21 // in question. Pfeiffer’s bbfs.c immediately stores the integer
22 // returned its call to open(...) into the file−handle field of a
23 // fuse_file_info struct and later uses it in calls to pread(...) and
24 // pwrite(...). So far as I can tell, this modification simply avoids
25 // some otherwise wasted code and effort.
26
27
28 // Josef Pfeiffer’s bbfs.c includes
29 #include <ctype.h>
30 #include <dirent.h>
31 #include <errno.h>
32 #include <fcntl.h>
33 // #include <fuse.h> I don’t currently have this file and don’t need it.
34 #include <libgen.h>
35 #include <limits.h>
36 #include <stdlib.h>
37 #include <stdio.h>
38 #include <string.h>
39 #include <unistd.h>
40 #include <sys/types.h>
41
42 // Here we include
43 #include </usr/include/linux/fs.h>
44 #include <sys/stat.h> // this has our official definition of stat
45 #include <dirent.h> // this has our official definition of dirent
46 #include <errno.h>
47 #include <vector>
48 #include <string>
49 #include <cassert>
50 #include <sstream>
51 #include <map>
52 #include "my_stubs.H"
53
54 using namespace std;
55
56 // This definition of inode is from http://www.makelinux.net/books/lkd2/ch12lev1
sec6.
57 // I’ve commented out all fields whose types are not yet defined.
58 struct inode {
59 // struct hlist_node i_hash; /* hash list */
60 // struct list_head i_list; /* list of inodes */
61 // struct list_head i_dentry; /* list of dentries */
62 unsigned long i_ino; /* inode number */
63 //* atomic_t i_count; /* reference counter */
64 //* umode_t i_mode; /* access permissions */
65 unsigned int i_nlink; /* number of hard links */
66 uid_t i_uid; /* user id of owner */
67 gid_t i_gid; /* group id of owner */
68 // kdev_t i_rdev; /* real device node */
69 //* loff_t i_size; /* file size in bytes */
70 struct timespec i_atime; /* last access time */
71 struct timespec i_mtime; /* last modify time */
72 struct timespec i_ctime; /* last change time */

73 unsigned int i_blkbits; /* block size in bits */
74 unsigned long i_blksize; /* block size in bytes */
75 unsigned long i_version; /* version number */
76 unsigned long i_blocks; /* file size in blocks */
77 unsigned short i_bytes; /* bytes consumed */
78 // spinlock_t i_lock; /* spinlock */
79 // struct rw_semaphore i_alloc_sem; /* nests inside of i_sem */
80 // struct semaphore i_sem; /* inode semaphore */
81 struct inode_operations *i_op; /* inode ops table */
82 struct file_operations *i_fop; /* default inode ops */
83 struct super_block *i_sb; /* associated superblock */
84 struct file_lock *i_flock; /* file lock list */
85 struct address_space *i_mapping; /* associated mapping */
86 // struct address_space i_data; /* mapping for device */
87 // struct dquot *i_dquot[MAXQUOTAS]; /* disk quotas for inode */
88 // struct list_head i_devices; /* list of block devices */
89 struct pipe_inode_info *i_pipe; /* pipe information */
90 struct block_device *i_bdev; /* block device driver */
91 unsigned long i_dnotify_mask; /* directory notify mask */
92 struct dnotify_struct *i_dnotify; /* dnotify */
93 unsigned long i_state; /* state flags */
94 unsigned long dirtied_when; /* first dirtying time */
95 unsigned int i_flags; /* filesystem flags */
96 unsigned char i_sock; /* is this a socket? */
97 // atomic_t i_writecount; /* count of writers */
98 void *i_security; /* security module */
99 // __u32 i_generation; /* inode version number */
100 union {
101 void *generic_ip; /* filesystem−specific info */
102 } u;
103 };
104
105
106 map< ino_t, inode > ilist;
107
108
109 inline // a simple utility for splitting strings at a find−pattern.
110 vector<string>
111 split(const string s, const string pat ) {
112 string c;
113 vector<string> v;
114 int i = 0;
115 for (;;) {
116 int t = s.find(pat,i);
117 int j = ( t == string::npos ) ? s.size() : t;
118 string c = s.substr(i,j−i);
119 v.push_back(c);
120 i = j+pat.size();
121 if ( t == string::npos ) return v;
122 }
123 }
124
125
126 // class my_DIR { // This is my version of DIR
127 // public:
128 // dirent* base; // not a pointer, rather an off_t
129 // dirent* offset;
130 // ino_t fh;
131 // dirent* max_offset; // we need to initialize this.
132 // my_DIR( ino_t x )
133 // : fh(x), max_offset((dirent*)ilist[fh].i_size)
134 // {}
135 // };
136
137
138 int ok = 0;
139 int err = −1;
140
141 // called at line #95 of bbfs.c
142 // for details see: http://manpages.ubuntu.com/manpages/hardy/man2/stat.2.html
143 int my_lstat( const char* path, struct stat *statbuf ) {
144 // ino_t fh = find_inode(path);
145 // collect attributes from ilist[fh] and load them into struct stat.

6 return err;
147 }
148
149 // called at line #125 of bbfs.c
150 int my_readlink( const char *path, char *link, size_t size ) {
151 return err;
152 }
153
154 // called at line #168 of bbfs.c. See line #151.
155 int my_mknod( const char *path, mode_t mode, dev_t dev ) {
156 // returns error status only. To get the handle for the inode
157 // simply search to the dirent via path.
158 return err;
159 }
160
161 // called at line #186 of bbfs.c
162 int my_mkdir( const char *path, mode_t mode ) {
163 return err;
164 }
165
166 // called at line #203 of bbfs.c
167 int my_unlink( const char *path ) {
168 return err;
169 }
170
171 // called at line #220 of bbfs.c
172 int my_rmdir( const char *path ) {
173 return err;
174 }
175
176 // called at line #241 of bbfs.c
177 int my_symlink(const char *path, const char *link) {
178 return err;
179 }
180
181 // called at line #261 of bbfs.c
182 int my_rename( const char *path, const char *newpath ) {
183 return err;
184 }
185
186 // called at line #279 of bbfs.c
187 int my_link(const char *path, const char *newpath) {
188 return err;
189 }
190
191 // called at line #296 of bbfs.c
192 int my_chmod(const char *path, mode_t mode) {
193 return err;
194 }
195
196 // called at line #314 of bbfs.c
197 int my_chown(const char *path, uid_t uid, gid_t gid) {
198 return err;
199 }
200
201 // called at line #331 of bbfs.c
202 int my_truncate(const char *path, off_t newsize) {
203 return err;
204 }
205
206 // called at line #349 of bbfs.c
207 int my_utime(const char *path, struct utimbuf *ubuf) {
208 return err;
209 }
210
211 // called at line #376 of bbfs.c. Returns file handle not a file descriptor
212 int my_open( const char *path, int flags ) {
213 // Write a function called "lookup" that takes a full path and the
214 // handle of a directory and returns the handle of the inode having
215 // that name. To do so, split full path. Then starting with the
216 // handle of the root directory, look up each segment in the
217 // directory whose handle you currently have. Use a linear search
218 // that consists of successive calls to readdir() to find the

9 // corresponding directory entry, which has type struct dirent.
220 // Return its d_fileno, unless there’s an error and then return −1.
221 return err;
222 }
223
224 // called at line #411 of bbfs.c Note that our firt arg is an fh not an fd
225 int my_pread( int fh, char *buf, size_t size, off_t offset ) {
226 return err;
227 }
228
229 // called at line #439 of bbfs.c Note that our firt arg is an fh not an fd
230 int my_pwrite( int fh, const char *buf, size_t size, off_t offset ) {
231 return err;
232 }
233
234 // called at line #463 of bbfs.c
235 int my_statvfs(const char *fpath, struct statvfs *statv) {
236 return err;
237 }
238
239 // called at line #530 of bbfs.c
240 int my_close( ino_t fh ) {
241 return err;
242 }
243
244 // called at line #553 of bbfs.c
245 int my_fdatasync( ino_t fh ) {
246 return err;
247 }
248
249 // called at line #556 of bbfs.c
250 int my_fsync( ino_t fh ) {
251 return err;
252 }
253
254 // called at line #575 of bbfs.c
255 int my_lsetxattr( const char *fpath, const char *name, const char *value, size_t
 size, int flags ) {
256 return err;
257 }
258
259 // called at line #592 of bbfs.c
260 int my_lgetxattr( const char *fpath, const char *name, char *value, size_t size,
int flags ) {
261 return err;
262 }
263
264 // called at line #613 of bbfs.c
265 int my_llistxattr( const char *path, char *list, size_t size ) {
266 return err;
267 }
268
269 // called at line #634 of bbfs.c
270 int my_lremovexattr( const char *path, const char *name ) {
271 return err;
272 }
273
274
275 // In the next three functions, we need to create a my_DIR* from a file
276 // handle, i.e., an (ino_t) fh. So, how to do that?
277
278 // It doesn’t say this anywhere that I can find, but a my_DIR has to be
279 // similar to an open file. However, rather than creating a stream of
280 // bytes (chars) from the file, it creates a stream of directory
281 // entries. The key difference is how much you increment the offset
282 // counter each time you move to the next item.
283
284 // Note that at line #742 of bbfs.c, Professor Pfeiffer converts a
285 // file handle into a my_DIR via "(uintptr_t) fh." But, his file handles
286 // are indices of byte streams, while our are the addresses of inodes.
287
288 // I recommend that we simply maintain the counter as an
289 // directory−entry index and increment it by one each time. Then

0 // multiply it by the size of a directory entry to get the offset of
291 // the directory entry that it refers to within the directory. To get
292 // the address of that directory entry, we simply add the address of
293 // the corresponding block of the directory.
294
295 // // called at line #659 of bbfs.c
296 // my_DIR* my_opendir( const char* fpath ) {
297 // return err;
298 // }
299
300 // called at lines #707 and #726 of bbfs.c
301 // int my_readdir( my_DIR* dp ) {
302 // return err;
303 // }
304
305 // // called at line #742 of bbfs.c
306 // int my_closedir( my_DIR* dp ) {
307 // return err;
308 // }
309
310 // called at line #826 of bbfs.c
311 int my_access( const char *fpath, int mask ) {
312 return err;
313 }
314
315 // called at line #856 of bbfs.c
316 int my_creat( const char *fpath, mode_t mode ) {
317 return err;
318 }
319
320 // called at line #887 of bbfs.c
321 int my_ftruncate( ino_t fh, off_t offset ) {
322 return err;
323 }
324
325 // called at line #921 of bbfs.c
326 // for details see: http://manpages.ubuntu.com/manpages/hardy/man2/stat.2.html
327 int my_fstat( ino_t fh, struct stat* statbuf ) {
328 return err;
329 }
330
331 // called at line #1015 of bbfs.c
332 char* my_realpath( const char* path, char* resolved_path ) {
333 assert( resolved_path != NULL );
334 return 0;
335 }
336
337
338 // Here are my helper functions ==================================
339
340 // In the code below opendir is defined in terms of find_inode,
341 // which is defined in terms of lookup,
342 // which is defined in terms of opendir and readdir.
343 // The header file takes care of that circularity.
344
345 typedef char block[4096];
346
347 block* blocks[15];
348
349 MY_DIR* fhopendir( ino_t fh ) {
350 // if ( fh is not the handle of a directory ) return not−a−directory error;
351 MY_DIR * tmp = new MY_DIR;
352 tmp−>fh = fh;
353 }
354
355
356 // called at lines #707 and #726 of bbfs.c
357 dirent* my_readdir( MY_DIR* dirp ) {
358 off_t tmp = (dirp−>offset)−>d_reclen;
359 dirp−>offset += tmp;
360 return ( dirp−>offset < dirp−>max_offset) ? dirp−>offset : 0 ;
361 }

3
364 // called at line #742 of bbfs.c
365 int my_closedir( MY_DIR* dirp ) {
366 delete dirp;
367 }
368
369
370 ino_t lookup( string name, ino_t fh ) {
371 // finds and returns ino_t of file whose name is name in directory fh.
372 // This function will be used by open() and opendir().
373 MY_DIR* dirp = fhopendir( fh ); // fhopendir checks if fh is handle of a dir.
374 if ( ! dirp ) return err; // cannot_open_error
375 while ( dirent* dp = my_readdir(dirp) ) {
376 string s = dp−>d_name; // converts C string to C++ string
377 if ( s == name ) {
378 my_closedir(dirp);
379 if ( dp−>d_type != DT_REG && dp−>d_type != DT_DIR ) {
380 return err; // wrong−file−type error
381 // later we may add more types
382 } else {
383 return dp−>d_fileno;
384 }
385 }
386 }
387 my_closedir(dirp); // close MY_DIR asap, to reset internal data
388 return err; // name−not−found
389 }
390
391
392 ino_t find_inode( string fullpath ) {
393 // fullpath must starts at the root. // See how Pfeiffer does it.
394 ino_t fh = 2; // 2 is the i_no of the root directory.
395 vector<string> v = split( fullpath, "/" );
396 for ( int i = 1; i != v.size(); ++ i ) { // omit the initial null segment
397 fh = lookup( v[i], fh );
398 }
399 }
400
401 struct inode * find_inode( ino_t ) {
402 // the actual code for this will depend on how you implement this map
403 return 0;
404 }
405
406 // called at line #659 of bbfs.c
407 MY_DIR * my_opendir( char fullpath[PATH_MAX] ) {
408 return fhopendir( find_inode( fullpath ) );
409 }
410
411 // ===================== a crude c++ approach ======================
412
413 class file {
414 public:
415 inode ino;
416 };
417
418 class regfile : file {
419 string content;
420 };
421
422 class openfile : regfile {
423 stringstream(content);
424 };
425
426 class directory : file {
427 public:
428 map<string,file> themap; // a balanced binary tree.
429 };
430
431
432 template< typename T1, typename T2>
433 string pickle(map<T1,T2> m) {
434 typename map<T1,T2>::iterator it;
435 string s;

 stringstream ss(s);
437 for ( it = m.begin(); it != m.end(); ++it ) {
438 // This requires some kind of separation/terminaton symbol at the end of eac
h.
439 ss << it.first;
440 ss << it.second;
441 }
442 } 
