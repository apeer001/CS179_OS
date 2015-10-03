#include "../ucrcse/fuse-tutorial-2014-06-12/src/fuse.h"
#include<stdio.h>

struct fuse_operations fs_oper = {
		

};

int fs_getattr(const char* path, struct stat *statbuf){
	return 0;
}

int fs_readlink(const char* path, char* link, size_t size){
	return 0;
}

/*fs_getdir(){

}*/

int fs_mkmod(const char* path, mode_t mode, dev_t dev){
	return 0;
}

int fs_mkdir(const char* path, mode_t mode){
	return 0;
}

int fs_unlink(const char *path){
	return 0;
}

int fs_rmdir(const char *path){
	return 0;
}

int fs_symlink(const char *path, const char *link){
	return 0;
}

int fs_rename(const char *path, const char *newpath){
	return 0;
}

int fs_link(const char *path, const char *newpath){
	return 0;
}

int fs_chmod(const char *path, mode_t mode ){
	return 0;
}

int fs_chown(const char *path, uid_t uid, gid_t gid){
	return 0;
}

int fs_truncate(const char *path, off_t newsize){
	return 0;
}

int fs_utime(const char *path, struct utimbuf *ubuf){
	return 0;
}

int fs_open(const char *path, struct fuse_file_info *fi){
	return 0;
}

int fs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
	return 0;
}

int fs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
	return 0;
}

int fs_statfs(const char *path, struct statvfs *statv){
	return 0;
}

int fs_flush(const char *path, struct fuse_file_info *fi){
	return 0;
}

int fs_release(const char *path, struct fuse_file_info *fi){
	return 0;
}

int fs_fsync(const char *path, int datasync, struct fuse_file_info *fi){
	return 0;
}

int fs_setxattr(const char *path, const char *name, const char *value, size_t size){
	return 0;
}

int fs_getxattr(const char *path, const char *name, char *value, size_t size){
	return 0;
}

int fs_listxattr(const char *path, char *list, size_t size){
	return 0;
}

int fs_removexattr(const char *path, const char *name){
	return 0;
}

int fs_opendir(const char *path, struct fuse_file_info *fi ){
	return 0;
}

int fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi){
	return 0;

}

int fs_releasedir(const char *path, struct fuse_file_info *fi){
	return 0;

}

int fs_fsyncdir(const char *path, int datasync, struct fuse_file_info *fi){
	return 0;
}

void fs_init(struct fuse_conn_info *conn){
}

void fs_destroy(void *userdata){
}

int fs_access(const char *path, int mask){
	return 0;
}

int fs_create(const char *path, mode_t mode, struct fuse_file_info *fi){
	return 0;
}

int fs_ftruncate(const char *path, off_t offset, struct fuse_file_info *fi){
	return 0;

}

int fs_fgetattr(const char *path, struct stat *statbuf, struct fuse_file_info *fi){
	return 0;
}

int main(){
	printf("hello world\n");
	return 0;
}
