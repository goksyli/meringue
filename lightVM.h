#ifndef LIGHTVM_H
#define LIGHTVM_H
#include <linux/kvm.h>
#include <stdbool.h>
struct lightVM_t{
	int fd_kvm;
	int fd_vm;
	__u64 size;
	void * addr;
	int mmap_size;
};

struct vcpu {
	unsigned id;
	int fd_vcpu;
	struct kvm_run * run_state;
	struct kvm_regs regs;
	struct kvm_sregs sregs;
	bool is_running;
};

struct kvm_caps 
{
	const char * name;
	int code;
};

#define DEFINE_KVM_CAP(caps)	\
	.name = #caps,			\
	.code = caps			\

#define DEFINE_KVM_CAP_DUMMY	\
	.name = "Dummy",		\
	.code = 0				\
/*add space on intel (3 pages)
the region must be within the first 4GB
of the guest physical address
*/
#if 0
#define TSS_ADDRESS 0xffffffffffffd000
#endif

#define TSS_ADDRESS 0xfffbd000

#define MB_SHIFT	(20)
#define KB_SHIFT	(10)
#define GB_SHIFT	(30)

#define KVM_32BIT_MAX_MEM_SIZE	(1ULL << 32)
#define KVM_32BIT_GAP_SIZE		(768 << 20)
#define KVM_32BIT_GAP_START		(KVM_32BIT_MAX_MEM_SIZE - KVM_32BIT_GAP_SIZE)

#define MIN_RAM_SIZE_MB			(64ULL)
#define MIN_RAM_SIZE_BYTE		(MIN_RAM_SIZE_MB << MB_SHIFT)


#define NR_VCPUS 	8


int kvm_init(struct lightVM_t *pLightVM);
void kvm_exit(struct lightVM_t *pLightVM);
#endif