@echo off
cl.exe /analyze demo.cpp /EHsc /MT /D_UNICODE /DUNICODE /std:c++20 /Fo32AnalyzeOutput /link /MACHINE:X86

