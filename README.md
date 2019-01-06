## TL;DR

unclog is a zero-config, yet highly flexible logging library for C. It currently only supports POSIX-style OSes and needs pthread-style mutexes.

### Compile and Install

./autogen.sh
./configure
make
make install

### Performance

|Type|msgs/sec|
|-|-|
|Without Output|4.7M|
|Static fprintf|570k|
|Primitve Implementation|347k|
|Optimized Implementation|400k|

## The Name

I googled nclog (for new C logging library) and got results for unclog. Seemed like a good name, and since its not far from unclog to uncle, *Bob's your unclog*.
