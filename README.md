# How to debug CGI applications written in C/C++ with GDB

As you probably already know, CGI, Common Gateway Interface, is a standard
protocol for web servers to execute programs by passing HTTP request data into
the program via standard input and environment variables, and returning the
program's standard output as the HTTP response. Unless FastCGI or SCGI is used,
the executable program would be started as a separate process by the web server
for each request and torn down at the end.

GDB, the GNU Debugger, is commonly used to debug C and C++ programs and it
supports attaching to running processes. As the CGI process is short-lived, you
need to delay its exit to have enough time to attach the debugger while the
process is still running. For casual debugging the easiest option is to simply
use `sleep()` in an endless loop at the breakpoint location and exit the loop
with the debugger once it is attached to the program. There are other, more
complicated options that I don't cover here.

The following example assumes Ubuntu Linux and Apache.

## Setting up CGI support in Apache

First install and enable the CGI module:

    sudo a2enmod cgi

Then configure a CGI-enabled virtual host:

    <VirtualHost *:80>
        ServerName      cgi-test.example.com
        DocumentRoot    /var/www/cgi-test/htdocs
        CustomLog       /var/log/apache2/cgi-test.access.log combined
        ErrorLog        /var/log/apache2/cgi-test.error.log

        TimeOut         600

        ScriptAlias /cgi-bin/ /var/www/cgi-test/cgi-bin/

        <Directory "/var/www/cgi-test/cgi-bin">
            AllowOverride None
            Options +ExecCGI -MultiViews +SymLinksIfOwnerMatch
            Order allow,deny
            Allow from all
        </Directory>
    </VirtualHost>

Note the `TimeOut` parameter - it reserves 10 minutes for debugging instead of
the default one minute. After timeout is reached, Apache kills the CGI process
and returns the *504 Gateway timeout* response.

Finally, restart Apache:

    sudo service apache2 restart

## Compiling and running the example CGI application

Install build tools:

    sudo apt-get install build-essentials cmake cgdb

Compile the application:

    cmake .
    make

Copy the application to `cgi-bin` directory:

    cp cgi-debugging-example /var/www/cgi-test/cgi-bin

Open the URL that runs the application in browser:

<http://cgi-test.example.com/cgi-bin/cgi-debugging-example>

The browser will show the loading icon as the application enters the endless
loop and can now be attached to with GDB.

## Attaching the debugger and debugging

It is recommended to use CGDB instead of plain GDB. CGDB is a curses frontend
to GDB that provides the familiar GDB text interface with a split screen
showing the source as it executes.

You can find the CGI process ID with `pgrep`:

    pgrep -l cgi-debugging

Attach CGDB to the process (the process is paused when debugger attaches):

    sudo cgdb cgi-debugging-example $(pgrep cgi-debugging)

Next you need to exit the infinite loop and `wait_for_gdb_to_attach()` function
to reach the "breakpoint" in your application. The trick here is to step out of
`sleep()` until you reach `wait_for_gdb_to_attach()` and set the value of the
variable `is_waiting` with the debugger so that `while (is_waiting)` exits:

    (gdb) finish
    Run till exit from 0x8a0920 __nanosleep_nocancel () at syscall-template.S:81
    0x8a07d4 in __sleep (seconds=0) at sleep.c:137
    (gdb) finish
    Run till exit from 0x8a07d4 in __sleep (seconds=0) at sleep.c:137
    wait_for_gdb_to_attach () at cgi-debugging-example.c:6
    Value returned is $1 = 0
    (gdb) set is_waiting = 0 # <- to exit while
    (gdb) finish
    Run till exit from wait_for_gdb_to_attach () cgi-debugging-example.c:6
    main () at cgi-debugging-example.c:13

You could also force return with `return`, but that may mess up application
state and cause crashes. Or you could use `next` to step out of functions
instead of `finish`.

Once you are out of `wait_for_gdb_to_attach()`, you can continue debugging the
program or let it run to completion:

    (gdb) next
    (gdb) continue
    Continuing.
    [Inferior 1 (process 1005) exited normally]
    (gdb) quit

The browser should now show the program output - *Hello!*
