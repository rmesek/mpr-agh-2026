```bash
srun --time=0:20:00 --ntasks 2 --nodes 2 --ntasks-per-node=1 --partition=plgrid --account=plgmpr26-cpu --pty /bin/bash
```

```bash
ml gcc/13.2.0 openmpi/4.1.6-gcc-13.2.0 rust/1.88.0-gcccore-14.3.0 clang/18.1.8-gcccore-13.3.0

export CARGO_TARGET_DIR="$SCRATCH/target"
mkdir -p $CARGO_TARGET_DIR
cargo build --release
mpirun -np 2 $CARGO_TARGET_DIR/release/lab-04-mpi
```

```bash
mpirun -np 2 --mca pml ucx --mca btl self $CARGO_TARGET_DIR/release/lab-04-mpi
```

```bash
UCX_LOG_LEVEL=info
mpirun -np 2 -mca pml_base_verbose 100 $CARGO_TARGET_DIR/release/lab-04-mpi 2>&1 | grep "PML"
```