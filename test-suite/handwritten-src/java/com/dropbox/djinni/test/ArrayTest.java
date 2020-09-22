package com.dropbox.djinni.test;

import junit.framework.TestCase;
import static org.junit.Assert.*;

public class ArrayTest extends TestCase {
    public void test() {
        String[] inputStrings = new String[]{"1", "2", "3"};
        String[] expectedStrings = new String[]{"3", "2", "1"};
        assertArrayEquals(TestArray.testStringArray(inputStrings), expectedStrings);

        int[] inputIntegers = new int[]{1, 2, 3};
        int[] expectedIntegers = new int[]{3, 2, 1};
        assertArrayEquals(TestArray.testIntArray(inputIntegers), expectedIntegers);

        Vec2[] inputRecords = new Vec2[]{new Vec2(1, 1), new Vec2(2, 2), new Vec2(3,3)};
        Vec2[] expectedRecords = new Vec2[]{new Vec2(3, 3), new Vec2(2, 2), new Vec2(1,1)};
        assertArrayEquals(TestArray.testRecordArray(inputRecords), expectedRecords);

        int[][] inputArrays = new int[][]{new int[] {1,1,1}, new int[] {2,2,2}, new int[] {3,3,3}};
        int[][] expectedArrays = new int[][]{new int[] {3,3,3}, new int[] {2,2,2}, new int[] {1,1,1}};
        assertArrayEquals(TestArray.testArrayOfArray(inputArrays), expectedArrays);
    }
}
