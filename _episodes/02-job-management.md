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

## Recap: Basics of Job Submission

FIXME: very basic sbatch, script files (with account and partition) and squeue

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

### Specifying Job Resource Requirements

FIXME: --ntasks, --nodes, --tasks-per-node, --cpus-per-task, --exclusive

One thing that is absolutely critical when working on an HPC system
is specifying the resources required to run a job, which allows the scheduler to
find the right time and place to schedule our job. If you do not specify
requirements (such as the amount of time you need), you will likely be stuck
with your site's default resources, which is probably not what you want.

When launching a job, we can specify the resources our job needs in our job script,
using `#SBATCH <parameter>`. We've used this format before when indicating the account and queue to use.
You may have seen some of these parameters before, but let's take a look at the most important
ones and how they relate to each other.

For these parameters, by default a *task* refers to a single CPU unless otherwise indicated.

- `--nodes` - the total number of machines or nodes to request
- `--ntasks` - the total number of CPU cores (across requested nodes) your job needs. Generally, this will be 1 unless you're running MPI jobs which are
able to use multiple CPU cores. In which case, it essentially specifies the number of MPI ranks to start across the nodes. For example, if `--nodes=4` and `--ntasks=8`, we're requesting 4 nodes each with 2 CPUs
- `--exclusive` - this indicates we'd like exclusive access to the nodes we request, such that they are shared with no other jobs, regardless of how many CPUs we actually need. If we are running jobs that require particularly large amounts of memory, CPUs, or disk access, this may be important.

> ## Being Specific
>
> Write and submit a job script - perhaps adapted from our previous one - that requests a total of 3 nodes with 2 CPUs
> per node.
>
> > ## Solution
> >
> > ```
> > #!/bin/bash -l
> > #SBATCH --account=ds007
> > #SBATCH --partition=cosma7
> > #SBATCH --nodes=3
> > #SBATCH --ntasks=6
> > #SBATCH --time=00:00:30
> >
> > echo "This script is running on ... "
> > hostname
> > ```
> > {: .language-bash}
> > 
> > ```
> > [yourUsername@login7a [cosma7] ~]$ sbatch multi-node-job.sh
> > ...
> > [yourUsername@login7a [cosma7] ~]$ squeue -U yourUsername
> > ```
> > {: .language-bash}
> >
> > ```
> > JOBID PARTITION     NAME     USER ST       TIME  NODES NODELIST(REASON)
> > 6485195 cosma7-pa mul-job. dc-crou1  R       0:05      3 m[7400-7402]
> > ```
> > {: .output}
> > 
> > Here we can see that the job is using a total of three nodes as we'd like - `m7400`, `m7401` and `m7402`.
>{: .solution}
{: .challenge}

We can also specify the number of CPUs to request per node explicitly using `--ntasks-per-node`. In this case, we
use this with `--nodes` and we don't need to specify `--ntasks` at all. So using our above example with `--nodes=4`,
to get our desired total 8 CPUs we'd specify `--ntasks-per-node=2`.

> ## Being Even More Specific
>
> Write and submit a job script that uses `--nodes` and `--ntasks-per-node` to request a total of 3 nodes with 2 CPUs
> per node.
>
> > ## Solution
> >
> > ```
> > #!/bin/bash -l
> > #SBATCH --account=ds007
> > #SBATCH --partition=cosma7
> > #SBATCH --nodes=3
> > #SBATCH --ntasks-per-node=2
> > #SBATCH --time=00:00:30
> >
> > echo "This script is running on ... "
> > hostname
> > ```
> > {: .language-bash}
> > 
> > Once submitted, using `squeue` we should the same results as before, again using a total of three nodes.
>{: .solution}
{: .challenge}

We'll be looking at how we can make full use of these parameters with MPI jobs later in this lesson.

Up until now, the number of tasks have been synonymous with the number of CPUs. However, we can also specify a multiple
number of CPUs per task too, by using `--cpus-per-task` - so if your job is multithreaded, for example it makes use of
OpenMP, you can specify how many  threads it needs using this parameter.

> ## What about Parameters Named -A, -N, -J and others?
> 
> You may also see some other shorter parameter names. In Slurm, some `SBATCH` options also have shortened forms, but
> mean the same thing. For example:
> 
> |**Long form**       | **Short form**     |
> |--------------------|--------------------|
> |`--job-name`        | `-J`               |
> |`--nodes`           | `-N`               |
> |`--ntasks`          | `-n`               |
> |`--cpus-per-task`   | `-c`               |
> 
> Not all parameters have a short form, for example `--nodes-per-task` and `--exclusive`.
>
> In practice you can use either, although using the longer form is more verbose and helps remove ambiguity,
> particularly for newer users.
{: .callout}

> ## Only Request What you Need
> 
> When specifying resources your job will need it's important not to ask for too much.
> Firstly, because any resources you request but don't use (e.g. CPUs, memory, GPUs) will be wasted
> and potentially cost more in terms of your account usage, but also because requesting larger resources
> will take longer to queue. It also means that other user's jobs that would would have been a better fit for these
> resources may also take longer to run. It's good to remember that we are part of a wider community of HPC users, and
> as with any shared resource, we should act responsibly when using it.
> 
> As we'll see later, using the `sacct` command we're able to find out what resources previous jobs actually used, and
> then optimise the resources we request in future job runs.
{: .callout}

## Managing Job Submissions

### Monitoring a Job

As we've seen, we can check on our job's status by using the command `squeue`. Let's take a look in more detail.

```
[yourUsername@login7a [cosma7] ~]$ squeue -u yourUsername
```
{: .language-bash}

You may find it looks like this:

```
  JOBID PARTITION     NAME     USER ST       TIME  NODES NODELIST(REASON)
5791510 cosma7-pa example- yourUser PD       0:00      1 (Priority)
```
{: .output}

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
- How long the job has run for (`RunTime`), and the job's maximum specified duration (`TimeLimit`)
- The job's `SubmitTime`, as well as the job's `StartTime` for execution: this may be the actual start time, or the expected start time if set in the future. The expected `EndTime` is also specified, although if it isn't specified directly in the job script this isn't always exactly `StartTime` + specified duration; it's often rounded up, perhaps to the nearest minute.
- The queue assigned for the job is the `cosma7-pauper` queue, and that the job is running on the `m7401` node
- The resources assigned to the job are a single node (`NumNodes=1`) with 28 CPU cores, for a single task with 1 CPU core per task. Pretty much as we would expect given what we requested in our job script, although note that we may get more resources than what we asked for. For example in this instance, we can see that we actually obtained a node with 28 CPUs (although we won't use them)
- We didn't specify a working directory within which to execute the job, so the default `WorkDir` is our home directory
- The error and output file locations, as specified by `StdErr` and `StdOut`

You can find more information on `scontrol` and the various fields in the [Slurm documentation](https://slurm.schedmd.com/scontrol.html).

Note that typing `scontrol` on its own will enter an interactive state with an `scontrol` prompt which allows
you to enter subcommands (like `show` or `update`) directly to the `scontrol` command. You can exit by typing `quit`.

> ## Let's Wait a Bit...
> 
> It may be useful to defer the scheduling of a job until some point in the future.
> We can specify a delayed start time (or even delayed start date) by using the `--begin` parameter in our job script.
> For example, adding `#SBATCH --begin=16:00` will delay starting the job until 16:00 (assuming there is an appropriate
> node available at that time). We can also specify a relative delay time, for example passing `now+1hour` instead.
> 
> Launch the job again, but this time specify a start time of 1 minute in the future in the job script. How does the
> `scontrol` output change for a delayed job?
> 
> > ## Solution
> >
> > First add `#SBATCH --begin=now+1minute` to the parameters in the job script, and relaunch using `sbatch`.
> >
> > You should see in the job's `scontrol` output:
> >
> > - `JobState=PENDING`, `RunTime=00:00:00`, and `AccrueTime` is `Unknown`, since it's waiting to run and no runtime has yet elapsed
> > - `EligibleTime` will specify the calculated start time one minute into the future
>{: .solution}
{: .challenge}

## Querying Job Resources

FIXME: sstat - which resources is a job using?
FIXME: sacct - same, but for completed jobs
