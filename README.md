# C callstats.io library for Janus

Library for correlating and sending stats to [callstats.io](https://callstats.io)

<br />

### Dependencies   

   * [jansson](https://github.com/akheron/jansson)
   * [libjwt](https://github.com/benmcollins/libjwt)
   * [libcurl](https://curl.haxx.se/libcurl/c/)
   * [sqlite3](https://www.sqlite.org/capi3ref.html)
   * [glib](https://developer.gnome.org/glib/)

<br />

### Compiling with [Janus-gateway](https://github.com/meetecho/janus-gateway)

Steps:

1. git clone the `Janus-gateway` repository
2. git clone this repository inside `Janus-gateway/events/` as `callstats`
3. inside `Janus-gateway/conf/` make a copy the file `janus.eventhandler.sample.cfg.sample`
file, and, rename the copy to `janus.eventhandler.callstats.cfg.sample`
4. Edit `Makefile.am` inside Janus-gateway as follows -
Below the code snippet -
```C
    if ENABLE_SAMPLEEVH
    .
    .
    endif
```
Add this code snippet -
```C
    if ENABLE_CALLSTATSEVH
    event_LTLIBRARIES += events/libjanus_callstatsevh.la
    events_libjanus_callstatsevh_la_SOURCES = events/janus_callstatsevh.c
    events_libjanus_callstatsevh_la_CFLAGS = $(events_cflags)
    events_libjanus_callstatsevh_la_LDFLAGS = $(events_ldflags) -lcurl -lsqlite3 -ljwt -ljansson -lglib
    events_libjanus_callstatsevh_la_LIBADD = $(events_libadd)
    conf_DATA += conf/janus.eventhandler.callstatsevh.cfg.sample
    EXTRA_DIST += conf/janus.eventhandler.callstatsevh.cfg.sample
    endif
```
5. Edit `Janus-gateway/configure.ac` as follows -

	* Find the code snippet -
	```C
	AC_ARG_ENABLE([all-handlers],
	      [AS_HELP_STRING([--disable-all-handlers],
			      [Disable building all event handlers (except those manually enabled)])],
	      [
	       AS_IF([test "x$enable_sample_event_handler" != "xyes"],
		     [enable_sample_event_handler=no])
	      ],
	[])
	```

	And add callstats_event_handler config as follows -

	```C
	AC_ARG_ENABLE([all-handlers],
	      [AS_HELP_STRING([--disable-all-handlers],
			      [Disable building all event handlers (except those manually enabled)])],
	      [
	       AS_IF([test "x$enable_sample_event_handler" != "xyes"],
		     [enable_sample_event_handler=no])
			   AS_IF([test "x$enable_callstats_event_handler" != "xyes"],
		     [enable_callstats_event_handler=no])
	      ],
	[])
	```

	* Find `PKG_CHECK_MODULES` code snippet, and, inside it
	find the code snippet -
	```C
	AS_IF([test "x$enable_sample_event_handler" != "xno"],
		       [
			 AC_DEFINE(HAVE_SAMPLEEVH)
			 JANUS_MANUAL_LIBS+=" -lcurl"
			 enable_sample_event_handler=yes
		       ])
	```

	below this, add the following code snippet to define the callstats event handler-
	```C
	AS_IF([test "x$enable_callstats_event_handler" != "xno"],
		       [
			 AC_DEFINE(HAVE_CALLSTATSEVH)
			 JANUS_MANUAL_LIBS+=" -lcurl -ljannson -lglib -lsqlite3 -ljwt"
			 enable_sample_event_handler=yes
		       ])
	```

	* Find the code snippet -
	```C
	   AM_CONDITIONAL([ENABLE_SAMPLEEVH], [test "x$enable_sample_event_handler" = "xyes"])
	```
	and add below it the following code snippet -

	```C
	   AM_CONDITIONAL([ENABLE_CALLSTATSEVH], [test "x$enable_callstats_event_handler" = "xyes"])
	```

	* Find the code snippet -

	```C
	AM_COND_IF([ENABLE_SAMPLEEVH],
		[echo "    Sample event handler:  yes"],
		[echo "    Sample event handler:  no"])
	````

	and add below it the following snippet

	```C
	M_COND_IF([ENABLE_CALLSTATSEVH],
		[echo "    Callstats event handler:  yes"],
		[echo "    Callstats event handler:  no"])
	```

6. Now install the dependencies of the Janus-gateway
7. Install the dependencies for this repo
8. Open a terminal. Go inside Janus-gateway repo and run following commands one by one
	* sudo sh autogen.sh
	* sudo ./configure --prefix=/opt/janus
	* sudo make
	* sudo make install
	* sudo make configs

**NOTE**: For reference, you can check out my Janus-gateway fork [here](https://github.com/code-master5/janus-gateway/tree/callstats-integration). This fork has the changes.

<br />

### Enabling Callstats Event Handler

After successfully compiling the Janus-gateway with Callstats Event Handler,
you need to enable it to send stats to callstats.io.

Follow the steps below to enable the callstats event-handler:

1. Open the `/opt/janus/etc/janus/janus.cfg` using `sudo` in any text editor
and find the snippet -
```C
; broadcast = yes
```
uncomment this snippet as follows -

```C
broadcast = yes
```
this tells Janus to broadcast events so that they can be received by any event-handler.

2. Open the `/opt/janus/etc/janus/janus.eventhandler.callstatsevh.cfg` using `sudo` in any text editor
and find the snippet -
```C
enabled = no
```
enable the event-handler by changing the above snippet as following -
```C
enabled = yes
```
3. Now you're good to go. Just type `/opt/janus/bin/janus` and press Enter to run the Janus instance
with callstats event handler enabled.
