---
title: "Using Different Job Types"
slug: "dirac-job-scheduling-job-types"
teaching: 0
exercises: 0
questions:
- "What types of job can I run on HPC systems?"
- "How do I run a job that uses a node exclusively?"
- "How can I submit an OpenMP job that makes use of multiple threads within a single CPU?"
- "How do I submit a job that uses Message Passing Interface (MPI) parallelisation?"
- "How can I submit the same job many times with different inputs?"
- "How can I interactively debug a running job?"
objectives:
- "First learning objective. (FIXME)"
keypoints:
- "First key point. Brief Answer to questions. (FIXME)"
---

So far we've learned about the overall process and the necessary "scaffolding" around job submission;
using various parameters to configure a job to use resources,
making use of software installed on compute nodes by loading and unloading modules,
submitting a job, and monitoring it until (hopefully successful) completion.
Using what we've seen so far, let's take this further and look at some key types of job that we can run on HPC systems
to take advantage of various types of parallelisation,
using examples written in the C programming language.
We'll begin with a simple serial hello world example,
and briefly explore various ways that code is parallelised and run to make best use of such systems.

## Serial

With a serial job we run a single job on one node within a single process.
Essentially, this is very similar to running some code via the command line on your local machine.
Let's take a look at a simple example written in C (the full code can also be found in
[hello_world_serial.c](code/job_types/hello_world_serial.c).

~~~
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    printf("Hello world!\n");
}
~~~
{: .language-c}

After copying this into a file called `hello_world_serial.c`, we can then compile and run it, e.g.:

~~~
[yourUsername@login7a [cosma7] ~]$ gcc hello_world_serial.c -o hello_world_serial
[yourUsername@login7a [cosma7] ~]$ ./hello_world_serial
~~~

Depending on your system, you may need to preload a module to compile C (perhaps either using `cc` or `gcc`).
You should then see `Hello world!` printed to the terminal.

> ## Be Kind to the Login Nodes
> 
> It's worth remembering that the login node is often very busy managing lots of users logged in,
> creating and editing files and compiling software, and submitting jobs.
> As such, although running quick jobs directly on a login node is ok, for example to compile and quickly test some code,
> it's not intended for running computationally intensive jobs and these should always be submitted
> for execution on a compute node.
> 
> The login node is shared with all other users and your actions could cause issues for other people,
> so think carefully about the potential implications of issuing commands that may use large amounts of resource.
{: .callout}

Now, given this is a very simple serial job, we might write the following `hw_serial.sh` Slurm script to execute it:

~~~
#!/usr/bin/env bash
#SBATCH --account=yourAccount
#SBATCH --partition=aPartition
#SBATCH --job-name=hello_world_serial
#SBATCH --time=00:01:00
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --mem=1M
#SBATCH --chdir=/cosma/home/ds007/dc-crou1/job-scheduler-test

./hello_world_serial
~~~
{: .language-c}

Note here we are careful to specify only what resources we think we need.
In this case, a single node running a single process and very little memory (in fact, very likely a great deal less memory than that).

As before, as can submit this using `sbatch hw_serial.sh` to submit it, `squeue -u yourUsername` to monitor it until completion,
and then look at the `slurm-<job_number>.out` file to see the `Hello world!` output.

> ## Making Exclusive use of a Node
>
> We can use `#SBATCH --exclusive` to indicate we'd like exclusive access to the nodes we request,
> such that they are shared with no other jobs, regardless of how many CPUs we actually need.
> If we are running jobs that require particularly large amounts of memory, CPUs, or disk access, this may be important.
> However, as you might suspect, requesting exclusive use of a node may mean it takes some time to be allocated
> a whole node in which to run.
> Plus, as a responsible user, be careful to ensure you only request exclusive access to a node when your job needs it!
{: .callout}

## Multi-threaded via OpenMP

OpenMP allows programmers to identify and parallelize sections of code, enabling multiple threads to execute them concurrently.
This concurrency is achieved using a *shared-memory* model,
where all threads can access a common memory space and communicate through shared variables.

So with OpenMP, think of your program as a team with a leader (the master thread) and workers (the slave threads).
When your program starts, the leader thread takes the lead.
It identifies parts of the code that can be done at the same time and marks them,
and these marked parts are like tasks to be completed by the workers.
The leader then gathers a group of helper threads, and each helper tackles one of these marked tasks.
Each worker thread works independently, taking care of its task.
Then, once all the workers are done, they come back to the leader, and the leader continues with the rest of the program.

Let's look at a parallel version of hello world, which launches a number of threads.
You can find the code below in [hello_world_mp.c](code/job_types/hello_world_mp.c).

~~~
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    int num_threads, t;
    int *results;

    // Use an OpenMP library function to get the number of threads
    num_threads = omp_get_max_threads();

    // Create a buffer to hold the integer results from each thread
    results = malloc(sizeof(*results) * num_threads);

    // In parallel, within each thread available, store the thread's
    // number in our shared results buffer
    #pragma omp parallel shared(results)
    {
        int my_thread = omp_get_thread_num();
        results[my_thread] = my_thread;
    }

    for (t = 0; t < num_threads; t++)
    {
        printf("Hello world thread number received from thread %d\n", t);
    }
}
~~~
{: .language-c}

OpenMP makes use of compiler directives to indicate which sections we wish to run in parallel worker threads on a single CPU.
Compiler directives are special comments that are picked up by the C compiler and tell the compiler to behave a certain way
with the code being compiled.

In this example we use the `#pragma omp parallel` OpenMP compiler directive around a portion of the code,
so each worker thread will run this in parallel.
The number of threads that will run is set by the system and obtained using `omp_get_max_threads()`.

We also need to be clear how variables behave in parallel sections,
in particular to what extent they are shared between threads or private to each thread.
Here, we indicate that the `results` array is *shared* and accessible across all threads within this parallel code portion,
since in this case we want each worker's thread to add its thread number to our shared array.

Once this parallelised section's worker threads are complete, the program resumes a serial, single-threaded behaviour
within the master thread, and outputs the results array containing all the worker thread numbers.

Now before we compile and test it, we need to indicate how many threads we wish to run,
which is specified in the environment in a special variable and picked up by the program, so we'll do that first:

~~~
[yourUsername@login7a [cosma7] ~]$ export OMP_NUM_THREADS=3
[yourUsername@login7a [cosma7] ~]$ gcc hello_world_mp.c -o hello_world_mp
[yourUsername@login7a [cosma7] ~]$ ./hello_world_mp
~~~
{: .language-bash}

And we should see the following:

~~~
Hello world thread number received from thread 0
Hello world thread number received from thread 1
Hello world thread number received from thread 2
~~~
{: .output}

## Multi-process via Message Passing Interface (MPI)

Our previous example used multiple threads (i.e. parallel execution within a single process).
Let's take this parallelisation one step further to the level of a process,
where we run separate *processes* in parallel as opposed to threads.

At this level, things become more complicated!
Within a single thread we had the option to maintain access to variables across our threads,
but between processes, memory isn't shared, so if we want to share information between these processes we need another way to do it.
With MPI, this is done via sending and receiving messages between processes.

Now despite this inter-process communication being a greater overhead,
in general our master/worker model still holds.
In MPI, from the outset, when an MPI-enabled program is run, we have a number of processes executing in parallel.
Each of these processes is referred to as a *rank*, and one of these ranks (typically rank zero) is a coordinating, or master, rank.

So how does this look in a program?
You can find the code below in [hello_world_mp.c](code/job_types/hello_world_mp.c).

~~~
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
            printf("Hello world rank number received from %d\n", resultbuf[r]);
        }
    }

    MPI_Finalize();
}
~~~
{: .language-c}

You'll notice that the program is a fair bit more complex,
since here we need to explicitly coordinate the sending and receiving of messages and do some housekeeping for MPI itself.

After initialising MPI, in a similar vein to how we got the number of threads and threads identity,
we obtain the number of total ranks (processses) and our rank number.
Now in this example, for simplicity, we use a single MPI function `MPI_Gather` to simultaneously send the rank numbers
from each separate process to the coordinating rank.
Essentially, are sending `my_rank` (as `MPI_INT`, basically an integer) to rank `0`, which receives all responses,
including its own, within `resultbuf`.
Finally, if the rank is the coordinating rank, then the results are output.
The `if (my_rank == 0)` condition is important, since without it, all ranks would attempt to print the results,
since with MPI, typically all processes run the entire program.

Let's compile and run this now, ensuring we have the needed modules to compile MPI first.
On DiRAC's COSMA, this looks like:

~~~
[yourUsername@login7a [cosma7] ~]$ module load gnu_comp/13.1.0
[yourUsername@login7a [cosma7] ~]$ mpicc hello_world_mpi.c -o hello_world_mpi
~~~
{: .language-bash}

So note we need to use a specialised compiler, `mpicc`, to compile this MPI code.
Now we're able to run it, specifying how many separate processes we wish to run in parallel.
With MPI programs we typically run them using `mpiexec`,
and instead of specifying these in an environment variable,
we indicate this using the `-n` argument, e.g.:

~~~
[yourUsername@login7a [cosma7] ~]$ mpiexec -n 3 ./hello_world_mpi
~~~
{: .language-bash}

You should see:

~~~
Hello world rank number received from rank 0
Hello world rank number received from rank 1
Hello world rank number received from rank 2
~~~
{: .output}

## Array

So we've seen how parallelisation can be achieved using threads and processes,
but using a sophisticated job scheduler like Slurm, we are able to go a level higher using *job arrays*,
where we specify how many separate *jobs* we want running in parallel instead.



## Interactive

