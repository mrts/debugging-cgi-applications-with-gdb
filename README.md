# How to debug CGI applications with GDB

As you probably already know, CGI, Common Gateway Interface, is a standard
protocol for web servers to execute programs by passing HTTP request data into
the program via standard input and environment variables, and returning the
program's standard output as the HTTP response. Unless FastCGI or SCGI is used,
the executable program would be started as a separate process by the web server
for each request and torn down at the end.

GDB is commonly used to debug C and C++ programs and it supports attaching to
running processes. As the CGI process is short-lived, you need to delay its
exit to have enough time to attach the debugger while the process is still
running. For casual debugging the easiest option is to simply use plain
`sleep()` in an endless loop at the breakpoint location and exit the loop with
the debugger once it is attached to the program (there are other, more
complicated options that I don't cover here).

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

        ScriptAlias /cgi-bin/ /var/www/cgi-test/cgi-bin/

        <Directory "/var/www/cgi-test/cgi-bin">
            AllowOverride None
            Options +ExecCGI -MultiViews +SymLinksIfOwnerMatch
            Order allow,deny
            Allow from all
        </Directory>
    </VirtualHost>

Finally, restart Apache:

    sudo service apache2 restart

## Compiling and running the example CGI application

Install build tools:

    sudo apt-get install build-essentials cmake gdb

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

You can find the process ID with `pgrep`:

    pgrep -l cgi-debugging

Attach GDB to the process (the process is paused when debugger attaches):

    sudo -u gdb cgi-debugging-example $(pgrep cgi-debugging)

Exit the infinite loop by typing `return` until you reach `main`:

    (gdb) return
    Make __sleep return now? (y or n) y
    #0  0x000000000040086b in wait_for_gdb_to_attach ()
    (gdb) return
    Make selected stack frame return now? (y or n) y
    #0  0x000000000040087b in main ()

Now you can continue debugging the program or let it run to completion:

    (gdb) continue
    Continuing.
    [Inferior 1 (process 1005) exited normally]
    (gdb) quit

The browser should now show the program output - *Hello!*
