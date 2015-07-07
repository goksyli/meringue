#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/kvm.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "lightVM.h"




int kvm_init(struct lightVM_t *pLightVM)
{
	int ret;
	/*
	get kvm and vm file descriptors, 
	put in the struct lightVM_T
	*/
	/*todo add close on exec flag when opening*/
	int fd_kvm = pLightVM->fd_kvm = open("/dev/kvm", O_RDWR);
	if( fd_kvm == -1 ){
		printf("Error: Opening dev/kvm failed\n");
		return -1;
	}

	ret = ioctl(fd_kvm, KVM_GET_API_VERSION,0);
	if( ret != KVM_API_VERSION) /* exactly 12*/{
		printf("Debug: KVM API Version is %d\n",ret);
		return -1;
	}	

	/*
	Todo use inherent kvm for check
	for security and robustness, we should add API version
	and extension check 

	moreover get available :
	mmapsize
	cpuid
	msr
	*/

	/*todo add close on exec flag when opening*/
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

#if 0
int init_memory()
{
	void *addr = mmap(NULL, 10 * MB, PROT_READ | PROT_WRITE,
		MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

	struct kvm_userspace_memory_region region = {

	};
	
}
#endif

int main(int argc, char *argv[])
{
	struct lightVM_t lightVM;
	int ret = kvm_init(&lightVM);
	if( ret != 0 )
		return -1;
	kvm_exit(&lightVM);
	return 0;
}