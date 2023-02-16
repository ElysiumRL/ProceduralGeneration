@echo off
DEL /q/s *.sln
DEL /q/s .vs
DEL /q/s .idea
DEL /q/s Binaries
DEL /q/s DerivedDataCache
DEL /q/s Intermediate
DEL /q/s Saved

rmdir /q/s .vs
rmdir /q/s Binaries
rmdir /q/s DerivedDataCache
rmdir /q/s Intermediate
rmdir /q/s Saved