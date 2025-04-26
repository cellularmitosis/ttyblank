# ttyblank

A tty blanker / screensaver.

## Building

```
gcc -o ttyblank ttyblank.c
```

or just type `make`.

Copy the binary into `/usr/local/bin`.

## Usage

### Manual

To blank the terminal from an interactive session, invoke it manually:

```
/usr/local/bin/ttyblank
```

then press any key to unblank the terminal.

### SysVinit

Under SysV init, replace the first agetty in `/etc/inittab`:

```
c1:12345:respawn:/usr/local/bin/ttyblank
c2:2345:respawn:/sbin/agetty 38400 tty2 linux
c3:2345:respawn:/sbin/agetty 38400 tty3 linux
...
```

You can then press `alt+<right arrow>` or `alt+F2` to switch to tty2 if you need to log in.

### Systemd

Under systemd, run `systemctl edit getty@tty1.service`, then enter:

```
[Service]
ExecStart=
ExecStart=-/usr/sbin/ttyblank
Restart=always
```

You can then press `alt+<right arrow>` or `alt+F2` to switch to tty2 if you need to log in.

## Public Domain

The ttyblank.c source code file was written by Gemini AI and is released into the public domain.
