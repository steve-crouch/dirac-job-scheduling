---
title: "Introduction to HPC Job Scheduling"
slug: "dirac-job-scheduling-intro-job-scheduling"
teaching: 0
exercises: 0
questions:
- "What is a job scheduler and why does a cluster need one?"
- "What are DiRAC project allocations and how do they work?"
- "What are job queues?"
- "Which job queues typically exist across DiRAC and how do they differ?"
- "How are jobs prioritised?"
objectives:
- "Describe briefly what a job scheduler does."
- "Recap the fundamentals of Slurm job submission and monitoring."
keypoints:
- "A job scheduler ensures jobs are given the resources they need, and manages when and where jobs will run on an HPC resource."
- "We use `sbatch` and `squeue` to submit jobs and query their status."
---


## Job Scheduling: A Recap

An HPC system might have thousands of nodes and thousands of users. How do we
decide who gets what and when? How do we ensure that a task is run with the
resources it needs? This job is handled by a special piece of software called
the _scheduler_. On an HPC system, the scheduler manages which jobs run where
and when.

The following illustration compares these tasks of a job scheduler to a waiter
in a restaurant. If you can relate to an instance where you had to wait for a
while in a queue to get in to a popular restaurant, then you may now understand
why sometimes your job does not start instantly as on your laptop.

![Compare a job scheduler to a waiter in a restaurant]({{ page.root }}{% link fig/restaurant_queue_manager.svg %}){: width="650px"}

The scheduler used here is Slurm, and although
Slurm is not used everywhere, running jobs is quite similar
regardless of what software is being used. The exact syntax might change, but
the concepts remain the same.

### Fundamentals of Job Submission and Monitoring: A Recap

You may recall from a previous course, or your own experience, two basic Slurm commands;
namely `sbatch` to submit a job to an HPC resource, and `squeue` to query the state of a submitted job.
When we submit a job, we typically write a Slurm script which embodies the commands we wish to run on a compute node.
A job script (e.g. `basic-script.sh`) is typically written using the
[Bash shell](https://www.gnu.org/software/bash/manual/bash.html) language and looks something like this:

~~~
#!/bin/bash -l
#SBATCH --account=yourAccount
#SBATCH --partition=aPartition
#SBATCH --time=00:00:30

date
~~~
{: .language-bash}

The `#SBATCH` lines are special comments that provide additional information about our job to Slurm;
for example the account and partition (which we'll look at a bit later),
and the maximum time we expect the job to take when running.
The information provided here represents a bare minimum we need to provide to run a job,
and as we'll see, we can provide a lot more information to Slurm to support the running of many different types of jobs.

If we now replace `yourAccount` and `aPartition` with suitable values,
then save and submit that script using `sbatch basic-script.sh`,
we have a job identifier returned which we may use to query the status
of our job until it's completed using `squeue <jobid>`.
Once complete, we are able to read the job's log file (or files), typically held in `slurm-<jobid>.out`,
which show us any printed output from a job,
and depending on the HPC system, other information regarding how and where the job ran.

## DiRAC Project Allocations



## Understanding Queues

FIXME: properties, accountancy/quotas, priority system, prince & pauper queues


