@echo off
cl.exe demo.cpp /EHsc /MT /D_UNICODE /DUNICODE /std:c++20 /Fo32demo /link /SUBSYSTEM:CONSOLE /MANIFEST:EMBED /MACHINE:X86

