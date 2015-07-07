#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/kvm.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "lightVM.h"




int kvm_init(struct lightVM_t *pLightVM)
{
	int fd_kvm = pLightVM->fd_kvm = open("/dev/kvm", O_RDWR);
	if( fd_kvm == -1 ){
		printf("Error: Opening dev/kvm failed\n");
		return -1;
	}
	int fd_vm = pLightVM->fd_vm = ioctl(fd_kvm, KVM_CREATE_VM, 0);
	if( fd_vm == -1 ){
		printf("Error: Creating VM failed");
		return -1;
	}
	return 0;
}

void kvm_exit(struct lightVM_t *pLightVM)
{
	int fd_kvm = pLightVM->fd_kvm;
	int fd_vm = pLightVM->fd_vm;	
	close(fd_vm);
	close(fd_kvm);
}

int main(int argc, char *argv[])
{
	struct lightVM_t lightVM;
	int ret = kvm_init(&lightVM);
	if( ret != 0 )
		return -1;
	kvm_exit(&lightVM);
	return 0;
}