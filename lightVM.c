#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/kvm.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdbool.h>
#include "lightVM.h"
#include "util.h"
struct kvm_caps kvm_req_caps [] = {
	{ DEFINE_KVM_CAP_DUMMY },
	{ DEFINE_KVM_CAP(KVM_CAP_USER_MEMORY) },
	/* KVM_CAP_SET_IDENTITY_MAP_ADDR */
	{ DEFINE_KVM_CAP(KVM_CAP_IRQCHIP) },
	/* KVM_CAP_IOEVENTFD */
	/* KVM_CAP_IRQFD */
	{ DEFINE_KVM_CAP(KVM_CAP_PIT2) },
	/* KVM_CAP_PIT_STATE2 */
	{ DEFINE_KVM_CAP(KVM_CAP_EXT_CPUID) },
	/* KVM_CAP_SIGNAL_MSI */
	/* KVM_CAP_VCPU_EVENTS */
	/* KVM_CAP_ADJUST_CLOCK */
	/* KVM_CAP_XSAVE */
	/* KVM_CAP_XCRS */
	{ DEFINE_KVM_CAP(KVM_CAP_COALESCED_MMIO) },
	{ DEFINE_KVM_CAP(KVM_CAP_SET_TSS_ADDR) },

	
	{ DEFINE_KVM_CAP(KVM_CAP_IRQ_ROUTING) },

	{ DEFINE_KVM_CAP(KVM_CAP_HLT) },
	{ DEFINE_KVM_CAP(KVM_CAP_IRQ_INJECT_STATUS) },

	{ 0, 0 }
};

bool cap_supported(int fd_kvm, int code)
{
	int ret;

	ret = ioctl(fd_kvm, KVM_CHECK_EXTENSION, code);
	if( ret < 0 )
		return false;
	return ret;
}

int check_caps(int fd_kvm)
{
	int i;
	for( i = 1; ;i++)
		if( !kvm_req_caps[i].name)
			break;
		if( !cap_supported(fd_kvm, kvm_req_caps[i].code)){
			pr_err("Unsupported KVM CAP: %s", kvm_req_caps[i].name);
			return -i;
		}
	return 0;
}


void kvm_arch_init(int fd_vm)
{
	struct kvm_pit_config pit_config = { .flags = 0, };
	int ret;

	ret = ioctl(fd_vm,KVM_CREATE_PIT2,&pit_config);
	if( ret < 0 )
		die_perror("KVM_CREATE_PIT2 ioctl failed");
	ret = ioctl(fd_vm,KVM_CREATE_IRQCHIP);
	if( ret < 0 )
		die_perror("KVM_CREATE_IRQCHIP ioctl failed");


	ret = ioctl(fd_vm,KVM_SET_TSS_ADDR, TSS_ADDRESS);
	if( ret < 0 )
		die_perror("KVM_SET_TSS_ADDR ioctl failed");




}





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
		return -1;/*FIXME close /dev/kvm before return*/
	}	


	if( check_caps(fd_kvm)){
		pr_err("A required KVM cap is not supported by OS");
		return -1;
	}

	/*TODO add close on exec flag when opening*/
	int fd_vm = pLightVM->fd_vm = ioctl(fd_kvm, KVM_CREATE_VM, 0);
	if( fd_vm == -1 ){
		printf("Error: Creating VM failed");
		return -1;/*FIXME close /dev/kvm before return*/
	}
	/*
	TODO

	some arch specific init
	
	moreover get available :
	mmapsize
	cpuid
	msr
	*/
	kvm_arch_init(fd_vm);

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