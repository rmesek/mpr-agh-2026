// Run with 2 processes, e.g.:
// mpiexec -np 2 cargo run --release
#![deny(warnings)]

use mpi::traits::*;

const N_ITERS: usize = 1001;
const MSG_SIZES: [usize; 26] = [
    1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072,
    262144, 524288, 1048576, 2097152, 4194304, 8388608, 16777216, 33554432,
];

const SENDER: i32 = 0;
const RECEIVER: i32 = 1;
const TAG: i32 = 0;

fn main() {
    let universe = mpi::initialize().unwrap();
    let world = universe.world();
    let size = world.size();
    let rank = world.rank();

    if size != 2 {
        if rank == SENDER {
            println!("This application is meant to be run with 2 processes.");
        }
        world.abort(1);
    }

    // Print host information for benchmark processes.
    let hostname = mpi::environment::processor_name().unwrap_or_else(|_| "unknown".to_string());
    match rank {
        SENDER => println!("Process {} (sender) is running on {}", rank, hostname),
        RECEIVER => println!("Process {} (receiver) is running on {}", rank, hostname),
        _ => {}
    }

    if rank == SENDER {
        println!("Send Mode\tMessage Size [Bytes]\tThroughput [Mbit/s]\tLatency [ms]");
    }

    for msg_size in MSG_SIZES {
        let mut buffer = vec![255u8; msg_size];

        world.barrier();
        let start_time = mpi::environment::time();
        world.barrier();

        for _ in 0..N_ITERS {
            match rank {
                SENDER => {
                    world
                        .process_at_rank(RECEIVER)
                        .send_with_tag(&buffer[..], TAG);
                    world
                        .process_at_rank(RECEIVER)
                        .receive_into_with_tag(&mut buffer[..], TAG);
                }
                RECEIVER => {
                    world
                        .process_at_rank(SENDER)
                        .receive_into_with_tag(&mut buffer[..], TAG);
                    world
                        .process_at_rank(SENDER)
                        .send_with_tag(&buffer[..], TAG);
                }
                _ => unreachable!(),
            }
        }

        let end_time = mpi::environment::time();
        let one_way_latency = (end_time - start_time) / (2.0 * N_ITERS as f64);

        if rank == SENDER {
            let throughput_mbps = (msg_size as f64 * 8.0) / (one_way_latency * 1e6);
            let latency_ms = one_way_latency * 1e3;
            println!(
                "MPI_Send\t{}\t{}\t{}",
                msg_size, throughput_mbps, latency_ms
            );
        }
    }
}
