#include <shmem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    int i, err_count = 0;

    shmem_init();

    size_t          size     = 2048;
    int             me       = shmem_my_pe();
    int             n        = shmem_n_pes();
    int             pe       = (me + 1) % n;
    uint64_t *      message  = malloc(size * sizeof(uint64_t));
    static uint64_t sig_addr = 0;

    for (i = 0; i < size; i++) {
        message[i] = me;
    }

    uint64_t *data = shmem_calloc(size, sizeof(uint64_t));

    if (me != 0) {
        shmem_wait_until(&sig_addr, SHMEM_CMP_EQ, 1);
        memcpy(message, data, size * sizeof(uint64_t));
    }

    if (me != (n - 1)) {
        shmem_put_signal(data, message, size, &sig_addr, 1, pe);
    }

    free(message);
    shmem_free(data);

    shmem_finalize();
    return 0;
}
