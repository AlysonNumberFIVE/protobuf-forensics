# Protobuf Forensics
A small forensics tasks for finding anomalies in a protobuf session with its accompanying .mjr files.

## Quickstart

This repo has 2 directories; `binary_inspection_c` and `protobuf_inspection_go` that have C and Go code respectively. The C code manually unpacks and inspects the `.mjr` binaries in root, whereas the Go code inspects the `.json` and `.proto` file. Together, these projects put together a comprehensive forensics analysis of the presented session.

### Inside `binary_inspection_c`
Simply run
```
make all
```
to generate the `analyser` binary. From here, run either
```
./analyser ../*audio.mjr
```
or 
```
./analyser ../*video.mjr
```
and you will get an output in this format.
```
json length is 122
frame->recv_be is 0
first recv_ be is 0
last recv_ be is 372814
total recv is 372
total ts is 699.148125
=== timestamp of timestamp drifts ===
[1785394763010 747] [1785394763588 9833] [1785394763592 20406] [1785394763709 6863] [1785394763746 26871] [1785394764248 17158] [1785394764419 5473] [1785394764591 17127] [1785394764634 937] [1785394764635 26310] [1785394764636 26310] [1785394764718 34726] [1785394764826 3467] [1785394765290 1132] [1785394765370 5885] [1785394765419 16335] [1785394765725 2772] [1785394765840 753] [1785394766042 14373] [1785394766128 16246] [1785394766249 4025] here is 800013607
** video frame incomplete before ts 17132400 (packet 1797, prev m=0)
** video frame incomplete before ts 20454120 (packet 2327, prev m=0)
** video frame incomplete before ts 26040690 (packet 3233, prev m=0)
** video frame incomplete before ts 27832950 (packet 3539, prev m=0)
```
This output must be read alongside the accompanying `writeup.pdf` submitted in this repo.

### Inside `protobuf_inspection_go` 
Simply run 
```
go run .
```
and you will get an output like this
```
consecutive duplicate payloads: 163
  join         97
  signalling   66

repeated payloads (count / span / density):
  x39   span=269845 ms 0.1/s    event:["signalling",{"t":"av-talking","id":"kskVvUlDr5Eq4OWfR4Dp","state":true},{"sender":{"userId":
  x39   span=357790 ms 0.1/s    event:["signalling",{"t":"av-talking","id":"kskVvUlDr5Eq4OWfR4Dp","state":false},{"sender":{"userId"
  x19   span=5411   ms 3.5/s    event:["signalling",{"t":"mm","c":[1430,-87],"i":3740984,"w":"0e07cf38"},"s"]
  x13   span=14774  ms 0.9/s    event:["join",{"id":3740984,"guest":false,"readOnly":false,"allowInvite":false,"meta":{"profilePictu
  x10   span=3142   ms 3.2/s    event:["signalling",{"t":"mm","c":[1428,-87],"i":3740984,"w":"0e07cf38"},"s"]
  x9    span=6145   ms 1.5/s    event:["join",{"id":3740984,"guest":false,"readOnly":false,"allowInvite":false,"meta":{"profilePictu
  x9    span=36345  ms 0.2/s    event:["signalling",{"t":"pe","i":3740984,"w":"0e07cf38"},"s"]
  x8    span=21501  ms 0.4/s    event:["join",{"id":3740984,"guest":false,"readOnly":false,"allowInvite":false,"meta":{"profilePictu
  x8    span=6      ms 1333.3/s event:["join",{"id":3740984,"guest":false,"readOnly":false,"allowInvite":false,"meta":{"profilePictu
  x7    span=161733 ms 0.0/s    event:["signalling",{"t":"ChatUserInput.removePersonTyping","traceId":"6eea913e-bb6a-410b-936b-a8a8a
  x7    span=1284   ms 5.5/s    event:["signalling",{"t":"mm","c":[1428,-90],"i":3740984,"w":"0e07cf38"},"s"]
  x6    span=7      ms 857.1/s  event:["join",{"id":3740984,"guest":false,"readOnly":false,"allowInvite":false,"meta":{"profilePictu
  x6    span=92     ms 65.2/s   event:["join",{"id":3740984,"guest":false,"readOnly":false,"allowInvite":false,"meta":{"profilePictu
  x6    span=37     ms 162.2/s  event:["join",{"id":3740984,"guest":false,"readOnly":false,"allowInvite":false,"meta":{"profilePictu
  x6    span=9      ms 666.7/s  event:["join",{"id":3740984,"guest":false,"readOnly":false,"allowInvite":false,"meta":{"profilePictu
frequency
type: join, id: 3740984, frequency: 177
frequency
type: leave, id: 3740984, frequency: 3
len=41    00 00 05 82 d4 9b ce 02 00 00 01 27 13 10 72 6f 6f 74 73 68  |...........'..rootsh|
len=39    00 00 05 82 d4 9b ce 02 00 00 01 27 11 0e 72 6f 6f 74 63 61  |...........'..rootca|
len=33    00 00 05 82 d4 9b ce 02 00 00 01 27 0b 08 72 6f 6f 74 64 69  |...........'..rootdi|
len=48    00 00 05 82 d4 9b ce 02 00 00 01 27 1a 17 72 6f 6f 74 67 72  |...........'..rootgr|
```
Read the accompanying `writeup.pdf` to intepreet this output.

## Reading the Writeup
A write-up containing the forensics process of this project is accompanied in this repo. It contains references to these terminal outputs and will provide context for both output, as well as the use of both Golang and C.



### Written by Alyson Ngonyama






