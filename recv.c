// ****************************************************************************
// This example shows how to recv some packets.
// ****************************************************************************

// Build it with:
//   ./build.sh
//
// Run it with:
//   sudo ./dpdk_recv
//
// Check it works by sending some packets from another machine, eg:
//   ping <destination address>


// DPDK headers
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_mbuf.h>

// Standard headers
#include <stdio.h>


// ****************************************************************************
// Tweak these values if you want.
// ****************************************************************************

// I believe that when packets arrive at the NIC, they are put into the RX ring
// and remain there until rte_eth_rx_burst() is called. So the size of the ring
// must be calculated based on the max time between calls to rte_eth_rx_burst()
// and the max packets per second you expect to handle.
#define RX_RING_SIZE 128

// DPDK has a pool allocator. Every "mbuf" holds one packets. This specifies
// how many packets are in the pool. I think this includes all those in the TX
// and RX rings, plus any that are currently in the hands of the application.
#define NUM_MBUFS 8191

// This value is probably irrelevant in this single threaded app. See
// the docs for the cache_size param of rte_mempool_create().
#define MBUF_CACHE_SIZE 0

// Everyone seems to use 32. Nobody seems to know why.
#define BURST_SIZE 32

// In DPDK, a "port" is a NIC. We will use the first NIC DPDK finds.
#define DPDK_PORT_ID 0


// ****************************************************************************
// Do not tweak these values.
// ****************************************************************************

// DPDK supports many queues per port. Simple apps only need one TX queue and
// one RX queue. You only need more than one if you are doing something like
// scatter/gather.
#define DPDK_QUEUE_ID_RX 0


static const struct rte_eth_conf port_conf_default = {
    .rxmode = { .max_rx_pkt_len = ETHER_MAX_LEN }
};


static void port_init(struct rte_mempool *mbuf_pool) {
    // Configure the Ethernet device.
    const int num_rx_queues = 1;
    const int num_tx_queues = 0;
    struct rte_eth_conf port_conf = port_conf_default;
    if (rte_eth_dev_configure(DPDK_PORT_ID, num_rx_queues, num_tx_queues, &port_conf)) {
        rte_exit(EXIT_FAILURE, "rte_eth_dev_configure() failed.\n");
    }

    // Set up RX queue.
    if (rte_eth_rx_queue_setup(DPDK_PORT_ID, DPDK_QUEUE_ID_RX, RX_RING_SIZE,
            rte_eth_dev_socket_id(DPDK_PORT_ID), NULL, mbuf_pool) < 0) {
        rte_exit(EXIT_FAILURE, "Couldn't setup RX queue.\n");
    }

    // Start the Ethernet port.
    if (rte_eth_dev_start(DPDK_PORT_ID) < 0) {
        rte_exit(EXIT_FAILURE, "Device start failed.\n");
    }

    // Enable RX in promiscuous mode for the Ethernet device.
    rte_eth_promiscuous_enable(DPDK_PORT_ID);
}


int main(int argc, char *argv[]) {
    // Initialize the Environment Abstraction Layer. All DPDK apps must do this.
    if (rte_eal_init(argc, argv) < 0) {
        rte_exit(EXIT_FAILURE, "Error with EAL initialization\n");
    }

    // Creates a new mempool in memory to hold the mbufs.
    struct rte_mempool *mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL", NUM_MBUFS,
        MBUF_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
    if (!mbuf_pool) {
        rte_exit(EXIT_FAILURE, "Couldn't create mbuf pool\n");
    }

    port_init(mbuf_pool);

    while (1) {
        struct rte_mbuf *mbufs[BURST_SIZE];
        unsigned num_recvd = rte_eth_rx_burst(DPDK_PORT_ID, DPDK_QUEUE_ID_RX, mbufs, BURST_SIZE);
        for (unsigned i = 0; i < num_recvd; i++) {
            printf("Received packet: ");

            // Print 10 bytes of UDP payload (without checking the packet is UDP).
            char const *pack_data = rte_pktmbuf_mtod(mbufs[i], char const *);
            for (int j = 42; j < 52; j++) {
                putchar(pack_data[j]);
            }
            putchar('\n');

            rte_pktmbuf_free(mbufs[i]);
        }
    }

    return 0;
}
