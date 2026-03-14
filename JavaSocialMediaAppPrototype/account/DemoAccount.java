package account;
import java.util.ArrayList;
import java.util.Scanner;

public class DemoAccount {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        ArrayList<Account> accounts = new ArrayList<>();
        
        String username = "";

        System.out.println("Enter account names, one per line.");
        System.out.println("Press Ctrl-z (Windows), Cmd-d (Mac), or Ctrl-d (Linux) to exit\n");
        
        while(scanner.hasNextLine()) {
            try {
                accounts.add(new Account(scanner.nextLine()));
            } catch (Exception e) {
                System.err.println("\nThat account name wasn't valid! Try again:\n");
            }
        }
        
        if(accounts.size() > 2) {
            accounts.get(0).setStatus(AccountStatus.Muted);
            accounts.get(1).setStatus(AccountStatus.Blocked);
            System.out.println("\nI muted the first account and blocked the second!");
        }
        
        System.out.println("\nHere are the accounts you just created:\n");
        
        for(Account a : accounts) System.out.println(a.toString());
    }
}
