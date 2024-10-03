#pragma once
// It is NOT necessary to call them in your app.
bool __stdcall Initialize(void* data);
bool __stdcall Initialize_Thread(void* data);
bool __stdcall UnInitialize_Thread(void* data);
bool __stdcall UnInitialize(void* data);

