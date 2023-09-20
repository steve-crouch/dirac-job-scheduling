---
title: "Job Submission and Management"
slug: "dirac-job-scheduling-job-management"
teaching: 0
exercises: 0
questions:
- "How do I request specific resources to use for a job?"
- "What is the life cycle of a job?"
- "What can I do to specify how my job will run?"
- "How can I find out the status of my running jobs?"
objectives:
- "First learning objective. (FIXME)"
keypoints:
- "First key point. Brief Answer to questions. (FIXME)"
---

## Selecting Resources for Jobs

### Determining Queues to which you have Access

Before we submit a job, we need to specify a queue to which it will be submitted.
You can find out the queues you have access rights for, and their state, using:

```bash
[yourUsername@login7a [cosma7] ~]$ sinfo -s
```

You should see something like the following, although this will vary for each DiRAC resource:

```
PARTITION         AVAIL  TIMELIMIT   NODES(A/I/O/T) NODELIST
cosma7               up 3-00:00:00     80/137/7/224 m[7229-7452]
cosma7-pauper        up 1-00:00:00     80/137/7/224 m[7229-7452]
cosma7-shm           up 3-00:00:00          0/1/0/1 mad02
cosma7-shm-pauper    up 1-00:00:00          0/1/0/1 mad02
cosma7-bench         up 1-00:00:00          0/0/1/1 m7452
cosma7-rp            up 3-00:00:00     82/140/2/224 m[7001-7224]
cosma7-rp-pauper*    up 1-00:00:00     82/140/2/224 m[7001-7224]
```

Here we can see the general availability of each of these queues (also known as partitions), as well as the maximum
time limit for jobs on each of these queues (in `days-hours:minutes:seconds` format).
For example, on the `cosma7` queue there is a 3-day limit, whilst the `cosma7-pauper` queue has a 1-day limit.

Of particular interest is the `NODES` column, which gives us an overview of the state of these resources, and hence
allows us to select those resources that are sufficiently available for jobs we'd like to submit.
It indicates how many nodes are:

- `A`ctive - these are running jobs
- `I`dle - no jobs are running
- `O`ther - these nodes are down, or otherwise unavailable
- The `T`otal number of nodes

The `NODELIST` is a summary of those nodes in a particular queue.

### Specifying Job Requirements

FIXME: --ntasks, --nodes, --tasks-per-node, --cpus-per-task, --exclusive




## Managing Job Submissions

### Monitoring a Job

While the job is waiting to run, it goes into a list of jobs called the _queue_.
To check on our job's status, we check the queue using the command `squeue`:

```
[yourUsername@login7a [cosma7] ~]$ squeue -u yourUsername
```
{: .language-bash}

You may find it looks like this:

```
  JOBID PARTITION     NAME     USER ST       TIME  NODES NODELIST(REASON)
5791510 cosma7-pa example- yourUser PD       0:00      1 (Priority)
```
{: .language-bash}

We can see all the details of our job, including the partition, user, and also the state of the job (in the `ST` column). In this case, we can see it is in the PD or PENDING state. Typically, a successful job run will go through the following states:

- `PD` - *pending*: sometimes our jobs might need to wait in a queue first before they can be allocated to a node to run
- `R` - *running*: job has an allocation and is currently running
- `CG` - *completing*: job is in the process of completing
- `CD` - *completed*: the job is completed

However, there are a number of reasons why jobs may end due to a failure or other condition, including:

- `OOM` - *ouf of memory*: the job attempted to use more memory during execution than what was available
- `S` - *suspended*: job has an allocation, but it has been suspended to allow for other jobs to use the resources
- `CA` - *cancelled*: the job was explicitly cancelled, either by the user or system administrator, and may or may not have been started
- `F` - *failed*: the job has terminated with a non-zero exit code or has failed for another reason

You can get a full list of job status codes via the [SLURM documentation](https://slurm.schedmd.com/squeue.html#lbAG).

> ## A Reminder: Where's the Output?
>
> On the login node, this script printed output to the terminal -- but
> now, when `squeue` shows the job has finished,
> nothing was printed to the terminal.
>
> Cluster job output is typically redirected to a file in the directory you
> launched it from. on DiRAC, for example, the output file looks like `slurm-<job_number>.out`,
> with `<job_number>` representing the unique identifier for the job.
> Use `ls` to find and `cat` to read the file.
{: .callout}

### Cancelling a Job

Sometimes we'll make a mistake and need to cancel a job. This can be done with
the `scancel` command. Let's submit a job and then cancel it using
its job number (remember to change the walltime so that it runs long enough for
you to cancel it before it is killed!).

```
[yourUsername@login7a [cosma7] ~]$ sbatch example-job.sh
[yourUsername@login7a [cosma7] ~]$ squeue -u yourUsername
```
{: .language-bash}

```
Submitted batch job 5791551

  JOBID PARTITION     NAME     USER ST       TIME  NODES NODELIST(REASON)
5791551 cosma7-pa hello-wo yourUser PD       0:00      1 (Priority)
```
{: .output}

Now cancel the job with its job number (printed in your terminal). A clean
return of your command prompt indicates that the request to cancel the job was
successful.

```
[yourUsername@login7a [cosma7] ~]$ scancel 5791551
# It might take a minute for the job to disappear from the queue...
[yourUsername@login7a [cosma7] ~]$ squeue -u yourUsername
```
{: .language-bash}

```
...(no output when there are no jobs to display)...
```
{: .output}

> ## Cancelling multiple jobs
>
> We can also cancel all of our jobs at once using the -u option. This will delete all jobs for a specific user (in this case, yourself). Note that you can only delete your own jobs.
>
> Try submitting multiple jobs and then cancelling them all.
>
> > ## Solution
> >
> > First, submit a trio of jobs:
> >
> > ```
> > [yourUsername@login7a [cosma7] ~]$ sbatch example-job.sh
> > [yourUsername@login7a [cosma7] ~]$ sbatch example-job.sh
> > [yourUsername@login7a [cosma7] ~]$ sbatch example-job.sh
> > ```
> > {: .language-bash}
> >
> > Then, cancel them all:
> >
> > ```
> > [yourUsername@login7a [cosma7] ~]$ scancel -u yourUsername
> > ```
> > {: .language-bash}
>{: .solution}
{: .challenge}

### Backfilling

When developing code or testing configurations it is usually the case that you don’t need a lot of time.
When that is true and the queues are busy backfilling can be really useful.

If there are idle nodes then that means they are available to run jobs, or that they are being kept so that a
job can run in the future. The time between when a job needs those nodes and now is the backfill window
and jobs that need less than that time will be scheduled to run.

You can check if a particular Slurm scheduler is configured to use a backfill technique by doing the following:

```
[yourUsername@login7a [cosma7] ~]$ scontrol show config | grep SchedulerType
```
{: .language-bash}

This use of the `scontrol` command allows us to see the configuration of Slurm, and by piping its output (using `|`) to
the `grep`command, we can search for a particular configuration field called `SchedulerType` which holds the method used
for scheduling. If a backfill scheduler is being used, you should see the following:

```
SchedulerType           = sched/backfill
```
{: .output}

If the `builtin` scheduler is used instead, then jobs will be executed in order of submission, regardless if there
are any free resources to run your job immediately.

> ## DiRAC COSMA: Handy Backfill Tools
> 
> On DiRAC's COSMA you are able to see backfill availability windows for various queues
> (e.g. for COSMA5, COSMA7,and COSMA8) by using a particular backfill command. For example, for COSMA5:
> 
> ```
> [yourUsername@login7a [cosma7] ~]$ c5backfill
> ```
> {: .language-bash}
> 
> ```
> # Backfill windows available for cosma queues:
> #
> No. hosts/slots:  1 / 16
> Runtime:          31 hours 49 minutes 26 seconds
> Hostnames:        m5336
> 
> No. hosts/slots:  5 / 80
> Runtime:          72 hours 00 minutes 00 seconds
> Hostnames:        m[5185,5315,5337,5366,5377]
> ```
> {: .output}
>
> So here, we can see that on COSMA5 we have only one node available for 31 hours and 49 minutes, and 5 nodes available
> for 72 hours.
{: .callout}

### Inspecting and Controlling the State of Jobs

So far we've used the `squeue` command to check the general status of pending and running jobs. But we can also obtain
even more information using the `scontrol` command regarding the job's status, configuration and use of resources.

Let's create a new job to test this, that uses a single node and a single CPU on that node to run a single basic task:

```
#!/usr/bin/env bash
#SBATCH --account=ds007
#SBATCH --partition=cosma7
#SBATCH --nodes=1
#SBATCH --tasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --ntasks=1
#SBATCH --time=00:00:30
#SBATCH --job-name=specific-job

sleep 25
echo "This script is running on ... "
hostname
```
{: .language-bash}

Next, launch our new job:

```
[yourUsername@login7a [cosma7] ~]$ sbatch specific-job.sh
```
{: .language-bash}

...then make a note of the job ID returned, e.g.:

```
...
Submitted batch job 6484453
```
{: .output}

We can then use this job ID to ask Slurm for more information about it:

```
[yourUsername@login7a [cosma7] ~]$ scontrol show jobid=6484453
```
{: .language-bash}

```
JobId=6484457 JobName=specific-job
   UserId=dc-crou1(21859) GroupId=ds007(20133) MCS_label=N/A
   Priority=100001 Nice=0 Account=ds007 QOS=normal
   JobState=RUNNING Reason=None Dependency=(null)
   Requeue=1 Restarts=0 BatchFlag=1 Reboot=0 ExitCode=0:0
   RunTime=00:00:08 TimeLimit=00:01:00 TimeMin=N/A
   SubmitTime=2023-09-20T10:52:13 EligibleTime=2023-09-20T10:52:13
   AccrueTime=2023-09-20T10:52:13
   StartTime=2023-09-20T10:52:14 EndTime=2023-09-20T10:53:14 Deadline=N/A
   SuspendTime=None SecsPreSuspend=0 LastSchedEval=2023-09-20T10:52:14 Scheduler=Main
   Partition=cosma7-pauper AllocNode:Sid=login7c:153560
   ReqNodeList=(null) ExcNodeList=(null)
   NodeList=m7401
   BatchHost=m7401
   NumNodes=1 NumCPUs=28 NumTasks=1 CPUs/Task=1 ReqB:S:C:T=0:0:*:*
   TRES=cpu=28,mem=510000M,node=1,billing=28
   Socks/Node=* NtasksPerN:B:S:C=0:0:*:* CoreSpec=*
   MinCPUsNode=1 MinMemoryNode=0 MinTmpDiskNode=0
   Features=(null) DelayBoot=00:00:00
   OverSubscribe=NO Contiguous=0 Licenses=(null) Network=(null)
   Command=/cosma/home/ds007/dc-crou1/example-job.sh
   WorkDir=/cosma/home/ds007/dc-crou1
   StdErr=/cosma/home/ds007/dc-crou1/slurm-6484457.out
   StdIn=/dev/null
   StdOut=/cosma/home/ds007/dc-crou1/slurm-6484457.out
   Power=
```
{: .output}

In particular, we can see:

- As we might expect the `JobState` is `RUNNING`, although it may be `PENDING` if waiting to be assigned to a node, or if we weren't fast enough running the `scontrol` command it might be `COMPLETED`
- The job's `SubmitTime`, as well as the job's `StartTime` for execution: this may be the actual start time, or the expected start time if set in the future. The expected `EndTime` is also specified, although if it isn't specified directly in the job script this isn't always exactly `StartTime` + specified duration; it's often rounded up, perhaps to the nearest minute.
- The queue assigned for the job is the `cosma7-pauper` queue, and that the job is running on the `m7401` node
- The resources assigned to the job are a single node (`NumNodes=1`) with 28 CPU cores, for a single task with 1 CPU core per task. Pretty much as we would expect given what we requested in our job script, although note that we may get more resources than what we asked for. For example in this instance, we can see that we actually obtained a node with 28 CPUs (although we won't use them)
- We didn't specify a working directory within which to execute the job, so the default `WorkDir` is our home directory
- The error and output file locations, as specified by `StdErr` and `StdOut`

You can find more information on `scontrol` and the various fields in the [Slurm documentation](https://slurm.schedmd.com/scontrol.html).

Note that typing `scontrol` on its own will enter an interactive state with an `scontrol` prompt which allows
you to enter subcommands (like `show` or `update`) directly to the `scontrol` command. You can exit by typing `quit`.

## Querying Job Resources

FIXME: sstat - which resources is a job using?
FIXME: sacct - same, but for completed jobs
