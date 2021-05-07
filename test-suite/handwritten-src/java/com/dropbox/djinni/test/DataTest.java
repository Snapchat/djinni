package com.dropbox.djinni.test;
import junit.framework.TestCase;
import static org.junit.Assert.*;
import java.nio.ByteBuffer;

public class DataTest extends TestCase {
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
