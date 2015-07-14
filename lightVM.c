#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/kvm.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdbool.h>
#include <linux/types.h>
#include <sys/mman.h>
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

struct lightVM_t lightVM;

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

/* The KVM_RUN ioctl (cf.) communicates with userspace via a shared
memory region.  This ioctl returns the size of that region. */
int get_mmap_size(int fd_kvm)
{
	int ret;
	ret = ioctl(fd_kvm,KVM_GET_VCPU_MMAP_SIZE,0);
	if( ret < 0 )
		die_perror("KVM_GET_VCPU_MMAP_SIZE failed");
	return ret;
}

static __u64 host_ram_size(void)
{
	long page_size;
	long nr_pages;

	nr_pages = sysconf(_SC_PHYS_PAGES);
	if( nr_pages < 0 ) {
		pr_warning("sysconf(_SC_PHYS_PAGES) failed");
		return 0;
	}

	page_size = sysconf(_SC_PAGE_SIZE);
	if( page_size < 0 ){
		pr_warning("sysconf(_SC_PAGE_SIZE) failed");
		return 0;
	}

	return (nr_pages*page_size) >> MB_SHIFT;
}

#define RAM_SIZE_RATIO	0.8

static __u64 get_ram_size(int nr_cpus)
{
	__u64 available;
	__u64 ram_size;

	ram_size = 64 * (nr_cpus + 3);

	available = host_ram_size() * RAM_SIZE_RATIO;
	if( !available )
		available = MIN_RAM_SIZE_MB;
	if( ram_size > available )
		ram_size = available;

	return ram_size;
}





int init_memory(int fd_vm)
{
	int ret;
	__u64 size = get_ram_size(4);

	lightVM.size = size;
	void *addr = mmap(NULL, size, PROT_READ | PROT_WRITE,
		MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	lightVM.addr = addr;
	struct kvm_userspace_memory_region region = {
		.slot = 0,
		.flags = 0,
		.guest_phys_addr = 0,
		.memory_size = size,
		.userspace_addr = (__u64)addr
	};
	/* Don't use KVM_SET_MOMERY_REGION which has been removed */
	ret = ioctl(fd_vm, KVM_SET_USER_MEMORY_REGION, &region);
	if( ret < 0 )
		return -errno;

	return 0;
}

void kvm_delete_memory(void *start,__u64 size)
{
	munmap(start,size);
}
int kvm_init(struct lightVM_t *pLightVM)
{
	int ret;
	int mmap_size;
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

	/*
	moreover get available :
	mmapsize
	cpuid
	msr
	*/
	mmap_size = get_mmap_size(fd_kvm);


	/*TODO add close on exec flag when opening*/
	int fd_vm = pLightVM->fd_vm = ioctl(fd_kvm, KVM_CREATE_VM, 0);
	if( fd_vm == -1 ){
		printf("Error: Creating VM failed");
		return -1;/*FIXME close /dev/kvm before return*/
	}
	/*
	TODO

	some arch specific init

	refactor into files by devices 
	
	*/
	kvm_arch_init(fd_vm);

	ret = init_memory(fd_vm);

	return 0;
}

void kvm_exit(struct lightVM_t *pLightVM)
{
	int fd_kvm = pLightVM->fd_kvm;
	int fd_vm = pLightVM->fd_vm;
	__u64 size = pLightVM->size;
	void* addr = pLightVM->addr;
	kvm_delete_memory(addr,size);	
	close(fd_vm);
	close(fd_kvm);
}





int main(int argc, char *argv[])
{
	
	int ret = kvm_init(&lightVM);
	if( ret != 0 )
		return -1;
	kvm_exit(&lightVM);
	return 0;
}