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

``` bash
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

FIXME: squeue, scancel, backfilling, scontrol

### Monitoring a Job

While the job is waiting to run, it goes into a list of jobs called the _queue_.
To check on our job's status, we check the queue using the command `squeue`:

``` bash
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

## Cancelling a Job

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

## Backfilling

When testing configurations for running existing code, or 

## Querying Job Resources

FIXME: sstat - which resources is a job using?
FIXME: sacct - same, but for completed jobs
