fdef __cplusplus
8 extern "C" {
9 #endif
10
11
12 typedef struct { // This is my version of DIR
13 ino_t fh;
14 struct dirent * base; // not a pointer, rather an off_t??
15 struct dirent * offset;
16 struct dirent * max_offset; // We still need to initialize this.
17 } MY_DIR;
18
19 // called at line #95 of bbfs.c
20 int my_lstat( const char* path, struct stat *statbuf );
21
22 // called at line #125 of bbfs.c
23 int my_readlink( const char *path, char *link, size_t size );
24
25 // called at line #168 of bbfs.c. See line #151.
26 int my_mknod( const char *path, mode_t mode, dev_t dev );
27
28 // called at line #186 of bbfs.c
29 int my_mkdir( const char *path, mode_t mode );
30
31 // called at line #203 of bbfs.c
32 int my_unlink( const char *path );
33
34 // called at line #220 of bbfs.c;
35 int my_rmdir( const char *path );
36
37 // called at line #241 of bbfs.c
38 int my_symlink(const char *path, const char *link);
39
40 // called at line #261 of bbfs.c
41 int my_rename( const char *path, const char *newpath );
42
43 // called at line #279 of bbfs.c
44 int my_link(const char *path, const char *newpath);
45
46 // called at line #296 of bbfs.c
47 int my_chmod(const char *path, mode_t mode);
48
49 // called at line #314 of bbfs.c
50 int my_chown(const char *path, uid_t uid, gid_t gid);
51
52 // called at line #331 of bbfs.c
53 int my_truncate(const char *path, off_t newsize);
54
55 // called at line #349 of bbfs.c
56 int my_utime(const char *path, struct utimbuf *ubuf);
57
58 // called at line #376 of bbfs.c. Returns file handle not a file descriptor
59 int my_open( const char *path, int flags );
60
61
62 // called at line #411 of bbfs.c Note that our firt arg is an fh not an fd
63 int my_pread( int fh, char *buf, size_t size, off_t offset );
64
65 // called at line #439 of bbfs.c Note that our firt arg is an fh not an fd
66 int my_pwrite( int fh, const char *buf, size_t size, off_t offset );
67 // int my_pwrite( int fh, char *buf, size_t size, off_t offset );
68
69 // called at line #463 of bbfs.c
70 int my_statvfs(const char *fpath, struct statvfs *statv);
71
72 // called at line #530 of bbfs.c
73 int my_close( ino_t fh );

// called at line #553 of bbfs.c
76 int my_fdatasync( ino_t fh );
77
78 // called at line #556 of bbfs.c
79 int my_fsync( ino_t fh );
80
81 // called at line #575 of bbfs.c
82 int my_lsetxattr( const char *fpath, const char *name, const char *value, size_t
 size, int flags );
83
84 // called at line #592 of bbfs.c
85 int my_lgetxattr( const char *fpath, const char *name, char *value, size_t size,
 int flags );
86
87 // called at line #613 of bbfs.c
88 int my_llistxattr( const char *path, char *list, size_t size );
89
90 // called at line #634 of bbfs.c
91 int my_lremovexattr( const char *path, const char *name );
92
93
94 // In the next three functions, we need to create a my_DIR* from a file
95 // handle, i.e., an (ino_t) fh. So, how to do that?
96
97 // It doesn’t say this anywhere that I can find, but a my_DIR has to be
98 // similar to an open file. However, rather than creating a stream of
99 // bytes (chars) from the file, it creates a stream of directory
100 // entries. The key difference is how much you increment the offset
101 // counter each time you move to the next item.
102
103 // Note that at line #742 of bbfs.c, Professor Pfeiffer converts a
104 // file handle into a DIR via "(uintptr_t) fh." But, his file handles
105 // are indices of byte streams, while our are the addresses of inodes.
106
107 // I recommend that we simply maintain the counter as an
108 // directory−entry index and increment it by one each time. Then
109 // multiply it by the size of a directory entry to get the offset of
110 // the directory entry that it refers to within the directory. To get
111 // the address of that directory entry, we simply add the address of
112 // the corresponding block of the directory.
113
114 // called at line #659 of bbfs.c
115 MY_DIR * my_opendir( char fpath[PATH_MAX] );
116
117 // called at line #707 and #726 of bbfs.c
118 struct dirent *my_readdir( MY_DIR * dp );
119
120 // called at line #742 of bbfs.c
121 int my_closedir( MY_DIR* dp );
122
123 // called at line #826 of bbfs.c
124 int my_access( const char *fpath, int mask );
125
126 // called at line #856 of bbfs.c
127 int my_creat( const char *fpath, mode_t mode );
128
129 // called at line #887 of bbfs.c
130 int my_ftruncate( ino_t fh, off_t offset );
131
132 // called at line #921 of bbfs.c
133 // for details see: http://manpages.ubuntu.com/manpages/hardy/man2/stat.2.html
134 int my_fstat( ino_t fh, struct stat* statbuf );
135
136 // called at line #1015 of bbfs.c
137 char* my_realpath( const char* path, char* resolved_path );
138
139 #ifdef __cplusplus
140 }
141 #endif
