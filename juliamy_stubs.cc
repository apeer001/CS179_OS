// Note: this code is a work−in−progress. This version: 3:15PM PDT
 // 10/26/15.

 // The difference between the functions in my_stubs.cc and those of
 // the Unix system (say those in glibc) is that those in glibc have
 // code in them and mine don’t. I intend that their prototypes and
 // behavior be identical to those of their glibc counterparts, with
 // three minor exceptions:

 // * open(...) returns the handle of a file rather than the
 // descriptor of an open file.

 // * pread(...) and pwrite(...) take
 // the handle of a file rather than the descriptor of an open file
 // as their respective first arguments.

 // Recall that openfiles are to files what stringstreams are to strings.

 // In our case, we have no need to create an open file from the file
 // in question. Pfeiffer’s bbfs.c immediately stores the integer
 // returned its call to open(...) into the file−handle field of a
 // fuse_file_info struct and later uses it in calls to pread(...) and
 // pwrite(...). So far as I can tell, this modification simply avoids
 // some otherwise wasted code and effort.


 // Josef Pfeiffer’s bbfs.c includes
 #include <ctype.h>
 #include <dirent.h>
 #include <errno.h>
 #include <fcntl.h>
 // #include <fuse.h> I don’t currently have this file and don’t need it.
 #include <libgen.h>
 #include <limits.h>
 #include <stdlib.h>
 #include <stdio.h>
 #include <string.h>
 #include <unistd.h>
 #include <sys/types.h>

 // Here we include
 #include </usr/include/linux/fs.h>
 #include <sys/stat.h> // this has our official definition of stat
 #include <dirent.h> // this has our official definition of dirent
 #include <errno.h>
 #include <vector>
 #include <string>
 #include <cassert>
 #include <sstream>
 #include <map>
 #include "my_stubs.H"

 using namespace std;

 // This definition of inode is from http://www.makelinux.net/books/lkd2/ch12lev1sec6.
 // I’ve commented out all fields whose types are not yet defined.
 struct inode {
 // struct hlist_node i_hash; /* hash list */
 // struct list_head i_list; /* list of inodes */
 // struct list_head i_dentry; /* list of dentries */
 unsigned long i_ino; /* inode number */
 //* atomic_t i_count; /* reference counter */
 //* umode_t i_mode; /* access permissions */
 unsigned int i_nlink; /* number of hard links */
 uid_t i_uid; /* user id of owner */
 gid_t i_gid; /* group id of owner */
 // kdev_t i_rdev; /* real device node */
 //* loff_t i_size; /* file size in bytes */
 struct timespec i_atime; /* last access time */
 struct timespec i_mtime; /* last modify time */
 struct timespec i_ctime; /* last change time */
 unsigned int i_blkbits; /* block size in bits */
 unsigned long i_blksize; /* block size in bytes */
 unsigned long i_version; /* version number */
 unsigned long i_blocks; /* file size in blocks */
 unsigned short i_bytes; /* bytes consumed */
 // spinlock_t i_lock; /* spinlock */
 // struct rw_semaphore i_alloc_sem; /* nests inside of i_sem */
 // struct semaphore i_sem; /* inode semaphore */
 struct inode_operations *i_op; /* inode ops table */
 struct file_operations *i_fop; /* default inode ops */
 struct super_block *i_sb; /* associated superblock */
 struct file_lock *i_flock; /* file lock list */
 struct address_space *i_mapping; /* associated mapping */
 // struct address_space i_data; /* mapping for device */
 // struct dquot *i_dquot[MAXQUOTAS]; /* disk quotas for inode */
 // struct list_head i_devices; /* list of block devices */
 struct pipe_inode_info *i_pipe; /* pipe information */
 struct block_device *i_bdev; /* block device driver */
 unsigned long i_dnotify_mask; /* directory notify mask */
 struct dnotify_struct *i_dnotify; /* dnotify */
 unsigned long i_state; /* state flags */
 unsigned long dirtied_when; /* first dirtying time */
 unsigned int i_flags; /* filesystem flags */
 unsigned char i_sock; /* is this a socket? */
 // atomic_t i_writecount; /* count of writers */
 void *i_security; /* security module */
 // __u32 i_generation; /* inode version number */
 union {
 void *generic_ip; /* filesystem−specific info */
 } u;
 };


 map< ino_t, inode > ilist;


 inline // a simple utility for splitting strings at a find−pattern.
 vector<string>
 split(const string s, const string pat ) {
 string c;
 vector<string> v;
 int i = 0;
 for (;;) {
 int t = s.find(pat,i);
 int j = ( t == string::npos ) ? s.size() : t;
 string c = s.substr(i,j−i);
 v.push_back(c);
 i = j+pat.size();
 if ( t == string::npos ) return v;
 }
 }


 // class my_DIR { // This is my version of DIR
 // public:
 // dirent* base; // not a pointer, rather an off_t
 // dirent* offset;
 // ino_t fh;
 // dirent* max_offset; // we need to initialize this.
 // my_DIR( ino_t x )
 // : fh(x), max_offset((dirent*)ilist[fh].i_size)
 // {}
 // };


 int ok = 0;
 int err = −1;

 // called at line #95 of bbfs.c
 // for details see: http://manpages.ubuntu.com/manpages/hardy/man2/stat.2.html
 int my_lstat( const char* path, struct stat *statbuf ) {
 // ino_t fh = find_inode(path);
 // collect attributes from ilist[fh] and load them into struct stat.
 return err;
 }

 // called at line #125 of bbfs.c
 int my_readlink( const char *path, char *link, size_t size ) {
 return err;
 }

 // called at line #168 of bbfs.c. See line #151.
 int my_mknod( const char *path, mode_t mode, dev_t dev ) {
 // returns error status only. To get the handle for the inode
 // simply search to the dirent via path.
 return err;
 }

 // called at line #186 of bbfs.c
 int my_mkdir( const char *path, mode_t mode ) {
 return err;
 }

 // called at line #203 of bbfs.c
 int my_unlink( const char *path ) {
 return err;
 }

 // called at line #220 of bbfs.c
 int my_rmdir( const char *path ) {

 }

 // called at line #241 of bbfs.c
 int my_symlink(const char *path, const char *link) {
 return err;
 }

 // called at line #261 of bbfs.c
 int my_rename( const char *path, const char *newpath ) {
 return err;
 }

 // called at line #279 of bbfs.c
 int my_link(const char *path, const char *newpath) {
 return err;
 }

 // called at line #296 of bbfs.c
 int my_chmod(const char *path, mode_t mode) {
 return err;
 }

 // called at line #314 of bbfs.c
 int my_chown(const char *path, uid_t uid, gid_t gid) {
 return err;
 }

 // called at line #331 of bbfs.c
 int my_truncate(const char *path, off_t newsize) {
 return err;
 }

 // called at line #349 of bbfs.c
 int my_utime(const char *path, struct utimbuf *ubuf) {
 return err;
 }

 // called at line #376 of bbfs.c. Returns file handle not a file descriptor
 int my_open( const char *path, int flags ) {
 // Write a function called "lookup" that takes a full path and the
 // handle of a directory and returns the handle of the inode having
 // that name. To do so, split full path. Then starting with the
 // handle of the root directory, look up each segment in the
 // directory whose handle you currently have. Use a linear search
 // that consists of successive calls to readdir() to find the
 // corresponding directory entry, which has type struct dirent.
 // Return its d_fileno, unless there’s an error and then return −1.
 return err;
 }

 // called at line #411 of bbfs.c Note that our firt arg is an fh not an fd
 int my_pread( int fh, char *buf, size_t size, off_t offset ) {
 return err;
 }

 // called at line #439 of bbfs.c Note that our firt arg is an fh not an fd
 int my_pwrite( int fh, const char *buf, size_t size, off_t offset ) {
 return err;
 }

 // called at line #463 of bbfs.c
 int my_statvfs(const char *fpath, struct statvfs *statv) {
 return err;
 }

 // called at line #530 of bbfs.c
 int my_close( ino_t fh ) {
 return err;
 }

 // called at line #553 of bbfs.c
 int my_fdatasync( ino_t fh ) {
 return err;
 }

 // called at line #556 of bbfs.c
 int my_fsync( ino_t fh ) {
 return err;
 }

 // called at line #575 of bbfs.c
 int my_lsetxattr( const char *fpath, const char *name, const char *value, size_t size, int flags ) {
 return err;
 }

 // called at line #592 of bbfs.c
 int my_lgetxattr( const char *fpath, const char *name, char *value, size_t size, int flags ) {
 return err;
 }

 // called at line #613 of bbfs.c
 int my_llistxattr( const char *path, char *list, size_t size ) {
 return err;
 }

 // called at line #634 of bbfs.c
 int my_lremovexattr( const char *path, const char *name ) {
 return err;
 }


 // In the next three functions, we need to create a my_DIR* from a file
 // handle, i.e., an (ino_t) fh. So, how to do that?

 // It doesn’t say this anywhere that I can find, but a my_DIR has to be
 // similar to an open file. However, rather than creating a stream of
 // bytes (chars) from the file, it creates a stream of directory
 // entries. The key difference is how much you increment the offset
 // counter each time you move to the next item.

 // Note that at line #742 of bbfs.c, Professor Pfeiffer converts a
 // file handle into a my_DIR via "(uintptr_t) fh." But, his file handles
 // are indices of byte streams, while our are the addresses of inodes.

 // I recommend that we simply maintain the counter as an
 // directory−entry index and increment it by one each time. Then
 // multiply it by the size of a directory entry to get the offset of
 // the directory entry that it refers to within the directory. To get
 // the address of that directory entry, we simply add the address of
 // the corresponding block of the directory.

 // // called at line #659 of bbfs.c
 // my_DIR* my_opendir( const char* fpath ) {
 // return err;
 // }

 // called at lines #707 and #726 of bbfs.c
 // int my_readdir( my_DIR* dp ) {
 // return err;
 // }

 // // called at line #742 of bbfs.c
 // int my_closedir( my_DIR* dp ) {
 // return err;
 // }

 // called at line #826 of bbfs.c
 int my_access( const char *fpath, int mask ) {
 return err;
 }

 // called at line #856 of bbfs.c
 int my_creat( const char *fpath, mode_t mode ) {
 return err;
 }

 // called at line #887 of bbfs.c
 int my_ftruncate( ino_t fh, off_t offset ) {
 return err;
 }

 // called at line #921 of bbfs.c
 // for details see: http://manpages.ubuntu.com/manpages/hardy/man2/stat.2.html
 int my_fstat( ino_t fh, struct stat* statbuf ) {
 return err;
 }

 // called at line #1015 of bbfs.c
 char* my_realpath( const char* path, char* resolved_path ) {
 assert( resolved_path != NULL );
 return 0;
 }


 // Here are my helper functions ==================================

 // In the code below opendir is defined in terms of find_inode,
 // which is defined in terms of lookup,
 // which is defined in terms of opendir and readdir.
 // The header file takes care of that circularity.

 typedef char block[4096];

 block* blocks[15];

 MY_DIR* fhopendir( ino_t fh ) {
 // if ( fh is not the handle of a directory ) return not−a−directory error;
 MY_DIR * tmp = new MY_DIR;
 tmp−>fh = fh;
 }


 // called at lines #707 and #726 of bbfs.c
 dirent* my_readdir( MY_DIR* dirp ) {
 off_t tmp = (dirp−>offset)−>d_reclen;
 dirp−>offset += tmp;
 return ( dirp−>offset < dirp−>max_offset) ? dirp−>offset : 0 ;
 }


 // called at line #742 of bbfs.c
 int my_closedir( MY_DIR* dirp ) {
 delete dirp;
 }


 ino_t lookup( string name, ino_t fh ) {
 // finds and returns ino_t of file whose name is name in directory fh.
 // This function will be used by open() and opendir().
 MY_DIR* dirp = fhopendir( fh ); // fhopendir checks if fh is handle of a dir.
 if ( ! dirp ) return err; // cannot_open_error
 while ( dirent* dp = my_readdir(dirp) ) {
 string s = dp−>d_name; // converts C string to C++ string
 if ( s == name ) {
 my_closedir(dirp);
 if ( dp−>d_type != DT_REG && dp−>d_type != DT_DIR ) {
 return err; // wrong−file−type error
 // later we may add more types
 } else {
 return dp−>d_fileno;
 }
 }
 }
 my_closedir(dirp); // close MY_DIR asap, to reset internal data
 return err; // name−not−found
 }


 ino_t find_inode( string fullpath ) {
 // fullpath must starts at the root. // See how Pfeiffer does it.
 ino_t fh = 2; // 2 is the i_no of the root directory.
 vector<string> v = split( fullpath, "/" );
396 for ( int i = 1; i != v.size(); ++ i ) { // omit the initial null segment
 fh = lookup( v[i], fh );
 }
 }

 struct inode * find_inode( ino_t ) {
 // the actual code for this will depend on how you implement this map
 return 0;
 }

 // called at line #659 of bbfs.c
 MY_DIR * my_opendir( char fullpath[PATH_MAX] ) {
 return fhopendir( find_inode( fullpath ) );
 }

 // ===================== a crude c++ approach ======================

 class file {
 public:
 inode ino;
 };

 class regfile : file {
 string content;
 };

 class openfile : regfile {
 stringstream(content);
 };

 class directory : file {
 public:
 map<string,file> themap; // a balanced binary tree.
 };


 template< typename T1, typename T2>
 string pickle(map<T1,T2> m) {
 typename map<T1,T2>::iterator it;
 string s;
 stringstream ss(s);
 for ( it = m.begin(); it != m.end(); ++it ) {
 // This requires some kind of separation/terminaton symbol at the end of eac
h.
 ss << it.first;
 ss << it.second;
 }
 }
