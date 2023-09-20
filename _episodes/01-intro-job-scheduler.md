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
- "First learning objective. (FIXME)"
keypoints:
- "First key point. Brief Answer to questions. (FIXME)"
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

The scheduler used here is Slurm. Although
Slurm is not used everywhere, running jobs is quite similar
regardless of what software is being used. The exact syntax might change, but
the concepts remain the same.

### Basic Slurm Commands



## DiRAC Project Allocations

## Understanding Queues

FIXME: properties, accountancy/quotas, priority system, prince & pauper queues


