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
- "Use a popular compiler to compile a C program before testing and submitting it."
- "Use a compute node for a job exclusively."
- "Describe how an OpenMP job parallelises its computation."
- "Compile and submit an OpenMP job."
- "Describe how an MPI job parallelises its computation."
- "Compile and submit an MPI job."
- "Highlight the key differences between OpenMP and MPI jobs."
- "Define and submit an array job to execute multiple tasks within a single job."
- "Use an interactive job to run commands remotely on a compute node."
keypoints:
- "The login node is shared with all other users so be careful not to run complex programs on them."
- "OpenMP allows programmers to specify sections of code to execute in parallel threads on a single CPU, using compiler directives."
- "MPI programs make use of message-passing between multiple processes to coordinate communication."
- "OpenMP uses a *shared memory* model, whilst MPI uses a *distributed memory* model."
- "An array job is a self-contained set of multiple jobs - known as *tasks* - managed as a single job."
- "Interactive jobs allow us to interact directly with a compute node, so are very useful for debugging and exploring the running of code in real-time."
- "Interactive jobs consume resources while an interactive session is active, so we must be careful to use them efficiently."
- "Run and develop applications at a small scale first, before making use of powerful scaling techniques, to avoid potentially expensive consumption of resources."
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
This concurrency is achieved using a *shared memory* model,
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

FIXME: use a submission script instead

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
With OpenMP we had the option to maintain access to variables across our threads,
but between processes, memory isn't shared, so if we want to share information between these processes we need another way to do it.
MPI uses a *distributed memory* model, so communication is done via sending and receiving messages between processes.

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

Let's compile this now. On DiRAC's COSMA, this looks like:

~~~
[yourUsername@login7a [cosma7] ~]$ module load gnu_comp/13.1.0
[yourUsername@login7a [cosma7] ~]$ mpicc hello_world_mpi.c -o hello_world_mpi
~~~
{: .language-bash}

So note we need to use a specialised compiler, `mpicc`, to compile this MPI code.

Now we're able to run it, and specify how many separate processes we wish to run in parallel.
However, since this is a multi-processing job we should submit it via Slurm.
Let's create a new submission script named `hw_mpi.sh` that executes this MPI job:

~~~
#!/usr/bin/env bash
#SBATCH --account=yourAccount
#SBATCH --partition=aPartition
#SBATCH --job-name=hello_world_mpi
#SBATCH --time=00:01:00
#SBATCH --nodes=1
#SBATCH --ntasks=3
#SBATCH --mem=1M

module load gnu_comp/11.1.0
module load openmpi/4.1.4

mpiexec ./hello_world_mpi
~~~
{: .language-c}

On this particular HPC setup on DiRAC's COSMA, we need to load both a suitable compiler as well as the OpenMPI module
so we can run MPI jobs; your system may differ!

Note also that we specify `3` to `--ntasks` this time to reflect the number of processes we wish to run.

You'll also see that with MPI programs we use `mpiexec` to run them,
and instead of specifying the number of parallel processes in an environment variable,
`mpiexec` will use the number of processes available to the job; in this case 3.

> ## Efficient use of Resources
> 
> Beware using `--ntasks` correctly when submitting non-MPI jobs.
> Setting this to a number greater than 1 will have the effect of running the job that many times,
> regardless of whether it's using MPI or not,
> so 
> It also has the effect of multiple jobs overwriting, as opposed to appending to, the jobs' output log file,
> so the fact this has happened may not be obvious.
{: .callout}

In the Slurm output file You should see:

~~~
Hello world rank number received from rank 0
Hello world rank number received from rank 1
Hello world rank number received from rank 2
~~~
{: .output}

## Array

So we've seen how parallelisation can be achieved using threads and processes,
but using a sophisticated job scheduler like Slurm, we are able to go a level higher using *job arrays*,
where we specify how many *separate jobs* (as *tasks*) we want running in parallel instead.

One way we might do this is using a simple for loop within a Bash script to submit multiple jobs.
For example, to submit three jobs, we could do:

~~~
for JOB_NUMBER in {1..3}; do
    sbatch a-script.sh $JOB_NUMBER
done
~~~
{: .language-bash}

In certain circumstances this may be a suitable approach,
particularly if each task differs substantially, and you need to control over each subtask explicitly
or change configuration resource request parameters for each job task.

However, job arrays have some unique advantages over this approach;
namely that job arrays are *self-contained*, so each array task is linked to the same job submission ID,
which means we can use `sacct` and `squeue` to query them as a whole submission.
Plus, we need no additional code to make it work, so it's generally a simpler way to do it.

To make use of a job array approach in a Slurm we add an additional `--array` parameter to our submission script.
So let's create a new `hw_serial_array.sh` script that uses it:

~~~
#!/usr/bin/env bash
#SBATCH --account=yourAccount
#SBATCH --partition=aPartition
#SBATCH --job-name=hello_world_serial_array
#SBATCH --array=1-3
#SBATCH --time=00:01:00
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --mem=1M

echo "Task $SLURM_ARRAY_TASK_ID"
./hello_world_serial
sleep 30
~~~
{: .language-bash}

`$SLURM_ARRAY_TASK_ID` is a shell environment variable that holds the number of the individual task running.

Given the jobs are trivial and finish very quickly,
we've added a `sleep 30` command at the end so each task takes an additional 30 seconds to run,
so that you should be able to see the array job in the queue before it disappears from the list.
If we now submit this job with `sbatch` and then use `squeue -j jobID` we should see something like the following,
a single entry but with `[1-3]` in the `JOBID` indicating the three subtasks as part of this job:

~~~
             JOBID PARTITION     NAME     USER ST       TIME  NODES NODELIST(REASON)
     6803105_[1-3] cosma7-pa hello_wo dc-crou1 PD       0:00      1 (Priority)
~~~
{: .output}

> ## Canceling Array Jobs
> 
> We are able to completely cancel an array job and all its tasks using `scancel jobID` as with a normal job.
> With the job above, `scancel 6803105` would do this, as would `scancel 6803105_[1-3]`,
> where we indicate explicitly all the tasks in the range `1-3`.
> But we're able to cancel individual tasks as well using, for example, `scancel 6803205_1` for the first task.
> 
> Submit the array job again and use `scancel` to cancel only the second and third tasks.
> 
> > ## Solution
> > 
> > ~~~
> > [yourUsername@login7a [cosma7] ~]$ scancel 6803205_2
> > [yourUsername@login7a [cosma7] ~]$ scancel 6803205_3
> > ~~~
> > {: .language-bash}
> > 
> > Or:
> >
> > ~~~
> > [yourUsername@login7a [cosma7] ~]$ scancel 6803205_[2-3]
> > ~~~
> > {: .language-bash}
> >
> > As with cancelling a normal job, the `slurm-` output log files for tasks will still be produced containing any output
> > up until the point the tasks are cancelled.
> {: .solution}
{: .challenge}

## Interactive

We've seen that we can use the login nodes on an HPC resource to test our code (in a small way) before we submit it.
But sometimes when developing more complex code it would be useful to have access in some way to the compute nodes themselves,
particularly to explore or debug an issue.
For this we can use interactive jobs.

By reserving a compute note explicitly for this purpose,
an interactive job will grant us an interactive session on a compute node that meets our job requirements,
although of course, as with any job, this may not be granted immediately!
Then, once the interactive session is running, we are able to enter commands and have their output visible on our
terminal as if we had direct access to the compute node.

To submit a request for an interactive job where we wish to reserve a single node and two cores,
we can use Slurm's `srun` command:

~~~
[yourUsername@login7a [cosma7] ~]$ srun --account=yourAccount --partition=aPartition --nodes=1 --ntasks-per-node=2 --time=00:10:00 --pty /bin/bash
~~~
{: .language-bash}

So as well as the account/partition and the number of nodes and cores, 
we are requesting 10 minutes of interactive time (after which the interactive job will exit),
and that the job will run a Bash shell on the node which we'll use to interact with the job.

You should see the following, indicating the job is waiting, and then hopefully soon afterwards,
that the job has been allocated a suitable node:

~~~
srun: job 5608962 queued and waiting for resources
srun: job 5608962 has been allocated resources
FIXME: add prompt
~~~
{: .output}

At this point our interactive job is running our Bash shell remotely.
We can verify that we are on a compute node by entering `hostname`,
which will return the host name of the compute node on which the job is running.

At this point, we are able to use the `module`, `srun` and other commands
as they might appear within our job submission scripts:

~~~
FIXME prompt $ srun --ntasks=2 ./hello_world_mpi
~~~
{: .language-bash}

Hence, if we encounter issues

Importantly, note that whilst the terminal is active your allocation is consuming budget,
just as with a normal job, so be very aware to not leave an interactive session idle!
When you wish to exit your session use `exit`, or `Ctrl-D`.
You can check that your session is completed using `squeue -j` with the job ID as normal.

> ## Combined Power (with a Note of Caution)
> 
> For many job types it may also make sense to combine them together.
> As we've seen we're able to run MPI jobs on a compute node over an interactive session,
> and one very powerful approach to parallelism involves using both multi-threaded (OpenMP) *and* multi-process (MPI)
> at the same time (known as hybrid OpenMP/MPI).
> and it's very possible to also configure jobs to make use of job arrays with these approaches too.
> 
> As you may imagine, using multiple approaches offers tremendous flexibility and power to vastly scale what you are
> able to accomplish,
> although it's worth remembering that these also have the tradeoff of consuming more resources, and thus more budget,
> at a commensurate rate.
> When running applications (or developing applications to run) on HPC resources
> it's therefore strongly recommended to first start with small, simple
> jobs until you have high confidence their behaviour is correct.
{: .callout}
