package message;
import account.Account;
import message.Message;


public class TestMessage {
    private static String stripDate(Message m) {
        String s = m.toString();
        int newlineIndex = s.indexOf('\n');
        if(newlineIndex >= 0) s = s.substring(newlineIndex+1);
        return s;
    }
    public static void main(String[] args) {
        int result = 0;
        int vector = 1;
        String expected = "";
        String actual = "";

        Account a1 = new Account("Prof Rice");
        Account a2 = new Account("Excellent Student");
    
        // Nominal case
        Message m1 = new Message(a1, null, "This is message #1");
        
        if(m1.getRepliedTo() != null) {
            System.err.println("\nERROR: Nominal case null repliedTo");
            System.err.println("----Expected: null");
            System.err.println("----Actual:   \n" + m1.getRepliedTo());
            System.err.println("----");
            result |= vector;
        }
        if(m1.getReply(0) != null) {
            System.err.println("\nERROR: Nominal case null reply[0]");
            System.err.println("----Expected: null");
            System.err.println("----Actual:   \n" + m1.getReply(0));
            System.err.println("----");
            result |= vector;
        }
        expected = "From: Prof Rice (1)\n\nThis is message #1";
        actual = stripDate(m1).trim();
        if(!expected.equals(actual)) {
            System.err.println("\nERROR: Nominal case toString");
            System.err.println("----Expected: \n" + expected);
            System.err.println("----Actual:   \n" + m1);
            System.err.println("----");
            result |= vector;
        }
        vector <<= 1;
        
        // Reply case
        Message m2 = new Message(a2, m1, "This is message #2 in response to message #1");
        
        if(m2.getRepliedTo() != m1) {
            System.err.println("\nERROR: Replied case m2.repliedTo() not message #1");
            System.err.println("----Expected: \n" + m1);
            System.err.println("----Actual:   \n" + m2.getRepliedTo());
            System.err.println("----");
            result |= vector;
        }
        if(m1.getReply(0) != m2) {
            System.err.println("\nERROR: Replied case m1.getReply(0) not message #2");
            System.err.println("----Expected: null");
            System.err.println("----Actual:   \n" + m1.getReply(0));
            System.err.println("----");
            result |= vector;
        }
        if(m1.getReply(1) != null) {
            System.err.println("\nERROR: Replied case m1.getReply(1) not null");
            System.err.println("----Expected: null");
            System.err.println("----Actual:   \n" + m1.getReply(1));
            System.err.println("----");
            result |= vector;
        }
        expected = "From: Excellent Student (2)\nIn reply to: Prof Rice (1)\n\nThis is message #2 in response to message #1";
        actual = stripDate(m2).trim();
        if(!expected.equals(actual)) {
            System.err.println("\nERROR: Replied case toString message #2");
            System.err.println("----Expected: \n" + expected);
            System.err.println("----Actual:   \n" + actual);
            System.err.println("----");
            result |= vector;
        }
        expected = "From: Prof Rice (1)\nReplies: Excellent Student (2)\n\nThis is message #1";
        actual = stripDate(m1).trim();
        if(!expected.equals(actual)) {
            System.err.println("\nERROR: Replied case toString message #1");
            System.err.println("----Expected: \n" + expected);
            System.err.println("----Actual:   \n" + actual);
            System.err.println("----");
            result |= vector;
        }
        vector <<= 1;
        
        // Double reply case
        Message m3 = new Message(a1, m1, "This is message #3 in response to message #1");
        
        if(m3.getRepliedTo() != m1) {
            System.err.println("\nERROR: Double replied case m3.repliedTo() not message #1");
            System.err.println("----Expected: \n" + m1);
            System.err.println("----Actual:   \n" + m3.getRepliedTo());
            System.err.println("----");
            result |= vector;
        }
        if(m1.getReply(0) != m2) {
            System.err.println("\nERROR: Double replied case m1.getReply(0) not message #2");
            System.err.println("----Expected: null");
            System.err.println("----Actual:   \n" + m1.getReply(0));
            System.err.println("----");
            result |= vector;
        }
        if(m1.getReply(1) != m3) {
            System.err.println("\nERROR: Double replied case m1.getReply(0) not message #3");
            System.err.println("----Expected: null");
            System.err.println("----Actual:   \n" + m1.getReply(1));
            System.err.println("----");
            result |= vector;
        }
        if(m1.getReply(2) != null) {
            System.err.println("\nERROR: Double replied case m1.getReply(2) not null");
            System.err.println("----Expected: null");
            System.err.println("----Actual:   \n" + m1.getReply(2));
            System.err.println("----");
            result |= vector;
        }
        expected = "From: Prof Rice (1)\nIn reply to: Prof Rice (1)\n\nThis is message #3 in response to message #1";
        actual = stripDate(m3).trim();
        if(!expected.equals(actual)) {
            System.err.println("\nERROR: Double replied case toString message #3");
            System.err.println("----Expected: \n" + expected);
            System.err.println("----Actual:   \n" + actual);
            System.err.println("----");
            result |= vector;
        }
        expected = "From: Prof Rice (1)\nReplies: Excellent Student (2), Prof Rice (1)\n\nThis is message #1";
        actual = stripDate(m1).trim();
        if(!expected.equals(actual)) {
            System.err.println("\nERROR: Replied case toString message #1");
            System.err.println("----Expected: \n" + expected);
            System.err.println("----Actual:   \n" + actual);
            System.err.println("----");
            result |= vector;
        }
        vector <<= 1;
        
        // Show results
        if(result != 0) 
            System.err.println("\nFAIL: error code " + result);
        System.exit(result);
    }

}
