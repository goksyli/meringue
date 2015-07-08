#ifndef LIGHTVM_H
#define LIGHTVM_H
typedef struct lightVM_t{
	int fd_kvm;
	int fd_vm;
}lightVM_t;

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



int kvm_init(struct lightVM_t *pLightVM);
void kvm_exit(struct lightVM_t *pLightVM);
#endif