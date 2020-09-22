package com.dropbox.djinni.test;

import java.util.ArrayList;
import junit.framework.TestCase;
import djinni.test.Test.AddressBook;
import djinni.test.Test.Person;
import com.snapchat.djinni.Outcome;

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

        Outcome<Person, Integer> r = ProtoTests.stringToProtoOutcome("tom");
        assertEquals(r.result().get().getName(), "tom");
    }
}
