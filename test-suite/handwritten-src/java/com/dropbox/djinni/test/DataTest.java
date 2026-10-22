package com.dropbox.djinni.test;
import junit.framework.TestCase;
import static org.junit.Assert.*;
import com.snapchat.djinni.DataRefHelper;
import java.nio.ByteBuffer;

public class DataTest extends TestCase {
    private static final int TWO_MB = 2 * 1024 * 1024;

    DataRefTest test;

    @Override
    protected void setUp() {
        test = DataRefTest.create();
    }

    public void testSend() {
        byte[] input = new byte[]{0, 1, 2, 3};
        ByteBuffer buf = ByteBuffer.allocateDirect(4);
        buf.put(input);
        test.sendData(buf);
        byte[] output = test.retriveAsBin();
        assertArrayEquals(input, output);
    }

    public void testSendMutable() {
        byte[] input = new byte[]{0, 1, 2, 3};
        ByteBuffer buf = ByteBuffer.allocateDirect(4);
        buf.put(input);
        test.sendMutableData(buf);
        buf.rewind();
        byte[] output = new byte[4];
        buf.get(output);
        assertArrayEquals(new byte[]{3, 2, 1, 0}, output);
    }

    public void testGenerateData() {
        ByteBuffer buf = test.generateData();
        byte[] output = new byte[4];
        buf.get(output);
        assertArrayEquals(new byte[]{0, 1, 2, 3}, output);
    };

    public void testDataFromVec() {
        ByteBuffer buf = test.dataFromVec();
        byte[] output = new byte[4];
        buf.get(output);
        assertArrayEquals(new byte[]{0, 1, 2, 3}, output);
    }

    public void testDataFromStr() {
        ByteBuffer buf = test.dataFromStr();
        byte[] output = new byte[4];
        buf.get(output);
        assertArrayEquals(new byte[]{'a', 'b', 'c', 'd'}, output);
    }

    public void testPreferNativeMemoryIsOffByDefault() {
        long before = DataRefHelper.getNativeAllocationBytes();
        ByteBuffer buf = test.generateDataPreferNative(TWO_MB);
        assertEquals(TWO_MB, buf.capacity());
        assertEquals(before, DataRefHelper.getNativeAllocationBytes());
    }

    public void testPreferNativeMemoryStaysWithinLimit() {
        long before = DataRefHelper.getNativeAllocationBytes();
        DataRefHelper.setNativeAllocationLimit(before + 3 * 1024 * 1024);
        try {
            ByteBuffer first = test.generateDataPreferNative(TWO_MB);
            assertEquals(before + TWO_MB, DataRefHelper.getNativeAllocationBytes());
            assertTrue(first.isDirect());
            assertEquals(0, first.get(TWO_MB - 1));
            first.put(TWO_MB - 1, (byte) 7);
            assertEquals(7, first.get(TWO_MB - 1));

            ByteBuffer second = test.generateDataPreferNative(TWO_MB);
            assertEquals(TWO_MB, second.capacity());
            assertEquals(before + TWO_MB, DataRefHelper.getNativeAllocationBytes());

            ByteBuffer small = test.generateDataPreferNative(1024);
            assertEquals(1024, small.capacity());
            assertEquals(before + TWO_MB, DataRefHelper.getNativeAllocationBytes());
        } finally {
            DataRefHelper.setNativeAllocationLimit(0);
        }
    }

    public void testNegativeNativeAllocationLimitThrows() {
        try {
            DataRefHelper.setNativeAllocationLimit(-1);
            fail();
        } catch (IllegalArgumentException expected) {
        }
    }

    public void testSendDataView() {
        byte[] input = new byte[]{0, 1, 2, 3};
        ByteBuffer buf = ByteBuffer.allocateDirect(4);
        buf.put(input);
        byte[] output = test.sendDataView(buf);
        assertArrayEquals(input, output);
    }

    public void testRecvDataView() {
        ByteBuffer buf = test.recvDataView();
        byte[] output = new byte[4];
        buf.get(output);
        assertArrayEquals(new byte[]{0, 1, 2, 3}, output);
    }
}
