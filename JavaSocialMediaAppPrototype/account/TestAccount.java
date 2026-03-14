package account;
public class TestAccount {
    public static void main(String[] args) {
        int result = 0;
        int vector = 1;
        String expected = "";
        String actual = "";
    
        // Nominal case
        Account a1 = new Account("Prof Rice");
        if(a1.isMuted() || a1.isBlocked()) {
            System.err.println("\nERROR: Nominal case default status");
            System.err.println("    Expected: false false");
            System.err.println("    Actual:   " + a1.isMuted() + " " + a1.isBlocked());
            result |= vector;
        }
        expected = "Prof Rice (1)";
        actual = a1.toString().trim();
        if(!expected.equals(actual)) {
            System.err.println("\nERROR: Nominal case toString");
            System.err.println("    Expected: " + expected);
            System.err.println("    Actual:   " + a1);
            result |= vector;
        }
        vector <<= 1;
        
        // Blocked case
        a1.setStatus(AccountStatus.Blocked);
        if(!a1.isMuted() || !a1.isBlocked()) {
            System.err.println("\nERROR: Blocked case status");
            System.err.println("    Expected: true true");
            System.err.println("    Actual:   " + a1.isMuted() + " " + a1.isBlocked());
            result |= vector;
        }
        expected = "Prof Rice (1) [Blocked]";
        actual = a1.toString().trim();
        if(!expected.equals(actual)) {
            System.err.println("\nERROR: Blocked case toString");
            System.err.println("    Expected: " + expected);
            System.err.println("    Actual:   " + a1);
            result |= vector;
        }
        vector <<= 1;
        
        // Muted case
        a1.setStatus(AccountStatus.Muted);
        if(!a1.isMuted() || a1.isBlocked()) {
            System.err.println("\nERROR: Muted case muted status");
            System.err.println("    Expected: true false");
            System.err.println("    Actual:   " + a1.isMuted() + " " + a1.isBlocked());
            result |= vector;
        }
        expected = "Prof Rice (1) [Muted]";
        actual = a1.toString().trim();
        if(!expected.equals(actual)) {
            System.err.println("\nERROR: Muted case toString");
            System.err.println("    Expected: " + expected);
            System.err.println("    Actual:   " + a1);
            result |= vector;
        }
        vector <<= 1;
        
        // Incrementing ID case
        a1 = new Account("Excellent Student");
        expected = "Excellent Student (2)";
        actual = a1.toString().trim();
        if(!expected.equals(actual)) {
            System.err.println("\nERROR: Incrementing ID case toString");
            System.err.println("    Expected: " + expected);
            System.err.println("    Actual:   " + a1);
            result |= vector;
        }
        vector <<= 1;

        // Zero-length name case
        try {
            a1 = new Account("");
            System.err.println("\nERROR: Zero-length name case");
            System.err.println("    Expected: IllegalArgumentException");
            System.err.println("    Actual:   (No exception)");
            result |= vector;
        } catch(IllegalArgumentException e) { // This is the CORRECT path!
        } catch(Exception e) {
            System.err.println("\nERROR: Zero-length name case");
            System.err.println("    Expected: IllegalArgumentException");
            System.err.println("    Actual:   " + e.getClass().getSimpleName());
            result |= vector;
        }
        vector <<= 1;

        // Show results
        if(result != 0) 
            System.err.println("\nFAIL: error code " + result);
        System.exit(result);
    }

}
