# Backdoor detection example with CBD

- [Backdoor detection example with CBD](#backdoor-detection-example-with-cbd)
  - [1) Introduction](#1-introduction)
  - [2) Preparation](#2-preparation)
    - [2.1) CSL build](#21-csl-build)
      - [2.1.1) Commands and authentication levels](#211-commands-and-authentication-levels)
        - [2.1.1.1) Specification reading](#2111-specification-reading)
        - [2.1.1.2) Specification modeling](#2112-specification-modeling)
      - [2.1.2) Syscalls](#212-syscalls)
      - [2.1.3) Final CSL specification](#213-final-csl-specification)
    - [2.2) `qmail` compilation](#22-qmail-compilation)
      - [2.2.1) AFL++ instrumentation](#221-afl-instrumentation)
      - [2.2.2) Normal compilation](#222-normal-compilation)
  - [3) Launch analysis !](#3-launch-analysis-)
    - [3.1) Time budget optimization](#31-time-budget-optimization)
    - [3.2) Fuzzing options optimization](#32-fuzzing-options-optimization)
    - [3.4) CBD run !](#34-cbd-run-)

## 1) Introduction

As an IOT manufacturer, I want to make a device that receive mails through the POP3 protocol. I would like to use an on-shelf tool to do that, and `qmail` could be a great candidate.

But, I would like to test the downloaded version of `qmail` to be sure there is no bakdoors within it.

## 2) Preparation

First of all, before doing an analysis with CBD, it is crucial to carefully read and understand the protocol specification, especially the part related to user authentication.

In the case of `qmail`, we will read the RFC 1939.

### 2.1) CSL build

#### 2.1.1) Commands and authentication levels

##### 2.1.1.1) Specification reading

By reading the RFC 1939, we can build this automaton that correspond to authentication states of the POP3 protocol :
![POP3 authentication automaton](.assets/POP3_automaton.png)

So, normally, an unauthenticated user can only use these commands :
```
QUIT
USER <name>
APOP <name> <digest>
```

If he could use the `LIST` command, which is only accessible under the `Transaction` state, the POP3 implementation is compromised.

##### 2.1.1.2) Specification modeling

After the read of the RFC 1939, we could build this CSL file :
```
levels:
  level authorization:
    USER (arg == "milhouse" => level = authorization_login_milhouse | _ => level = authorization),
    QUIT (_);

  level authorization_login_milhouse:
    PASS (arg == "lisa" => level = transaction | _ => level = authorization),
    USER (arg == "milhouse" => level = authorization_login_milhouse | _ => level = authorization),
    QUIT (_);

  level transaction:
    STAT,
    LIST (_),
    RETR (_),
    DELE (_),
    NOOP (_),
    RSET,
    QUIT (_ => level = update);

  level update:
    QUIT (_ => level = authorization);
```

We can see we added a new authentication level : `authorization_login_milhouse`. This authentication level permits to model a legitimate user (Here, `milhouse`) authentication with his proper password (`lisa`).

Each levels contain commands that represent transitions based on their arguments :
- For the command `USER` under the level `authorization`, arguments are :
  - `milhouse` : This is our legitimate user. By using the command `USER milhouse` under the authentication level `authorization`, the current authentication level become `authorization_login_milhouse`
  - `_` : This is a placeholder for "Any arguments other than `milhouse`". When the command `USER arg` is used with `arg` $\ne$ `"milhouse"`, the current authentication level is still `authorization`
- For the command `LIST` under the level `transaction`, arguments are :
  - `_` : A placeholder for "Any arguments". When this command is used, the current authentication level is still `transaction`

At the end, at the top of the CSL file, it is mandatory to write which authentication level is the initial (Eg. the initial). Here, it is the `Authorization` level :
```
initial_level = authorization;
```

Then, there are other useful options :
| Option | Values | Meaning |
|--------|--------|---------|
| `case_sensitivity` | `yes\|no` | Commands are case-sensitive or not |
| `arg_separator` | `'<character>' multiple?` | Argument separator in commands. The optional `multiple` argument tells the separator can be multiple between the command and its arg |
| `other_commands` | `yes\|no` | Generate or not templates to find other commands that are not in the CSL specification |

More details about these options are available in the `cslprocess` documentation

#### 2.1.2) Syscalls

The other important thing to do is building syscalls description that correspond to accepted and refused commands.

For example, let's study this sequence :
```
USER milhouse
LIST
```

When it is used with a clear version of qmail, the answer is this :
```
+OK <634145.1732876265@localhost>
+OK
-ERR authorization first
```

In term of syscalls, the execution is like that :
<details>
<summary>Syscalls emitted by `qmail` when trying this command sequence :</summary>

```
execve("./qmail-popup.ori", ["./qmail-popup.ori", "localhost", "./checkpassword", "./qmail-pop3d", "./Mailbox/"], 0x7fff68609f60 /* 67 vars */) = 0
[ Process PID=634474 runs in 32 bit mode. ]
brk(NULL)                               = 0x8a93000
mmap2(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0xf7fb3000
access("/etc/ld.so.preload", R_OK)      = -1 ENOENT (Aucun fichier ou dossier de ce nom)
openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = 3
statx(3, "", AT_STATX_SYNC_AS_STAT|AT_NO_AUTOMOUNT|AT_EMPTY_PATH, STATX_BASIC_STATS, {stx_mask=STATX_BASIC_STATS|STATX_MNT_ID, stx_attributes=0, stx_mode=S_IFREG|0644, stx_size=111895, ...}) = 0
mmap2(NULL, 111895, PROT_READ, MAP_PRIVATE, 3, 0) = 0xf7f97000
close(3)                                = 0
openat(AT_FDCWD, "/lib/libc.so.6", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = 3
read(3, "\177ELF\1\1\1\3\0\0\0\0\0\0\0\0\3\0\3\0\1\0\0\0\0N\2\0004\0\0\0"..., 512) = 512
statx(3, "", AT_STATX_SYNC_AS_STAT|AT_NO_AUTOMOUNT|AT_EMPTY_PATH, STATX_BASIC_STATS, {stx_mask=STATX_BASIC_STATS|STATX_MNT_ID, stx_attributes=0, stx_mode=S_IFREG|0755, stx_size=2455492, ...}) = 0
mmap2(NULL, 2082620, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0xf7d9a000
mmap2(0xf7dbd000, 1437696, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x23000) = 0xf7dbd000
mmap2(0xf7f1c000, 471040, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x182000) = 0xf7f1c000
mmap2(0xf7f8f000, 12288, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1f5000) = 0xf7f8f000
mmap2(0xf7f92000, 18236, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0xf7f92000
close(3)                                = 0
set_thread_area({entry_number=-1, base_addr=0xf7fb4440, limit=0x0fffff, seg_32bit=1, contents=0, read_exec_only=0, limit_in_pages=1, seg_not_present=0, useable=1}) = 0 (entry_number=12)
set_tid_address(0xf7fb44a8)             = 634474
set_robust_list(0xf7fb44ac, 12)         = 0
rseq(0xf7fb48e0, 0x20, 0, 0x53053053)   = 0
mprotect(0xf7f8f000, 8192, PROT_READ)   = 0
mprotect(0x804d000, 4096, PROT_READ)    = 0
mprotect(0xf7fed000, 8192, PROT_READ)   = 0
ugetrlimit(RLIMIT_STACK, {rlim_cur=8192*1024, rlim_max=RLIM_INFINITY}) = 0
munmap(0xf7f97000, 111895)              = 0
rt_sigaction(SIGALRM, {sa_handler=0x8049350, sa_mask=[], sa_flags=0}, NULL, 8) = 0
rt_sigaction(SIGPIPE, {sa_handler=SIG_IGN, sa_mask=[], sa_flags=0}, NULL, 8) = 0
getpid()                                = 634474
pselect6(2, NULL, [1], NULL, {tv_sec=1200, tv_nsec=0}, NULL) = 1 (out [1], left {tv_sec=1199, tv_nsec=999998261})
write(1, "+OK <634474.1732876333@localhost"..., 35+OK <634474.1732876333@localhost>
) = 35
pselect6(1, [0], NULL, NULL, {tv_sec=1200, tv_nsec=0}, NULL) = 1 (in [0], left {tv_sec=1199, tv_nsec=999999477})
read(0, "USER milhouse\nLIST\n", 128)   = 19
pselect6(2, NULL, [1], NULL, {tv_sec=1200, tv_nsec=0}, NULL) = 1 (out [1], left {tv_sec=1199, tv_nsec=999999199})
write(1, "+OK \r\n", 6+OK 
)                 = 6
pselect6(2, NULL, [1], NULL, {tv_sec=1200, tv_nsec=0}, NULL) = 1 (out [1], left {tv_sec=1199, tv_nsec=999999327})
write(1, "-ERR authorization first\r\n", 26-ERR authorization first
) = 26
pselect6(1, [0], NULL, NULL, {tv_sec=1200, tv_nsec=0}, NULL) = 1 (in [0], left {tv_sec=1199, tv_nsec=999999695})
read(0, "", 128)                        = 0
exit_group(1)                           = ?
+++ exited with 1 +++
```
</details>

The goal is to define rules to filter this garbage, to be able to map launched commands to their answer.

So, there will be two categories of syscalls : Syscalls related to accepted command, and syscalls associated to refused command.

By examining the trace, we can see two interesting syscalls to determine if the issued command is accepted or refused :
```
write(1, "+OK \r\n", 6) = 6
write(1, "-ERR authorization first\r\n", 26) = 26
```

We can infer this oracle to determine if a command is accepted or refused (Pseudo code) :
```C
regex_t accepted = "write(1, \"+OK \r\n\", 6)";
regex_t refused = "write(1, \"-ERR authorization first\r\n\", 26)";

if (match(syscall, accepted) = true)
{
  command.isAccepted = true;
}
else if (match(syscall, refused) == true)
{
  command.isRefused = true;
}
```

In depth, this oracle permits to map issued commands to their corresponding answer syscall :
| Command         | Syscall                                        |
|-----------------|------------------------------------------------|
| `USER milhouse` | `write(1, "+OK \r\n", 6)`                      |
| `LIST`          | `write(1, "-ERR authorization first\r\n", 26)` |

Finally, after analysis and tests, the CSL built in [2.1.1.2)](#2112-specification-modeling) will be complemented with :
```
syscall_monitor:
  command_to_run_args = "localhost ${BASE_DIRECTORY}/checkpassword ${BASE_DIRECTORY}/qmail-pop3d ${BASE_DIRECTORY}/Mailbox";

  accepted:
    write(1, "+OK .*", ".*");

  refused:
    write(1, "Error: Unknown command", ".*"),
    write(1, "Unauthorized", ".*"),
    write(1, "-ERR.*", ".*");
```

> The `command_to_run_args` contains the arguments passed to the program during oracle testing. `${BASE_DIRECTORY}` is a placeholder replaced by the path of the target gave to CBD

<details>
<summary>False-positive reduction with "ignored" syscalls</summary>

In the full `strace` trace, there is a problematic syscall launched during the program initialization :
<code>write(1, "+OK <634474.1732876333@localhost"..., 35)</code>

This syscall is always launched during the `qmail` initialization, and is caught by the `accepted` syscall rule regex (`write(1, "+OK .*", ".*");`).

In practice, this mechanism will produce lot of false positives during the analysis.

The workaround is declaring the syscall `write(1, "+OK <.*", ".*")` as `ignored`, at the end of the `syscall_monitor` part of the CSL specification :

```
syscall_monitor:
  [...]

  ignored:
    write(1, "+OK <.*", ".*");
```

Syscalls in this part are ignored by `syscall_monitor` !
</details>

#### 2.1.3) Final CSL specification

<details>
<summary>Content of the final CSL specification</summary>

```
case_sensitivity = no;
arg_separator = ' ' multiple;
initial_level = authorization;
other_commands = yes;

levels:
  level authorization:
    USER (arg == "milhouse" => level = authorization_login_milhouse | _ => level = authorization),
    QUIT (_);

  level authorization_login_milhouse:
    PASS (arg == "lisa" => level = transaction | _ => level = authorization),
    USER (arg == "milhouse" => level = authorization_login_milhouse | _ => level = authorization),
    QUIT (_);

  level transaction:
    STAT,
    LIST (_),
    RETR (_),
    DELE (_),
    NOOP (_),
    RSET,
    QUIT (_ => level = update);

  level update:
    QUIT (_ => level = authorization);

syscall_monitor:
  command_to_run_args = "localhost ${BASE_DIRECTORY}/checkpassword ${BASE_DIRECTORY}/qmail-pop3d ${BASE_DIRECTORY}/Mailbox";

  accepted:
    write(1, "+OK .*", ".*");

  refused:
    write(1, "Error: Unknown command", ".*"),
    write(1, "Unauthorized", ".*"),
    write(1, "-ERR.*", ".*");

  ignored:
    write(1, "+OK <.*", ".*");
```
</details>

### 2.2) `qmail` compilation

Our target (Here, `qmail`) will need to be compiled fourth times :
- In a normal way, for the oracle testing
- With some AFL++ instrumentations, to permit to fuzz the program efficiently

:warning: <span style="color: red">It is important that all compiled target variants are in the same directory !</span>

#### 2.2.1) AFL++ instrumentation

As explained in [readme](../README.md), CBD is built around [AFL++](https://github.com/AFLplusplus/AFLplusplus) to build test cases.

So, `qmail` will be compiled with `afl-cc` or `afl-clang-fast` in several ways :
- With the LTO optimization (`afl-clang-lto`)
  - At the end, we will have `target.lto.afl`
- With CmpLog (`afl-clang-lto` + Environment variable `AFL_LLVM_CMPLOG=1`)
  - At the end, we will have `target.lto.cmplog`
- With LAF-Intel (`afl-clang-fast` + Environment variable `AFL_LLVM_LAF_ALL=1`)
  - At the end, we will have `target.laf-intel.afl`

For `qmail`, it is done by editing the file `conf-cc`, to change the compiler to the AFL++ one.

#### 2.2.2) Normal compilation

Just compile `qmail` normally, with the default compiler.

## 3) Launch analysis !

Now, we have all things we need to analyze `qmail` with CBD !

Let's look up to the CBD command line arguments (Shown by launching CBD without any args) :
```
Usage : ./cbd [OPTIONS] <csl_file> <binary_to_fuzz_RADICAL> <time_to_fuzz_a_wildcard_in_s> <max_command_sequences_depth> <number_of_concurrent_threads> [binary_to_fuzz_args...]
```

### 3.1) Time budget optimization

First of all, it is crucial to determine the time budget allocated to the analysis. Higher it is, more backdoors could be detected.

In CBD, the time budget could be gave in two different manners :
- Uniformly : For each command, the time budget is the same
- By layers :
  - Let this command sequence :
    ```
    USER milhouse
    PASS homer
    LIST
    ```

    In this sequence, the first layer is `USER milhouse`, the second one `PASS homer` and the third one `LIST`. It is possible to set the time budget by layer : For example, for the first one, we set $60\text{s}$, the same for the second one and $1\text{s}$ for the last one
  - In this case, we will use the option "`-T`", and set the parameter `<time_to_fuzz_a_wildcard_in_s>` to "60 60 1" (With double quotes !).

### 3.2) Fuzzing options optimization

CBD provides some fuzzing options. If we launch it without any arg, we can see this :
```
-f : Full fuzzing mode => Fuzz with 1 main fuzzer, 1 cmplog and 3 compcov AFL++ instances
-s : Partial fuzzing mode => Fuzz with 1 main fuzzer, 1 cmplog and 1 compcov AFL++ instances
-c : Cmplog minimal fuzzing mode => Fuzz with only 1 main cmplog AFL++ instance
-C : Compcov minimal fuzzing mode => Fuzz with only 1 main compcov AFL++ instance
-t : Text-only fuzzing => The fuzzer will only generate text
```

The "best" mode is "`-f`", because it will spawn 5 different fuzzers for one adversarial input, with a good diversity in input discovery.

There is a derived mode "`-s`", which is the same as "`-f`", but it will spawn one CompCov fuzzer instead of three.

At the end, there is "`-c`" and "`-C`", that will spawn only one Cmplog or one Compcov fuzzer per adversarial sequence.

Finally, "`-t`" constraints AFL++ to only generate text inputs (Concretly, it passes the option `-a text` to the fuzzers).

:white_check_mark: Please take note that CBD try to use the more threads it can use, in the limit of the number of maximum threads provided through the command line.

### 3.4) CBD run !

Now, we are ready to run the analysis on our `qmail` binary !

Here is an example of command-line to run CBD, but some things may change between installations (Paths, Maximum number of CPUs, etc.) :
```
./cbd -fT ./qmail.csl ./qmail-popup "60 60 60" 3 20 ./checkpassword ./qmail-pop3d ./Mailbox
```