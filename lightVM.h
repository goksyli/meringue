#ifndef LIGHTVM_H
#define LIGHTVM_H
typedef struct lightVM_t{
	int fd_kvm;
	int fd_vm;
}lightVM_t;

int kvm_init(struct lightVM_t *pLightVM);
void kvm_exit(struct lightVM_t *pLightVM);
#endif