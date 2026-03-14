package message;

import account.Account;
import java.util.List;
import java.util.ArrayList;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;

public class DirectMessage extends Message {
    private Account recipient;

    public DirectMessage(Account author, Account recipient, Message repliedTo, String body) {
        super(author, repliedTo, body);
        this.recipient = recipient;
    }

    public DirectMessage(BufferedReader in, List<Account> accounts, List<Group> groups) throws IOException {
        super((Account) null, (Message) null, (String) null);
        String authorName = in.readLine();
        this.author = findAccount(accounts, authorName);
        long time = Long.parseLong(in.readLine());
        this.date = new java.util.Date(time);
        String recipientName = in.readLine();
        this.recipient = findAccount(accounts, recipientName);
        this.body = in.readLine();
        int numReplies = Integer.parseInt(in.readLine());
        this.replies = new ArrayList<>();
        for (int i = 0; i < numReplies; i++) {
            Message r = Message.read(in, accounts, groups);
            if (r != null) {
                r.repliedTo = this;
                replies.add(r);
            }
        }
    }

    public void save(BufferedWriter out) throws IOException {
        out.write("DIRECT");
        out.newLine();
        out.write(author.getName());
        out.newLine();
        out.write(Long.toString(date.getTime()));
        out.newLine();
        out.write(recipient.getName());
        out.newLine();
        out.write(body == null ? "" : body);
        out.newLine();
        out.write(Integer.toString(replies.size()));
        out.newLine();
        for (Message r : replies) {
            r.save(out);
        }
    }
}
