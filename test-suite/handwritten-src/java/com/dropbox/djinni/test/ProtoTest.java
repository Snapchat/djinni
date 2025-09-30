package com.dropbox.djinni.test;

import java.util.ArrayList;
import junit.framework.TestCase;
import djinni.test.Test.AddressBook;
import djinni.test.Test.Person;
import djinni.test.Test.Person.PhoneType;
import djinni.test.Test.Priority;
import com.dropbox.djinni.test.ProtoTests;
import com.dropbox.djinni.test.RecordWithEmbeddedProto;
import com.dropbox.djinni.test.RecordWithProtobufEnum;

public class ProtoTest extends TestCase {
    public void testJavaToNative() {
        Person p1 = Person.newBuilder().setName("tom").setId(1).build();
        Person p2 = Person.newBuilder().setName("jerry").setId(2).build();
        AddressBook proto = AddressBook.newBuilder().addPeople(p1).addPeople(p2).build();
        ArrayList<String> r = ProtoTests.protoToStrings(proto);
        assertEquals(r.get(0), "tom");
        assertEquals(r.get(1), "jerry");
    }

    public void testNativeToJava() {
        ArrayList<String> names = new ArrayList<String>();
        names.add("tom");
        names.add("jerry");
        AddressBook proto = ProtoTests.stringsToProto(names);
        assertEquals(proto.getPeopleList().get(0).getName(), "tom");
        assertEquals(proto.getPeopleList().get(1).getName(), "jerry");
    }

    public void testZeroSizeNativeToJava() {
        AddressBook proto = ProtoTests.stringsToProto(new ArrayList<String>());
        assertEquals(proto.getSerializedSize(), 0);
    }

    public void testEmbeddedProto() {
        Person p = Person.newBuilder().setName("tom").setId(1).build();
        RecordWithEmbeddedProto rec = new RecordWithEmbeddedProto(p);
        String s = ProtoTests.embeddedProtoToString(rec);
        assertEquals(s, p.getName());
        rec = ProtoTests.stringToEmbeddedProto(s);
        assertEquals(s, rec.getPerson().getName());
    }

    public void testBoxing() {
        Person p1 = Person.newBuilder().setName("tom").setId(1).build();
        Person p2 = Person.newBuilder().setName("jerry").setId(2).build();
        ArrayList<Person> plist = new ArrayList<>();
        plist.add(p1);
        plist.add(p2);
        ArrayList<String> slist = ProtoTests.protoListToStrings(plist);
        assertEquals(slist.get(0), "tom");
        assertEquals(slist.get(1), "jerry");

        plist = ProtoTests.stringsToProtoList(slist);
        assertEquals(plist.get(0).getName(), "tom");
        assertEquals(plist.get(1).getName(), "jerry");

        String s = ProtoTests.optionalProtoToString(p1);
        assertEquals(s, "tom");
        Person p = ProtoTests.stringToOptionalProto("tom");
        assertEquals(p.getName(), "tom");
    }

    public void testProtobufEnumConversion() {
        // Test converting each enum value to string and back
        String mobileStr = ProtoTests.phoneTypeToString(PhoneType.MOBILE);
        assertEquals(mobileStr, "MOBILE");
        PhoneType mobile = ProtoTests.stringToPhoneType("MOBILE");
        assertEquals(mobile, PhoneType.MOBILE);

        String homeStr = ProtoTests.phoneTypeToString(PhoneType.HOME);
        assertEquals(homeStr, "HOME");
        PhoneType home = ProtoTests.stringToPhoneType("HOME");
        assertEquals(home, PhoneType.HOME);

        String workStr = ProtoTests.phoneTypeToString(PhoneType.WORK);
        assertEquals(workStr, "WORK");
        PhoneType work = ProtoTests.stringToPhoneType("WORK");
        assertEquals(work, PhoneType.WORK);
    }

    public void testProtobufEnumRoundTrip() {
        // Test all enum values survive round-trip conversion
        PhoneType[] phoneTypes = {PhoneType.MOBILE, PhoneType.HOME, PhoneType.WORK};
        
        for (PhoneType original : phoneTypes) {
            String str = ProtoTests.phoneTypeToString(original);
            PhoneType converted = ProtoTests.stringToPhoneType(str);
            assertEquals(original, converted);
        }
    }

    public void testProtobufEnumInRecord() {
        // Test enum embedded in a record
        RecordWithProtobufEnum mobileRecord = new RecordWithProtobufEnum(PhoneType.MOBILE, Priority.LOW);
        String mobileStr = ProtoTests.enumRecordToString(mobileRecord);
        assertEquals(mobileStr, "MOBILE");
        
        RecordWithProtobufEnum convertedRecord = ProtoTests.stringToEnumRecord("MOBILE");
        assertEquals(convertedRecord.getPhoneType(), PhoneType.MOBILE);

        // Test different enum values
        RecordWithProtobufEnum homeRecord = new RecordWithProtobufEnum(PhoneType.HOME, Priority.LOW);
        String homeStr = ProtoTests.enumRecordToString(homeRecord);
        assertEquals(homeStr, "HOME");
        
        RecordWithProtobufEnum convertedHomeRecord = ProtoTests.stringToEnumRecord("HOME");
        assertEquals(convertedHomeRecord.getPhoneType(), PhoneType.HOME);

        RecordWithProtobufEnum workRecord = new RecordWithProtobufEnum(PhoneType.WORK, Priority.LOW);
        String workStr = ProtoTests.enumRecordToString(workRecord);
        assertEquals(workStr, "WORK");
        
        RecordWithProtobufEnum convertedWorkRecord = ProtoTests.stringToEnumRecord("WORK");
        assertEquals(convertedWorkRecord.getPhoneType(), PhoneType.WORK);
    }

    public void testProtobufEnumRecordRoundTrip() {
        // Test record with enum survives round-trip conversion
        PhoneType[] phoneTypes = {PhoneType.MOBILE, PhoneType.HOME, PhoneType.WORK};
        
        for (PhoneType phoneType : phoneTypes) {
            RecordWithProtobufEnum original = new RecordWithProtobufEnum(phoneType, Priority.LOW);
            String str = ProtoTests.enumRecordToString(original);
            RecordWithProtobufEnum converted = ProtoTests.stringToEnumRecord(str);
            assertEquals(original.getPhoneType(), converted.getPhoneType());
        }
    }

    public void testTopLevelProtobufEnumConversion() {
        // Test converting each top-level enum value to string and back
        String lowStr = ProtoTests.priorityToString(Priority.LOW);
        assertEquals(lowStr, "LOW");
        Priority low = ProtoTests.stringToPriority("LOW");
        assertEquals(low, Priority.LOW);

        String mediumStr = ProtoTests.priorityToString(Priority.MEDIUM);
        assertEquals(mediumStr, "MEDIUM");
        Priority medium = ProtoTests.stringToPriority("MEDIUM");
        assertEquals(medium, Priority.MEDIUM);

        String highStr = ProtoTests.priorityToString(Priority.HIGH);
        assertEquals(highStr, "HIGH");
        Priority high = ProtoTests.stringToPriority("HIGH");
        assertEquals(high, Priority.HIGH);

        String urgentStr = ProtoTests.priorityToString(Priority.URGENT);
        assertEquals(urgentStr, "URGENT");
        Priority urgent = ProtoTests.stringToPriority("URGENT");
        assertEquals(urgent, Priority.URGENT);
    }

    public void testTopLevelProtobufEnumRoundTrip() {
        // Test enum round-trip conversion
        Priority[] priorities = {Priority.LOW, Priority.MEDIUM, Priority.HIGH, Priority.URGENT};
        
        for (Priority priority : priorities) {
            String str = ProtoTests.priorityToString(priority);
            Priority converted = ProtoTests.stringToPriority(str);
            assertEquals(priority, converted);
        }
    }

    public void testTopLevelProtobufEnumInRecord() {
        // Test record containing top-level enum - test priority field access
        RecordWithProtobufEnum lowRecord = new RecordWithProtobufEnum(PhoneType.MOBILE, Priority.LOW);
        assertEquals(lowRecord.getPriority(), Priority.LOW);
        
        RecordWithProtobufEnum urgentRecord = new RecordWithProtobufEnum(PhoneType.HOME, Priority.URGENT);
        assertEquals(urgentRecord.getPriority(), Priority.URGENT);

        // Test that both enum fields work in the same record
        RecordWithProtobufEnum mixedRecord = new RecordWithProtobufEnum(PhoneType.WORK, Priority.HIGH);
        assertEquals(mixedRecord.getPhoneType(), PhoneType.WORK);
        assertEquals(mixedRecord.getPriority(), Priority.HIGH);
    }

    public void testTopLevelProtobufEnumRecordRoundTrip() {
        // Test both enum fields in record round-trip
        Priority[] priorities = {Priority.LOW, Priority.MEDIUM, Priority.HIGH, Priority.URGENT};
        PhoneType[] phoneTypes = {PhoneType.MOBILE, PhoneType.HOME, PhoneType.WORK};
        
        for (int i = 0; i < priorities.length; i++) {
            PhoneType phoneType = phoneTypes[i % phoneTypes.length];
            Priority priority = priorities[i];
            
            RecordWithProtobufEnum original = new RecordWithProtobufEnum(phoneType, priority);
            // Test that priority field is preserved
            assertEquals(original.getPriority(), priority);
            assertEquals(original.getPhoneType(), phoneType);
        }
    }
}
