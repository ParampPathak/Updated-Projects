package abuta;
import menu.Menu;
import menu.MenuItem;
import message.Message;
import message.Post;
import message.DirectMessage;
import message.Group;
import account.Account;
import java.util.List;
import java.util.ArrayList;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
public class Abuta {
    private List<Account> accounts;
    private List<Group> groups;
    private Message message;
    private Menu menu;
    private String output = "";
    private boolean running = true;
    public static void main(String[] args) {
        new Abuta().mdi();
    }
    public Abuta() {
        accounts = new ArrayList<>();
        groups = new ArrayList<>();
        accounts.add(new Account("Gandalf"));
        accounts.add(new Account("Frodo"));
        accounts.add(new Account("Mark"));
        accounts.add(new Account("Param"));
        accounts.add(new Account("Harry Potter"));
        groups.add(new Group("Power"));
        groups.add(new Group("Wisdom"));
        groups.add(new Group("Courage"));
        groups.add(new Group("Light"));
        groups.add(new Group("Darkness"));
        message = new Post(accounts.get(0), groups.get(0), "Welcome to your new social media experience!", null);
        menu = new Menu();
        menu.addMenuItem(new MenuItem("New Abuta", this::newAbuta));
        menu.addMenuItem(new MenuItem("Show Replied-To Message", this::showRepliedTo));
        menu.addMenuItem(new MenuItem("Show Reply", this::showReply));
        menu.addMenuItem(new MenuItem("Reply to Message", this::reply));
        menu.addMenuItem(new MenuItem("Save", this::save));
        menu.addMenuItem(new MenuItem("Save As", this::saveAs));
        menu.addMenuItem(new MenuItem("Open", this::open));
        menu.addMenuItem(new MenuItem("Exit", this::endApp));
    }
    private void mdi() {
        while (running) {
            System.out.println("\n" + message);
            System.out.println(output);
            output = "";
            Integer choice = Menu.getInt(menu.toString() + "\nSelection: ", null, null);
            if (choice == null) {
                System.out.println("Invalid or canceled input.");
                continue;
            }
            if (choice < 0 || choice > 7) {
                System.out.println("Please select a valid option [0..7].");
                continue;
            }
            menu.run(choice);
        }
    }
    private void newAbuta() {
        message = new Post(accounts.get(0), groups.get(0), "Welcome to your new social media experience!", null);
        output = "New Abuta session started.";
    }
    private void endApp() {
        running = false;
    }
    private void showRepliedTo() {
        if (message.getRepliedTo() == null) {
            output = "This message was not in reply to any other message.";
        } else {
            message = message.getRepliedTo();
        }
    }
    private void showReply() {
        if (message.getNumReplies() == 0) {
            output = "This message has no replies.";
        } else if (message.getNumReplies() == 1) {
            message = message.getReply(0);
        } else {
            Integer choice = Menu.selectItemFromList("Choose a reply:", message.getReplies(), null, null);
            if (choice == null) {
                output = "Canceled or invalid selection.";
            } else {
                message = message.getReply(choice);
            }
        }
    }
    private void reply() {
        char type = Menu.getChar("Enter 'P' for Post or 'D' for DirectMessage: ", null, null);
        if (type == 'P' || type == 'p') {
            int index = Menu.selectItemFromList("Select Author:", accounts, null, null);
            Account author = accounts.get(index);
            index = Menu.selectItemFromList("Select Group:", groups, null, null);
            Group group = groups.get(index);
            String text = Menu.getString("Enter message text: ", null, null);
            Message newMessage = new Post(author, group, text, message);
            message.addReply(newMessage);
            output = "Post reply added!";
        } else if (type == 'D' || type == 'd') {
            int index = Menu.selectItemFromList("Select Author:", accounts, null, null);
            Account author = accounts.get(index);
            index = Menu.selectItemFromList("Select Recipient:", accounts, null, null);
            Account recipient = accounts.get(index);
            String text = Menu.getString("Enter message text: ", null, null);
            Message newMessage = new DirectMessage(author, recipient, message, text);
            message.addReply(newMessage);
            output = "DirectMessage reply added!";
        } else {
            output = "Invalid message type. Reply canceled.";
        }
    }
    private void save() {
        String filename = Menu.getString("Enter filename to save to: ", null, null);
        if (filename == null) {
            output = "Canceled or invalid filename.";
            return;
        }
        try (BufferedWriter bw = new BufferedWriter(new FileWriter(filename))) {
            Message root = message;
            while (root.getRepliedTo() != null) {
                root = root.getRepliedTo();
            }
            root.save(bw);
            output = "Saved successfully!";
        } catch (IOException e) {
            output = "Error saving: " + e.getMessage();
        }
    }
    private void saveAs() {
        String newFilename = Menu.getString("Enter new filename to save as: ", null, null);
        if (newFilename == null) {
            output = "Canceled or invalid filename.";
            return;
        }
        try (BufferedWriter bw = new BufferedWriter(new FileWriter(newFilename))) {
            Message root = message;
            while (root.getRepliedTo() != null) {
                root = root.getRepliedTo();
            }
            root.save(bw);
            output = "Saved successfully!";
        } catch (IOException e) {
            output = "Error saving: " + e.getMessage();
        }
    }
    private void open() {
        String filename = Menu.getString("Enter filename to open: ", null, null);
        if (filename == null) {
            output = "Canceled or invalid filename.";
            return;
        }
        try (BufferedReader br = new BufferedReader(new FileReader(filename))) {
            message = Message.read(br, accounts, groups);
            output = "Opened successfully!";
        } catch (IOException e) {
            output = "Error opening: " + e.getMessage();
        }
    }
}
