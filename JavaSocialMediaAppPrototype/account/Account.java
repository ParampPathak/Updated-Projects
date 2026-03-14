package account;

public class Account {
    private String name;
    private int id;
    private static int nextID = 1;
    private AccountStatus status;

    public Account(String name) {
        if (name == null || name.trim().isEmpty()) {
            throw new IllegalArgumentException("Name cannot be empty");
        }
        this.name = name;
        this.id = nextID++;
        this.status = AccountStatus.Normal;
    }

    public int getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public AccountStatus getStatus() {
        return status;
    }

    public void setStatus(AccountStatus status) {
        this.status = status;
    }

    public boolean isMuted() {
        return status == AccountStatus.Muted;
    }

    public boolean isBlocked() {
        return status == AccountStatus.Blocked;
    }

    @Override
    public String toString() {
        String result = name + " (" + id + ")";
        if (status != AccountStatus.Normal) {
            result += " [" + status + "]";
        }
        return result;
    }
}
