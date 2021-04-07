# Android benchmark app

The perftest app measures the overhead of common djinni types. Example output:

```
djinni_perf_benchmark: name                 ,     avg,      sd,     min,     p50,     p90,     max
djinni_perf_benchmark: cppTests             ,    3555,    4998,    2577,    3230,    3538,  130692
djinni_perf_benchmark: baseline             ,    1786,    1475,    1500,    1653,    1961,   46269
djinni_perf_benchmark: argString 16         ,    4622,    2843,    3616,    3885,    5692,   78769
djinni_perf_benchmark: argString 128        ,    4893,    1170,    4231,    4500,    5693,   21500
djinni_perf_benchmark: argString 4096       ,   24407,    2833,   22423,   23807,   24923,   54692
djinni_perf_benchmark: argString 16384      ,  109606,    6807,   99000,  109077,  114000,  203231
djinni_perf_benchmark: argBinary 128        ,    8834,    2865,    7692,    8077,    9461,   73192
djinni_perf_benchmark: argBinary 4096       ,   80466,   31684,   70461,   74577,   80346,  723769
djinni_perf_benchmark: argBinary 16384      ,  283083,   13145,  265846,  280615,  291961,  398847
djinni_perf_benchmark: argEnumSixValue      ,    8309,   45089,    2077,    5885,    9615, 1357308
djinni_perf_benchmark: argRecordSixInt      ,    8145,   13855,    5961,    6692,    8000,  299154
djinni_perf_benchmark: argListInt 128       ,  353718,   61784,  327000,  342500,  371384, 1514347
djinni_perf_benchmark: argArrayInt 128      ,    8210,    2571,    6923,    8039,    8961,   54654
djinni_perf_benchmark: argObject            ,   13025,    5478,   11115,   11846,   14730,  167346
djinni_perf_benchmark: argListObject 128    , 1515533,   72282, 1479230, 1503308, 1527153, 2802923
djinni_perf_benchmark: argListRecord 128    ,  834520,   21093,  809731,  828885,  854077, 1194654
djinni_perf_benchmark: argArrayRecord 128   ,  847624,   58961,  811500,  836692,  870385, 1997654
djinni_perf_benchmark: returnInt            ,    1773,     897,    1423,    1693,    1884,   25731
djinni_perf_benchmark: returnListInt 1      ,   11785,   37466,    7308,    9577,   10461,  949770
djinni_perf_benchmark: returnListInt 10     ,   28896,   25874,   24346,   27077,   28115,  674923
djinni_perf_benchmark: returnListInt 128    ,  280811,    8874,  260884,  279654,  290500,  337000
djinni_perf_benchmark: returnArrayInt 1     ,    7872,    2467,    5808,    8000,    8461,   62384
djinni_perf_benchmark: returnArrayInt 10    ,    6455,    1215,    5961,    6269,    6538,   29577
djinni_perf_benchmark: returnArrayInt 128   ,    8858,    1509,    7885,    8346,    9923,   32538
djinni_perf_benchmark: returnString 16      ,    5285,    3481,    3769,    5461,    6000,  101192
djinni_perf_benchmark: returnString 128     ,    5605,    1046,    5192,    5461,    5692,   27538
djinni_perf_benchmark: returnString 4096    ,   57891,    5292,   55462,   56769,   60692,  176769
djinni_perf_benchmark: returnString 16384   ,  230499,   65783,  220538,  225231,  234577, 1919885
djinni_perf_benchmark: returnBinary 128     ,    6618,    5160,    5115,    6769,    7385,  157539
djinni_perf_benchmark: returnBinary 4096    ,  108987,  137775,   70000,   72846,  111115, 1596308
djinni_perf_benchmark: returnBinary 16384   ,  329263,  316021,  277885,  283654,  307731, 5322000
djinni_perf_benchmark: returnObject         ,    5317,   11287,    3730,    5077,    5461,  359577
djinni_perf_benchmark: returnListObject 10  ,   42861,   11334,   39884,   41693,   44038,  388769
djinni_perf_benchmark: returnListObject 100 ,  364438,   74708,  351423,  360115,  368923, 2713269
djinni_perf_benchmark: returnListRecord 1   ,    8620,    3061,    6615,    8808,    9500,   63731
djinni_perf_benchmark: returnListRecord 10  ,   34469,    2589,   32539,   33884,   35884,   61461
djinni_perf_benchmark: returnListRecord 128 ,  387307,   20957,  362769,  382692,  404270,  727461
djinni_perf_benchmark: returnArrayRecord 1  ,    8247,    3213,    6423,    7192,    9384,   77308
djinni_perf_benchmark: returnArrayRecord 10 ,   36677,   13011,   32230,   34808,   39269,  404346
djinni_perf_benchmark: returnArrayRecord 128,  385255,   18660,  365077,  381500,  402346,  675846
```

Where the `cppTests` test copies a 256-byte buffer in C++ while the `baseline`
test does nothing. They serve as baselines for comparison with djinni
marshalling overhead. All duration values are in nanoseconds.

## Build and install

Build with `bazel build perftest` in the perftest directory. Then install the
apk to the mobile device:`adb install ../bazel-bin/perftest/perftest.apk`.

## Running the benchmark

Run the `perftest` app on device, and press the `Run Tests` button. Then watch
output in logcat: `adb logcat |grep djinni_perf_benchmark`.
