---
title: "Accessing System Resources using Modules"
slug: "dirac-job-scheduling-using-modules"
teaching: 0
exercises: 0
questions:
- "How can I make use of HPC system resources such as compilers, libraries, and other tools?"
- "What are HPC modules, and how do I use them?"
objectives:
- "Load and use a software package."
- "Explain how the shell environment changes when the module mechanism loads or unloads packages."
- "Use modules in a job script."
keypoints:
- "First key point. Brief Answer to questions. (FIXME)"
---

On a high-performance computing system, it is seldom the case that the software
we want to use is available when we log in. It is installed, but we will need
to "load" it before it can run.

Before we start using individual software packages, however, we should
understand the reasoning behind this approach. The three biggest factors are:

- software incompatibilities
- versioning
- dependencies

Software incompatibility is a major headache for programmers. Sometimes the
presence (or absence) of a software package will break others that depend on
it. Two of the most famous examples are Python 2 and 3 and C compiler versions.
Python 3 famously provides a `python` command that conflicts with that provided
by Python 2. Software compiled against a newer version of the C libraries and
then used when they are not present will result in a nasty `'GLIBCXX_3.4.20'
not found` error, for instance.

Software versioning is another common issue. A team might depend on a certain
package version for their research project - if the software version was to
change (for instance, if a package was updated), it might affect their results.
Having access to multiple software versions allow a set of researchers to
prevent software versioning issues from affecting their results.

Dependencies are where a particular software package (or even a particular
version) depends on having access to another software package (or even a
particular version of another software package). For example, the VASP
materials science software may depend on having a particular version of the
FFTW (Fastest Fourier Transform in the West) software library available for it
to work.

## Environment Modules

Environment modules are the solution to these problems. A _module_ is a
self-contained description of a software package -- it contains the
settings required to run a software package and, usually, encodes required
dependencies on other software packages.

There are a number of different environment module implementations commonly
used on HPC systems: the two most common are _TCL modules_ and _Lmod_. Both of
these use similar syntax and the concepts are the same so learning to use one
will allow you to use whichever is installed on the system you are using. In
both implementations the `module` command is used to interact with environment
modules. An additional subcommand is usually added to the command to specify
what you want to do. For a list of subcommands you can use `module -h` or
`module help`. As for all commands, you can access the full help on the _man_
pages with `man module`.

On login you may start out with a default set of modules loaded or you may
start out with an empty environment; this depends on the setup of the system
you are using.

### Listing Available Modules

To see available software modules, use `module avail`:

```
[yourUsername@login7a [cosma7] ~]$ module avail
```
{: .language-bash}

```
--------------------- /cosma/local/Modules/modulefiles/mpi ---------------------
hpcx-mt/2.2             intel_mpi/2020            openmpi/4.0.3               
intel_mpi/2017          intel_mpi/2020-update1    openmpi/4.0.5               
intel_mpi/2018          intel_mpi/2020-update2    openmpi/4.1.1               
intel_mpi/2019          mvapich2_mpi/2.3.6        openmpi/4.1.1.no-ucx        
intel_mpi/2019-update1  mvapich2_mpi/2.3.6-debug  openmpi/4.1.4               
intel_mpi/2019-update2  mvapich2_mpi/2.3.7-1      openmpi/4.1.4-romio-lustre  
intel_mpi/2019-update3  openmpi/3.0.1(default)    openmpi/20190429            
intel_mpi/2019-update4  openmpi/4.0.1             rockport-settings           

------------------ /cosma/local/Modules/modulefiles/compilers ------------------
aocc/1.3.0                intel_comp/2019-update2            
aocc/2.0.0                intel_comp/2019-update3            
aocc/2.2.0                intel_comp/2019-update4            
...
```
{: .output}

### Listing Currently Loaded Modules

You can use the `module list` command to see which modules you currently have
loaded in your environment. If you have no modules loaded, you will see a
message telling you so

```
[yourUsername@login7a [cosma7] ~]$ module list
```
{: .language-bash}

Depending on your system, you may find something like the following:

```
Currently Loaded Modulefiles:
 1) cosma/2018               3) armforge/22.0.2   5) gadgetviewer/1.1.3  
 2) python/2.7.15(default)   4) hdfview/3.1.4     6) utils/201805      
```
{: .output}

Or alternatively, you may find it returns with `No Modulefiles Currently Loaded.`

## Loading and Unloading Software

To load a software module, use `module load`. In this example we will use
Python 3.

Initially, Python 3 is not loaded. We can test this by using the `which`
command. `which` looks for programs the same way that Bash does, so we can use
it to tell us where a particular piece of software is stored.

```
[yourUsername@login7a [cosma7] ~]$ which python3
```
{: .language-bash}

FIXME: /modules/missing-python.snip

We can load the `python3` command with `module load`:

FIXME: /modules/module-load-python.snip

FIXME: /modules/python-executable-dir.snip

So, what just happened?

To understand the output, first we need to understand the nature of the `$PATH`
environment variable. `$PATH` is a special environment variable that controls
where a UNIX system looks for software. Specifically `$PATH` is a list of
directories (separated by `:`) that the OS searches through for a command
before giving up and telling us it can't find it. As with all environment
variables we can print it out using `echo`.

```
[yourUsername@login7a [cosma7] ~]$ echo $PATH
```
{: .language-bash}

FIXME: /modules/python-module-path.snip

You'll notice a similarity to the output of the `which` command. In this case,
there's only one difference: the different directory at the beginning. When we
ran the `module load` command, it added a directory to the beginning of our
`$PATH`. Let's examine what's there:

FIXME: /modules/python-ls-dir-command.snip

FIXME: /modules/python-ls-dir-output.snip

Taking this to its conclusion, `module load` will add software to your `$PATH`.
It "loads" software. A special note on this - depending on which version of the
`module` program that is installed at your site, `module load` will also load
required software dependencies.

FIXME: /modules/software-dependencies.snip

Note that this module loading process happens principally through
the manipulation of environment variables like `$PATH`. There
is usually little or no data transfer involved.

The module loading process manipulates other special environment
variables as well, including variables that influence where the
system looks for software libraries, and sometimes variables which
tell commercial software packages where to find license servers.

The module command also restores these shell environment variables
to their previous state when a module is unloaded.

## Software Versioning

So far, we've learned how to load and unload software packages. This is very
useful. However, we have not yet addressed the issue of software versioning. At
some point or other, you will run into issues where only one particular version
of some software will be suitable. Perhaps a key bugfix only happened in a
certain version, or version X broke compatibility with a file format you use.
In either of these example cases, it helps to be very specific about what
software is loaded.

Let's examine the output of `module avail` more closely.

```
[yourUsername@login7a [cosma7] ~]$ module avail
```
{: .language-bash}

FIXME: /modules/available-modules.snip

FIXME: /modules/wrong-gcc-version.snip

> ## Using Software Modules in Scripts
>
> Create a job that is able to run `python3 --version`. Remember, no software
> is loaded by default! Running a job is just like logging on to the system
> (you should not assume a module loaded on the login node is loaded on a
> compute node).
>
> > ## Solution
> >
> > ```
> > [yourUsername@login7a [cosma7] ~]$ nano python-module.sh
> > [yourUsername@login7a [cosma7] ~]$ cat python-module.sh
> > ```
> > {: .language-bash}
> >
> > ```
> > {{ site.remote.bash_shebang }}
> >
> > module load python/3.10.1
> >
> > python3 --version
> > ```
> > {: .output}
> >
> > ```
> > [yourUsername@login7a [cosma7] ~]$ sbatch python-module.sh
> > ```
> > {: .language-bash}
> {: .solution}
{: .challenge}
