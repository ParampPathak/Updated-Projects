package message;
import message.Group;
public class TestGroup {
    public static void main(String[] args) {
        Group g1 = new Group("Java Developers");
        System.out.println("Group Created: " + g1);
        g1.disable();
        System.out.println("After disable: " + g1);
        g1.enable();
        System.out.println("After enable: " + g1);
        try {
            Group g2 = new Group("");
        } catch (IllegalArgumentException e) {
            System.out.println("Caught expected exception for empty group name.");
        }
    }
}
