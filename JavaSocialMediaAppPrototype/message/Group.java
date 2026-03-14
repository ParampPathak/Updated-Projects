package message;

/**
*Represents a group with a name and an active status.
*The group can be enabled or disabled as needed.
*@author Param Pathak
*@version 1.0
*@since P04
*/
public class Group {
    private String name;
    private boolean active;

    public Group(String name) {
        if (name == null || name.trim().isEmpty()) {
            throw new IllegalArgumentException("Group name cannot be empty");
        }
        this.name = name;
        this.active = true;
    }

    public String getName() {
        return name;
    }

    public boolean isActive() {
        return active;
    }

    public void disable() {
        this.active = false;
    }

    public void enable() {
        this.active = true;
    }

    @Override
    public String toString() {
        return name + (active ? "" : " [inactive]");
    }
}
