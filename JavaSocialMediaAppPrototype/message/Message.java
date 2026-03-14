package message;
import account.Account;
import message.Group;
import java.util.Date;
import java.util.ArrayList;
import java.util.List;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
public class Message {
    protected Account author;
    protected Date date;
    protected Message repliedTo;
    protected List<Message> replies;
    protected String body;
    public Message(Account author, Message repliedTo, String body) {
        this.author = author;
        this.date = new Date();
        this.repliedTo = repliedTo;
        if (repliedTo != null) {
            repliedTo.addReply(this);
        }
        this.replies = new ArrayList<>();
        this.body = body;
    }
    public Message(BufferedReader in, List<Account> accounts, List<Group> groups, Message repliedTo) throws IOException {
        String authorName = in.readLine();
        this.author = findAccount(accounts, authorName);
        long time = Long.parseLong(in.readLine());
        this.date = new Date(time);
        this.body = in.readLine();
        this.repliedTo = repliedTo;
        if (this.repliedTo != null) {
            this.repliedTo.addReply(this);
        }
        int numReplies = Integer.parseInt(in.readLine());
        this.replies = new ArrayList<>();
        for (int i = 0; i < numReplies; i++) {
            String className = in.readLine();
            Message child;
            if (className.equals("message.Post") || className.equals("POST")) {
                child = new Post(in, accounts, groups);
            } else if (className.equals("message.DirectMessage") || className.equals("DIRECT")) {
                child = new DirectMessage(in, accounts, groups);
            } else {
                child = new Message(in, accounts, groups, this);
            }
            replies.add(child);
        }
    }
    public Message getRepliedTo() {
        return repliedTo;
    }
    public int getNumReplies() {
        return replies.size();
    }
    public Message getReply(int index) {
        if (index >= 0 && index < replies.size()) {
            return replies.get(index);
        }
        return null;
    }
    public List<Message> getReplies() {
        return replies;
    }
    public void addReply(Message m) {
        replies.add(m);
    }
    public void save(BufferedWriter out) throws IOException {
        out.write("MESSAGE");
        out.newLine();
        out.write(author.getName());
        out.newLine();
        out.write(Long.toString(date.getTime()));
        out.newLine();
        out.write(body == null ? "" : body);
        out.newLine();
        out.write(Integer.toString(replies.size()));
        out.newLine();
        for (Message r : replies) {
            out.write(r.getClass().getName());
            out.newLine();
            r.save(out);
        }
    }
    public static Message read(BufferedReader in, List<Account> accounts, List<Group> groups) throws IOException {
        String type = in.readLine();
        if (type == null) return null;
        if (type.equals("MESSAGE")) {
            return new Message(in, accounts, groups, null);
        } else if (type.equals("message.Post") || type.equals("POST")) {
            return new Post(in, accounts, groups);
        } else if (type.equals("message.DirectMessage") || type.equals("DIRECT")) {
            return new DirectMessage(in, accounts, groups);
        }
        return null;
    }
    protected static Account findAccount(List<Account> list, String name) {
        for (Account a : list) {
            if (a.getName().equals(name)) {
                return a;
            }
        }
        return null;
    }
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("Date: ").append(date).append('\n');
        sb.append("From: ").append(author).append('\n');
        if (repliedTo != null) {
            sb.append("In reply to: ").append(repliedTo.author).append('\n');
        }
        if (!replies.isEmpty()) {
            sb.append("Replies: ");
            String sep = "";
            for (int i = 0; i < replies.size(); i++) {
                sb.append(sep).append("[").append(i).append("] ").append(replies.get(i).author);
                sep = ", ";
            }
            sb.append('\n');
        }
        sb.append('\n').append(body).append('\n');
        return sb.toString();
    }
}
