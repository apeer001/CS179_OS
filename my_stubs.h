// my_stubs.H

// These are the prototypes of the functions defined in my_stubs.cc,
// plus the definition of the struct my_DIR and the macro MY_DIR,
// which is needed to compile our modified version of bbfs.c.

#ifdef __cplusplus
extern "C" {
#endif


typedef struct { // This is my version of DIR
ino_t fh;
struct dirent * base; // not a pointer, rather an off_t??
struct dirent * offset;
struct dirent * max_offset; // We still need to initialize this.
} MY_DIR;

// called at line #95 of bbfs.c
int my_lstat( const char* path, struct stat *statbuf );

// called at line #125 of bbfs.c
int my_readlink( const char *path, char *link, size_t size );

// called at line #168 of bbfs.c. See line #151.
int my_mknod( const char *path, mode_t mode, dev_t dev );

// called at line #186 of bbfs.c
int my_mkdir( const char *path, mode_t mode );

// called at line #203 of bbfs.c
int my_unlink( const char *path );

// called at line #220 of bbfs.c;
int my_rmdir( const char *path );

// called at line #241 of bbfs.c
int my_symlink(const char *path, const char *link);

// called at line #261 of bbfs.c
int my_rename( const char *path, const char *newpath );

// called at line #279 of bbfs.c
int my_link(const char *path, const char *newpath);

// called at line #296 of bbfs.c
int my_chmod(const char *path, mode_t mode);

// called at line #314 of bbfs.c
int my_chown(const char *path, uid_t uid, gid_t gid);

// called at line #331 of bbfs.c
int my_truncate(const char *path, off_t newsize);

// called at line #349 of bbfs.c
int my_utime(const char *path, struct utimbuf *ubuf);

// called at line #376 of bbfs.c. Returns file handle not a file descriptor
int my_open( const char *path, int flags );


// called at line #411 of bbfs.c Note that our firt arg is an fh not an fd
int my_pread( int fh, char *buf, size_t size, off_t offset );

// called at line #439 of bbfs.c Note that our firt arg is an fh not an fd
int my_pwrite( int fh, const char *buf, size_t size, off_t offset );
// int my_pwrite( int fh, char *buf, size_t size, off_t offset );

// called at line #463 of bbfs.c
int my_statvfs(const char *fpath, struct statvfs *statv);

// called at line #530 of bbfs.c
int my_close( ino_t fh );

// called at line #553 of bbfs.c
int my_fdatasync( ino_t fh );

// called at line #556 of bbfs.c
int my_fsync( ino_t fh );

// called at line #575 of bbfs.c
int my_lsetxattr( const char *fpath, const char *name, const char *value, size_t
size, int flags );

// called at line #592 of bbfs.c
int my_lgetxattr( const char *fpath, const char *name, char *value, size_t size,
int flags );

// called at line #613 of bbfs.c
int my_llistxattr( const char *path, char *list, size_t size );

// called at line #634 of bbfs.c
int my_lremovexattr( const char *path, const char *name );


// In the next three functions, we need to create a my_DIR* from a file
// handle, i.e., an (ino_t) fh. So, how to do that?

// It doesn’t say this anywhere that I can find, but a my_DIR has to be
// similar to an open file. However, rather than creating a stream of
// bytes (chars) from the file, it creates a stream of directory
// entries. The key difference is how much you increment the offset
// counter each time you move to the next item.

// Note that at line #742 of bbfs.c, Professor Pfeiffer converts a
// file handle into a DIR via "(uintptr_t) fh." But, his file handles
// are indices of byte streams, while our are the addresses of inodes.

// I recommend that we simply maintain the counter as an
// directory−entry index and increment it by one each time. Then
// multiply it by the size of a directory entry to get the offset of
// the directory entry that it refers to within the directory. To get
// the address of that directory entry, we simply add the address of
// the corresponding block of the directory.

// called at line #659 of bbfs.c
MY_DIR * my_opendir( char fpath[PATH_MAX] );

// called at line #707 and #726 of bbfs.c
struct dirent *my_readdir( MY_DIR * dp );

// called at line #742 of bbfs.c
int my_closedir( MY_DIR* dp );

// called at line #826 of bbfs.c
int my_access( const char *fpath, int mask );

// called at line #856 of bbfs.c
int my_creat( const char *fpath, mode_t mode );

// called at line #887 of bbfs.c
int my_ftruncate( ino_t fh, off_t offset );

// called at line #921 of bbfs.c
// for details see: http://manpages.ubuntu.com/manpages/hardy/man2/stat.2.html
int my_fstat( ino_t fh, struct stat* statbuf );

// called at line #1015 of bbfs.c
char* my_realpath( const char* path, char* resolved_path );

#ifdef __cplusplus
}
#endif
