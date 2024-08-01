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
- "HPC systems use a module loading/unloading system to provide access to software."
- "To see the available modules on a system, we use `module avail`."
- "The software installed across the DiRAC sites can be different in terms of what's installed and the versions that are available."
- "`module list` will show us which modules we currently have loaded."
- "We use `module load` and `module unload` to grant and remove access to modules on the system."
- "We should only keep loaded those modules we actively wish to use, and try to avoid loading multiple versions of the same software."
---

On a high-performance computing system, it is seldom the case that the software
we want to use - things like compilers and libraries - is available when we log in.
It is installed, but we will need to "load" it before it can run.

Before we start using individual software packages, however, we should
understand the reasoning behind this approach. The three biggest factors are
*software incompatibilities*, *versioning*, and *dependencies*.

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

To see available software modules, use `module avail`.

~~~
module avail
~~~
{: .language-bash}

On COSMA, it looks something like the following, although your site output will differ:

~~~
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
~~~
{: .output}

> ## What About Partial Matches?
> 
> A useful feature of `module avail` is that it also works on partial matches that begin with a given argument.
> For example, `module avail x` would display a shortened list of any modules beginning with `x`.
> This is handy if you need to search for a particular module but can't remember the full name,
> or would like a succinct list of all versions of a particular module.
> 
> Using `module avail`, how many versions of `openmpi` are on your HPC system?
> 
> > ## Solution
> >
> > Typing `module avail openmpi` on DiRAC's COSMA HPC resource, at the time of writing we get:
> > 
> > ~~~
> > --------------------- /cosma/local/Modules/modulefiles/mpi ---------------------
> > openmpi/3.0.1(default)  openmpi/4.0.5         openmpi/4.1.4               
> > openmpi/4.0.1           openmpi/4.1.1         openmpi/4.1.4-romio-lustre  
> > openmpi/4.0.3           openmpi/4.1.1.no-ucx  openmpi/20190429   
> > ~~~
> > {: .output}
> > 
> > So, a total of 9 module versions of `openmpi`. On Tursa:
> >
> > ~~~
> > --------------------------------------- /home/y07/shared/tursa-modules ---------------------------------------
> > openmpi/4.1.5
> > ------------------------------ /mnt/lustre/tursafs1/apps/cuda-12.3-modulefiles -------------------------------
> > openmpi/4.1.5-cuda12.3  
> > 
> > ----------------------------- /mnt/lustre/tursafs1/apps/cuda-11.4.1-modulefiles ------------------------------
> > openmpi/4.1.1-cuda11.4.1  
> > 
> > ----------------------------------- /mnt/lustre/tursafs1/apps/modulefiles ------------------------------------
> > openmpi/4.0.4  openmpi/4.1.1 
> > ~~~
> > {: .output}
> > 
> > On DiAL3:
> > ~~~
> > ------------------------------------------- /cm/shared/modulefiles -------------------------------------------
> >    openmpi4/intel/4.0.5
> > ~~~
> > {: .output}
> > 
> > And on CSD3, we have something like:
> > ~~~
> > -------------------------------------- /usr/local/software/modulefiles ---------------------------------------
> > openmpi/3.1.4-gcc-7.2.0  
> > 
> > ----------------------------------- /usr/local/Cluster-Config/modulefiles ------------------------------------
> > openmpi-GDR/gnu/1.10.7_cuda-8.0  openmpi/gcc/9.2/4.0.1  openmpi/gcc/9.3/4.0.4  
> > openmpi-GDR/gnu/2.1.1_cuda-8.0   openmpi/gcc/9.2/4.0.2  openmpi/pgi/3.0.0     
> > ~~~
> > {: .output}
>{: .solution}
{: .challenge}


### Listing Currently Loaded Modules

You can use the `module list` command to see which modules you currently have
loaded in your environment. If you have no modules loaded, you will see a
message telling you so

~~~
module list
~~~
{: .language-bash}

Depending on your system, you may find something like the following:

~~~
Currently Loaded Modulefiles:
 1) cosma/2018               3) armforge/22.0.2   5) gadgetviewer/1.1.3  
 2) python/2.7.15(default)   4) hdfview/3.1.4     6) utils/201805      
~~~
{: .output}

Depending on your site, you may find it returns with much a shorter list, or perhaps `No Modulefiles Currently Loaded.`

> ## More or Less Information?
> 
> Using the `-l` switch with `module list` will give you more information about those modules loaded;
> namely, any additional version information for each module loaded and the last date/time the module was
> modified on the system.
> Conversely, using the `-t` switch will give you the output in a terse format, as a simple list of modules
> one per line.
> 
> These switches also work with `avail`.
> Using the `-l` switch with this command,
> determine the date a particular version of a module (such as openmpi or Python) was modified.
> 
> > ## Solution
> > 
> > For example, using `module avail -l openmpi/4.1.4` on COSMA at time of writing, we get:
> >
> > ~~~
> > - Package/Alias -----------------------.- Versions --------.- Last mod. -------
> > /cosma/local/Modules/modulefiles/mpi:
> > openmpi/4.1.4                                               2022/11/28 11:11:31
> > openmpi/4.1.4-romio-lustre                                  2022/09/14 10:48:34
> > ~~~
> > {: .output}
> >
> > Using `module avail -l openmpi/pgi/3.0.0` on CSD3, we get:
> >
> > ~~~
> > - Package/Alias -----------------------.- Versions --------.- Last mod. -------
> > /usr/local/Cluster-Config/modulefiles:
> > openmpi/pgi/3.0.0                                           2018/05/17 14:25:11
> > ~~~
> > {: .output}
>{: .solution}
{: .challenge}

## Loading and Unloading Software

To gain or remove access to the typically numerous software modules we have available to us on an HPC system,
we *load* or *unload* them.

### Loading Software

To load a software module, we use `module load`.

Whilst the DiRAC sites have some modules in common,
there are many differences in what software modules are available and not all modules are available
on all sites.
So in this example, for simplicity whilst investigating module loading,
we'll load a different module depending on your site (so make a note of it!):

- Durham COSMA: `julia`
- Edinburgh Tursa: `cmake`
- Leicester DiAL3: `ffmpeg`
- Cambridge CSD3: `bison`

We won't use or investigate any of the packages in any detail, but merely use them to demonstrate the use of modules.
They're handy for training purposes, since the module names equate to the commands used to run them.
Note that some of these commands are actually available on multiple sites across DiRAC.

Initially, our module is not loaded. We can test this by using the `which`
command.
`which` looks for programs the same way that Bash does, so we can use
it to tell us where a particular piece of software is stored.
So on DiAL3, we could do the following:

~~~
which ffmpeg
~~~
{: .language-bash}

On your own site, substitute `ffmpeg` with the module above for your site.

You'll likely get something like the following, complaining that it can't find the command within our
environment:

~~~
/usr/bin/which: no ffmpeg in (/cm/local/apps/lua/5.4.0/bin:/home/dc-crou1/.local/bin:/home/dc-crou1/bin:/cm/shared/apps/hwloc/1.11.11/bin:/usr/local/bin:/usr/bin:/usr/local/sbin:/usr/sbin)
~~~
{: .output}

So we can now try to load our module with `module load`,
so for DiAL3, for example:

~~~
module load ffmpeg
which ffmpeg
~~~
{: .language-bash}

Which now shows us, in the case of DiAL3 and `ffmpeg`:

~~~
/cm/shared/apps/ffmpeg/5.0.1/bin/ffmpeg
~~~
{: .output}

> ## Why Not Specify the Version of the Module?
> 
> Note here we aren't specifying the precise version of the module that we want for simplicity here.
> However, feel free to use `module avail <module_name>` to determine the versions available on your HPC system
> and then load a specific version if you wish, e.g. `module load julia/1.9.1`
> 
> At some point or other, you will run into issues where only one particular version of some software will be suitable.
> Perhaps a key bugfix only happened in a certain version, or version X broke compatibility with a file format you use.
> In either of these example cases, it helps to be very specific about what software is loaded.
{: .callout}

So, what just happened?

To understand the output, first we need to understand the nature of the `$PATH`
environment variable. `$PATH` is a special environment variable that controls
where a UNIX system looks for software. Specifically `$PATH` is a list of
directories (separated by `:`) that the OS searches through for a command
before giving up and telling us it can't find it. As with all environment
variables we can print it out using `echo`.

~~~
echo $PATH
~~~
{: .language-bash}

On COSMA (with the Julia module loaded) this looks like:

~~~
/cosma/local/julia/1.9.1:/cosma/local/matlab/R2020b/bin:/cosma/local/gadgetviewer/1.1.4/bin:/cosma/local/hdfview/HDFView/3.1.4/bin:/cosma/local/arm/forge/22.0.2/bin:/cosma/local/Python/2.7.15/bin:/cosma/local/bin:/usr/lib64/qt-3.3/bin:/cosma/local/Modules/default/bin:/usr/local/bin:/usr/bin:/usr/local/sbin:/usr/sbin
~~~
{: .output}

You'll notice a similarity to the output of the `which` command. In this case,
there's only one difference: the different directory at the beginning. When we
ran the `module load` command, it added a directory to the beginning of our
`$PATH`.
Let's examine what's there (your particular path will differ depending on your site and the command):

~~~
ls /cosma/local/julia/1.9.1
~~~
{: .language-bash}

So for Julia's directory location on COSMA, it looks like:

~~~
base          contrib          etc              LICENSE.md  README.md    test           VERSION
CITATION.bib  CONTRIBUTING.md  HISTORY.md       Makefile    src          THIRDPARTY.md
CITATION.cff  deps             julia            Make.inc    stdlib       usr
cli           doc              julia.spdx.json  NEWS.md     sysimage.mk  usr-staging
~~~
{: .output}

Taking this to its conclusion, `module load` will therefore add software to your `$PATH`,
which is what is meant by *loading* software: we are essentially changing our command line environment
so we are able to make use of the software.

> ## What About Loading Dependencies?
> 
> A special note on this - depending on which version of the
`module` program that is installed at your site, `module load` may also load
required software dependencies as well,
> or make specific mention that other modules need to be loaded beforehand.
>
> To demonstrate, on DiRAC's COSMA resource, let's assume we want to load a particular version of OpenMPI:
> 
> ~~~
> module load openmpi/4.1.4
> ~~~
> {: .language-bash}
> 
> In this case, at the time of writing we get the following:
> 
> ~~~
> A compiler must be chosen before loading the openmpi module.
>  Please load one of the following compiler modules:
>  
>          aocc_comp/4.0.0
>          gnu_comp/11.1.0
>          gnu_comp/13.1.0
>          gnu_comp/9.3.0
>          intel_comp/2022.1.2
>          intel_comp/2022.3.0
> ~~~
> {: .output}
> 
> So here, we need to explicitly load one of these compiler options before we are able to load OpenMPI.
> e.g. `module load gnu_comp/13.1.0`.
> Depending on your system and how it's configured, your mileage will differ!
{: .callout}

### How Loading Affects the Environment

Note that this module loading process happens principally through
the manipulation of environment variables like `$PATH`. There
is usually little or no data transfer involved.

The module loading process manipulates other special environment
variables as well, including variables that influence where the
system looks for software libraries, and sometimes variables which
tell commercial software packages where to find license servers.

The module command also restores these shell environment variables
to their previous state when a module is unloaded.

If we need such detail, we are able to see the changes that would be made to our environment using `module display`.
For example, on Tursa with `cmake`: 

~~~
module display cmake
~~~
{:. language-bash}

~~~
-------------------------------------------------------------------
/home/y07/shared/tursa-modules/cmake/3.27.4:

conflict        cmake
prepend-path    PATH /home/y07/shared/utils/core/cmake/3.27.4/bin
prepend-path    CPATH /home/y07/shared/utils/core/cmake/3.27.4/include
prepend-path    LD_LIBRARY_PATH /home/y07/shared/utils/core/cmake/3.27.4/lib
prepend-path    MANPATH /home/y07/shared/utils/core/cmake/3.27.4/man
-------------------------------------------------------------------
~~~
{: .output}

So here, we can see that loading version 3.27.4 of `cmake` will add `/home/y07/shared/utils/core/cmake/3.27.4/bin` to the start of our path.
We can also see that it adds `/home/y07/shared/utils/core/cmake/3.27.4/man` to a variable called `$MANPATH`,
which is a specific path that contains locations of additional software manual pages we can access.
Once `cmake` is loaded, we are thus able to then use `man cmake` to access its manual page,
which is really useful in general for seeing information about commands, their parameters, and how to use them.

> ## Loading Multiple Versions of the Same Module?
> 
> You may ask so what if we load multiple versions of the same module?
> Depending on how your system is configured, this may be possible, e.g.
> on COSMA:
> 
> ~~~
> module load julia/1.9.1
> module load julia/1.5.3
> ~~~
> {: .language-bash}
> 
> In some cases, you may encounter incompatibility dependency conflicts,
> particularly with underlying libraries.
> However you may not see any error at all, which could give rise to confusion.
> One way around this would be to exit your current terminal session and reconnect to the HPC resource which will reset
> your environment.
> But what about within the same login session?
> To remedy this, see the next section for how to *unload* modules.
{: .callout}

### Unloading Software

Conversely, we may wish to unload modules we have previously loaded.
This is useful if we no longer need to use a module, or require another version of the module.
In general, it's always good practice to unload modules you aren't currently using.

For example, assuming we already have Julia loaded, we can unload it using, e.g. on COSMA, with `julia`:

~~~
module unload julia
~~~
{: .language-bash}

Depending on your site, use `module unload` with the module you loaded earlier.

Note we don't have to specify the version number.
Once unloaded, our environment no longer allows us to make use of the software until we load it again.

If we want to unload all modules in our environment, we can use the `module purge` command.
But we aware that this will also remove any modules that are loaded automatically by default upon login.

> ## Using Software Modules in Scripts
>
> We've so far explored how to load modules within an interactive command line session,
> but if we want to make use of modules in our jobs we also need to load them in our job scripts
> so they are loaded on compute nodes when the job runs.
> 
> Create a job that is able to show the version of the module command you loaded earlier, e.g.:
> 
> - CSD3: `bison --version`
> - DiAL3: `ffmpeg -version` (note it's only using one hyphen!)
> - Tursa: `cmake --version`
> - COSMA: `julia --version`
> 
> Remember, no software
> is loaded by default! Running a job is very similar to logging on to the system,
> therefore you should not assume a module loaded on the login node is loaded on a
> compute node.
>
> > ## Solution
> >
> > In `version-module.sh` (again, replacing `yourAccount` and `aPartition`, but also replacing `cmake` with
> > the command for your site if it isn't `cmake`):
> >
> > ~~~
> > #!/bin/bash -l
> > #SBATCH --account=yourAccount
> > #SBATCH --partition=aPartition
> > #SBATCH --nodes=1
> > #SBATCH --ntasks-per-node=1
> > #SBATCH --time=00:00:30
> > 
> > module load cmake
> >
> > cmake --version
> > ~~~
> > {: .language-bash}
> >
> > ~~~
> > sbatch version-module.sh
> > ~~~
> > {: .language-bash}
> {: .solution}
{: .challenge}
