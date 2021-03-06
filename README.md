Command line tool for the UVa Online Judge website

UVA-CLI is an interactive shell where you can type commands to submit
and check your submissions.

Features
========
- Remembers your account info and encrypts your passwords.
- Password-less submission.
- Check most recent submission status.

Requirements
============
To run, you'd need Java Runtime 1.6 and above.
To check your java version, do `java --version` at the command line.
To build, please see BUILDING section.

Running
=======
All dependencies are packaged in to the jar for your convenience.

If you prefer to build it yourself, please see the Building section.

Before you run, please generate an SSH key by following the instructions
https://help.github.com/articles/generating-ssh-keys

The program assumes your private key is at `~/.ssh/id_rsa` where `~` is
your home directory.
The program will use the key to encrypt all your account passwords.

Then to run, do `java -jar uva-cli-1.0.0-all.jar`

Usage
=====
Run the uva-cli-xxx-all.jar file at the command prompt.

UVA-CLI is an interactive shell which you can type commands.
Commands are of the syntax: `<action> <arg1> <arg2> ...`

Settings are saved in the JSON format at `~/.uva-cli`
where ~ is your home directory. Please do not modify manually.
All passwords are encrypted.

The following are possible actions:

add
----
Syntax: add {type} {username} {password}

Adds a user account.
All accounts will be preserved even after you quit the program.

Currently uva is the only supported type.

remove
------
Syntax: remove {username}

Removes a user account

use
---
Syntax: use {username}

Sets a user account as current.
If {username} is omitted, sets the current account to none.
The current account setting will be preserved even after you quit the program.

show
----
Syntax: show 

Shows all user accounts

send
----
Syntax: send {problem-id} {fileName}

Sends a code file. {fileName} is relative to the current directory, which
is where you ran the `java -jar ...` command to start uva-cli

The program will auto-detect the language using the file name extension:
- .java  : Java
- .cpp   : C++
- .c     : C
- .pascal / .pas / .p : Pascal 

status
------
Syntax: status

Prints out the latest ten submissions.

quit / exit
-----------
Saves all settings including account info and exits the program.

Example usage
=============
<pre>
> add uva john_doe 12345
Account added

> use john_doe

> send 123 code.cpp
Logging in...
Sending code...
Sent OK

> status
Getting status...
SubId     | ProbId |      Verdict     |  Lang  | Runtime | 
11605207      757           accepted      C++     0.020
11605200      757           accepted      C++     0.024
...
</pre>


Building
========
To build, you'd need:
- Java JDK 1.6+
- libjava

First, do a `git clone https://github.com/jwill/uva-cli.git`

libjava
-------
- `git clone https://github.com/jwill/libjava.git` 
- Follow the instructions to build libjava 
- Copy libjava.jar to the uva-cli/lib dir

uva-cli
-------
- Run `gradle jar` or `gradlew jar` if you don't have Gradle installed.

More features coming soon
==========================
- Log in only once instead of logging in on every send
- Supports more coding websites
- Auto-retry submitting the solution (when UVA is down)
- Connects to UVAtoolkit
- Get statistics on a problem

Credits
=======
- UVA website
- uHunt API
- json.org
- lucastan (https://github.com/lucastan)

Disclaimer
==========
While every effort has been made to ensure this software is free of defects, 
this software comes with ABSOLUTELY NO WARRANTY to the maximum extent 
permitted by the law.

License
=======
lucastan has yet to decide on a license to apply to the source code. So my fork is also undetermined. 


Please let me know if there is any problems.

