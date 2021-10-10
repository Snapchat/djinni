package com.snapchat.djinni.benchmark

import android.app.Activity;
import android.os.Bundle
import android.util.Log
import android.view.View
import com.snapchat.djinni.benchmark.DjinniPerfBenchmark
import com.snapchat.djinni.benchmark.EnumSixValue
import com.snapchat.djinni.benchmark.ObjectPlatform
import com.snapchat.djinni.benchmark.RecordSixInt
import java.io.File
import java.nio.ByteBuffer
import kotlin.math.roundToInt
import kotlin.math.roundToLong

internal class ObjectPlatformImpl : ObjectPlatform() {
    override fun onDone() {
    }
}

class MainActivity : Activity() {

    val tag = "djinni_perf_benchmark"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        System.loadLibrary("perftest");

        findViewById<View>(R.id.runtests).setOnClickListener {
            runTestSuite()
        }
    }

    private fun calculateAverage(samples: DoubleArray): Double {
        var sum = 0.0

        for (sample in samples) {
            sum += sample
        }

        return sum / samples.size
    }

    private fun calculateSD(samples: DoubleArray): Double {
        val average = calculateAverage(samples)

        var sum = 0.0

        for (sample in samples) {
            sum += Math.pow(sample - average, 2.0)
        }

        return Math.sqrt(sum / samples.size)
    }

    // 0.0 <= percentile <= 1.0
    private fun percentileFromSortedArray(samples: DoubleArray, percentile: Double) : Double {
        return samples[((samples.size - 1) * percentile).roundToInt()]
    }

    private fun measure(name: String,
                        lambda: () -> Unit, times: Int = 1000) {
        // Implementation of System.nanoTime()
        // is in https://android.googlesource.com/platform/libcore/+/7757924/luni/src/main/native/java_lang_System.cpp

        val samples = DoubleArray(times)
        var i = 0
        repeat (times) {
            val t1 = System.nanoTime()
            lambda()
            val t2 = System.nanoTime()
            val diffNano = t2 - t1
            samples[i++] = 0.0 + diffNano
         }

        samples.sort()

        val average = calculateAverage(samples).roundToLong()
        val sd = calculateSD(samples).roundToLong()
        val min = percentileFromSortedArray(samples, 0.0).roundToLong()
        val p50 = percentileFromSortedArray(samples, 0.5).roundToLong()
        val p90 = percentileFromSortedArray(samples, 0.9).roundToLong()
        val max = percentileFromSortedArray(samples, 1.0).roundToLong()

        Log.i(tag, "${name.padEnd(21)},${average.toString().padStart(8)},${sd.toString().padStart(8)},${min.toString().padStart(8)},${p50.toString().padStart(8)},${p90.toString().padStart(8)},${max.toString().padStart(8)}");
    }

    private fun runTestSuitePerf(dpb: DjinniPerfBenchmark) {
        val minCount = 16 // Note hard-coded string below of this size
        val lowCount = 128
        val highCount = 4096
        val hugeCount = 16384

        var ns = 0L

        Log.i(tag, "name                 ,     avg,      sd,     min,     p50,     p90,     max");

        measure("cppTests", {ns = dpb.cppTests()})
        measure("baseline", {dpb.baseline()})

        for (count in listOf(lowCount, highCount, hugeCount)) {
            val bb = ByteBuffer.allocateDirect(count)
            for (i in 0..count - 1) bb.put(i, i.toByte())
            measure("argDataView " + count, {dpb.argDataView(bb)})
        }

        for (count in listOf(lowCount, highCount, hugeCount)) {
            val bb = ByteBuffer.allocateDirect(count)
            for (i in 0..count - 1) bb.put(i, i.toByte())
            measure("argDataRef " + count, {dpb.argDataRef(bb)})
        }

        val s = "1234567890ABCDEF" // minCount
        measure("argString " + minCount, {dpb.argString(s)})

        var sl = String()
        for (i in 0..lowCount / minCount) sl = sl + s
        measure("argString " + lowCount, {dpb.argString(sl)})

        var sh = String()
        for (i in 0..highCount / lowCount) sh = sh + sl
        measure("argString " + highCount, {dpb.argString(sh)})

        var sg = String()
        for (i in 0..hugeCount / highCount) sg = sg + sh
        measure("argString " + hugeCount, {dpb.argString(sg)})

        for (count in listOf(lowCount, highCount, hugeCount)) {
            val ba = ByteArray(count, { it.toByte() })
            measure("argBinary " + count, {dpb.argBinary(ba)})
        }

        val e = EnumSixValue.FIRST
        measure("argEnumSixValue", {dpb.argEnumSixValue(e)})

        val r = RecordSixInt(1,2,3,4,5,6)
        measure("argRecordSixInt", {dpb.argRecordSixInt(r)})

        val li = ArrayList<Long>(lowCount)
        for (i in 0..lowCount - 1) li.add(0L + i)
        measure("argListInt " + lowCount, {dpb.argListInt(li)})

        var ai = LongArray(lowCount) { it * 1L }
        measure("argArrayInt " + lowCount, {dpb.argArrayInt(ai)})
        
        val o = ObjectPlatformImpl()
        measure("argObject", {dpb.argObject(o)})

        val lo = ArrayList<ObjectPlatform>(lowCount)
        for (i in 0..lowCount - 1) lo.add(ObjectPlatformImpl())
        measure("argListObject " + lowCount, {dpb.argListObject(lo)})

        val lr = ArrayList<RecordSixInt>(lowCount)
        for (i in 0..lowCount - 1) lr.add(RecordSixInt(1, 2, 3, 4, 5, 6))
        measure("argListRecord " + lowCount, {dpb.argListRecord(lr)})

        val ar = ArrayList<RecordSixInt>(lowCount)
        for (i in 0..lowCount - 1) ar.add(RecordSixInt(1, 2, 3, 4, 5, 6))
        measure("argArrayRecord " + lowCount, {dpb.argArrayRecord(ar)})

        measure("returnInt", {val ri = dpb.returnInt(42)})

        for (count in listOf(1, 10, lowCount)) {
            measure("returnListInt " + count, { val rli = dpb.returnListInt(count)})
        }

        for (count in listOf(1, 10, lowCount)) {
            measure("returnArrayInt " + count, { val rai = dpb.returnArrayInt(count)})
        }

        for (count in listOf(minCount, lowCount, highCount, hugeCount)) {
            measure("returnString " + count, { val rs = dpb.returnString(count)})
        }

        for (count in listOf(lowCount, highCount, hugeCount)) {
            measure("returnBinary " + count, { val rb = dpb.returnBinary(count)})
        }

        measure("returnObject", { val ro = dpb.returnObject()})

        for (count in listOf(10, 100)) {
            measure("returnListObject " + count, { val rlo = dpb.returnListObject(count)})
        }

        for (count in listOf(1, 10, lowCount)) {
            measure("returnListRecord " + count, { val rlr = dpb.returnListRecord(count)})
        }

        for (count in listOf(1, 10, lowCount)) {
            measure("returnArrayRecord " + count, { val rar = dpb.returnArrayRecord(count)})
        }
    }

    private fun roundTrip(dpb: DjinniPerfBenchmark, testValue: String) {
        val outputValue = dpb.roundTripString(testValue)
        val good = outputValue == testValue
        Log.d(tag, "roundTripString " + "good: " + good + ", in: " + testValue +  ", out: " + outputValue)
    }

    private fun runTestSuiteUTF(dpb: DjinniPerfBenchmark)
    {
        // This test should only break if an Android implementation didn't return zero-terminated strings.
        val size = 42
        val s = dpb.returnString(size)
        Log.d(tag, "returnString " + "received size: " + s.length + " expected size: " + size)

        // Verify that complex UTF-8 characters are handled correctly.
        val pileOfPoo = "\uD83D\uDCA9" // 💩
        roundTrip(dpb, pileOfPoo)
        val aCapon = "𠜎"
        roundTrip(dpb, aCapon)

        // Some selections from https://github.com/microsoft/regexp-i18n/blob/master/test/RegExpI18n.spec.ts
        val arabic = "العربية"
        roundTrip(dpb, arabic)
        val greek = "Ελληνικό"
        roundTrip(dpb, greek)
        val tibetan = "ས藏文有三十个辅音字母具体如下表所示འ་ཆུངམཚམསམཐུན་འཚམསའཐུནདགེ་འདུནསྤྱིར་藏文音节འགྲེམས་སྟོན་结构规则要分析藏文结构必须先得找出根字母然后其他的部分根据结构规则就能找到"
        roundTrip(dpb, tibetan)
        val georgian = "ქართული"
        roundTrip(dpb, georgian)
        val syriac = "ܣܘܪܝܬ"
        roundTrip(dpb, syriac)
    }

    private fun runTestSuite() {
        val dpb = DjinniPerfBenchmark.getInstance()
        if (dpb != null) {
            runTestSuitePerf(dpb)
            runTestSuiteUTF(dpb)
        }
    }
}
