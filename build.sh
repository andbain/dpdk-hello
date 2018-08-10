#!/bin/bash

function build_exe {
    CFLAGS="-g -Wall -Wshadow -Wold-style-definition -O0" #  -flto
    DPDK_DIR=$HOME/dpdk-18.02

    # The massive list of compile and link flags below were carefully
    # reverse engineered from the DPDK 18.02 example app Makefiles.
    # They might not be correct for other versions.

    gcc $2 \
    -o $1 $CFLAGS -m64 -pthread -march=native -DRTE_MACHINE_CPUFLAG_SSE \
    -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 \
    -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 \
    -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES \
    -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX \
    -DRTE_MACHINE_CPUFLAG_RDRAND -DRTE_MACHINE_CPUFLAG_FSGSBASE \
    -DRTE_MACHINE_CPUFLAG_F16C -DRTE_MACHINE_CPUFLAG_AVX2 \
    -I$DPDK_DIR/build/include \
    -L$DPDK_DIR/build/lib \
    -lrte_flow_classify -lrte_pipeline -lrte_table -lrte_port -lrte_pdump \
    -lrte_distributor -lrte_ip_frag -lrte_gro -lrte_gso \
    -lrte_meter -lrte_lpm -Wl,--whole-archive -lrte_acl \
    -Wl,--no-whole-archive -lrte_jobstats -lrte_metrics \
    -lrte_bitratestats -lrte_latencystats -lrte_power -lrte_timer \
    -lrte_efd -Wl,--whole-archive -lrte_cfgfile -lrte_hash \
    -lrte_member -lrte_vhost -lrte_kvargs -lrte_mbuf -lrte_net \
    -lrte_ethdev -lrte_bbdev -lrte_cryptodev -lrte_security \
    -lrte_eventdev -lrte_rawdev -lrte_mempool -lrte_mempool_ring \
    -lrte_ring -lrte_pci -lrte_eal -lrte_cmdline -lrte_reorder \
    -lrte_sched -lrte_kni -lrte_bus_pci -lrte_bus_vdev \
    -lrte_mempool_stack -lrte_pmd_af_packet -lrte_pmd_ark \
    -lrte_pmd_avf -lrte_pmd_avp -lrte_pmd_bnxt -lrte_pmd_bond \
    -lrte_pmd_cxgbe -lrte_pmd_e1000 -lrte_pmd_ena -lrte_pmd_enic \
    -lrte_pmd_fm10k -lrte_pmd_failsafe -lrte_pmd_i40e \
    -lrte_pmd_ixgbe -lrte_pmd_kni -lrte_pmd_lio -lrte_pmd_mlx4 \
    -libverbs -lmlx4 -lrte_pmd_nfp -lrte_pmd_null \
    -lrte_pmd_qede -lrte_pmd_ring -lrte_pmd_softnic \
    -lrte_pmd_sfc_efx -lrte_pmd_tap -lrte_pmd_thunderx_nicvf \
    -lrte_pmd_vdev_netvsc -lrte_pmd_virtio -lrte_pmd_vhost \
    -lrte_pmd_vmxnet3_uio -lrte_pmd_bbdev_null -lrte_pmd_null_crypto \
    -lrte_pmd_crypto_scheduler -lrte_pmd_skeleton_event \
    -lrte_pmd_sw_event -lrte_pmd_octeontx_ssovf -lrte_mempool_octeontx \
    -lrte_pmd_octeontx -lrte_pmd_opdl_event -lrte_pmd_skeleton_rawdev \
    -Wl,--no-whole-archive -lrt -lm -lnuma -ldl -Wl,-export-dynamic
}

build_exe dpdk_send send.c
build_exe dpdk_recv recv.c
