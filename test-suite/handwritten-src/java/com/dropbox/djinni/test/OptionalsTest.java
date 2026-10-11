package com.dropbox.djinni.test;

import junit.framework.TestCase;

public class OptionalsTest extends TestCase {
    public void testMixedRecordTranslation() {
        MixedRecord rec = new MixedRecord(5, OptionalTestHelpers.getBaseRecord());
        rec.setInterfaceVal(OptionalTestHelpers.optionalInterface());

        MixedRecord expected = OptionalTestHelpers.getMixedRecord();
        assertEquals(rec.getReqInt(), expected.getReqInt());
        assertEquals(rec.getReqRecord().getVal(), expected.getReqRecord().getVal());
        assertEquals(rec.getOptRecord(), null);
        assertEquals(rec.getInterfaceVal(), expected.getInterfaceVal());
    }

    public void testOptionalRecordTranslation() {
        OptionalRecord rec = new OptionalRecord();
        rec.setOptInt(6);

        OptionalRecord expected = OptionalTestHelpers.getOptionalRecord();
        assertEquals(rec.getOptInt(), expected.getOptInt());
        assertEquals(rec.getOptInterface(), null);
        assertEquals(rec.getOptRecord(), null);
    }

    public void testTranslation() {
        MixedRecord rec = new MixedRecord(1, OptionalTestHelpers.getBaseRecord());
        rec.setOptRecord(new BaseRecord(5));

        MixedRecord translatedRec = OptionalTestHelpers.mixedRecordId(rec);
        assertEquals(rec.getReqInt(), translatedRec.getReqInt());
        assertEquals(rec.getReqRecord().getVal(), translatedRec.getReqRecord().getVal());
        assertEquals(rec.getOptRecord().getVal(), translatedRec.getOptRecord().getVal());
        assertEquals(rec.getInterfaceVal(), null);
    }
}
