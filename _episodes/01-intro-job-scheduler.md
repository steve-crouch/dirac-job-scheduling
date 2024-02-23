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
- "Summarise the main ways researchers may request time on a DiRAC facility."
keypoints:
- "A job scheduler ensures jobs are given the resources they need, and manages when and where jobs will run on an HPC resource."
- "We use `sbatch` and `squeue` to submit jobs and query their status."
- "Access to DiRAC's resources is managed through the STFC's independent Resource Allocation Committee (RAC)."
- "Facility time may be requested through a number of mechanisms, namely in response to a Call for Proposals, the Director's Discretionary Award, and Seedcorn Time."
- "The [DiRAC website](https://dirac.ac.uk/getting-access/) has further information on the methods to request access, as well as application forms."
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
#!/usr/bin/env bash
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

[Access to DiRAC's resources](https://dirac.ac.uk/getting-access/) is managed through the STFC's independent Resource Allocation Committee (RAC),
which provides access through allocations to researchers who request time on the facility.
There are a number of mechanisms through which facility time can be requested:

- **Call for full Proposals**: the RAC puts out an annual invitation to the UK theory and modelling communities to apply for computational resources on the DiRAC HPC Facility, with applications taking the form of scientific, technical, or Research Software Engineer support (RSE) time.
- **Director's Discretionary Award**: from time to time the DiRAC Director invites the UK theory and modelling communities to apply for discretionary allocations of computational resources. Discretionary time can also be applied for if you find you will be using your code at a larger scale than was previously requested in a full call for proposals. Applications can be made at any time.
- **Seedcorn Time**: for researchers who would like to get a feel for HPC, test and benchmark codes, or see what  DiRAC resources can do for you before making a full application, an application can be made for seedcorn time. Existing users may also apply for seedcorn allocations to enable code development/testing on a service that is not currently part of their project allocation. You can apply for Seedcorn Time at any time.

For more information regarding these options,
and for online application forms and contact details for enquiries, see the [DiRAC website](https://dirac.ac.uk/getting-access/).

Once the submission and its technical case has been approved,
allocations are managed within time is allocated in 3-month chunks over the duration of the project,
which may be over a period of years.
Allocation usage is based primarily on core CPU hours or GPU hours.
Following a 3-month allocation project usage is reviewed and extended based on that review
(for instance, any non-use of the allocation during that 3-month window is queried,
with support provided to overcome any barriers to use.)
In addition, Research Software Engineering (RSE) support time may also be requested,
who can provide help with code optimisation, porting, re-factoring and performance analysis.


## Understanding Queues

FIXME: properties, accountancy/quotas, priority system, prince & pauper queues


