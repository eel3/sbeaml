console
=======

Sample application for SBEAML.

Target environments
-------------------

Windows, Linux, macOS.

console is written in ISO C99/C++11, and so probably works fine on other OS.

How to build
------------

Use make and Makefile. Target name is `all`.
For example, on Unix environment, `make -f build-unix-gcc.mk all`.

| Toolset                           | Makefile           |
|:----------------------------------|:-------------------|
| Linux                             | build-unix-gcc.mk  |
| macOS                             | build-mac-clang.mk |
| MinGW/TDM-GCC (with GNU make)     | build-win-gcc.mk   |
| Microsoft Visual C++ (with NMAKE) | build-win-vc.mak   |

Usage
-----

Simply execute `console`. console has no option.

Example
-------

<pre>
$ <kbd>console</kbd>
<samp>root->event_handler->on_init();
root->event_handler->on_appear();</samp>
<kbd>help</kbd>
<samp>exit    (no option)     Exit program.
help    (no option)     Show help message.
kill-gtimer     id      Kill global timer.
kill-timer      id      Kill timer.
pop-handler     [(no option)|tag-number|all]    Pop handlers.
post-msg-inner  (no option)     Post message (from main-loop() thread)
post-msg-outer  (no option)     Post message (from main thread)
push-handler    (no option)     Push next event handler.
set-gtimer      id timeout-millis repeat-[off|on]       Start global timer.
set-timer       id timeout-millis repeat-[off|on]       Start timer.</samp>
<kbd>push-handler</kbd>
<samp>root->event_handler->on_event(0x00000000);
root->event_handler->on_disappear();
inner->event_handler->on_init();
inner->event_handler->on_appear();</samp>
<kbd>pop-handler</kbd>
<samp>inner->event_handler->on_event(0x01000000);
inner->event_handler->on_disappear();
inner->event_handler->on_destroy();
inner->event_handler->release_user_data();
root->event_handler->on_appear();</samp>
<kbd>exit</kbd>
<samp>root->event_handler->on_destroy();
root->event_handler->release_user_data();</samp>
$ _
</pre>
