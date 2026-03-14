package message;

import account.Account;
import java.util.List;
import java.util.ArrayList;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;

public class Post extends Message {
    private Group group;

    public Post(Account author, Group group, String body, Message repliedTo) {
        super(author, repliedTo, body);
        this.group = group;
    }

    public Post(BufferedReader in, List<Account> accounts, List<Group> groups) throws IOException {
        super((Account) null, (Message) null, (String) null);
        String authorName = in.readLine();
        this.author = findAccount(accounts, authorName);
        long time = Long.parseLong(in.readLine());
        this.date = new java.util.Date(time);
        String groupName = in.readLine();
        this.group = findGroup(groups, groupName);
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
        out.write("POST");
        out.newLine();
        out.write(author.getName());
        out.newLine();
        out.write(Long.toString(date.getTime()));
        out.newLine();
        out.write(group.getName());
        out.newLine();
        out.write(body == null ? "" : body);
        out.newLine();
        out.write(Integer.toString(replies.size()));
        out.newLine();
        for (Message r : replies) {
            r.save(out);
        }
    }

    protected static Group findGroup(List<Group> list, String name) {
        for (Group g : list) {
            if (g.getName().equals(name)) {
                return g;
            }
        }
        return null;
    }
}
