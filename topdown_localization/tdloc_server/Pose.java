package localization;

public class Pose {
	private byte type;
	private byte id;
	
	private float x;
	private float y;
	private float yaw;
	
	private long timestamp;

	public Pose(byte type, byte id, float x, float y, float yaw, long timestamp) {
		super();
		this.type = type;
		this.id = id;
		this.x = x;
		this.y = y;
		this.yaw = yaw;
		this.timestamp = timestamp;
	}
	
	public String getObjIdentifier() {
		return new Byte(type).toString() + new Byte(id).toString();
	}

	public byte getType() {
		return type;
	}

	public void setType(byte type) {
		this.type = type;
	}

	public byte getId() {
		return id;
	}

	public void setId(byte id) {
		this.id = id;
	}

	public float getX() {
		return x;
	}

	public void setX(float x) {
		this.x = x;
	}

	public float getY() {
		return y;
	}

	public void setY(float y) {
		this.y = y;
	}

	public float getYaw() {
		return yaw;
	}

	public void setYaw(float yaw) {
		this.yaw = yaw;
	}

	public long getTimestamp() {
		return timestamp;
	}

	public void setTimestamp(long timestamp) {
		this.timestamp = timestamp;
	}
	
	public String toString() {
	  return "Pose(" + type + ", " + id + ", " + x + ", " + y + ", " + yaw + ", " + timestamp + ")";
	}
}
