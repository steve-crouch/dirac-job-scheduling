#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char** argv) {
    int my_rank, n_ranks;
    int *resultbuf;
    int r;

    MPI_Init(&argc, &argv);

    // Obtain the rank identifier for this process, and the total number of ranks
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);

    // Create buffer to hold rank numbers received from all ranks
    // This will include the coordinating rank (typically rank 0),
    // which also does the receiving
    resultbuf = malloc(sizeof(*resultbuf) * n_ranks);

    // All ranks send their rank identifier to rank 0
    MPI_Gather(&my_rank, 1, MPI_INT, resultbuf, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // If we're the coordinating rank (typically designated as rank 0),
    // then print out the rank numbers received
    if (my_rank == 0) {
        for (r = 0; r < n_ranks; r++) {
            printf("Hello world rank number received from rank %d\n", resultbuf[r]);
        }
    }

    MPI_Finalize();
}
